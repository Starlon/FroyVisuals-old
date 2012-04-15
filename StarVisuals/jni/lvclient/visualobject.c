/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004-2006 Dennis Smit <ds@nerds-incorporated.org>
 * Copyright (C) 2012 Daniel Hiepler <daniel-lva@niftylight.de>         
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *          Daniel Hiepler <daniel-lva@niftylight.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <libvisual/libvisual.h>
#include "visual.h"
#include "fps.h"


/** local variables */
static struct
{
    /* used to count FPS */
    Fps fps;
}_v;


        


/** VisualObject.init() */
JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_VisualObject_fpsInit(JNIEnv * env, 
                                                                         jobject  obj)
{
    fps_init(&_v.fps);
    return JNI_TRUE;
}


/** VisualObject.renderVisual() */
JNIEXPORT void JNICALL Java_com_starlon_starvisuals_VisualObject_renderVisual(JNIEnv * env, 
                                                                                   jobject  obj, 
                                                                                   jobject bitmap,
                                                                                   jint bin,
                                                                                   jint video)
{
    VisBin *b = (VisBin *) bin;
    VisVideo *bvideo = (VisVideo *) video;
        
    if(!visual_is_initialized() ||
       !bvideo ||
       !b || 
       !b->input || 
       !b->actor ||
       !b->actvideo)
    {
        LOGE("Not initialized properly");    
        return;
    }
        
    /* start fps timing */
    fps_startFrame(&_v.fps);

    /* run libvisual pipeline */
    visual_bin_run(b);

        
    /* lock bitmap for drawing */
    int ret;
    void *pixels;
    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) 
    {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    /* set buffer to pixels */
    visual_video_set_buffer(bvideo, pixels);
    
    /* depth transform */
    visual_video_depth_transform(bvideo, b->actvideo);

        
    /* unlock bitmap */
    AndroidBitmap_unlockPixels(env, bitmap);

    /* stop fps timing */
    fps_endFrame(&_v.fps);
}

/** VisLog -> android Log glue */
static void _log_handler(VisLogSeverity severity, const char *msg, const VisLogSource *source, void *priv)
{

    switch(severity)
    {
        case VISUAL_LOG_DEBUG:
            LOGI("(debug) %s(): %s", source->func, msg);
            break;
        case VISUAL_LOG_INFO:
            LOGI("(info) %s", msg);
            break;
        case VISUAL_LOG_WARNING:
            LOGW("(WARNING) %s", msg);
            break;
        case VISUAL_LOG_ERROR:
            LOGE("(ERROR) (%s:%d) %s(): %s", source->file, source->line, source->func, msg);
            break;
        case VISUAL_LOG_CRITICAL:
            LOGE("(CRITICAL) (%s:%d) %s(): %s", source->file, source->line, source->func, msg);
            break;
    }
}


/******************************************************************************
 ******************************************************************************/

/** LibVisual.init() */
JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_VisualObject_init(JNIEnv * env, jobject  obj)
{
    if(visual_is_initialized())
                return JNI_TRUE;

    LOGI("LibVisual.init(): %s", visual_get_version());

#ifndef NDEBUG
    /* endless loop to wait for debugger to attach */
    int foo = 1;
    while(foo);
#endif
       
    /* register VisLog handler to make it log to android logcat */
    visual_log_set_handler(VISUAL_LOG_DEBUG, _log_handler, NULL);
    visual_log_set_handler(VISUAL_LOG_INFO, _log_handler, NULL);
    visual_log_set_handler(VISUAL_LOG_WARNING, _log_handler, NULL);
    visual_log_set_handler(VISUAL_LOG_CRITICAL, _log_handler, NULL);
    visual_log_set_handler(VISUAL_LOG_ERROR, _log_handler, NULL);
    visual_log_set_verbosity(VISUAL_LOG_DEBUG);


    /* initialize libvisual */
    char *v[] = { "lvclient", NULL };
    char **argv = v;
    int argc=1;
    visual_init(&argc,  &argv);

     /* add our plugin search path */
    visual_plugin_registry_add_path("/data/data/org.libvisual.android/lib");

    return JNI_TRUE;
}


/** LibVisual.deinit() */
JNIEXPORT void JNICALL Java_com_starlon_starvisuals_VisualObject_deinit(JNIEnv * env, jobject  obj)
{
    LOGI("LibVisual.deinit()");
        
    if(visual_is_initialized())
        visual_quit();
}


