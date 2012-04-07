/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_lcdcontrol.c,v 1.21 2006/01/27 20:19:17 synap Exp $
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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <gettext.h>
#include <math.h>

#include <libvisual/libvisual.h>

#include "LCDControl.h"
#include "LCDEvent.h"


extern "C" const VisPluginInfo *get_plugin_info (int *count);

using namespace LCD;

namespace {

int lcdcontrol_init (VisPluginData *plugin);
int lcdcontrol_cleanup (VisPluginData *plugin);
int lcdcontrol_requisition (VisPluginData *plugin, int *width, int *height);
int lcdcontrol_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lcdcontrol_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lcdcontrol_palette (VisPluginData *plugin);
int lcdcontrol_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

} // End namespace


VISUAL_PLUGIN_API_VERSION_VALIDATOR

extern "C" const VisPluginInfo *get_plugin_info (int *count)
{
    static VisActorPlugin actor;
    actor.requisition = lcdcontrol_requisition;
    actor.palette = lcdcontrol_palette;
    actor.render = lcdcontrol_render;
    actor.vidoptions.depth = VISUAL_VIDEO_DEPTH_32BIT;

    static VisPluginInfo info[1];
    info[0].type = VISUAL_PLUGIN_TYPE_ACTOR;
    info[0].plugname = "lcdcontrol";
    info[0].name = "LCDControl";
    info[0].author = "Scott Sibley <sisibley@gmail.com>";
    info[0].version = "0.1";
    info[0].about = "LibVisual LCD Simulation";
    info[0].help = "This plugin simulates an LCD and is based on LCD4Linux.";
    info[0].init = lcdcontrol_init;
    info[0].cleanup = lcdcontrol_cleanup;
    info[0].events = lcdcontrol_events;
    info[0].plugin = VISUAL_OBJECT(&actor);

	*count = sizeof (info) / sizeof (*info);

	return info;
}

namespace {

void *my_thread_func(void *data)
{
    LCDControl *control = (LCDControl *)data;
    control->Start();
    return NULL;
}

int lcdcontrol_init (VisPluginData *plugin)
{
	LCDPrivate *priv;

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	priv = visual_mem_new0 (LCDPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	visual_palette_allocate_colors (&priv->pal, 256);

	visual_buffer_init_allocate (&priv->pcm, sizeof (float) * PCM_SIZE, visual_buffer_destroyer_free);

    visual_timer_init(&priv->timer);


    priv->thread = visual_thread_create(my_thread_func, priv, TRUE);

    priv->events = &plugin->eventqueue;

    visual_object_ref(VISUAL_OBJECT(priv->events));

    priv->control = new LCDControl(priv);

	return 0;
}


int lcdcontrol_cleanup (VisPluginData *plugin)
{
	LCDPrivate *priv = (LCDPrivate *)visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_palette_free_colors (&priv->pal);

	visual_object_unref (VISUAL_OBJECT (&priv->pcm));
	visual_object_unref (VISUAL_OBJECT (&priv->events));

	visual_mem_free (priv);

    priv->control->Stop();

    visual_thread_join(priv->thread);

    visual_thread_free(priv->thread);

    if(priv->control != NULL)
        delete priv->control;

    if(priv->pixels != NULL)
        delete priv->pixels;

    visual_object_unref(VISUAL_OBJECT(priv->events));

	return 0;
}

int lcdcontrol_requisition (VisPluginData *plugin, int *width, int *height)
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

int lcdcontrol_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	LCDPrivate *priv = (LCDPrivate *)visual_object_get_private (VISUAL_OBJECT (plugin));
	visual_video_set_dimension (video, width, height);

    if(priv->pixels != NULL)
        delete priv->pixels;

    priv->pixels = new uint32_t[width * height]; 

	return 0;
}

int lcdcontrol_events (VisPluginData *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
            {
/*
				lcdcontrol_dimension (plugin, ev.event.resize.video,
						ev.event.resize.width, ev.event.resize.height);
*/
				break;
            }
            case VISUAL_EVENT_GENERIC:
            {
                LCDEvent *lcd_event = (LCDEvent *)ev.event.generic.data_ptr;
                lcd_event->mFunc(lcd_event->mData);
                
                break;
            }
			default: /* to avoid warnings */
            {
				break;
            }
		}
	}

	return 0;
}


VisPalette *lcdcontrol_palette (VisPluginData *plugin)
{
	LCDPrivate *priv = (LCDPrivate *)visual_object_get_private (VISUAL_OBJECT (plugin));
	int i;

	for (i = 0; i < 256; i++) {
		priv->pal.colors[i].r = i;
		priv->pal.colors[i].g = i;
		priv->pal.colors[i].b = i;
	}

	return &priv->pal;
}

int lcdcontrol_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	LCDPrivate *priv = (LCDPrivate *)visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_audio_get_sample_mixed (audio, &priv->pcm, TRUE, 2,
			VISUAL_AUDIO_CHANNEL_LEFT,
			VISUAL_AUDIO_CHANNEL_RIGHT,
			1.0,
			1.0);

	float *pcmbuf = (float *)visual_buffer_get_data (&priv->pcm);

	uint8_t *buf = (uint8_t *) visual_video_get_pixels (video);

    visual_mutex_lock(&priv->control->mutex_);
    visual_mem_copy(priv->control->pcm, pcmbuf, PCM_SIZE * sizeof(float));
    visual_mem_copy(buf, priv->pixels, video->height * video->pitch);
    visual_mutex_unlock(&priv->control->mutex_);

	return 0;
}

} // end anonymous namespace
