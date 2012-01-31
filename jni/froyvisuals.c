/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <jni.h>
#include <time.h>
#include <android/log.h>
#include <android/bitmap.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <tinyalsa/asoundlib.h>
#include <libvisual.h>

#define  LOG_TAG    "FroyVisuals"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

/* Set to 1 to enable debug log traces. */
#define DEBUG 0

/* Set to 1 to optimize memory stores when generating plasma. */
#define OPTIMIZE_WRITES  1

static VisBin *bin;

/* Return current time in milliseconds */
static double now_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000. + tv.tv_usec/1000.;
}

/* simple stats management */
typedef struct {
    double  renderTime;
    double  frameTime;
} FrameStats;

#define  MAX_FRAME_STATS  200
#define  MAX_PERIOD_MS    1500

typedef struct {
    double  firstTime;
    double  lastTime;
    double  frameTime;

    int         firstFrame;
    int         numFrames;
    FrameStats  frames[ MAX_FRAME_STATS ];
} Stats;

static void
stats_init( Stats*  s )
{
    s->lastTime = now_ms();
    s->firstTime = 0.;
    s->firstFrame = 0;
    s->numFrames  = 0;
}

static void
stats_startFrame( Stats*  s )
{
    s->frameTime = now_ms();
}

static void
stats_endFrame( Stats*  s )
{
    double now = now_ms();
    double renderTime = now - s->frameTime;
    double frameTime  = now - s->lastTime;
    int nn;

    if (now - s->firstTime >= MAX_PERIOD_MS) {
        if (s->numFrames > 0) {
            double minRender, maxRender, avgRender;
            double minFrame, maxFrame, avgFrame;
            int count;

            nn = s->firstFrame;
            minRender = maxRender = avgRender = s->frames[nn].renderTime;
            minFrame  = maxFrame  = avgFrame  = s->frames[nn].frameTime;
            for (count = s->numFrames; count > 0; count-- ) {
                nn += 1;
                if (nn >= MAX_FRAME_STATS)
                    nn -= MAX_FRAME_STATS;
                double render = s->frames[nn].renderTime;
                if (render < minRender) minRender = render;
                if (render > maxRender) maxRender = render;
                double frame = s->frames[nn].frameTime;
                if (frame < minFrame) minFrame = frame;
                if (frame > maxFrame) maxFrame = frame;
                avgRender += render;
                avgFrame  += frame;
            }
            avgRender /= s->numFrames;
            avgFrame  /= s->numFrames;

            LOGI("frame/s (avg,min,max) = (%.1f,%.1f,%.1f) "
                 "render time ms (avg,min,max) = (%.1f,%.1f,%.1f)\n",
                 1000./avgFrame, 1000./maxFrame, 1000./minFrame,
                 avgRender, minRender, maxRender);
        }
        s->numFrames  = 0;
        s->firstFrame = 0;
        s->firstTime  = now;
    }

    nn = s->firstFrame + s->numFrames;
    if (nn >= MAX_FRAME_STATS)
        nn -= MAX_FRAME_STATS;

    s->frames[nn].renderTime = renderTime;
    s->frames[nn].frameTime  = frameTime;

    if (s->numFrames < MAX_FRAME_STATS) {
        s->numFrames += 1;
    } else {
        s->firstFrame += 1;
        if (s->firstFrame >= MAX_FRAME_STATS)
            s->firstFrame -= MAX_FRAME_STATS;
    }

    s->lastTime = now;
}

static void my_info_handler (const char *msg, const char *funcname, void *privdata)
{
    LOGI("libvisual INFO: %s: %s\n", __lv_progname, msg);
}
        
static void my_warning_handler (const char *msg, const char *funcname, void *privdata)
{
    if (funcname)
        LOGW("libvisual WARNING: %s: %s(): %s\n",
        __lv_progname, funcname, msg);
    else
	LOGW("libvisual WARNING: %s: %s\n", __lv_progname, msg);
}

static void my_critical_handler (const char *msg, const char *funcname, void *privdata)
{
    if (funcname)
        LOGW("libvisual CRITICAL: %s: %s(): %s\n",
        __lv_progname, funcname, msg);
    else
	LOGW("libvisual CRITICAL: %s: %s\n", __lv_progname, msg);
}

static void my_error_handler (const char *msg, const char *funcname, void *privdata)
{
    if (funcname)
        LOGW("libvisual ERROR: %s: %s(): %s\n",
        __lv_progname, funcname, msg);
    else
	LOGW("libvisual ERROR: %s: %s\n", __lv_progname, msg);
}
                                                                                        

JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_FroyVisualsView_renderFroyVisuals(JNIEnv * env, jobject  obj, jobject bitmap,  jlong  time_ms)
{
    AndroidBitmapInfo  info;
    void*              pixels;
    int                ret;
    static Stats       stats;
    static int         init;
    static VisVideo *bin_video = NULL;
    static int w = -1, h = -1;
    VisVideoDepth depth = visual_video_depth_enum_from_value(8);

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGB_565) {
        LOGE("Bitmap format is not RGB_565 !");
        return;
    }


    if (!init) {
            stats_init(&stats);
            init = 1;
	    if(!visual_is_initialized())
	    {
	            visual_init_path_add("/data/data/com.starlon.froyvisuals/lib");
	            visual_log_set_info_handler (my_info_handler, NULL);
	            visual_log_set_warning_handler (my_warning_handler, NULL);
	            visual_log_set_critical_handler (my_critical_handler, NULL);
	            visual_log_set_error_handler (my_error_handler, NULL);
	            visual_log_set_verboseness(VISUAL_LOG_VERBOSENESS_HIGH);
	            visual_init(0, NULL);
                    visual_thread_enable(FALSE);
	            visual_log(VISUAL_LOG_INFO, "Initialized libvisual");
	    }

            w = info.width;
            h = info.height;
            bin = visual_bin_new();
            visual_bin_set_supported_depth(bin, VISUAL_VIDEO_DEPTH_ALL);
            bin_video = visual_video_new();
            visual_video_set_depth(bin_video, depth);
            visual_video_set_dimension(bin_video, w, h);
            visual_video_set_pitch(bin_video, w * visual_video_bpp_from_depth(depth));
            visual_video_allocate_buffer(bin_video);
            visual_bin_set_video(bin, bin_video);
            visual_bin_connect_by_names(bin, "infinite", "alsa");
            visual_bin_depth_changed(bin);
            visual_bin_switch_set_style(bin, VISUAL_SWITCH_STYLE_DIRECT);
            visual_bin_realize(bin);
            visual_bin_sync(bin, FALSE);
    }


    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return;
    }

    stats_startFrame(&stats);


    if( info.width != w || info.height != h) {
            w = info.width;
            h = info.height;
            if(bin_video)
                visual_video_free_buffer(bin_video);
            visual_video_set_dimension(bin_video, w, h);
            visual_video_set_pitch(bin_video, w * visual_video_bpp_from_depth(depth));
            visual_video_allocate_buffer(bin_video);
            visual_bin_sync(bin, FALSE);
    }

    if(visual_bin_depth_changed(bin))
        visual_bin_sync(bin, TRUE);

    visual_bin_run(bin);

    VisVideo *bitmap_video = visual_video_new();
    visual_video_set_attributes(bitmap_video, w, h, w * 2, VISUAL_VIDEO_DEPTH_16BIT);
    visual_video_set_buffer(bitmap_video, pixels);

    visual_video_depth_transform(bitmap_video, bin_video);

    visual_object_unref(VISUAL_OBJECT(bitmap_video));

    AndroidBitmap_unlockPixels(env, bitmap);

    stats_endFrame(&stats);
}
