/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_lv_scope.c,v 1.21 2006/01/27 20:19:17 synap Exp $
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

#define PCM_SIZE	2048

typedef struct {
	VisPalette	pal;
	VisBuffer	pcm;
    double n, b, x, y, i, v, w, h, red, green, blue, linesize, skip, drawmode, t, d;
    double zo, z1, r1, r2, r3;
    int channel_source, needs_init;
    VisTimer timer;
} ScopePrivate;

int lv_scope_init (VisPluginData *plugin);
int lv_scope_cleanup (VisPluginData *plugin);
int lv_scope_requisition (VisPluginData *plugin, int *width, int *height);
int lv_scope_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_scope_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_scope_palette (VisPluginData *plugin);
int lv_scope_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static VisActorPlugin actor[] = {{
		.requisition = lv_scope_requisition,
		.palette = lv_scope_palette,
		.render = lv_scope_render,
		.vidoptions.depth = VISUAL_VIDEO_DEPTH_8BIT
	}};

	static VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "lv_scope",
		.name = "libvisual scope",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "Libvisual scope plugin",
		.help = "This is a test plugin that'll display a simple scope",

		.init = lv_scope_init,
		.cleanup = lv_scope_cleanup,
		.events = lv_scope_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int lv_scope_init (VisPluginData *plugin)
{
	ScopePrivate *priv;

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	priv = visual_mem_new0 (ScopePrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_palette_allocate_colors (&priv->pal, 256);

	visual_buffer_init_allocate (&priv->pcm, sizeof (float) * PCM_SIZE, visual_buffer_destroyer_free);

    priv->needs_init = TRUE;
    visual_timer_init(&priv->timer);

	return 0;
}

int lv_scope_cleanup (VisPluginData *plugin)
{
	ScopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_palette_free_colors (&priv->pal);

	visual_object_unref (VISUAL_OBJECT (&priv->pcm));

	visual_mem_free (priv);

	return 0;
}

int lv_scope_requisition (VisPluginData *plugin, int *width, int *height)
{
	int reqw, reqh;

	reqw = *width;
	reqh = *height;

	while (reqw % 2 || (reqw / 2) % 2)
		reqw--;

	while (reqh % 2 || (reqh / 2) % 2)
		reqh--;

	if (reqw < 32)
		reqw = 32;

	if (reqh < 32)
		reqh = 32;

	*width = reqw;
	*height = reqh;

	return 0;
}

int lv_scope_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	return 0;
}

int lv_scope_events (VisPluginData *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_scope_dimension (plugin, ev.event.resize.video,
						ev.event.resize.width, ev.event.resize.height);
				break;
			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

VisPalette *lv_scope_palette (VisPluginData *plugin)
{
	ScopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	int i;

	for (i = 0; i < 256; i++) {
		priv->pal.colors[i].r = i;
		priv->pal.colors[i].g = i;
		priv->pal.colors[i].b = i;
	}

	return &priv->pal;
}

void run_init(ScopePrivate *priv)
{
    priv->n = 64;
    priv->zo = 0;
    visual_timer_start(&priv->timer);
}

void run_frame(ScopePrivate *priv)
{
    priv->zo=visual_timer_elapsed_msecs(&priv->timer) / 1000.0;
    priv->r1=1/7.0;
    priv->r2=4/9.0;
    priv->r3=5/3.0;
}

void run_beat(ScopePrivate *priv)
{
    priv->zo = visual_timer_elapsed_msecs(&priv->timer) + 1000;
}

void run_point(ScopePrivate *priv)
{
    priv->r1=priv->r2*9333.2312311+priv->r3*33222.93329; 
    priv->r1=priv->r1-floor(priv->r1);
    priv->r2=priv->r3*6233.73553+priv->r1*9423.1323219; 
    priv->r2=priv->r2-floor(priv->r2);
    priv->r3=priv->r1*373.871324+priv->r2*43322.4323441; 
    priv->r3=priv->r3-floor(priv->r3);

    priv->z1=priv->r3-priv->zo;
    priv->z1=.5/(priv->z1-floor(priv->z1)+.2);

    priv->x=(priv->r2*2-1)*priv->z1;
    priv->y=(priv->r1*2-1)*priv->z1;
    priv->red=(1-exp(-priv->z1*priv->z1)) * 255.0; priv->green=priv->red; priv->blue=priv->red;
}

static __inline int makeint(double t)
{
  if (t <= 0.0) return 0;
  if (t >= 1.0) return 255;
  return (int)(t*255.0);
}

int lv_scope_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	ScopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	VisColor col;
	float *pcmbuf;
	int i, y, y_old, x;
	uint8_t *buf;
    int isBeat = 0;

    int a, l, lx = 0, ly = 0;
    int ws=(priv->channel_source&4)?1:0;
    int xorv=(ws*128)^128;
    int size=video->width/4.0;


	if (video == NULL)
		return -1;

	y = video->height >> 1;

    if(priv->needs_init)
    {
        run_init(priv);
        priv->needs_init = FALSE;
    }

	visual_audio_get_sample_mixed (audio, &priv->pcm, TRUE, 2,
			VISUAL_AUDIO_CHANNEL_LEFT,
			VISUAL_AUDIO_CHANNEL_RIGHT,
			1.0,
			1.0);

	pcmbuf = visual_buffer_get_data (&priv->pcm);
	visual_color_set (&col, 0, 0, 0);
	visual_video_fill_color (video, &col);

	buf = (uint8_t *) visual_video_get_pixels (video);

    priv->h = video->height;
    priv->w = video->width;
    priv->b = 0;

    run_frame(priv);

    if(isBeat)
        run_beat(priv);

    l = priv->n;

    if(l >= 128*size)
        l = 128*size - 1;


    for(a = 0; a < l; a++)
    {
        double r=(a*size)/(double)l;
        double s1=r-(int)r;
        int val1 = (pcmbuf[(int)r]/(float)UCHAR_MAX + 1) / 2.0 * 128;
        int val2 = (pcmbuf[(int)r+1]/(float)UCHAR_MAX + 1) / 2.0 * 128;
        double yr = (val1^xorv)*(1.0-s1)+(val2^xorv)*(s1);
        priv->y = yr/128.0;
        priv->i = (double)a/(double)(l-1);

        run_point(priv);

        //uint32_t this_color = (makeint(priv->blue) | (makeint(priv->green) << 8) | (makeint(priv->red) << 16) | (255 << 24));
        uint8_t this_color = (77 * makeint(priv->red) + 158 * priv->green * priv->blue);

        x = (int)((priv->x + 1) * (double)video->width * 0.5);
        y = (int)((priv->y + 1) * (double)video->height * 0.5);

        if(y >= 0 && y < video->height && x >= 0 && x < video->width)
        {
            buf[x+y*video->width] = this_color;
        }
    }

	y_old = video->height / 2;
	for (i = 0; i < video->width; i++) {
		int j;
        run_point(priv);

		y = (video->height / 2) + (pcmbuf[(i >> 1) % PCM_SIZE] * (video->height / 4));

		if (y > y_old) {
			for (j = y_old; j < y; j++) {
				buf[(j * video->pitch) + i] = 255;
			}
		} else {
			for (j = y; j < y_old; j++) {
				buf[(j * video->pitch) + i] = 255;
			}
		}
	}

	return 0;
}

