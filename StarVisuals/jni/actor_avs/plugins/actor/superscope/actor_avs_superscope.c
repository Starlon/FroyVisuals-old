/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_avs_superscope.c 1536 2011-05-19 17:50:14Z starlon $
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

/* FIXME TODO:
 *
 * config UI.
 */
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <lua/visscript-lua.h>

#include <libvisual/libvisual.h>

#include "avs_common.h"
#include "lvavs_pipeline.h"

typedef enum scope_runnable ScopeRunnable;

enum scope_runnable {
    SCOPE_RUNNABLE_INIT = 0,
    SCOPE_RUNNABLE_FRAME,
    SCOPE_RUNNABLE_BEAT,
    SCOPE_RUNNABLE_POINT,
};

typedef struct {
    char *runnables[4];

    double n, b, x, y, i, v, w, h, red, green, blue, linesize, skip, drawmode, t, d; 
    LVAVSPipeline *pipeline;


    int          channel_source;
    int             color_pos;
    VisPalette       pal;

    int             needs_init;

    AVSGfxColorCycler   *cycler;
} SuperScopePrivate;

int lv_superscope_init (VisPluginData *plugin);
int lv_superscope_cleanup (VisPluginData *plugin);
int lv_superscope_requisition (VisPluginData *plugin, int *width, int *height);
int lv_superscope_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_superscope_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_superscope_palette (VisPluginData *plugin);
int lv_superscope_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static const VisActorPlugin actor[] = {{
        .requisition = lv_superscope_requisition,
        .palette = lv_superscope_palette,
        .render = lv_superscope_render,
        .vidoptions.depth = VISUAL_VIDEO_DEPTH_32BIT

    }};

    static const VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_ACTOR, //".[avs]",

        .plugname = "avs_superscope",
        .name = "Libvisual AVS Render: superscope element",
        .author = "Dennis Smit <ds@nerds-incorporated.org>",
        .version = "0.1",
        .about = "The Libvisual AVS Render: superscope element",
        .help = "This is the superscope element for the libvisual AVS system",

        .init = lv_superscope_init,
        .cleanup = lv_superscope_cleanup,
        .events = lv_superscope_events,

        .plugin = VISUAL_OBJECT (&actor[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

void set_vars(SuperScopePrivate *priv)
{
    // Some of these don't need to be set.
    priv->n = visscript_get("n");
    priv->b = visscript_get("b");
    priv->x = visscript_get("x");
    priv->y = visscript_get("y");
    priv->i = visscript_get("i");
    priv->v = visscript_get("v");
    priv->w = visscript_get("w"); 
    priv->h = visscript_get("h");
    priv->t = visscript_get("t");
    priv->d = visscript_get("d");
    priv->red = visscript_get("red");
    priv->green = visscript_get("green");
    priv->blue = visscript_get("blue");
    priv->linesize = visscript_get("linesize");
    priv->skip = visscript_get("skip");
    priv->drawmode = visscript_get("drawmode");

}

int scope_load_runnable(SuperScopePrivate *priv, ScopeRunnable runnable, char *buf)
{
    visual_log_return_val_if_fail(buf != NULL, VISUAL_ERROR_GENERAL);

    if(priv->runnables[runnable] != NULL)
        free(priv->runnables[runnable]);
    priv->runnables[runnable] = strdup(buf);

    return 0;
}


int scope_run(SuperScopePrivate *priv, ScopeRunnable runnable)
{
    visual_log_return_val_if_fail(priv->runnables[runnable] != NULL, VISUAL_ERROR_GENERAL);

    visscript_eval(priv->runnables[runnable], FALSE);

    if(visscript_error())
        visual_log(VISUAL_LOG_CRITICAL, "Lua expression caused an error: %s", visscript_error());

    set_vars(priv);

    return 0;
}

int lv_superscope_init (VisPluginData *plugin)
{
    SuperScopePrivate *priv;
    VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
    int i;

    static VisParamEntry params[] = {
        VISUAL_PARAM_LIST_ENTRY_STRING ("point", "d=i+v*0.2; r=t+i*PI*4*count; x = cos(r)*d; y = sin(r)*d;"),
        VISUAL_PARAM_LIST_ENTRY_STRING ("frame", "t=t-0.01;count=(count or 0)+1;"),
        VISUAL_PARAM_LIST_ENTRY_STRING ("beat", ""),
        VISUAL_PARAM_LIST_ENTRY_STRING ("init", "n=800;"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("channel_source", 0),
        VISUAL_PARAM_LIST_ENTRY ("palette"),
        VISUAL_PARAM_LIST_ENTRY_INTEGER ("drawmode", 1),
        VISUAL_PARAM_LIST_END
    };

    visual_param_container_add_many(paramcontainer, params);

    priv = visual_mem_new0 (SuperScopePrivate, 1);
    priv->pipeline = visual_object_get_private(VISUAL_OBJECT(plugin));
    visual_object_ref(VISUAL_OBJECT(priv->pipeline));
    visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    visual_palette_allocate_colors (&priv->pal, 1);

    for (i = 0; i < priv->pal.ncolors; i++) {
        priv->pal.colors[i].r = i;
        priv->pal.colors[i].g = i;
        priv->pal.colors[i].b = i;
        priv->pal.colors[i].a = 0xff;
    }

    visual_param_entry_set_palette (visual_param_container_get (paramcontainer, "palette"), &priv->pal);

    visual_palette_free_colors (&priv->pal);

    priv->needs_init = TRUE;

    visscript_open();
    
    visscript_set("n", 32);
    visscript_set("b", 1);
    visscript_set("x", 1);
    visscript_set("y", 1);
    visscript_set("i", 1);
    visscript_set("v", 1);
    visscript_set("w", 1);
    visscript_set("h", 1);
    visscript_set("t", 1);
    visscript_set("d", 1);
    visscript_set("red", 255);
    visscript_set("green", 255);
    visscript_set("blue", 255);
    visscript_set("linesize", 1);
    visscript_set("skip", 1);
    visscript_set("drawmode", 0);

    return 0;
}

int lv_superscope_cleanup (VisPluginData *plugin)
{
    SuperScopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    int i;
    if(priv->pipeline != NULL)
        visual_object_unref(VISUAL_OBJECT(priv->pipeline));

    for(i = 0; i < 4; i++)
    {
        if(priv->runnables[i] != NULL)
            visual_mem_free(priv->runnables[i]);
    }

    visual_mem_free (priv);

    visscript_close();

    return 0;
}

int lv_superscope_requisition (VisPluginData *plugin, int *width, int *height)
{
    return 0;
}

int lv_superscope_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
    visual_video_set_dimension (video, width, height);

    return 0;
}

int lv_superscope_events (VisPluginData *plugin, VisEventQueue *events)
{
    SuperScopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    VisParamEntry *param;
    VisEvent ev;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_RESIZE:
                lv_superscope_dimension (plugin, ev.event.resize.video,
                        ev.event.resize.width, ev.event.resize.height);
                break;

            case VISUAL_EVENT_PARAM:
                param = ev.event.param.param;

                if (visual_param_entry_is (param, "point")) {
                    char *point = (visual_param_entry_get_string (param));
                    scope_load_runnable(priv, SCOPE_RUNNABLE_POINT, point);

                } else if (visual_param_entry_is (param, "frame")) {
                    char *frame = (visual_param_entry_get_string (param));
                    scope_load_runnable(priv, SCOPE_RUNNABLE_FRAME, frame);

                } else if (visual_param_entry_is (param, "beat")) {
                    char *beat = (visual_param_entry_get_string (param));
                    scope_load_runnable(priv, SCOPE_RUNNABLE_BEAT, beat);

                } else if (visual_param_entry_is (param, "init")) {
                    char *init = (visual_param_entry_get_string (param));
                    scope_load_runnable(priv, SCOPE_RUNNABLE_INIT, init);
                    priv->needs_init = TRUE;

                } else if (visual_param_entry_is (param, "channel_source"))

                    priv->channel_source = visual_param_entry_get_integer (param);

                else if (visual_param_entry_is (param, "drawmode"))

                    priv->drawmode = visual_param_entry_get_integer (param);

                else if (visual_param_entry_is (param, "palette")) {

                    VisPalette *pal;

                    pal = visual_param_entry_get_palette (param);

                    visual_palette_free_colors (&priv->pal);
                    visual_palette_allocate_colors (&priv->pal, pal->ncolors);
                    visual_palette_copy (&priv->pal, pal);

                    if (priv->cycler != NULL)
                        visual_object_unref (VISUAL_OBJECT (priv->cycler));

                    priv->cycler = avs_gfx_color_cycler_new (&priv->pal);
                    avs_gfx_color_cycler_set_mode (priv->cycler, AVS_GFX_COLOR_CYCLER_TYPE_TIME);
                    avs_gfx_color_cycler_set_time (priv->cycler, avs_config_standard_color_cycler_time ());
                }

                break;

            default:
                break;
        }
    }

    return 0;
}

VisPalette *lv_superscope_palette (VisPluginData *plugin)
{
    //SuperScopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

    return NULL;
}

static __inline int makeint(double t)
{
  if (t <= 0.0) return 0;
  if (t >= 1.0) return 255;
  return (int)(t*255.0);
}

int lv_superscope_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
    SuperScopePrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
    LVAVSPipeline *pipeline = priv->pipeline;
    int *buf = pipeline->framebuffer;
    int isBeat;

    float pcmbuf[BEAT_ADV_MAX];
    int size = BEAT_ADV_MAX/2;

    isBeat = pipeline->isBeat;

    if(priv->needs_init) {
        priv->needs_init = FALSE;
        scope_run(priv, SCOPE_RUNNABLE_INIT);
    }

    int a, l, lx = 0, ly = 0, x = 0, y = 0;
    int32_t current_color;
    int ws=(priv->channel_source&4)?1:0;
    int xorv=(ws*128)^128;

    if((priv->channel_source&3) >= 2)
    {
        for(x = 0; x < size; x++) {
            pcmbuf[x] = pipeline->audiodata[ws^1][0][x] / 2 + pipeline->audiodata[ws^1][1][x] / 2;
        }
    }
    else 
    {
        for(x = 0; x < size; x++) {
            pcmbuf[x] = pipeline->audiodata[ws^1][priv->channel_source&3][x];
        }
    }
    
    priv->color_pos++;

    if(priv->color_pos >= priv->pal.ncolors * 64) priv->color_pos = 0;

    {
        int p = priv->color_pos/64;
        int r = priv->color_pos&63;
        int c1, c2;
        int r1, r2, r3;
        c1 = visual_color_to_uint32(&priv->pal.colors[p]);
        if(p+1 < priv->pal.ncolors)
            c2=visual_color_to_uint32(&priv->pal.colors[p+1]);
        else c2 = visual_color_to_uint32(&priv->pal.colors[0]);

        r1 = (((c1&255)*(63-r))+((c2&255)*4))/64;
        r2 = ((((c1>>8)&255)*(63-r))+(((c2>>8)&255)*4))/64;
        r3 = ((((c1>>16)&255)*(63-r))+(((c2>>16)&255)*r))/64;

        current_color = r1|(r2<<8)|(r3<<16)|(255<<24);
    }

    priv->h = video->height;
    priv->w = video->width;
    priv->b = isBeat?1.0:0.0;
    priv->blue = (current_color&0xff)/255.0;
    priv->green = ((current_color>>8)&0xff)/255.0;
    priv->red = ((current_color>>16)&0xff)/255.0;
    priv->skip = 0.0;
    priv->linesize = (double) ((priv->pipeline->blendmode&0xff0000)>>16);
    priv->drawmode = priv->drawmode ? 1.0 : 0.0;

    
    visscript_set("h", priv->h);
    visscript_set("w", priv->w);
    visscript_set("b", priv->b);
    visscript_set("blue", priv->blue);
    visscript_set("green", priv->green);
    visscript_set("red", priv->red);
    visscript_set("skip", priv->skip);
    visscript_set("linesize", priv->linesize);
    visscript_set("drawmode", priv->drawmode);

    scope_run(priv, SCOPE_RUNNABLE_FRAME);

    if (isBeat)
        scope_run(priv, SCOPE_RUNNABLE_BEAT);

    l = priv->n;
    if (l >= 128*size)
        l = 128*size - 1;
    if(l < 2) 
        l = 2;

    for (a=0; a < l; a++) 
    {
        double r=(a*size)/(double)l;
        double s1=r-(int)r;
        int val1 = (pcmbuf[(int)r] + 1) / 2.0 * 128;
        int val2 = (pcmbuf[(int)r+1] + 1) / 2.0  * 128;
        double yr=(val1^xorv)*(1.0-s1)+(val2^xorv)*(s1);
        priv->v = yr/128.0;
        priv->i = a/(double)(l-1);
        priv->skip = 0.0;

        visscript_set("v", priv->v);
        visscript_set("i", priv->i);
        visscript_set("skip", priv->skip);

        scope_run(priv, SCOPE_RUNNABLE_POINT);

        x = (int)((priv->x + 1.0) * video->width * 0.5);
        y = (int)((priv->y + 1.0) * video->height * 0.5);


        if (priv->skip >= 0.00001)
            continue;

        uint32_t this_color = makeint(priv->blue) | (makeint(priv->green) << 8) | (makeint(priv->red) << 16) | (255 << 24);

        if (0 && priv->drawmode < 0.00001) {
            if (y >= 0 && y < video->height && x >= 0 && x < video->width) {
                BLEND_LINE(buf+x+y*video->width, this_color, pipeline->blendtable, pipeline->blendmode);
            }
        } else {
            if (a > 0) {
                if (y >= 0 && y < video->height && x >= 0 && x < video->width &&
                    ly >= 0 && ly < video->height && lx >= 0 && lx < video->width) {
                        VisColor color;
                        this_color = 0xffffffff;
                        visual_color_from_uint32(&color, this_color);

                        avs_gfx_line_ints(video, lx, ly, x, y, &color);
                }
            }
        }
        lx = x;
        ly = y;
    }

    return 0;
}

