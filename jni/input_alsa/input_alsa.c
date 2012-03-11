/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2012
 *
 * Authors: Scott Sibley <sisibley@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <gettext.h>
#include <libvisual/libvisual.h>

#include <tinyalsa/asoundlib.h>

typedef struct {
    struct pcm *pcmstream;
    struct pcm_config config;
    int loaded;
} alsaPrivate;

static int inp_alsa_init (VisPluginData *plugin);
static int inp_alsa_cleanup (VisPluginData *plugin);
static int inp_alsa_upload (VisPluginData *plugin, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static VisInputPlugin input[] = {{
        .upload = inp_alsa_upload
    }};

    static VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_INPUT,

        .plugname = "alsa",
        .name = "Alsa Input",
        .author = "Scott Sibley <sisibley@gmail.com>",
        .version = "0.1",
        .about = ("ALSA capture plugin"),
        .help = ("Use this plugin to capture PCM data from the ALSA record device"),
        .license = VISUAL_PLUGIN_LICENSE_LGPL,

        .init = inp_alsa_init,
        .cleanup = inp_alsa_cleanup,

        .plugin = VISUAL_OBJECT (&input[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int inp_alsa_init (VisPluginData *plugin)
{
    alsaPrivate *priv = visual_mem_new0 (alsaPrivate, 1);
    unsigned int device = 0;
    unsigned int channels = 2;
    unsigned int rate = 48000;

    visual_log_return_val_if_fail(priv != NULL, -1);
    visual_log_return_val_if_fail(plugin != NULL, -1);

    visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    priv->config.channels = channels;
    priv->config.rate = rate;
    priv->config.period_count = 4;
    priv->config.period_size = 1024;
    priv->config.format = PCM_FORMAT_S16_LE;
    priv->config.stop_threshold = 0;
    priv->config.start_threshold = 0;
    priv->config.silence_threshold = 0;
    priv->pcmstream = pcm_open(0, device, PCM_IN, &priv->config);

    if(!priv->pcmstream) {
        visual_log(VISUAL_LOG_WARNING, "Couldn't open pcm stream: %s", 
            pcm_get_error(priv->pcmstream));
        return VISUAL_ERROR_GENERAL;
    }

    VisParamContainer *paramcontainer = visual_plugin_get_params(plugin);

    static VisParamEntry params[] = {
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("isBeat", FALSE),
        VISUAL_PARAM_LIST_END
    };

    visual_param_container_add_many(paramcontainer, params);

    return 0;
}

int inp_alsa_cleanup (VisPluginData *plugin)
{
    alsaPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_log_return_val_if_fail(priv != NULL, -1);
    visual_log_return_val_if_fail(plugin != NULL, -1);

    pcm_close(priv->pcmstream);

    visual_mem_free (priv);

    return 0;
}

int inp_alsa_upload (VisPluginData *plugin, VisAudio *audio)
{
    alsaPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisParamContainer *paramcontainer = visual_plugin_get_params(plugin);
    VisParamEntry *entry = visual_param_container_get(paramcontainer, "isBeat");
    int isBeat, i;

    visual_log_return_val_if_fail(audio != NULL, -1);
    visual_log_return_val_if_fail(plugin != NULL, -1);

    visual_log_return_val_if_fail(priv != NULL, -1);

    if(pcm_is_ready(priv->pcmstream))
    {
        int size = pcm_get_buffer_size(priv->pcmstream) / 3;
        if(size > 0)
        {
            int16_t data[size];
            char scaled[size];
            if(!(pcm_read(priv->pcmstream, data, size)))
            {
        
                VisBuffer buffer;
        
                visual_buffer_init (&buffer, data, size/2, NULL);
        
                visual_audio_samplepool_input (audio->samplepool, &buffer, VISUAL_AUDIO_SAMPLE_RATE_48000,
                    VISUAL_AUDIO_SAMPLE_FORMAT_S16, VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);

                for(i = 0; i < size; i++)
                {
                    scaled[i] = data[size] / (float)USHRT_MAX * CHAR_MAX;
                }

                // FIXME Beat detection isn't working real well. It also slows us down.
                isBeat = visual_audio_is_beat_with_data(audio, VISUAL_BEAT_ALGORITHM_PEAK, scaled, size);

                visual_param_entry_set_integer(entry, isBeat);
            }
        }
    }


    return 0;
}

