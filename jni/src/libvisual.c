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
#include <android/log.h>

#include <libvisual/libvisual.h>
#include <SDL.h>

#define  LOG_TAG    "FroyVisuals"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define x_exit(msg) \
    printf ("Error: %s\n", msg); \
    exit (EXIT_FAILURE);

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

int x_vis;

/*
static void
xmms2_quit (void)
{
    xmmsc_visualization_shutdown (x_connection, x_vis);
    if (x_connection) {
        xmmsc_unref (x_connection);
    }
}

static void
xmms2_init (void)
{
    xmmsc_result_t *res;
    xmmsv_t *val;
    const char *errmsg;
    char *path = getenv ("XMMS_PATH");
    xmmsv_t *cfg;

    x_connection = xmmsc_init ("xmms2-libvisual");

    if (!x_connection || !xmmsc_connect (x_connection, path)){
        printf ("%s\n", xmmsc_get_last_error (x_connection));
        x_exit ("couldn't connect to xmms2d!");
    }

    res = xmmsc_visualization_init (x_connection);
    xmmsc_result_wait (res);
    val = xmmsc_result_get_value (res);
    if (xmmsv_get_error (val, &errmsg)) {
        x_exit (errmsg);
    }
    x_vis = xmmsc_visualization_init_handle (res);

    cfg = xmmsv_build_dict (XMMSV_DICT_ENTRY_STR ("type", "pcm"),
                            XMMSV_DICT_ENTRY_STR ("stereo", "1"),
                            XMMSV_DICT_END);

    res = xmmsc_visualization_properties_set (x_connection, x_vis, cfg);
    xmmsc_result_wait (res);
    val = xmmsc_result_get_value(res);
    if (xmmsv_get_error(val, &errmsg)) {
        x_exit (errmsg);
    }
    xmmsc_result_unref (res);
    xmmsv_unref (cfg);

    while (!xmmsc_visualization_started (x_connection, x_vis)) {
        res = xmmsc_visualization_start (x_connection, x_vis);
        if (xmmsc_visualization_errored (x_connection, x_vis)) {
            printf ("Couldn't start visualization transfer: %s\n",
                xmmsc_get_last_error (x_connection));
            exit (EXIT_FAILURE);
        }
        if (res) {
            xmmsc_result_wait (res);
            xmmsc_visualization_start_handle (x_connection, res);
            xmmsc_result_unref (res);
        }
    }

    atexit (xmms2_quit);
}
*/

/* SDL */
SDL_Surface *screen = 0;
SDL_Color    pal[256];

static void sdl_init(void);
static int sdl_event_handler(void);
static void sdl_quit(void);

static inline void sdl_lock(void) { if( SDL_MUSTLOCK( screen ) == SDL_TRUE ) SDL_LockSurface( screen ); }
static inline void sdl_unlock(void) { if( SDL_MUSTLOCK( screen ) == SDL_TRUE ) SDL_UnlockSurface( screen ); }

static inline int
sdl_isFullScreen(void)
{
    return (screen->flags & SDL_FULLSCREEN) > 0;
}

static inline void
sdl_toggleFullScreen(void)
{
    SDL_WM_ToggleFullScreen( screen );
    SDL_ShowCursor( (screen->flags & SDL_FULLSCREEN) > 0 ? SDL_DISABLE : SDL_ENABLE );
}

/* LIBVISUAL */
struct {
    VisVideo   *video;
    VisPalette *pal;
    VisBin     *bin;
    const char *plugin;
    int         pluginIsGL;
    int16_t     pcm_data[1024];
} v;

static void v_init (int, char**);
static uint v_render (void);
static void v_resize (int, int);

static void
v_cycleActor (int prev)
{
    v.plugin = (prev ? visual_actor_get_prev_by_name (v.plugin)
                     : visual_actor_get_next_by_name (v.plugin));
    if (!v.plugin) {
        v.plugin = (prev ? visual_actor_get_prev_by_name (0)
                         : visual_actor_get_next_by_name (0));
    }
    if (!strcmp (v.plugin, "gstreamer") || !strcmp (v.plugin, "gdkpixbuf")) {
        v_cycleActor (prev);
    }
}

int
main (int argc, char** argv)
{
int *t = NULL;
*t = 0xf;
    puts ("Controls: Arrow keys switch between plugins, TAB toggles fullscreen, ESC quits.");
    puts ("          Each plugin can has its own mouse/key bindings, too.");
    if (argc > 1) {
        v.plugin = argv[1];
    } else {
        puts ("Note: you can give your favourite libvisual plugin as command line argument.");
    }
    v.pluginIsGL = 0;

    sdl_init ();
    v_init(argc, argv);

    static Stats       stats;

    int render_time = 0;

    while (1) {
        stats_startFrame(&stats);
        render_time = v_render();
        stats_endFrame(&stats);
    }

    return EXIT_SUCCESS;
}

void
sdl_init (void)
{
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        x_exit (SDL_GetError());
    }
    atexit (sdl_quit);
}

void
sdl_quit (void)
{
    //FIXME crashes!
    //visual_bin_destroy( v.bin );
    //visual_quit();

    SDL_FreeSurface (screen);
    SDL_Quit();
}

static inline void
sdl_set_pal(void)
{
    if (v.pal) {
        int i;
        for (i = 0; i < 256; i++) {
            pal[i].r = v.pal->colors[i].r;
            pal[i].g = v.pal->colors[i].g;
            pal[i].b = v.pal->colors[i].b;
        }
    }
    SDL_SetColors( screen, pal, 0, 256 );
}

static void
sdl_create (int width, int height) {
    SDL_FreeSurface (screen);

    if (v.pluginIsGL)
    {
        const SDL_VideoInfo *videoinfo = SDL_GetVideoInfo();
        int videoflags;

        if (!videoinfo) {
            x_exit ("Could not get video info.");
        }

        videoflags  = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE | SDL_RESIZABLE;
        videoflags |= videoinfo->hw_available ? SDL_HWSURFACE : SDL_SWSURFACE;
        if (videoinfo->blit_hw) videoflags |= SDL_HWACCEL;

        SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
        screen = SDL_SetVideoMode (width, height, 16, videoflags);
    } else {
        screen = SDL_SetVideoMode (width, height, v.video->bpp * 8, SDL_RESIZABLE);
    }
    visual_video_set_buffer (v.video, screen->pixels);
    visual_video_set_pitch (v.video, screen->pitch);
}

int
sdl_event_handler(void)
{
    SDL_Event event;
    VisEventQueue *vevent;

    while (SDL_PollEvent (&event)) {
        vevent = visual_plugin_get_eventqueue (visual_actor_get_plugin (visual_bin_get_actor (v.bin)));

        switch (event.type) {
        case SDL_KEYUP:
            visual_event_queue_add_keyboard (vevent, (VisKey)event.key.keysym.sym, event.key.keysym.mod, VISUAL_KEY_UP);
            break;

        case SDL_KEYDOWN:
            visual_event_queue_add_keyboard (vevent, (VisKey)event.key.keysym.sym, event.key.keysym.mod, VISUAL_KEY_DOWN);

            switch (event.key.keysym.sym) {
            //PLUGIN CONTROLS
            case SDLK_F11:
            case SDLK_TAB:
                sdl_toggleFullScreen ();
                break;

            case SDLK_ESCAPE:
                if (sdl_isFullScreen ()) {
                    sdl_toggleFullScreen ();
                } else {
                    return 0;
                }
                break;

            case SDLK_LEFT:
                v_cycleActor (1);
                goto morph;

            case SDLK_RIGHT:
                v_cycleActor (0);

            morph:
                sdl_lock();
                  visual_bin_set_morph_by_name (v.bin, (char*)"flash");
                  visual_bin_switch_actor_by_name (v.bin, (char*)v.plugin);
                sdl_unlock();

                SDL_WM_SetCaption (v.plugin, 0);

                break;

            default:
                ;
            }
            break;

        case SDL_VIDEORESIZE:
            v_resize (event.resize.w, event.resize.h);
            break;

        case SDL_MOUSEMOTION:
            visual_event_queue_add_mousemotion (vevent, event.motion.x, event.motion.y);
            break;

        case SDL_MOUSEBUTTONDOWN:
            /*if (event.button.button == SDL_BUTTON_RIGHT)
            {
                sdl_toggleFullScreen();
                break;
            }*/
            visual_event_queue_add_mousebutton (vevent, event.button.button, VISUAL_MOUSE_DOWN, 0, 0);
            break;

        case SDL_MOUSEBUTTONUP:
            visual_event_queue_add_mousebutton (vevent, event.button.button, VISUAL_MOUSE_UP, 0, 0);
            break;

        case SDL_QUIT:
            return 0;

        default:
            ;
        }
    }

    return 1;
}


/*
static int
v_upload_callback (VisInput* input, VisAudio *audio, void* unused)
{
    VisBuffer buf;

    visual_buffer_init( &buf, v.pcm_data, 1024, 0 );
    visual_audio_samplepool_input( audio->samplepool, &buf, VISUAL_AUDIO_SAMPLE_RATE_44100,
        VISUAL_AUDIO_SAMPLE_FORMAT_S16, VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO );

    return 0;
}
*/

void
v_resize( int width, int height )
{
    visual_video_set_dimension( v.video, width, height );

    sdl_create( width, height );

    visual_bin_sync( v.bin, 0 );
}

void
v_init (int argc, char **argv)
{
    VisVideoDepth depth;

    visual_log_set_verboseness (VISUAL_LOG_VERBOSENESS_HIGH);
    visual_init_path_add("/data/data/com.starlon.froyvisuals/lib");
    visual_log_set_info_handler (my_info_handler, NULL);
    visual_log_set_warning_handler (my_warning_handler, NULL);
    visual_log_set_critical_handler (my_critical_handler, NULL);
    visual_log_set_error_handler (my_error_handler, NULL);
    visual_init (&argc, &argv);
    visual_thread_enable(FALSE);

    v.bin    = visual_bin_new ();
    depth  = visual_video_depth_enum_from_value( 16 );

    if (!v.plugin) {
        puts ("Available plugins:");
        while ((v.plugin = visual_actor_get_next_by_name (v.plugin))) {
            printf (" * %s\n", v.plugin);
        }
        v.plugin = visual_actor_get_next_by_name (0);
    }
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

    v.plugin = "lv_scope";
    visual_bin_connect_by_names (v.bin, (char*)v.plugin, "alsa");

    if (visual_bin_get_depth (v.bin) == VISUAL_VIDEO_DEPTH_GL)
    {
        visual_video_set_depth (v.video, VISUAL_VIDEO_DEPTH_GL);
        v.pluginIsGL = 1;
    }

    sdl_create (256, 64);

    SDL_WM_SetCaption (v.plugin, 0);

    /* Called so the flag is set to false, seen we create the initial environment here */
    visual_bin_depth_changed (v.bin);

/*
    VisInput *input = visual_bin_get_input (v.bin);
    if (visual_input_set_callback (input, v_upload_callback, NULL) < 0) {
        x_exit ("Cannot set input plugin callback");
    }
*/

    visual_bin_switch_set_style (v.bin, VISUAL_SWITCH_STYLE_DIRECT);
    /*visual_bin_switch_set_automatic (v.bin, 1);
    visual_bin_switch_set_steps (v.bin, 10);*/

    visual_bin_realize (v.bin);
    visual_bin_sync (v.bin, 0);

    LOGI ("Libvisual version %s; bpp: %d %s\n", visual_get_version(), v.video->bpp, (v.pluginIsGL ? "(GL)\n" : ""));
}

uint
v_render(void)
{

    /* On depth change */
    if (visual_bin_depth_changed (v.bin)) {
        sdl_lock ();

        v.pluginIsGL = (visual_bin_get_depth (v.bin) == VISUAL_VIDEO_DEPTH_GL);

        sdl_create (screen->w, screen->h);
        visual_bin_sync (v.bin, 1);

        sdl_unlock ();
    }

    long ticks = -SDL_GetTicks ();

    if (v.pluginIsGL) {
        visual_bin_run (v.bin);
        SDL_GL_SwapBuffers ();
    } else {
        sdl_lock ();
        visual_video_set_buffer (v.video, screen->pixels);
        visual_bin_run (v.bin);
        sdl_unlock ();

        v.pal = visual_bin_get_palette (v.bin);
        sdl_set_pal ();
        SDL_Flip (screen);
    }


    ticks += SDL_GetTicks ();
    return ticks;
}

/*
   public static native void nativeInit();
    public static native void nativeQuit();
    public static native void onNativeResize(int x, int y, int format);
    public static native void onNativeKeyDown(int keycode);
    public static native void onNativeKeyUp(int keycode);
    public static native void onNativeTouch(int action, float x,
                                            float y, float p);
    public static native void onNativeAccel(float x, float y, float z);
    public static native void nativeRunAudioThread();
*/

/*
 * Class:     com_starlon_froyvisuals_SDLActivity
 * Method:    nativeInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_SDLActivity_nativeInit
  (JNIEnv *env, jclass class)
{
}

/*
 * Class:     com_starlon_froyvisuals_SDLActivity
 * Method:    nativeQuit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_SDLActivity_nativeQuit
  (JNIEnv *env, jclass class)
{

}

/*
 * Class:     com_starlon_froyvisuals_SDLActivity
 * Method:    onNativeResize
 * Signature: (III)V
 */
JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_SDLActivity_onNativeResize
  (JNIEnv *env, jclass class, jint w, jint h, jint format)
{
	//v_resize(w, h);
}


/*
 * Class:     com_starlon_froyvisuals_SDLActivity
 * Method:    onNativeKeyDown
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_SDLActivity_onNativeKeyDown
  (JNIEnv * env, jclass class, jint keycode)
{

}

/*
 * Class:     com_starlon_froyvisuals_SDLActivity
 * Method:    onNativeKeyUp
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_SDLActivity_onNativeKeyUp
  (JNIEnv *env, jclass class, jint keycode)
{
}

/*
 * Class:     com_starlon_froyvisuals_SDLActivity
 * Method:    onNativeTouch
 * Signature: (IFFF)V
 */
JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_SDLActivity_onNativeTouch
  (JNIEnv *env, jclass class, jint action, jfloat x, jfloat y, jfloat p)
{

}

/*
 * Class:     com_starlon_froyvisuals_SDLActivity
 * Method:    onNativeAccel
 * Signature: (FFF)V
 */
JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_SDLActivity_onNativeAccel
  (JNIEnv *env, jclass class, jfloat x, jfloat y, jfloat z)
{

}

/*
 * Class:     com_starlon_froyvisuals_SDLActivity
 * Method:    nativeRunAudioThread
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_starlon_froyvisuals_SDLActivity_nativeRunAudioThread
  (JNIEnv *env, jclass class)
{

}
