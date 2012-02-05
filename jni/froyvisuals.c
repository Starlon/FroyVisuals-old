/***************************************************************************
 *   Based on work by Max Howell <max.howell@methylblue.com>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <sys/types.h>  //this must be _before_ sys/socket on freebsd
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
#include <tinyalsa/asound.h>
#include <libvisual/libvisual.h>

#define x_exit(msg) \
	printf ("Error: %s\n", msg); \
	exit (EXIT_FAILURE);

/* LIBVISUAL */
struct {
	VisVideo   *video;
	VisPalette *pal;
	VisBin     *bin;
	const char *plugin;
    const char *morph;
	int         pluginIsGL;
	int16_t     pcm_data[1024];
} v;

static void v_init (int, char**);
static uint v_render (void);
static void v_resize (int, int);

static void
v_cycleActor (int prev)
{
	v.plugin = (prev ? visual_actor_get_prev_by_name_nogl (v.plugin)
	                 : visual_actor_get_next_by_name_nogl (v.plugin));
	if (!v.plugin) {
		v.plugin = (prev ? visual_actor_get_prev_by_name (0)
						 : visual_actor_get_next_by_name (0));
	}
	if (!strcmp (v.plugin, "gstreamer") || !strcmp (v.plugin, "gdkpixbuf")) {
		v_cycleActor (prev);
	}
    v.morph = visual_morph_get_next_by_name(v.morph);
    if(!v.morph) {
        v.morph = visual_morph_get_next_by_name(0);
    }
}

JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_FroyVisualsView_initApp(JNIEnv * env, jobject  obj)
{
    if(!visual_is_initialized())
    {
            visual_log_set_verboseness(VISUAL_LOG_VERBOSENESS_HIGH);
            visual_init_path_add("/data/data/com.starlon.froyvisuals/lib");
            visual_log_set_info_handler (my_info_handler, NULL);
            visual_log_set_warning_handler (my_warning_handler, NULL);
            visual_log_set_critical_handler (my_critical_handler, NULL);
            visual_log_set_error_handler (my_error_handler, NULL);
            visual_init(0, NULL);
            visual_thread_enable(FALSE);
            visual_log(VISUAL_LOG_INFO, "LibVisual intialized...");
            visual_mem_set(&v_private, 0, sizeof(v_private));
    }

    visual_log(VISUAL_LOG_INFO, "FroyVisuals initialized...");
    return EXIT_SUCCESS;
}

void
v_resize( int width, int height )
{
	visual_video_set_dimension( v.video, width, height );

	visual_bin_sync( v.bin, 0 );
}

JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_FroyVisualsView_initApp(JNIEnv * env, jobject  obj)
{
	VisVideoDepth depth;

	visual_init (0, NULL);
	visual_log_set_verboseness (VISUAL_LOG_VERBOSENESS_VERBOSE);

	v.bin    = visual_bin_new ();
	depth  = visual_video_depth_enum_from_value( 16 );

	v.plugin = visual_actor_get_next_by_name_nogl (0);

	if (!visual_actor_valid_by_name (v.plugin)) {
		x_exit ("Actor plugin not found!");
	}

	visual_bin_set_supported_depth (v.bin, VISUAL_VIDEO_DEPTH_ALL);

	if (!(v.video = visual_video_new ())) {
		x_exit ("Cannot create a video surface");
	}
	if (visual_video_set_depth (v.video, depth) < 0) {
		x_exit ("Cannot set video depth");
	}

	visual_video_set_dimension (v.video, 640, 480);

	if (visual_bin_set_video (v.bin, v.video)) {
		x_exit ("Cannot set video");
	}

	visual_bin_connect_by_names (v.bin, (char*)v.plugin, "alsa");

	if (visual_bin_get_depth (v.bin) == VISUAL_VIDEO_DEPTH_GL)
	{
		visual_video_set_depth (v.video, VISUAL_VIDEO_DEPTH_GL);
		v.pluginIsGL = 1;
	}

	/* Called so the flag is set to false, seen we create the initial environment here */
	visual_bin_depth_changed (v.bin);

/*
	VisInput *input = visual_bin_get_input (v.bin);
	if (visual_input_set_callback (input, v_upload_callback, NULL) < 0) {
		x_exit ("Cannot set input plugin callback");
	}
*/

	visual_bin_switch_set_style (v.bin, VISUAL_SWITCH_STYLE_MORPH);
	visual_bin_switch_set_automatic (v.bin, 1);
	visual_bin_switch_set_steps (v.bin, 10);

	visual_bin_realize (v.bin);
	visual_bin_sync (v.bin, 0);

	printf ("Libvisual version %s; bpp: %d %s\n", visual_get_version(), v.video->bpp, (v.pluginIsGL ? "(GL)\n" : ""));
}

static inline void
v_set_pal(void)
{
	if (v.pal) {
		int i;
		for (i = 0; i < 256; i++) {
/*
			pal[i].r = v.pal->colors[i].r;
			pal[i].g = v.pal->colors[i].g;
			pal[i].b = v.pal->colors[i].b;
*/
		}
	}
	//SDL_SetColors( screen, pal, 0, 256 );
}

JNIEXPORT jboolean JNICALL Java_com_starlon_froyvisuals_FroyVisualsView_renderFroyVisuals(JNIEnv * env, jobject  obj, jobject bitmap)
{
    
    AndroidBitmapInfo  info;
    void*              pixels;
    int                ret;
    static Stats       stats;
    static int         init;
    static int w = -1, h = -1;
    VisBin *bin = v_private.bin;
    VisVideo *bin_video = v_private.bin_video;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return FALSE;
    }

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return FALSE;
    }

	/* On depth change */
	if (visual_bin_depth_changed (v.bin)) {
		v.pluginIsGL = (visual_bin_get_depth (v.bin) == VISUAL_VIDEO_DEPTH_GL);

		visual_bin_sync (v.bin, 1);
	}

	long ticks = -SDL_GetTicks ();

	if (v.pluginIsGL) {
		visual_bin_run (v.bin);
		//SDL_GL_SwapBuffers ();
	} else {
        visual_video_set_buffer(v.video, pixels);
        visual_video_set_depth(v.video, VISUAL_VIDEO_DEPTH_16BIT);
        visual_video_set_dimension(v.video, bitmap->width, bitmap->height);
        visual_video_set_depth(v.video, VISUAL_VIDEO_DEPTH_16BIT);
        visual_video_set_pitch(v.video, bitmap->width * bitmap->bpp);

		visual_bin_run (v.bin);

        VisVideo *vid = visual_bin_get_video(v.bin);

        visual_video_blit_overlay(vid, v.video);
		v.pal = visual_bin_get_palette (v.bin);
        v_set_pal();
	}

	return 0;
}
