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

#define PCM_BUF_SIZE 2048

typedef struct {
    VisRandomContext rContext;
    VisTimer timer;
    void *data;
    long size;
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

        .plugname = "dummy",
        .name = "dummy",
        .author = "Scott Sibley <sisibley@gmail.com>",
        .version = "0.1",
        .about = ("DUMMY input plugin for libvisual"),
        .help = ("This plugin generates a randomly generated supply of \"sound\" data - just random shorts."),
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
    size_t sz;
    FILE *file;
    alsaPrivate *priv = visual_mem_new0 (alsaPrivate, 1);

    file = fopen ( "/mnt/sdcard/sound.au" , "r" );
    fseek (file , 0 , SEEK_END );
    priv->size = ftell(file);  
    rewind(file);
    priv->data = visual_mem_malloc(priv->size);
    fread(priv->data, priv->size, 1, file);
    fclose ( file );

    visual_log_return_val_if_fail(priv != NULL, -1);
    visual_log_return_val_if_fail(plugin != NULL, -1);

    visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    visual_timer_init(&priv->timer);
    visual_timer_start(&priv->timer);

    visual_random_context_init(&priv->rContext, 0xededed);

    return 0;
}

int inp_alsa_cleanup (VisPluginData *plugin)
{
    alsaPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_log_return_val_if_fail(priv != NULL, -1);
    visual_log_return_val_if_fail(plugin != NULL, -1);

    visual_mem_free(priv->data);
    visual_mem_free (priv);

    return 0;
}

int inp_alsa_upload (VisPluginData *plugin, VisAudio *audio)
{
    VisBuffer buffer;
    uint16_t data[PCM_BUF_SIZE];
    uint16_t val;
    alsaPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_log_return_val_if_fail(audio != NULL, -1);
    visual_log_return_val_if_fail(plugin != NULL, -1);

    visual_log_return_val_if_fail(priv != NULL, -1);

    visual_random_context_set_seed(&priv->rContext, visual_timer_elapsed_msecs(&priv->timer));
/*
    for(i = 0; i < PCM_BUF_SIZE; i++)
    {
        data[i] = visual_random_context_float(&priv->rContext) * 16384;
    }
*/
    val = ((long)(visual_random_context_float(&priv->rContext) * 16384) % priv->size);
    visual_mem_copy(data, priv->data + val, PCM_BUF_SIZE * sizeof(int16_t));
    visual_buffer_init (&buffer, data, PCM_BUF_SIZE/2, NULL);
    visual_audio_samplepool_input (audio->samplepool, &buffer, 
        VISUAL_AUDIO_SAMPLE_RATE_48000,
        VISUAL_AUDIO_SAMPLE_FORMAT_S16, 
        VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);

    return 0;
}

