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
#include <math.h>
#include <libvisual/libvisual.h>

#define PCM_BUF_SIZE 2048
#define FREQUENCY 5
#define AMPLITUDE 12288

typedef struct {
    VisRandomContext rContext;
    VisTimer timer;
    void *data;
    long size;
} dummyPrivate;

static int inp_dummy_init (VisPluginData *plugin);
static int inp_dummy_cleanup (VisPluginData *plugin);
static int inp_dummy_upload (VisPluginData *plugin, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static VisInputPlugin input[] = {{
        .upload = inp_dummy_upload
    }};

    static VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_INPUT,

        .plugname = "debug",
        .name = "debug",
        .author = "Scott Sibley <sisibley@gmail.com>",
        .version = "0.1",
        .about = ("Debug input plugin for libvisual"),
        .help = ("This plugin generates a randomly generated supply of \"sound\" data - just random shorts."),
        .license = VISUAL_PLUGIN_LICENSE_LGPL,

        .init = inp_dummy_init,
        .cleanup = inp_dummy_cleanup,

        .plugin = VISUAL_OBJECT (&input[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int inp_dummy_init (VisPluginData *plugin)
{
    dummyPrivate *priv = visual_mem_new0 (dummyPrivate, 1);

    visual_return_val_if_fail(priv != NULL, -1);
    visual_return_val_if_fail(plugin != NULL, -1);

    visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    visual_timer_init(&priv->timer);
    visual_timer_start(&priv->timer);

    visual_random_context_init(&priv->rContext, 0xededed);

    return 0;
}

int inp_dummy_cleanup (VisPluginData *plugin)
{
    dummyPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_return_val_if_fail(priv != NULL, -1);
    visual_return_val_if_fail(plugin != NULL, -1);

    visual_mem_free(priv->data);
    visual_mem_free (priv);

    return 0;
}

int inp_dummy_upload (VisPluginData *plugin, VisAudio *audio)
{
    static double time = 0;
    int freq = FREQUENCY;
    int amp = AMPLITUDE;
    VisBuffer buffer;
    int16_t data[PCM_BUF_SIZE], i;
    int val1, val2;
    dummyPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_return_val_if_fail(audio != NULL, -1);
    visual_return_val_if_fail(plugin != NULL, -1);

    visual_return_val_if_fail(priv != NULL, -1);

    visual_random_context_set_seed(&priv->rContext, visual_timer_elapsed_msecs(&priv->timer));

    val1 = visual_random_context_int(&priv->rContext);

    for(i = 0; i < PCM_BUF_SIZE; i++)
    {
        val2 = val1 + i;
            
        data[i] = amp*cos(val2*freq*(2*M_PI)/PCM_BUF_SIZE);
    }

    visual_buffer_init (&buffer, data, PCM_BUF_SIZE/2, NULL);
    visual_audio_samplepool_input (audio->samplepool, &buffer, 
        VISUAL_AUDIO_SAMPLE_RATE_48000,
        VISUAL_AUDIO_SAMPLE_FORMAT_S16, 
        VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);

    return 0;
}

