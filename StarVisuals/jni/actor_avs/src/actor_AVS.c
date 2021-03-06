/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_AVS.c,v 1.12 2006-09-19 19:05:47 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <libvisual/libvisual.h>

#include "lvavs_preset.h"
#include "lvavs_pipeline.h"

typedef struct {
    AVSTree     *wtree;     /* The winamp AVS tree */

    LVAVSPreset *lvtree;    /* The LV AVS tree */
    LVAVSPipeline   *pipeline;  /* The LV AVS Render pipeline */

    int      needsnego; /* Pipeline out of sync, needs reneg ? */
} AVSPrivate;

int act_avs_init (VisPluginData *plugin);
int act_avs_cleanup (VisPluginData *plugin);
int act_avs_requisition (VisPluginData *plugin, int *width, int *height);
int act_avs_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int act_avs_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *act_avs_palette (VisPluginData *plugin);
int act_avs_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info(int *count);
const VisPluginInfo *get_plugin_info (int *count)
{
    static const VisActorPlugin actor[] = {{
        .requisition = act_avs_requisition,
        .palette = act_avs_palette,
        .render = act_avs_render,
        .vidoptions.depth = VISUAL_VIDEO_DEPTH_32BIT,
    }};

    static const VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_ACTOR,

        .plugname = "avs",
        .name = "Libvisual advanced visual studio plugin",
        .author = "Dennis Smit <ds@nerds-incorporated.org>",
        .version = "0.1",
        .about = "The libvisual advanced visual studio plugin",
        .help = "",

        .init = act_avs_init,
        .cleanup = act_avs_cleanup,
        .events = act_avs_events,

        .plugin = VISUAL_OBJECT (&actor[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int act_avs_init (VisPluginData *plugin)
{
    AVSPrivate *priv;
    char filename[256];
    VisParamEntry *param;
    memset(filename, 0, sizeof(filename));
    snprintf(filename, sizeof(filename) - 1, "%s/%s", getenv("HOME"), ".pipelines.xml");

    priv = visual_mem_new0 (AVSPrivate, 1);
    visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    if (FALSE && filename != NULL) {
        //priv->lvtree = lvavs_preset_new_from_preset (filename);
    } else {
        LVAVSPreset *preset;
        LVAVSPresetElement *blur1;
        LVAVSPresetElement *blur2;
        LVAVSPresetElement *blur3;
        LVAVSPresetElement *blur4;
        LVAVSPresetElement *superscope;
        LVAVSPresetElement *timescope;
        LVAVSPresetElement *stars;
        LVAVSPresetElement *ring;
        LVAVSPresetElement *clearscreen;
        LVAVSPresetElement *analyzer;
        LVAVSPresetElement *bumpscope;
        
        superscope = lvavs_preset_element_new(LVAVS_PRESET_ELEMENT_TYPE_PLUGIN, "avs_superscope");
        ring = lvavs_preset_element_new(LVAVS_PRESET_ELEMENT_TYPE_PLUGIN, "avs_ring");
        timescope = lvavs_preset_element_new(LVAVS_PRESET_ELEMENT_TYPE_PLUGIN, "avs_timescope");
        stars = lvavs_preset_element_new(LVAVS_PRESET_ELEMENT_TYPE_PLUGIN, "avs_stars");
        analyzer = lvavs_preset_element_new(LVAVS_PRESET_ELEMENT_TYPE_PLUGIN, "lv_analyzer");
        clearscreen = lvavs_preset_element_new(LVAVS_PRESET_ELEMENT_TYPE_PLUGIN, "avs_clearscreen");
        bumpscope = lvavs_preset_element_new(LVAVS_PRESET_ELEMENT_TYPE_PLUGIN, "bumpscope");
        blur1 = lvavs_preset_element_new(LVAVS_PRESET_ELEMENT_TYPE_PLUGIN, "avs_blur");
        blur2 = lvavs_preset_element_new(LVAVS_PRESET_ELEMENT_TYPE_PLUGIN, "avs_blur");
        blur3 = lvavs_preset_element_new(LVAVS_PRESET_ELEMENT_TYPE_PLUGIN, "avs_blur");
        blur4 = lvavs_preset_element_new(LVAVS_PRESET_ELEMENT_TYPE_PLUGIN, "avs_blur");

        preset = lvavs_preset_new ();
        preset->main = lvavs_preset_container_new ();

        visual_list_add(preset->main->members, clearscreen);
        //visual_list_add(preset->main->members, stars);
        visual_list_add(preset->main->members, ring);
        //visual_list_add(preset->main->members, blur3);
        //visual_list_add(preset->main->members, blur4);
        //visual_list_add(preset->main->members, superscope);


        static VisParamEntry params[] = {
            VISUAL_PARAM_LIST_ENTRY_STRING("init", "n = 50;"),
            VISUAL_PARAM_LIST_END
        };
        visual_param_container_add_many(superscope->pcont, params);

        priv->lvtree = preset;
    }

    if(priv->pipeline != NULL) {
        visual_object_unref(VISUAL_OBJECT(priv->pipeline));
        priv->pipeline = NULL;
    }
    priv->pipeline = lvavs_pipeline_new_from_preset (priv->lvtree);

    lvavs_pipeline_realize (priv->pipeline);

    priv->needsnego = TRUE;

    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);

    static VisParamEntry params[] = {
        VISUAL_PARAM_LIST_ENTRY_STRING ("filename", NULL),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("blendmode", 1),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("enabled", 1),
        VISUAL_PARAM_LIST_ENTRY_INTEGER("mode", 1),
        VISUAL_PARAM_LIST_ENTRY_INTEGER("inblendval", 128),
        VISUAL_PARAM_LIST_ENTRY_INTEGER("outblendval", 128),
        VISUAL_PARAM_LIST_ENTRY_INTEGER("bufferin", 0),
        VISUAL_PARAM_LIST_ENTRY_INTEGER("bufferout", 0),
        VISUAL_PARAM_LIST_ENTRY_INTEGER("ininvert", 0),
        VISUAL_PARAM_LIST_ENTRY_INTEGER("outinvert", 0),
        VISUAL_PARAM_LIST_ENTRY_INTEGER("beat_render", 0),
        VISUAL_PARAM_LIST_ENTRY_INTEGER("beat_render_frames", 1),
        
        VISUAL_PARAM_LIST_END
    };

    visual_param_container_add_many (paramcontainer, params);
    param = visual_param_container_get(paramcontainer, "filename");
    visual_param_entry_set_string(param, filename);

    return 0;
}

int act_avs_cleanup (VisPluginData *plugin)
{

    AVSPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    visual_mem_free (priv);

    return 0;
}

int act_avs_requisition (VisPluginData *plugin, int *width, int *height)
{
    int reqw, reqh;

    reqw = *width;
    reqh = *height;

    while (reqw % 4)
        reqw--;

    while (reqh % 4)
        reqh--;

    if (reqw < 32)
        reqw = 32;

    if (reqh < 32)
        reqh = 32;

    *width = reqw;
    *height = reqh;

    return 0;
}

int act_avs_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
    AVSPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    
    visual_video_set_dimension (video, width, height);

    if (priv->pipeline != NULL)
        lvavs_pipeline_negotiate (priv->pipeline, video);
    return 0;
}

int act_avs_events (VisPluginData *plugin, VisEventQueue *events)
{
    return 0;
    AVSPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisEvent *ev;
    VisParamEntry *param;

    while (visual_event_queue_poll_by_reference (events, &ev)) {
        switch (ev->type) {
            case VISUAL_EVENT_RESIZE:
                act_avs_dimension (plugin, ev->event.resize.video,
                        ev->event.resize.width, ev->event.resize.height);

                break;
            case VISUAL_EVENT_KEYDOWN:
            case VISUAL_EVENT_KEYUP:
            case VISUAL_EVENT_MOUSEMOTION:
            case VISUAL_EVENT_MOUSEBUTTONDOWN:
            case VISUAL_EVENT_MOUSEBUTTONUP:
            case VISUAL_EVENT_NEWSONG:
            case VISUAL_EVENT_QUIT:
            case VISUAL_EVENT_GENERIC:
            case VISUAL_EVENT_VISIBILITY:
                if (priv->pipeline != NULL)
                    lvavs_pipeline_propagate_event (priv->pipeline, ev);
                break;

            case VISUAL_EVENT_PARAM:
                param = ev->event.param.param;
                if(visual_param_entry_is (param, "enabled")) {
                    if(priv->pipeline != NULL)
                        priv->pipeline->enabled = visual_param_entry_get_integer(param);
                }
                if(visual_param_entry_is(param, "mode")) {
                    if(priv->pipeline != NULL)
                    priv->pipeline->mode = visual_param_entry_get_integer(param);
                }
                if(visual_param_entry_is(param, "inblendval")) {
                    if(priv->pipeline != NULL)
                    priv->pipeline->inblendval = visual_param_entry_get_integer(param);
                }
                if(visual_param_entry_is(param, "outblendval")) {
                    if(priv->pipeline != NULL)
                    priv->pipeline->outblendval = visual_param_entry_get_integer(param);
                }
                if(visual_param_entry_is(param, "bufferin")) {
                    if(priv->pipeline != NULL)
                    priv->pipeline->bufferin = visual_param_entry_get_integer(param);
                }
                if(visual_param_entry_is(param, "bufferout")) {
                    if(priv->pipeline != NULL)
                    priv->pipeline->bufferout = visual_param_entry_get_integer(param);
                }
                if(visual_param_entry_is(param, "ininvert")) {
                    if(priv->pipeline != NULL)
                    priv->pipeline->ininvert = visual_param_entry_get_integer(param);
                }
                if(visual_param_entry_is(param, "outinvert")) {
                    if(priv->pipeline != NULL)
                    priv->pipeline->outinvert = visual_param_entry_get_integer(param);
                }
                if(visual_param_entry_is(param, "beat_render")) {
                    if(priv->pipeline != NULL)
                    priv->pipeline->beat_render = visual_param_entry_get_integer(param);
                }
                if(visual_param_entry_is(param, "beat_render_frames")) {
                    if(priv->pipeline != NULL)
                    priv->pipeline->beat_render_frames = visual_param_entry_get_integer(param);
                }
                if(visual_param_entry_is (param, "blendmode")) {
                    
                    if(priv->pipeline != NULL)
                    priv->pipeline->blendmode = visual_param_entry_get_integer(param);
                }
                if (visual_param_entry_is (param, "filename")) {
                    char *filename = visual_param_entry_get_string (param);

                    //FIXME what's wtree?
                    if (priv->wtree != NULL)
                        visual_object_unref (VISUAL_OBJECT (priv->wtree));

                    priv->wtree = NULL;

                    if (priv->lvtree != NULL) // FIXME Why was this unref commented? 
                        visual_object_unref (VISUAL_OBJECT (priv->lvtree));

                    if (priv->pipeline != NULL)
                        visual_object_unref (VISUAL_OBJECT (priv->pipeline));


                    if (filename != NULL)
                        priv->lvtree = lvavs_preset_new_from_preset (filename);

                    if(priv->pipeline != NULL) {
                        visual_object_unref(VISUAL_OBJECT(priv->pipeline));
                        priv->pipeline = NULL;
                    }
                    priv->pipeline = lvavs_pipeline_new_from_preset (priv->lvtree);

                    lvavs_pipeline_realize (priv->pipeline);

                    priv->needsnego = TRUE;
                }
                break;
            default: /* to avoid warnings */
                break;
        }

        /* Unref reference to event */
        visual_object_unref (VISUAL_OBJECT (ev));
    }
    return 0;
}

VisPalette *act_avs_palette (VisPluginData *plugin)
{
    return NULL;
}

int act_avs_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    static int c = 0;
    AVSPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    if (priv->needsnego == TRUE) {
        lvavs_pipeline_negotiate (priv->pipeline, video);

        priv->needsnego = FALSE;
        return 0;
    }

    if(c++ < 100) return;

    lvavs_pipeline_run (priv->pipeline, video, audio);

    return 0;
}

