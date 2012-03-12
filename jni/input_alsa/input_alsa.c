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
    int device;
    int card;
} alsaPrivate;

static int inp_alsa_init (VisPluginData *plugin);
static int inp_alsa_cleanup (VisPluginData *plugin);
static int inp_alsa_upload (VisPluginData *plugin, VisAudio *audio);
static int inp_alsa_events (VisPluginData *plugin, VisEventQueue *events);

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
        .events = inp_alsa_events,

        .plugin = VISUAL_OBJECT (&input[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int inp_alsa_init (VisPluginData *plugin)
{
    alsaPrivate *priv = visual_mem_new0 (alsaPrivate, 1);
    unsigned int channels = 2;
    unsigned int rate = 44100;

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
    priv->device = 0;
    priv->card = 0;
    priv->pcmstream = pcm_open(priv->device, priv->card, PCM_IN, &priv->config);

    if(!priv->pcmstream) {
        visual_log(VISUAL_LOG_WARNING, "Couldn't open pcm stream: %s", 
            pcm_get_error(priv->pcmstream));
        return -1;
    }

    VisParamContainer *paramcontainer = visual_plugin_get_params(plugin);

    static VisParamEntry params[] = {
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("isBeat", FALSE),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("device", 0),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("card", 0),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("channels", 2),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("rate", 44100),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("period_count", 4),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("period_size", 1024),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("stop_threshold", 0),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("start_threshold", 0),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("silence_threshold", 0),

        VISUAL_PARAM_LIST_END
    };

    visual_param_container_add_many(paramcontainer, params);


    return 0;
}

int inp_alsa_cleanup (VisPluginData *plugin)
{
    visual_log_return_val_if_fail(plugin != NULL, -1);

    alsaPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_log_return_val_if_fail(priv != NULL, -1);

    pcm_close(priv->pcmstream);

    visual_mem_free (priv);

    return 0;
}

static int inp_alsa_events (VisPluginData *plugin, VisEventQueue *events)
{
    visual_log_return_val_if_fail(plugin != NULL, -1);

    alsaPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_log_return_val_if_fail(priv != NULL, -1);

    VisEvent ev;
    VisParamEntry *param;
    int newConfig = FALSE;

    while(visual_event_queue_poll(events, &ev))
    {
        switch(ev.type)
        {
            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                if(visual_param_entry_is(param, "device"))
                {
                    priv->device = visual_param_entry_get_integer(param);
                    newConfig = TRUE;
                }
                else if (visual_param_entry_is(param, "card"))
                {
                    priv->card = visual_param_entry_get_integer(param);
                    newConfig = TRUE;
                }
                else if (visual_param_entry_is(param, "rate"))
                {
                    priv->config.rate = visual_param_entry_get_integer(param);
                    newConfig = TRUE;
                }
                else if (visual_param_entry_is(param, "period_count"))
                {
                    priv->config.period_count = visual_param_entry_get_integer(param);
                    newConfig = TRUE;
                }
                else if (visual_param_entry_is(param, "period_size"))
                {
                    priv->config.period_size = visual_param_entry_get_integer(param);
                    newConfig = TRUE;
                }
                else if (visual_param_entry_is(param, "stop_threshold"))
                {
                    priv->config.stop_threshold = visual_param_entry_get_integer(param);
                    newConfig = TRUE;
                }
                else if (visual_param_entry_is(param, "start_threshold"))
                {
                    priv->config.start_threshold = visual_param_entry_get_integer(param);
                    newConfig = TRUE;
                }
                else if (visual_param_entry_is(param, "silence_threshold"))
                {
                    priv->config.silence_threshold = visual_param_entry_get_integer(param);
                    newConfig = TRUE;
                }
                break;
            default:
                // Huh?
                break;
        }
    }

    if(newConfig)
    {
        char fn[256];
        snprintf(fn, sizeof(fn), "/dev/snd/pcmC%uD%u%c", priv->card, priv->device, 'c');
    
        if(access(fn, R_OK) != 0)
        {
            //FIXME rigid calls this evil. Evil is Microsoft. This is life or death. :( 
            //if(chmod(fn, S_IROTH) != 0)
            visual_log(VISUAL_LOG_WARNING, "ALSA input - device is not readable: %s", fn);
            return -1;
        }

        if(priv->pcmstream != NULL)
            pcm_close(priv->pcmstream);

        priv->pcmstream = pcm_open(priv->device, priv->card, PCM_IN, &priv->config);

        if(!priv->pcmstream) 
        {
            visual_log(VISUAL_LOG_WARNING, "Couldn't open pcm stream: %s", 
                pcm_get_error(priv->pcmstream));
            return -1;
        }
    }

    return 0;
}

int inp_alsa_upload (VisPluginData *plugin, VisAudio *audio)
{
    visual_log_return_val_if_fail(plugin != NULL, -1);

    alsaPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_log_return_val_if_fail(priv != NULL, -1);

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
        
                visual_audio_samplepool_input (audio->samplepool, &buffer, VISUAL_AUDIO_SAMPLE_RATE_44100,
                    VISUAL_AUDIO_SAMPLE_FORMAT_S16, VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);

                for(i = 0; i < size; i++)
                {
                    scaled[i] = data[size] / (float)SHRT_MAX * CHAR_MAX;
                }

                // FIXME Beat detection isn't working real well. It also slows us down.
                isBeat = visual_audio_is_beat_with_data(audio, VISUAL_BEAT_ALGORITHM_PEAK, scaled, size);

                visual_param_entry_set_integer(entry, isBeat);
            }
        }
    }


    return 0;
}

