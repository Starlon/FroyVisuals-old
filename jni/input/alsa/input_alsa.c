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
#include "libvisual.h"

#include <tinyalsa/asoundlib.h>

#define PCM_BUF_SIZE 2048

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
		.name = "alsa",
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

	visual_log_return_val_if_fail(priv != NULL, -1);
	visual_log_return_val_if_fail(plugin != NULL, -1);

	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	priv->config.channels = 2;
	priv->config.rate = 48000;
	priv->config.period_count = 4;
	priv->config.format = PCM_FORMAT_S16_LE;
	priv->config.start_threshold = 0;
	priv->config.stop_threshold = 0;
	priv->config.silence_threshold = 0;
	priv->pcmstream = pcm_open(0, 0, PCM_IN, &priv->config);

	if(!priv->pcmstream) {
		visual_log(VISUAL_LOG_WARNING, "Couldn't open pcm stream.");
		return -1;
	}

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
	int16_t data[PCM_BUF_SIZE];
	alsaPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_log_return_val_if_fail(audio != NULL, -1);
	visual_log_return_val_if_fail(plugin != NULL, -1);

	visual_log_return_val_if_fail(priv != NULL, -1);

	if(pcm_is_ready(priv->pcmstream))
	{
		if(!(pcm_read(priv->pcmstream, data, PCM_BUF_SIZE / 2)))
		{
	
			VisBuffer buffer;
	
			visual_buffer_init (&buffer, data, PCM_BUF_SIZE/2, NULL);
	
			visual_audio_samplepool_input (audio->samplepool, &buffer, VISUAL_AUDIO_SAMPLE_RATE_48000,
				VISUAL_AUDIO_SAMPLE_FORMAT_S16, VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);
		}
	}


	return 0;
}

