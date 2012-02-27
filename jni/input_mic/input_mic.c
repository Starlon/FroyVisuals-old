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
#include <jni.h>
#include <libvisual/libvisual.h>

#define PCM_BUF_SIZE 2048
#define FREQUENCY 5
#define AMPLITUDE 12288

// Hopefully we only have 1 mic? Or at least one connection to the plugin. :/
struct {
    int16_t *pcm_data;
    int size;
    VisAudioSampleRateType rate;
    VisAudioSampleChannelType channels;
    VisAudioSampleFormatType encoding;
} pcm_ref;

typedef struct {
    VisRandomContext rContext;
    VisTimer timer;
    void *data;
    long size;
} micPrivate;

static int inp_mic_init (VisPluginData *plugin);
static int inp_mic_cleanup (VisPluginData *plugin);
static int inp_mic_upload (VisPluginData *plugin, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

// Reinitialize audio fields.
JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_NativeHelper_resizePCM(jint size, jint samplerate, jint channels, jint encoding)
{
    if(pcm_ref.pcm_data)
        visual_mem_free(pcm_ref.pcm_data);
    pcm_ref.pcm_data = visual_mem_malloc(sizeof(int16_t) * size);
    pcm_ref.size = size;
    switch(samplerate)
    {
        case 8000:
            pcm_ref.rate = VISUAL_AUDIO_SAMPLE_RATE_8000;
        break;
        case 11250:
            pcm_ref.rate = VISUAL_AUDIO_SAMPLE_RATE_11250;
        break;
        case 22500:
            pcm_ref.rate = VISUAL_AUDIO_SAMPLE_RATE_22500;
        break;
        case 32000:
            pcm_ref.rate = VISUAL_AUDIO_SAMPLE_RATE_32000;
        break;
        case 44100:
            pcm_ref.rate = VISUAL_AUDIO_SAMPLE_RATE_44100;
        break;
        case 48000:
            pcm_ref.rate = VISUAL_AUDIO_SAMPLE_RATE_48000;
        break;
        case 96000:
            pcm_ref.rate = VISUAL_AUDIO_SAMPLE_RATE_96000;
        break;
    }
    pcm_ref.channels = VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO;
    pcm_ref.encoding = VISUAL_AUDIO_SAMPLE_FORMAT_S16;
}

const VisPluginInfo *get_plugin_info (int *count)
{
    static VisInputPlugin input[] = {{
        .upload = inp_mic_upload
    }};

    static VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_INPUT,

        .plugname = "mic",
        .name = "mic",
        .author = "Scott Sibley <sisibley@gmail.com>",
        .version = "0.1",
        .about = ("Mic input plugin for libvisual"),
        .help = ("You sing into the mic, and FroyVisuals goes nuts. Sing loud. With feeling."),
        .license = VISUAL_PLUGIN_LICENSE_LGPL,

        .init = inp_mic_init,
        .cleanup = inp_mic_cleanup,

        .plugin = VISUAL_OBJECT (&input[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int inp_mic_init (VisPluginData *plugin)
{
    micPrivate *priv = visual_mem_new0 (micPrivate, 1);

    visual_log_return_val_if_fail(priv != NULL, -1);
    visual_log_return_val_if_fail(plugin != NULL, -1);

    visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    visual_timer_init(&priv->timer);
    visual_timer_start(&priv->timer);

    visual_random_context_init(&priv->rContext, 0xededed);

    return 0;
}

int inp_mic_cleanup (VisPluginData *plugin)
{
    micPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_log_return_val_if_fail(priv != NULL, -1);
    visual_log_return_val_if_fail(plugin != NULL, -1);

    visual_mem_free(priv->data);
    visual_mem_free (priv);

    return 0;
}

int inp_mic_upload (VisPluginData *plugin, VisAudio *audio)
{
    VisBuffer buffer;
    int16_t data[pcm_ref.size], i;
    micPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    visual_log_return_val_if_fail(audio != NULL, -1);
    visual_log_return_val_if_fail(plugin != NULL, -1);

    visual_log_return_val_if_fail(priv != NULL, -1);

    for(i = 0; i < pcm_ref.size; i++)
    {
        data[i] = pcm_ref.pcm_data[i];
    }

    visual_buffer_init (&buffer, data, pcm_ref.size/2, NULL);
    visual_audio_samplepool_input (audio->samplepool, &buffer, 
        VISUAL_AUDIO_SAMPLE_RATE_48000,
        VISUAL_AUDIO_SAMPLE_FORMAT_S16, 
        VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);

    return 0;
}

JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_NativeHelper_uploadAudio(JNIEnv * env, jobject  obj, jshortArray data)
{
    int i;
    jshort *pcm;
    jsize len = (*env)->GetArrayLength(env, data);
    pcm = (*env)->GetShortArrayElements(env, data, NULL);
    for(i = 0; i < len && i < pcm_ref.size; i++)
    {
        pcm_ref.pcm_data[i] = pcm[i];
    }
    (*env)->ReleaseShortArrayElements(env, data, pcm, 0);
}

