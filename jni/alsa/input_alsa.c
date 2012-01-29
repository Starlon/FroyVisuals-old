/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Vitaly V. Bursov <vitalyvb@urk,net>
 *
 * Authors: Vitaly V. Bursov <vitalyvb@urk,net>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: input_alsa.c,v 1.23 2006/02/13 20:36:11 synap Exp $
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

#define PCM_BUF_SIZE 4096

typedef struct {
	struct pcm *pcmstream;
	struct pcm_config config;
	int loaded;
} alsaPrivate;

static int inp_alsa_init (VisPluginData *plugin);
static int inp_alsa_cleanup (VisPluginData *plugin);
static int inp_alsa_upload (VisPluginData *plugin, VisAudio *audio);

static const int  inp_alsa_var_btmul      = sizeof (short);
static const char *inp_alsa_var_cdevice   = "hw:0,0";
static const int  inp_alsa_var_samplerate = 44100;
static const int  inp_alsa_var_channels   = 2;

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
		.author = "Vitaly V. Bursov <vitalyvb@urk.net>",
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
	alsaPrivate *priv;
	int device = 0;
	int channels = 2;
	int rate = 44100;
	int bits = 16;
/*
	unsigned int rate = inp_alsa_var_samplerate;
	unsigned int exact_rate;
	unsigned int tmp;
	int dir;
	int err;
*/
/*
#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif
*/
	visual_log_return_val_if_fail(plugin != NULL, -1);

	priv = visual_mem_new0 (alsaPrivate, 1);
	visual_log_return_val_if_fail(priv != NULL, -1);

	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	priv->config.channels = 2;
	priv->config.rate = 44100;
	priv->config.period_size = 1024;
	priv->config.period_count = 4;
	priv->config.format = PCM_FORMAT_S16_LE;
	priv->config.start_threshold = 0;
	priv->config.stop_threshold = 0;
	priv->config.silence_threshold = 0;
	priv->pcmstream = pcm_open(0, 0, PCM_IN, &priv->config);
	if(!priv->pcmstream)
		visual_log(VISUAL_LOG_WARNING, "Couldn't open pcm stream.");
	if(!pcm_is_ready(priv->pcmstream))
		visual_log(VISUAL_LOG_WARNING, "Could not read from pcm stream.");

	priv->loaded = 1;

	return 0;
}

int inp_alsa_cleanup (VisPluginData *plugin)
{
	alsaPrivate *priv = NULL;

	visual_log_return_val_if_fail(plugin != NULL, -1);
	priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	visual_log_return_val_if_fail(priv != NULL, -1);

	if (priv->loaded == 1)
		pcm_close(priv->pcmstream);

	visual_mem_free (priv);

	return 0;
}

int inp_alsa_upload (VisPluginData *plugin, VisAudio *audio)
{
	int16_t data[PCM_BUF_SIZE];
	alsaPrivate *priv = NULL;
	int i;

	visual_log_return_val_if_fail(audio != NULL, -1);
	visual_log_return_val_if_fail(plugin != NULL, -1);
	priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	visual_log_return_val_if_fail(priv != NULL, -1);

	if(pcm_is_ready(priv->pcmstream))
	{
		if(!(pcm_read(priv->pcmstream, data, PCM_BUF_SIZE / 2)))
		{
	
			VisBuffer buffer;
	
			visual_buffer_init (&buffer, data, PCM_BUF_SIZE/2, NULL);
	
			visual_audio_samplepool_input (audio->samplepool, &buffer, VISUAL_AUDIO_SAMPLE_RATE_44100,
				VISUAL_AUDIO_SAMPLE_FORMAT_S16, VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);
		}
	}


	return 0;
}

