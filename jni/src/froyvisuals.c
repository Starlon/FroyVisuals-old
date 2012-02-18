/***************************************************************************
 *   Based on work by xmms2 team. #xmms2 irc.freenode.org
 *   Based on work by Max Howell <max.howell@methylblue.com>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <sys/types.h>
//#include <sys/socket.h>
//#include <sys/un.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <jni.h>
#include <time.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <libvisual/libvisual.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>

#define DEVICE_DEPTH VISUAL_VIDEO_DEPTH_16BIT

#define  LOG_TAG    "FroyVisuals"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define MORPH "alphablend"
#define ACTOR "bumpscope"
#define INPUT "dummy"

/* LIBVISUAL */
struct {
	VisVideo   *video;
    VisPalette  *pal;
	VisBin     *bin;
	const char *actor_name;
    const char *morph_name;
    const char *input_name;
	int         pluginIsGL;
	int16_t     pcm_data[1024];
    int      pcm_size;
} v;

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

            visual_log(VISUAL_LOG_INFO, "frame/s (avg,min,max) = (%.1f,%.1f,%.1f) "
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

VisVideo *new_video(int w, int h, VisVideoDepth depth, void *pixels)
{
    VisVideo *video = visual_video_new();
    visual_video_set_depth(video, depth);
    visual_video_set_dimension(video, w, h);
    visual_video_set_pitch(video, visual_video_bpp_from_depth(depth) * w);
    visual_video_set_buffer(video, pixels);
    return video;
}

static void v_cycleActor (int prev)
{
    v.actor_name = (prev ? visual_actor_get_prev_by_name (v.actor_name)
                     : visual_actor_get_next_by_name (v.actor_name));
    if (!v.actor_name) {
        v.actor_name = (prev ? visual_actor_get_prev_by_name (0)
                         : visual_actor_get_next_by_name (0));
    }

    v.morph_name = visual_morph_get_next_by_name(v.morph_name);
    if(!v.morph_name) {
        v.morph_name = visual_morph_get_next_by_name(0);
    }
}

// For fallback audio source.
JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_FroyVisualsView_uploadAudio(JNIEnv * env, jobject  obj, jshortArray data)
{
    int i;
    jshort *pcm;
    jsize len = (*env)->GetArrayLength(env, data);
    pcm = (*env)->GetShortArrayElements(env, data, NULL);
    for(i = 0; i < len && i < sizeof(v.pcm_data); i++)
    {
        v.pcm_data[i] = pcm[i];
    }
    (*env)->ReleaseShortArrayElements(env, data, pcm, 0);
}


JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_FroyVisualsView_switchActor(JNIEnv * env, jobject  obj, jboolean prev)
{
    const char *morph = v.morph_name;

    v_cycleActor((int)prev);

    visual_log(VISUAL_LOG_INFO, "Switching actors %s -> %s", morph, v.morph_name);

    //visual_bin_set_morph_by_name (v.bin, (char *)v.morph_name);
    visual_bin_switch_actor_by_name(v.bin, (char *)v.actor_name);
}

JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_FroyVisualsView_mouseMotion(JNIEnv * env, jobject  obj, jfloat x, jfloat y)
{
    return;
    visual_log(VISUAL_LOG_INFO, "Mouse motion: x %f, y %f", x, y);
    VisPluginData *plugin = visual_actor_get_plugin(visual_bin_get_actor(v.bin));
    VisEventQueue *eventqueue = visual_plugin_get_eventqueue(plugin);
    visual_event_queue_add_mousemotion(eventqueue, x, y);
}

JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_FroyVisualsView_mouseButton(JNIEnv * env, jobject  obj, jint button, jfloat x, jfloat y)
{
    return;
    visual_log(VISUAL_LOG_INFO, "Mouse button: button %d, x %f, y %f", button, x, y);
    VisPluginData *plugin = visual_actor_get_plugin(visual_bin_get_actor(v.bin));
    VisEventQueue *eventqueue = visual_plugin_get_eventqueue(plugin);
        VisMouseState state = VISUAL_MOUSE_DOWN;
    visual_event_queue_add_mousebutton(eventqueue, button, state, x, y);
}


JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_FroyVisualsView_screenResize(JNIEnv * env, jobject  obj, jint w, jint h)
{
    visual_log(VISUAL_LOG_INFO, "Screen resize w %d h %d", w, h);

    VisPluginData *plugin = visual_actor_get_plugin(visual_bin_get_actor(v.bin));
    VisEventQueue *eventqueue = visual_plugin_get_eventqueue(plugin);
    visual_event_queue_add_resize(eventqueue, v.video, w, h);
}

JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_FroyVisualsView_keyboardEvent(JNIEnv * env, jobject  obj, jint x, jint y)
{
    return;
    VisEventQueue *eventqueue = visual_plugin_get_eventqueue(visual_actor_get_plugin(visual_bin_get_actor(v.bin)));
    VisKey keysym;
    int keymod;
    VisKeyState state;
    visual_event_queue_add_keyboard(eventqueue, keysym, keymod, state);
}

// Is this even needed? What happens when the app is quietly discarded?
// Seems in Android 4.0 you can kill an app by swiping it.
JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_FroyVisualsView_visualsQuit(JNIEnv * env, jobject  obj)
{
    visual_video_free_buffer(v.video);
    visual_object_unref(VISUAL_OBJECT(v.video));
    visual_object_unref(VISUAL_OBJECT(v.bin));
    visual_quit();
}

void app_main(int w, int h)
{
    int depthflag;
    VisVideoDepth depth;

    if(!visual_is_initialized())
    {
        visual_init_path_add("/data/data/com.starlon.froyvisuals/lib");
    	visual_log_set_verboseness (VISUAL_LOG_VERBOSENESS_HIGH);
        visual_log_set_info_handler (my_info_handler, NULL);
        visual_log_set_warning_handler (my_warning_handler, NULL);
        visual_log_set_critical_handler (my_critical_handler, NULL);
        visual_log_set_error_handler (my_error_handler, NULL);
    
    	visual_init (0, NULL);
        memset(&v, 0, sizeof(v));
    }

    v.morph_name = MORPH;
    v.actor_name = ACTOR;
    v.input_name = INPUT;

	v.bin    = visual_bin_new ();

	if (!visual_actor_valid_by_name (v.actor_name)) {
		visual_log(VISUAL_LOG_CRITICAL, ("Actor plugin not found!"));
        return;
	}

	visual_bin_set_supported_depth (v.bin, VISUAL_VIDEO_DEPTH_ALL);
    visual_bin_set_preferred_depth(v.bin, VISUAL_VIDEO_DEPTH_8BIT);

    VisActor *actor = visual_actor_new((char*)v.actor_name);
    VisInput *input = visual_input_new((char*)v.input_name);

    depthflag = visual_actor_get_supported_depth(actor);
    depth = visual_video_depth_get_highest(depthflag);

	v.video = visual_video_new();
    visual_video_set_dimension(v.video, w, w);
    visual_video_set_depth(v.video, depth);
    visual_video_set_pitch(v.video, w * visual_video_bpp_from_depth(depth));
    visual_video_allocate_buffer(v.video);
    visual_bin_set_video(v.bin, v.video);

	visual_bin_switch_set_style (v.bin, VISUAL_SWITCH_STYLE_DIRECT);
	visual_bin_switch_set_automatic (v.bin, 1);
	visual_bin_switch_set_steps (v.bin, 10);

    visual_bin_connect(v.bin, actor, input);
    if((v.pluginIsGL = (visual_bin_get_depth (v.bin) == VISUAL_VIDEO_DEPTH_GL)))
    {
        visual_video_set_depth(v.video, VISUAL_VIDEO_DEPTH_GL);
        visual_video_free_buffer(v.video);
        visual_video_allocate_buffer(v.video);
    }
	visual_bin_realize (v.bin);
	visual_bin_sync (v.bin, 0);
    visual_bin_depth_changed(v.bin);


	printf ("Libvisual version %s; bpp: %d %s\n", visual_get_version(), v.video->bpp, (v.pluginIsGL ? "(GL)\n" : ""));
}

JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_FroyVisualsView_initApp(JNIEnv * env, jobject  obj, jint w, jint h)
{
    app_main(w, h);
}

JNIEXPORT jboolean JNICALL Java_com_starlon_froyvisuals_FroyVisualsView_render(JNIEnv * env, jobject  obj, jobject bitmap, jint dur)
{
    
    AndroidBitmapInfo  info;
    void*              pixels;
    int                ret;
    static Stats       stats;
    int depthflag;
    VisVideoDepth depth;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return FALSE;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return FALSE;
    }

    stats_startFrame(&stats);

    VisVideo *vid = new_video(info.width, info.height, DEVICE_DEPTH, pixels);

    if(visual_bin_depth_changed(v.bin) || 
        (info.width != v.video->width || 
        info.height != v.video->height) ) 
    {
		v.pluginIsGL = (visual_bin_get_depth (v.bin) == VISUAL_VIDEO_DEPTH_GL);
        depthflag = visual_bin_get_depth(v.bin);
        depth = visual_video_depth_get_highest(depthflag);
        if(v.pluginIsGL)
        {
    		visual_video_set_depth (v.video, VISUAL_VIDEO_DEPTH_GL);
        } else
        {
            visual_video_set_depth(v.video, depth);
        }
        visual_video_set_dimension(v.video, info.width, info.height);
        visual_video_set_pitch(v.video, info.width * visual_video_bpp_from_depth(depth));
        if(visual_video_get_pixels(v.video))
            visual_video_free_buffer(v.video);
        visual_video_allocate_buffer(v.video);
        visual_bin_sync(v.bin, TRUE);
    }

	if (0 && v.pluginIsGL) {
        //FIXME
		//visual_bin_run (v.bin);
	} else {

		visual_bin_run (v.bin);
    }

    visual_video_blit_overlay(vid, v.video, 0, 0, FALSE);
    //visual_video_depth_transform(vid, v.video);

    visual_object_unref(VISUAL_OBJECT(vid));

    AndroidBitmap_unlockPixels(env, bitmap);

    stats_endFrame(&stats);

	return TRUE;
}
