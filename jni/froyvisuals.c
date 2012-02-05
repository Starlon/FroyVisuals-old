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
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <jni.h>
#include <time.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <libvisual/libvisual.h>

#define  LOG_TAG    "FroyVisuals"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define DEPTH VISUAL_VIDEO_DEPTH_16BIT

#define x_exit(msg) \
	printf ("Error: %s\n", msg); \
	exit (EXIT_FAILURE);

/* LIBVISUAL */
struct {
	VisVideo   *video;
	VisBin     *bin;
	const char *plugin;
    const char *morph;
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

JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_FroyVisualsView_switchActor(JNIEnv * env, jobject  obj, jint prev)
{
	v.plugin = (prev ? visual_actor_get_prev_by_name_nogl (v.plugin)
	                 : visual_actor_get_next_by_name_nogl (v.plugin));
	if (!v.plugin) {
		v.plugin = (prev ? visual_actor_get_prev_by_name (0)
						 : visual_actor_get_next_by_name (0));
	}
    v.morph = visual_morph_get_next_by_name(v.morph);
    if(!v.morph) {
        v.morph = visual_morph_get_next_by_name(0);
    }

    visual_bin_set_morph_by_name (v.bin, (char *)v.morph);
    visual_bin_switch_actor_by_name(v.bin, (char *)v.plugin);
	visual_video_set_depth (v.video, visual_bin_get_depth(v.bin));
}

JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_FroyVisualsView_mouseMotion(JNIEnv * env, jobject  obj, jfloat x, jfloat y)
{
    visual_log(VISUAL_LOG_INFO, "Mouse motion: x %f, y %f", x, y);
    VisPluginData *plugin = visual_actor_get_plugin(visual_bin_get_actor(v.bin));
    VisEventQueue *eventqueue = visual_plugin_get_eventqueue(plugin);
    visual_event_queue_add_mousemotion(eventqueue, x, y);
}

JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_FroyVisualsView_mouseButton(JNIEnv * env, jobject  obj, jint button, jfloat x, jfloat y)
{
    visual_log(VISUAL_LOG_INFO, "Mouse button: button %d, x %f, y %f", button, x, y);
    VisPluginData *plugin = visual_actor_get_plugin(visual_bin_get_actor(v.bin));
    VisEventQueue *eventqueue = visual_plugin_get_eventqueue(plugin);
        VisMouseState state = VISUAL_MOUSE_DOWN;
    visual_event_queue_add_mousebutton(eventqueue, button, state, x, y);
}


JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_FroyVisualsView_screenResize(JNIEnv * env, jobject  obj, jint w, jint h)
{
    visual_log(VISUAL_LOG_INFO, "Screen resize w %d h %d", w, h);

	visual_video_set_dimension (v.video, w, h);
    visual_video_set_pitch(v.video, w * visual_video_bpp_from_depth(VISUAL_VIDEO_DEPTH_8BIT));
    visual_video_free_buffer(v.video);
    visual_video_allocate_buffer(v.video);

	visual_bin_sync( v.bin, 0 );

    VisPluginData *plugin = visual_actor_get_plugin(visual_bin_get_actor(v.bin));
    VisEventQueue *eventqueue = visual_plugin_get_eventqueue(plugin);
    visual_event_queue_add_resize(eventqueue, v.video, w, h);
}

JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_FroyVisualsView_keyboardEvent(JNIEnv * env, jobject  obj, jint x, jint y)
{
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

JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_FroyVisualsView_initApp(JNIEnv * env, jobject  obj)
{

	visual_log_set_verboseness (VISUAL_LOG_VERBOSENESS_HIGH);

    visual_init_path_add("/data/data/com.starlon.froyvisuals/lib");
    visual_log_set_info_handler (my_info_handler, NULL);
    visual_log_set_warning_handler (my_warning_handler, NULL);
    visual_log_set_critical_handler (my_critical_handler, NULL);
    visual_log_set_error_handler (my_error_handler, NULL);

	visual_init (LOG_TAG);

	v.bin    = visual_bin_new ();

	v.plugin = visual_actor_get_next_by_name(0);

    if(!v.plugin)
        visual_log(VISUAL_LOG_CRITICAL, "-----------------------no plugin");
    else
        visual_log(VISUAL_LOG_CRITICAL, "==================yes plugin %s", v.plugin);

	if (!visual_actor_valid_by_name (v.plugin)) {
		visual_log(VISUAL_LOG_CRITICAL, ("Actor plugin not found!"));
        x_exit("Exiting...");
        return;
	}

    v.plugin = "lv_scope";

	visual_bin_set_supported_depth (v.bin, VISUAL_VIDEO_DEPTH_ALL);
    visual_bin_set_depth(v.bin, VISUAL_VIDEO_DEPTH_8BIT);
    visual_bin_set_preferred_depth(v.bin, VISUAL_VIDEO_DEPTH_32BIT);

	v.video = visual_video_new ();
    visual_video_set_attributes(v.video, 32, 32, 32, VISUAL_VIDEO_DEPTH_8BIT);
    visual_video_allocate_buffer(v.video);
	visual_bin_set_video (v.bin, v.video);

    v.plugin = "lv_scope";

	if (visual_bin_get_depth (v.bin) == VISUAL_VIDEO_DEPTH_GL)
	{
		visual_video_set_depth (v.video, VISUAL_VIDEO_DEPTH_GL);
		v.pluginIsGL = 1;
	}

	visual_bin_connect_by_names (v.bin, (char*)v.plugin, "alsa");

	visual_bin_switch_set_style (v.bin, VISUAL_SWITCH_STYLE_MORPH);
	visual_bin_switch_set_automatic (v.bin, 1);
	visual_bin_switch_set_steps (v.bin, 10);

	visual_bin_depth_changed (v.bin);
	visual_bin_realize (v.bin);
	visual_bin_sync (v.bin, 0);

	printf ("Libvisual version %s; bpp: %d %s\n", visual_get_version(), v.video->bpp, (v.pluginIsGL ? "(GL)\n" : ""));
}

JNIEXPORT jboolean JNICALL Java_com_starlon_froyvisuals_FroyVisualsView_renderFroyVisuals(JNIEnv * env, jobject  obj, jobject bitmap)
{
    
    AndroidBitmapInfo  info;
    void*              pixels;
    int                ret;
    static Stats       stats;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return FALSE;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return FALSE;
    }

    stats_startFrame(&stats);

    VisVideo vid;
    visual_video_init(&vid);
    visual_video_set_attributes(&vid, info.width, info.height, info.width * visual_video_depth_value_from_enum(DEPTH) / 8, DEPTH);
    visual_video_set_buffer(&vid, pixels);

	/* On depth change */
	if (visual_bin_depth_changed (v.bin)) {
		v.pluginIsGL = (visual_bin_get_depth (v.bin) == VISUAL_VIDEO_DEPTH_GL);

		visual_bin_sync (v.bin, 1);
	}

    visual_video_depth_transform(&vid, v.video);

	if (0 && v.pluginIsGL) {
        //FIXME
		//visual_bin_run (v.bin);
	} else {

		visual_bin_run (v.bin);

    }

    visual_video_depth_transform(&vid, v.video);

    AndroidBitmap_unlockPixels(env, bitmap);

    stats_endFrame(&stats);

	return TRUE;
}
