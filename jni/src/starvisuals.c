
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <jni.h>
#include <time.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <libvisual/libvisual.h>
#include <tinyalsa/asoundlib.h>
#include <asound.h>

#define DEVICE_DEPTH VISUAL_VIDEO_DEPTH_32BIT

#define  LOG_TAG    "FroyVisuals"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

// Initial plugins. Preferences should override these.
#define MORPH "alphablend"
#define ACTOR "jakdaw"
#define INPUT "dummy"

#define URL_GPLv2 "http://www.gnu.org/licenses/gpl-2.0.txt"
#define URL_GPLv3 "http://www.gnu.org/licenses/gpl-3.0.txt"
#define URL_LGPL "http://www.gnu.org/licenses/lgpl-3.0.txt"
#define URL_BSD "http://www.opensource.org/licenses/bsd-license.php"

struct {
    int16_t *pcm_data;
    int size;
    VisAudioSampleRateType rate;
    VisAudioSampleChannelType channels;
    VisAudioSampleFormatType encoding;
} pcm_ref;

/* LIBVISUAL */
struct {
    VisVideo   *video;
    VisPalette  *pal;
    VisBin     *bin;
    const char *actor_name;
    const char *morph_name;
    const char *input_name;
    int         pluginIsGL;
} v;

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
}

static void v_cycleMorph (int prev)
{
    v.morph_name = visual_morph_get_next_by_name(v.morph_name);
    if(!v.morph_name) {
        v.morph_name = visual_morph_get_next_by_name(0);
    }
}

static void v_cycleInput(int prev)
{
    v.input_name = visual_input_get_next_by_name(v.input_name);
    if(!v.input_name)
    {
        v.input_name = visual_input_get_next_by_name(0);
    }
}

static int v_upload_callback (VisInput* input, VisAudio *audio, void* unused)
{
    visual_log_return_val_if_fail(input != NULL, VISUAL_ERROR_GENERAL);
    visual_log_return_val_if_fail(audio != NULL, VISUAL_ERROR_GENERAL);
    visual_log_return_val_if_fail(pcm_ref.pcm_data != NULL, VISUAL_ERROR_GENERAL);

    VisBuffer buf;

    visual_buffer_init( &buf, pcm_ref.pcm_data, pcm_ref.size, 0 );
    visual_audio_samplepool_input( audio->samplepool, &buf, pcm_ref.rate, pcm_ref.encoding, pcm_ref.channels);

    return 0;
}


// ---------- INPUT ----------

// Get the VisInput at the requested index.
VisPluginRef *get_input(int index)
{
    VisList *list = visual_input_get_list();

    int count = visual_list_count(list);

    visual_log_return_val_if_fail(index < count, NULL);


    return visual_list_get(list, index);
}

int get_input_index()
{
    VisList *list = visual_input_get_list();
    int count = visual_list_count(list), i;
    for(i = 0; i < count; i++)
    {
        VisPluginRef *ref = visual_list_get(list, i);
        if(ref->info->plugname && !strcmp(v.input_name, ref->info->plugname))
            return i;
    }
    return -1;

}

void set_input()
{

    visual_object_unref(VISUAL_OBJECT(v.bin->input));
    VisInput *input = visual_input_new(v.input_name);
    visual_input_realize(input);
    visual_bin_set_input(v.bin, input);
    visual_bin_sync(v.bin, FALSE);

    if(strstr(v.input_name, "mic"))
    {
    	VisInput *input = visual_bin_get_input (v.bin);
    	if (visual_input_set_callback (input, v_upload_callback, NULL) < 0) {
    	    visual_log(VISUAL_LOG_CRITICAL, "Unable to set mic input callback.");	
    	}
    }


}

JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_cycleInput(JNIEnv *env, jobject obj, jint prev)
{
    v_cycleInput(prev);
    set_input();
    visual_log(VISUAL_LOG_DEBUG, "Just changed input to %s", v.input_name);
    return get_input_index();
}


// Get the count of available input plugins.
JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_inputCount(JNIEnv *env, jobject obj)
{
    return visual_list_count(visual_input_get_list());
}

// Get the index of the current plugin. 
// Note that this index may change as new plugins are added.
JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_inputGetCurrent(JNIEnv *env)
{
    return get_input_index();
}

// Set the current input plugin to that at the provided index.
// Note that this does not immediately cause the plugin to change.
// It only sets the name for when the plugin does change.
// This name could change between calling this function and an actual plugin change!
JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_inputSetCurrent(JNIEnv *env, jobject obj, jint index)
{
    VisList *list = visual_input_get_list();
    int count = visual_list_count(list);

    if(index >= count)
        return FALSE;
    
    VisPluginRef *ref = visual_list_get(list, index);

    v.input_name = ref->info->plugname;

    set_input();
    return TRUE;
}

// Set the current input plugin by its name. Do nothing and return false if the plugin doesn't exist.
JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_inputSetCurrentByName(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *input = (*env)->GetStringUTFChars(env, name, &iscopy);
    if(visual_input_valid_by_name(input))
    {
        v.input_name = input;
        set_input();
        return TRUE;
    }
    return FALSE;
}


// Get the input's plugin name.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_inputGetName(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_input(index);

    visual_log_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->plugname));
}

// Get the input's plugin longname.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_inputGetLongName(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_input(index);

    visual_log_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->name));
}

// Get the input's plugin author.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_inputGetAuthor(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_input(index);

    visual_log_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->author));
}

// Get the input's plugin version.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_inputGetVersion(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_input(index);

    visual_log_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->version));
}

// Get the input's plugin about string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_inputGetAbout(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_input(index);

    visual_log_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->about));
}

// Get the input's plugin help string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_inputGetHelp(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_input(index);

    visual_log_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->help));
}

// Get the input's plugin license string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_inputGetLicense(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_input(index);

    char text[256];

    visual_log_return_val_if_fail(ref != NULL, NULL);

    char *license = ref->info->license;

    visual_mem_set(text, 0, sizeof(text));

    if(strcmp(license, "GPLv2"))
        visual_mem_copy(text, URL_GPLv2, strlen(URL_GPLv2));
    else if(strcmp(license, "GPLv3"))
        visual_mem_copy(text, URL_GPLv3, strlen(URL_GPLv3));
    else if(strcmp(license, "LGPL"))
        visual_mem_copy(text, URL_LGPL, strlen(URL_LGPL));
    else if(strcmp(license, "BSD"))
        visual_mem_copy(text, URL_BSD, strlen(URL_BSD));

    return ((*env)->NewStringUTF(env, text));

}

VisParamEntry *get_input_param_entry(int index)
{
    VisParamContainer *cont = visual_plugin_get_params(visual_input_get_plugin(v.bin->input));

    visual_log_return_val_if_fail(cont != NULL, NULL);

    int count = visual_list_count(&cont->entries);

    visual_log_return_val_if_fail(index < count, NULL);

    VisParamEntry *entry = (VisParamEntry *)visual_list_get(&cont->entries, index);

    visual_log_return_val_if_fail(entry != NULL, NULL);

    return entry;
}

JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_inputParamGetCount(JNIEnv *env, jobject obj)
{
    VisParamContainer *cont = visual_plugin_get_params(visual_input_get_plugin(v.bin->input));

    visual_log_return_val_if_fail(cont != NULL, 0);

    int count = visual_list_count(&cont->entries);

    return count;
}

JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_inputParamGetType(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *chars = ((*env)->GetStringUTFChars(env, name, &iscopy));

    jstring string;

    VisParamContainer *cont = visual_plugin_get_params(visual_input_get_plugin(v.bin->input));

    VisParamEntry *entry = visual_param_container_get(cont, chars);

    visual_log_return_val_if_fail(entry != NULL, NULL);

    switch(entry->type)
    {
        case VISUAL_PARAM_ENTRY_TYPE_STRING:
            string = ((*env)->NewStringUTF(env, "string"));
            break;
        case VISUAL_PARAM_ENTRY_TYPE_INTEGER:
            string = ((*env)->NewStringUTF(env, "integer"));
            break;
        case VISUAL_PARAM_ENTRY_TYPE_FLOAT:
            string = ((*env)->NewStringUTF(env, "float"));
            break;
        case VISUAL_PARAM_ENTRY_TYPE_DOUBLE:
            string = ((*env)->NewStringUTF(env, "double"));
            break;
        default:
            return NULL;
            break;
    }

    return string;
}

JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_inputParamGetName(JNIEnv *env, jobject obj, jint index)
{
    VisParamEntry *entry = get_input_param_entry(index);
    
    jstring string = ((*env)->NewStringUTF(env, visual_param_entry_get_name(entry)));
    
    return string;
}

JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_inputParamGetString(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *chars = ((*env)->GetStringUTFChars(env, name, &iscopy));
    visual_log_return_val_if_fail(chars != NULL, NULL);

    VisParamContainer *cont = visual_plugin_get_params(visual_input_get_plugin(v.bin->input));

    VisParamEntry *entry = visual_param_container_get(cont, chars);

    jstring string = ((*env)->NewStringUTF(env, visual_param_entry_get_string(entry)));

    return string;
}

JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_inputParamSetString(JNIEnv *env, jobject obj, jstring name, jstring newstring)
{
    jboolean iscopy;
    const char *param_name = ((*env)->GetStringUTFChars(env, name, &iscopy));
    const char *new_string = ((*env)->GetStringUTFChars(env, newstring, &iscopy));

    visual_log_return_val_if_fail(param_name != NULL, FALSE);
    visual_log_return_val_if_fail(new_string != NULL, FALSE);

    VisParamContainer *cont = visual_plugin_get_params(visual_input_get_plugin(v.bin->input));

    VisParamEntry *entry = visual_param_container_get(cont, param_name);

    int ret = visual_param_entry_set_string(entry, (char *)new_string);

    return !ret;
}

JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_inputParamGetInteger(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *string = ((*env)->GetStringUTFChars(env, name, &iscopy));
    visual_log_return_val_if_fail(string != NULL, 0);

    VisParamContainer *cont = visual_plugin_get_params(visual_input_get_plugin(v.bin->input));

    VisParamEntry *entry = visual_param_container_get(cont, string);

    int val = visual_param_entry_get_integer(entry);

    return val;
}

JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_inputParamSetInteger(JNIEnv *env, jobject obj, jstring name, jint newint)
{
    jboolean iscopy;
    const char *param_name = ((*env)->GetStringUTFChars(env, name, &iscopy));
    int new_int = newint;

    visual_log_return_val_if_fail(param_name != NULL, FALSE);

    VisParamContainer *cont = visual_plugin_get_params(visual_input_get_plugin(v.bin->input));

    VisParamEntry *entry = visual_param_container_get(cont, param_name);

    int ret = visual_param_entry_set_integer(entry, new_int);

    return !ret;
}

JNIEXPORT jfloat JNICALL Java_com_starlon_starvisuals_NativeHelper_inputParamGetFloat(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *string = ((*env)->GetStringUTFChars(env, name, &iscopy));
    visual_log_return_val_if_fail(string != NULL, 0.0f);

    VisParamContainer *cont = visual_plugin_get_params(visual_input_get_plugin(v.bin->input));

    VisParamEntry *entry = visual_param_container_get(cont, string);

    float val = visual_param_entry_get_float(entry);

    return val;
}

JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_inputParamSetFloat(JNIEnv *env, jobject obj, jstring name, jfloat newfloat)
{
    jboolean iscopy;
    const char *param_name = ((*env)->GetStringUTFChars(env, name, &iscopy));
    float new_float = newfloat;

    visual_log_return_val_if_fail(param_name != NULL, FALSE);

    VisParamContainer *cont = visual_plugin_get_params(visual_input_get_plugin(v.bin->input));

    VisParamEntry *entry = visual_param_container_get(cont, param_name);

    int ret = visual_param_entry_set_float(entry, new_float);

    return !ret;
}

JNIEXPORT jdouble JNICALL Java_com_starlon_starvisuals_NativeHelper_inputParamGetDouble(JNIEnv *env, jobject obj, jstring name)
{    
    jboolean iscopy;
    const char *string = ((*env)->GetStringUTFChars(env, name, &iscopy));

    visual_log_return_val_if_fail(string != NULL, 0.0);

    VisParamContainer *cont = visual_plugin_get_params(visual_input_get_plugin(v.bin->input));

    VisParamEntry *entry = visual_param_container_get(cont, string);

    double val = visual_param_entry_get_double(entry);

    return val;
}

JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_inputParamSetDouble(JNIEnv *env, jobject obj, jstring name, jdouble newdouble)
{
    jboolean iscopy;
    const char *param_name = ((*env)->GetStringUTFChars(env, name, &iscopy));
    double new_double = newdouble;

    visual_log_return_val_if_fail(param_name != NULL, FALSE);

    VisParamContainer *cont = visual_plugin_get_params(visual_input_get_plugin(v.bin->input));

    VisParamEntry *entry = visual_param_container_get(cont, param_name);

    int ret = visual_param_entry_set_double(entry, new_double);

    return !ret;
}



// ------ MORPH ------

// Get the VisMorph at the requested index.
VisPluginRef *get_morph(int index)
{
    VisList *list = visual_morph_get_list();

    int count = visual_list_count(list);

    visual_log_return_val_if_fail(index >= 0 && index < count, NULL);

    return visual_list_get(list, index);
}

int get_morph_index()
{
    VisList *list = visual_morph_get_list();
    int count = visual_list_count(list), i;
    for(i = 0; i < count; i++)
    {
        VisPluginRef *ref = visual_list_get(list, i);
        if(ref->info->plugname && !strcmp(v.morph_name, ref->info->plugname))
            return i;
    }
    return -1;

}

JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_cycleMorph(JNIEnv *env, jobject obj, jint prev)
{
    v_cycleMorph(prev);
    visual_bin_set_morph_by_name(v.bin, (char *)v.morph_name);
    return get_morph_index();
}


// Get the count of available morph plugins.
JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_morphCount(JNIEnv *env, jobject obj)
{
    return visual_list_count(visual_morph_get_list());
}

// Get the index of the current plugin. 
// Note that this index may change as new plugins are added.
JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_morphGetCurrent(JNIEnv *env)
{
    VisMorph *morph = visual_bin_get_morph(v.bin);
    if(morph)
        return morph->plugin->ref->index;
    return -1;
}

// Set the current morph plugin to that at the provided index.
// Note that this does not immediately cause the plugin to change.
// It only sets the name for when the plugin does change.
// This name could change between calling this function and an actual plugin change!
JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_morphSetCurrent(JNIEnv *env, jobject obj, jint index)
{
    VisList *list = visual_morph_get_list();

    int count = visual_list_count(list);

    if(index >= count)
        return FALSE;
    
    VisPluginRef *ref = visual_list_get(list, index);

    v.morph_name = ref->info->plugname;

    return TRUE;
}

// Set the current morph by name. use finalizeSwitch() to apply this change.
JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_morphSetCurrentByName(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *morph = (*env)->GetStringUTFChars(env, name, &iscopy);
    if(visual_morph_valid_by_name(morph))
    {
        v.morph_name = morph;
        return TRUE;
    }
    return FALSE;
}

// Get the morph plugin's name string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_morphGetName(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_morph(index);

    visual_log_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->plugname));
}

// Get the morph plugin's long name string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_morphGetLongName(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_morph(index);

    visual_log_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->name));
}

// Get the morph plugin's author string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_morphGetAuthor(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_morph(index);

    visual_log_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->author));
}

// Get the morph plugin's version string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_morphGetVersion(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_morph(index);

    visual_log_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->version));

}

// Get the morph plugin's about string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_morphGetAbout(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_morph(index);

    visual_log_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->about));
}

// Get the morph plugin's help string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_morphGetHelp(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_morph(index);

    visual_log_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->help));
}

// Get the morph plugin's license string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_morphGetLicense(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_morph(index);

    char text[256];

    visual_log_return_val_if_fail(ref != NULL, NULL);

    char *license = ref->info->license;

    visual_mem_set(text, 0, sizeof(text));

    if(strcmp(license, "GPLv2"))
        visual_mem_copy(text, URL_GPLv2, strlen(URL_GPLv2));
    else if(strcmp(license, "GPLv3"))
        visual_mem_copy(text, URL_GPLv3, strlen(URL_GPLv3));
    else if(strcmp(license, "LGPL"))
        visual_mem_copy(text, URL_LGPL, strlen(URL_LGPL));
    else if(strcmp(license, "BSD"))
        visual_mem_copy(text, URL_BSD, strlen(URL_BSD));

    return ((*env)->NewStringUTF(env, text));

}

VisParamEntry *get_morph_param_entry(int index)
{
    VisParamContainer *cont = visual_plugin_get_params(visual_morph_get_plugin(v.bin->morph));

    visual_log_return_val_if_fail(cont != NULL, NULL);

    int count = visual_list_count(&cont->entries);

    visual_log_return_val_if_fail(index < count, NULL);

    VisParamEntry *entry = (VisParamEntry *)visual_list_get(&cont->entries, index);

    visual_log_return_val_if_fail(entry != NULL, NULL);

    return entry;
}

JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_morphParamGetCount(JNIEnv *env, jobject obj)
{
    VisParamContainer *cont = visual_plugin_get_params(visual_morph_get_plugin(v.bin->morph));

    visual_log_return_val_if_fail(cont != NULL, 0);

    int count = visual_list_count(&cont->entries);

    return count;
}

JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_morphParamGetName(JNIEnv *env, jobject obj, jint index)
{
    VisParamEntry *entry = get_morph_param_entry(index);
    
    jstring string = ((*env)->NewStringUTF(env, visual_param_entry_get_name(entry)));
    
    return string;
}

JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_morphParamGetType(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *chars = ((*env)->GetStringUTFChars(env, name, &iscopy));

    jstring string;

    VisParamContainer *cont = visual_plugin_get_params(visual_morph_get_plugin(v.bin->morph));

    VisParamEntry *entry = visual_param_container_get(cont, chars);

    visual_log_return_val_if_fail(entry != NULL, NULL);

    switch(entry->type)
    {
        case VISUAL_PARAM_ENTRY_TYPE_STRING:
            string = ((*env)->NewStringUTF(env, "string"));
            break;
        case VISUAL_PARAM_ENTRY_TYPE_INTEGER:
            string = ((*env)->NewStringUTF(env, "integer"));
            break;
        case VISUAL_PARAM_ENTRY_TYPE_FLOAT:
            string = ((*env)->NewStringUTF(env, "float"));
            break;
        case VISUAL_PARAM_ENTRY_TYPE_DOUBLE:
            string = ((*env)->NewStringUTF(env, "double"));
            break;
        default:
            return NULL;
            break;
    }

    return string;
}


JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_morphParamGetString(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *chars = ((*env)->GetStringUTFChars(env, name, &iscopy));
    visual_log_return_val_if_fail(chars != NULL, NULL);

    VisParamContainer *cont = visual_plugin_get_params(visual_morph_get_plugin(v.bin->morph));

    VisParamEntry *entry = visual_param_container_get(cont, chars);

    jstring string = ((*env)->NewStringUTF(env, visual_param_entry_get_string(entry)));

    return string;
}

JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_morphParamSetString(JNIEnv *env, jobject obj, jstring name, jstring newstring)
{
    jboolean iscopy;
    const char *param_name = ((*env)->GetStringUTFChars(env, name, &iscopy));
    const char *new_string = ((*env)->GetStringUTFChars(env, newstring, &iscopy));

    visual_log_return_val_if_fail(param_name != NULL, FALSE);
    visual_log_return_val_if_fail(new_string != NULL, FALSE);

    VisParamContainer *cont = visual_plugin_get_params(visual_morph_get_plugin(v.bin->morph));

    VisParamEntry *entry = visual_param_container_get(cont, param_name);

    int ret = visual_param_entry_set_string(entry, (char *)new_string);

    return !ret;
}

JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_morphParamGetInteger(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *string = ((*env)->GetStringUTFChars(env, name, &iscopy));
    visual_log_return_val_if_fail(string != NULL, 0);

    VisParamContainer *cont = visual_plugin_get_params(visual_morph_get_plugin(v.bin->morph));

    VisParamEntry *entry = visual_param_container_get(cont, string);

    int val = visual_param_entry_get_integer(entry);

    return val;
}

JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_morphParamSetInteger(JNIEnv *env, jobject obj, jstring name, jint newint)
{
    jboolean iscopy;
    const char *param_name = ((*env)->GetStringUTFChars(env, name, &iscopy));
    int new_int = newint;

    visual_log_return_val_if_fail(param_name != NULL, FALSE);

    VisParamContainer *cont = visual_plugin_get_params(visual_morph_get_plugin(v.bin->morph));

    VisParamEntry *entry = visual_param_container_get(cont, param_name);

    int ret = visual_param_entry_set_integer(entry, new_int);

    return !ret;
}

JNIEXPORT jfloat JNICALL Java_com_starlon_starvisuals_NativeHelper_morphParamGetFloat(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *string = ((*env)->GetStringUTFChars(env, name, &iscopy));
    visual_log_return_val_if_fail(string != NULL, 0.0f);

    VisParamContainer *cont = visual_plugin_get_params(visual_morph_get_plugin(v.bin->morph));

    VisParamEntry *entry = visual_param_container_get(cont, string);

    float val = visual_param_entry_get_float(entry);

    return val;
}

JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_morphParamSetFloat(JNIEnv *env, jobject obj, jstring name, jfloat newfloat)
{
    jboolean iscopy;
    const char *param_name = ((*env)->GetStringUTFChars(env, name, &iscopy));
    float new_float = newfloat;

    visual_log_return_val_if_fail(param_name != NULL, FALSE);

    VisParamContainer *cont = visual_plugin_get_params(visual_morph_get_plugin(v.bin->morph));

    VisParamEntry *entry = visual_param_container_get(cont, param_name);

    int ret = visual_param_entry_set_float(entry, new_float);

    return !ret;
}

JNIEXPORT jdouble JNICALL Java_com_starlon_starvisuals_NativeHelper_morphParamGetDouble(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *string = ((*env)->GetStringUTFChars(env, name, &iscopy));
    visual_log_return_val_if_fail(string != NULL, 0.0);

    VisParamContainer *cont = visual_plugin_get_params(visual_morph_get_plugin(v.bin->morph));

    VisParamEntry *entry = visual_param_container_get(cont, string);

    double val = visual_param_entry_get_double(entry);

    return val;
}

JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_morphParamSetDouble(JNIEnv *env, jobject obj, jstring name, jdouble newdouble)
{
    jboolean iscopy;
    const char *param_name = ((*env)->GetStringUTFChars(env, name, &iscopy));
    double new_double = newdouble;

    visual_log_return_val_if_fail(param_name != NULL, FALSE);

    VisParamContainer *cont = visual_plugin_get_params(visual_morph_get_plugin(v.bin->morph));

    VisParamEntry *entry = visual_param_container_get(cont, param_name);

    int ret = visual_param_entry_set_double(entry, new_double);

    return !ret;
}

// ------ ACTORS ------

// Get the VisActor at the requested index.
VisPluginRef *get_actor(int index)
{
    VisList *list = visual_actor_get_list();
    VisPluginRef *ref;

    int count = visual_list_count(list);

    visual_log_return_val_if_fail(index >= 0 && index < count, NULL);

    ref = visual_list_get(list, index);

    return ref;
}

int get_actor_index()
{
    VisList *list = visual_actor_get_list();
    int count = visual_list_count(list), i;
    for(i = 0; i < count; i++)
    {
        VisPluginRef *ref = visual_list_get(list, i);
        if(ref->info->plugname && !strcmp(v.actor_name, ref->info->plugname))
            return i;
    }
    return -1;

}

JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_cycleActor(JNIEnv *env, jobject obj, jint prev)
{
    v_cycleActor(prev);
    visual_bin_switch_actor_by_name(v.bin, (char *)v.actor_name);
    return get_actor_index();
}

// Get the count of available actor plugins.
JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_actorCount(JNIEnv *env, jobject obj)
{
    return visual_list_count(visual_actor_get_list());
}

// Get the index of the current plugin. 
// Note that this index may change as new plugins are added.
JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_actorGetCurrent(JNIEnv *env)
{
    return get_actor_index();
    /*
    VisList *list = visual_actor_get_list();
    int count = visual_list_count(list), i;
    for(i = 0; i < count; i++)
    {
        VisPluginRef *ref = visual_list_get(list, i);
        if(ref->info->plugname && !strcmp(v.actor_name, ref->info->plugname))
            return i;
    }
    return -1;
    */
}

// Set the current actor plugin to that at the provided index.
// Note that this does not immediately cause the plugin to change.
// It only sets the name for when the plugin does change.
// This name could change between calling this function and an actual plugin change!
JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_actorSetCurrent(JNIEnv *env, jobject obj, jint index)
{
    VisList *list = visual_actor_get_list();
    int count = visual_list_count(list);

    if(index >= count)
        return FALSE;
    
    VisPluginRef *ref = visual_list_get(list, index);

    v.actor_name = ref->info->plugname;

    return TRUE;
}

// Set the current actor by its name.
JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_actorSetCurrentByName(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *actor = (*env)->GetStringUTFChars(env, name, &iscopy);
    if(visual_actor_valid_by_name(actor))
    {
        v.actor_name = actor;

        return TRUE;
    }
    return FALSE;
}


// Get the actor's plugin name.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_actorGetName(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_actor(index);

    visual_log_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->plugname));
}

// Get the actor's long name.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_actorGetLongName(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_actor(index);
    const char *name;

    visual_log_return_val_if_fail(ref != NULL, NULL);
    name = ref->info->name;

    return ((*env)->NewStringUTF(env, (char *)name));
}

// Get the actor's author.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_actorGetAuthor(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_actor(index);

    visual_log_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->author));
}

// Get the actor's version string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_actorGetVersion(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_actor(index);

    visual_log_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->version));
}

// Get the actor's about string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_actorGetAbout(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_actor(index);

    visual_log_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->about));
}

// Get the actor's help string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_actorGetHelp(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_actor(index);

    visual_log_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->help));
}

// Get the actor's license string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_actorGetLicense(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_actor(index);
    char text[256];

    visual_log_return_val_if_fail(ref != NULL, NULL);

    char *license = ref->info->license;

    visual_mem_set(text, 0, sizeof(text));

    if(strcmp(license, "GPLv2"))
        visual_mem_copy(text, URL_GPLv2, strlen(URL_GPLv2));
    else if(strcmp(license, "GPLv3"))
        visual_mem_copy(text, URL_GPLv3, strlen(URL_GPLv3));
    else if(strcmp(license, "LGPL"))
        visual_mem_copy(text, URL_LGPL, strlen(URL_LGPL));
    else if(strcmp(license, "BSD"))
        visual_mem_copy(text, URL_BSD, strlen(URL_BSD));

    return ((*env)->NewStringUTF(env, text));
}

JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_actorGetParamsCount(JNIEnv *env, jobject obj)
{
    VisParamContainer *params = visual_plugin_get_params(visual_actor_get_plugin(v.bin->actor));

    int count = visual_list_count(&params->entries);
    
    return count;
}

VisParamEntry *get_actor_param_entry(int index)
{
    VisParamContainer *cont = visual_plugin_get_params(visual_actor_get_plugin(v.bin->actor));

    visual_log_return_val_if_fail(cont != NULL, NULL);

    int count = visual_list_count(&cont->entries);

    visual_log_return_val_if_fail(index < count, NULL);

    VisParamEntry *entry = (VisParamEntry *)visual_list_get(&cont->entries, index);

    visual_log_return_val_if_fail(entry != NULL, NULL);

    return entry;
}

JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_actorParamGetCount(JNIEnv *env, jobject obj)
{
    VisParamContainer *cont = visual_plugin_get_params(visual_actor_get_plugin(v.bin->actor));

    visual_log_return_val_if_fail(cont != NULL, 0);

    int count = visual_list_count(&cont->entries);

    return count;
}

JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_actorParamGetType(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *chars = ((*env)->GetStringUTFChars(env, name, &iscopy));

    jstring string;

    VisParamContainer *cont = visual_plugin_get_params(visual_actor_get_plugin(v.bin->actor));

    VisParamEntry *entry = visual_param_container_get(cont, chars);

    visual_log_return_val_if_fail(entry != NULL, NULL);

    switch(entry->type)
    {
        case VISUAL_PARAM_ENTRY_TYPE_STRING:
            string = ((*env)->NewStringUTF(env, "string"));
            break;
        case VISUAL_PARAM_ENTRY_TYPE_INTEGER:
            string = ((*env)->NewStringUTF(env, "integer"));
            break;
        case VISUAL_PARAM_ENTRY_TYPE_FLOAT:
            string = ((*env)->NewStringUTF(env, "float"));
            break;
        case VISUAL_PARAM_ENTRY_TYPE_DOUBLE:
            string = ((*env)->NewStringUTF(env, "double"));
            break;
        default:
            return NULL;
            break;
    }

    return string;
}

JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_actorParamGetName(JNIEnv *env, jobject obj, jint index)
{
    VisParamEntry *entry = get_actor_param_entry(index);
    
    jstring string = ((*env)->NewStringUTF(env, visual_param_entry_get_name(entry)));
    
    return string;
}

JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_actorParamGetString(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *chars = ((*env)->GetStringUTFChars(env, name, &iscopy));
    visual_log_return_val_if_fail(chars != NULL, NULL);

    VisParamContainer *cont = visual_plugin_get_params(visual_actor_get_plugin(v.bin->actor));

    VisParamEntry *entry = visual_param_container_get(cont, chars);

    jstring string = ((*env)->NewStringUTF(env, visual_param_entry_get_string(entry)));

    return string;
}

JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_actorParamSetString(JNIEnv *env, jobject obj, jstring name, jstring newstring)
{
    jboolean iscopy;
    const char *param_name = ((*env)->GetStringUTFChars(env, name, &iscopy));
    const char *new_string = ((*env)->GetStringUTFChars(env, newstring, &iscopy));

    visual_log_return_val_if_fail(param_name != NULL, FALSE);
    visual_log_return_val_if_fail(new_string != NULL, FALSE);

    VisParamContainer *cont = visual_plugin_get_params(visual_actor_get_plugin(v.bin->actor));

    VisParamEntry *entry = visual_param_container_get(cont, param_name);

    int ret = visual_param_entry_set_string(entry, (char *)new_string);

    return !ret;
}


JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_actorParamGetInteger(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *string = ((*env)->GetStringUTFChars(env, name, &iscopy));
    visual_log_return_val_if_fail(string != NULL, 0);

    VisParamContainer *cont = visual_plugin_get_params(visual_actor_get_plugin(v.bin->actor));

    VisParamEntry *entry = visual_param_container_get(cont, string);

    int val = visual_param_entry_get_integer(entry);

    return val;
}

JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_actorParamSetInteger(JNIEnv *env, jobject obj, jstring name, jint newint)
{
    jboolean iscopy;
    const char *param_name = ((*env)->GetStringUTFChars(env, name, &iscopy));
    int new_int = newint;

    visual_log_return_val_if_fail(param_name != NULL, FALSE);

    VisParamContainer *cont = visual_plugin_get_params(visual_actor_get_plugin(v.bin->actor));

    VisParamEntry *entry = visual_param_container_get(cont, param_name);

    int ret = visual_param_entry_set_integer(entry, new_int);

    return !ret;
}

JNIEXPORT jfloat JNICALL Java_com_starlon_starvisuals_NativeHelper_actorParamGetFloat(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *string = ((*env)->GetStringUTFChars(env, name, &iscopy));
    visual_log_return_val_if_fail(string != NULL, 0.0f);

    VisParamContainer *cont = visual_plugin_get_params(visual_actor_get_plugin(v.bin->actor));

    VisParamEntry *entry = visual_param_container_get(cont, string);

    float val = visual_param_entry_get_float(entry);

    return val;
}

JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_actorParamSetFloat(JNIEnv *env, jobject obj, jstring name, jfloat newfloat)
{
    jboolean iscopy;
    const char *param_name = ((*env)->GetStringUTFChars(env, name, &iscopy));
    float new_float = newfloat;

    visual_log_return_val_if_fail(param_name != NULL, FALSE);

    VisParamContainer *cont = visual_plugin_get_params(visual_actor_get_plugin(v.bin->actor));

    VisParamEntry *entry = visual_param_container_get(cont, param_name);

    float ret = visual_param_entry_set_float(entry, new_float);

    return !ret;
}

JNIEXPORT jdouble JNICALL Java_com_starlon_starvisuals_NativeHelper_actorParamGetDouble(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *string = ((*env)->GetStringUTFChars(env, name, &iscopy));
    visual_log_return_val_if_fail(string != NULL, 0.0);

    VisParamContainer *cont = visual_plugin_get_params(visual_actor_get_plugin(v.bin->actor));

    VisParamEntry *entry = visual_param_container_get(cont, string);

    double val = visual_param_entry_get_double(entry);

    return val;
}

JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_actorParamSetDouble(JNIEnv *env, jobject obj, jstring name, jdouble newdouble)
{
    jboolean iscopy;
    const char *param_name = ((*env)->GetStringUTFChars(env, name, &iscopy));
    double new_double = newdouble;

    visual_log_return_val_if_fail(param_name != NULL, FALSE);

    VisParamContainer *cont = visual_plugin_get_params(visual_actor_get_plugin(v.bin->actor));

    VisParamEntry *entry = visual_param_container_get(cont, param_name);

    double ret = visual_param_entry_set_double(entry, new_double);

    return !ret;
}



/* End of plugin and parameter getters and setters. */



// For fallback audio source.
JNIEXPORT void JNICALL Java_com_starlon_starvisuals_NativeHelper_uploadAudio(JNIEnv * env, jobject  obj, jshortArray data)
{
    int i;
    jshort *pcm;
    jsize len = (*env)->GetArrayLength(env, data);
    pcm = (*env)->GetShortArrayElements(env, data, NULL);
    for(i = 0; i < len && i < pcm_ref.size; i++)
    {
        pcm_ref.pcm_data[i] = pcm[i];
    }
    (*env)->ReleaseShortArrayElements(env, data, pcm, 0);
}

// Reinitialize audio fields.
JNIEXPORT void JNICALL Java_com_starlon_starvisuals_NativeHelper_resizePCM(jint size, jint samplerate, jint channels, jint encoding)
{
    if(pcm_ref.pcm_data != NULL)
        visual_mem_free(pcm_ref.pcm_data);
    pcm_ref.pcm_data = visual_mem_malloc(sizeof(int16_t) * size);
    pcm_ref.size = size;
    switch(samplerate)
    {
        case 8000:
            pcm_ref.rate = VISUAL_AUDIO_SAMPLE_RATE_8000;
        break;
        case 11250:
            pcm_ref.rate = VISUAL_AUDIO_SAMPLE_RATE_11250;
        break;
        case 22500:
            pcm_ref.rate = VISUAL_AUDIO_SAMPLE_RATE_22500;
        break;
        case 32000:
            pcm_ref.rate = VISUAL_AUDIO_SAMPLE_RATE_32000;
        break;
        case 44100:
            pcm_ref.rate = VISUAL_AUDIO_SAMPLE_RATE_44100;
        break;
        case 48000:
            pcm_ref.rate = VISUAL_AUDIO_SAMPLE_RATE_48000;
        break;
        case 96000:
            pcm_ref.rate = VISUAL_AUDIO_SAMPLE_RATE_96000;
        break;
    }

    // It seems lv only supports stereo? hmm
    pcm_ref.channels = VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO;

    switch(encoding)
    {
        case sizeof(int16_t):
            pcm_ref.encoding = VISUAL_AUDIO_SAMPLE_FORMAT_S16;
            break;
        case sizeof(int8_t):
            pcm_ref.encoding = VISUAL_AUDIO_SAMPLE_FORMAT_S8;
            break;
    }
}

// Increment or decrement actor and morph
// Variable 'prev' is used to shift morph plugin around. 
// 0=left, 1=right, 2=up, 3=down, 4=cycle.. Any other and the current value is used.
JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_finalizeSwitch(JNIEnv * env, jobject  obj, jint prev)
{

    VisMorph *bin_morph = visual_bin_get_morph(v.bin);
    const char *morph = v.morph_name;

    visual_log(VISUAL_LOG_INFO, "Switching actors %s -> %s", morph, v.morph_name);
    
    if(bin_morph && !visual_morph_is_done(bin_morph))
        return FALSE;



    if(prev == 0) {
        v.morph_name = "slide_left";
    }
    else if(prev == 1)
    {
        v.morph_name = "slide_right";
    }
    else if(prev == 2)
    {
        v.morph_name = "slide_up";
    }
    else if(prev == 3)
    {
        v.morph_name = "slide_down";
    } 
    else if(prev == -1)
    {
        v_cycleMorph((int)prev);
    }

    visual_bin_set_morph_by_name (v.bin, (char *)v.morph_name);

    v_cycleActor((int)prev);
    visual_bin_switch_actor_by_name(v.bin, (char *)v.actor_name);

    return TRUE;
}

// Set the VisBin's plugins. This causes the actor to change immediately.
JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_updatePlugins(JNIEnv * env, jobject  obj)
{
    VisMorph *bin_morph = visual_bin_get_morph(v.bin);
    VisInput *input;


    // Make sure the input and actor names are valid.
    visual_log_return_val_if_fail(visual_actor_valid_by_name((char *)v.actor_name), FALSE);
    visual_log_return_val_if_fail(visual_input_valid_by_name((char *)v.input_name), FALSE);

    // If this we're set to morph between actors, then set the new morph.
    // Fail if the last morph is not done!
    // Make sure the morph name is valid.
    if(v.bin->morphstyle == VISUAL_SWITCH_STYLE_MORPH && bin_morph)
    {
        visual_log_return_val_if_fail(visual_morph_is_done(bin_morph), FALSE);
        visual_log_return_val_if_fail(visual_morph_valid_by_name((char *)v.morph_name), FALSE);

        visual_bin_set_morph_by_name(v.bin, (char *)v.morph_name);
    }

    // Clean up old input plugin.
    input = visual_bin_get_input(v.bin);
    visual_object_unref(VISUAL_OBJECT(input));

    // Set the new input plugin.
    set_input();
    //input = visual_input_new((char *)v.input_name);
    //visual_bin_set_input(v.bin, input);

    // Tell lv to switch to specified actor plugin.
    visual_bin_switch_actor_by_name(v.bin, (char *)v.actor_name);

    // Level the playing field.
    visual_bin_realize(v.bin);

    // Sync VisBin without events.
    visual_bin_sync(v.bin, FALSE);

    return TRUE;
}

// Set the VisBin's morph style -- to morph or not to morph.
JNIEXPORT void JNICALL Java_com_starlon_starvisuals_NativeHelper_setMorphStyle(JNIEnv * env, jobject  obj, jboolean morph)
{
    if(morph)
        visual_bin_switch_set_style(v.bin, VISUAL_SWITCH_STYLE_MORPH);
    else
        visual_bin_switch_set_style(v.bin, VISUAL_SWITCH_STYLE_DIRECT);
}

// Pump mouse motion events to the current actor.
JNIEXPORT void JNICALL Java_com_starlon_starvisuals_NativeHelper_mouseMotion(JNIEnv * env, jobject  obj, jfloat x, jfloat y)
{
    visual_log(VISUAL_LOG_INFO, "Mouse motion: x %f, y %f", x, y);
    VisPluginData *plugin = visual_actor_get_plugin(visual_bin_get_actor(v.bin));
    VisEventQueue *eventqueue = visual_plugin_get_eventqueue(plugin);
    visual_event_queue_add_mousemotion(eventqueue, x, y);
}

// Pump mouse button events to the current actor.
JNIEXPORT void JNICALL Java_com_starlon_starvisuals_NativeHelper_mouseButton(JNIEnv * env, jobject  obj, jint button, jfloat x, jfloat y)
{
    visual_log(VISUAL_LOG_INFO, "Mouse button: button %d, x %f, y %f", button, x, y);
    VisPluginData *plugin = visual_actor_get_plugin(visual_bin_get_actor(v.bin));
    VisEventQueue *eventqueue = visual_plugin_get_eventqueue(plugin);
        VisMouseState state = VISUAL_MOUSE_DOWN;
    visual_event_queue_add_mousebutton(eventqueue, button, state, x, y);
}


// Pump resize events to the current actor.
JNIEXPORT void JNICALL Java_com_starlon_starvisuals_NativeHelper_screenResize(JNIEnv * env, jobject  obj, jint w, jint h)
{
    visual_log(VISUAL_LOG_INFO, "Screen resize w %d h %d", w, h);

    VisPluginData *plugin = visual_actor_get_plugin(visual_bin_get_actor(v.bin));
    VisEventQueue *eventqueue = visual_plugin_get_eventqueue(plugin);
    visual_event_queue_add_resize(eventqueue, v.video, w, h);
}

// Pump keyboard events to the current actor.
JNIEXPORT void JNICALL Java_com_starlon_starvisuals_NativeHelper_keyboardEvent(JNIEnv * env, jobject  obj, jint x, jint y)
{
    VisEventQueue *eventqueue = visual_plugin_get_eventqueue(visual_actor_get_plugin(visual_bin_get_actor(v.bin)));
    VisKey keysym = 0;
    int keymod = 0;
    VisKeyState state = 0;
    visual_event_queue_add_keyboard(eventqueue, keysym, keymod, state);
}

    
JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_isBeat(JNIEnv *env, jobject obj)
{
    VisParamContainer *paramcontainer = visual_plugin_get_params(v.bin->input->plugin);
    VisParamEntry *entry = visual_param_container_get(paramcontainer, "isBeat");

    if(entry != NULL)
        return visual_param_entry_get_integer(entry);

    return FALSE;
}

JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_getBPM(JNIEnv *env, jobject obj)
{
    VisBeat *beat = visual_audio_get_beat(v.bin->input->audio);
    return beat->bpm;
}

JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_getBPMConfidence(JNIEnv *env, jobject obj)
{
    VisBeat *beat = visual_audio_get_beat(v.bin->input->audio);
    return beat->confidence;
}


JNIEXPORT void JNICALL Java_com_starlon_starvisuals_NativeHelper_newSong(JNIEnv *env, jobject obj)
{
    VisBeat *beat = visual_audio_get_beat(v.bin->input->audio);
    visual_beat_change_song(beat);
}


// Is this even needed? What happens when the app is quietly discarded?
// Seems in Android 4.0 you can kill an app by swiping it.
JNIEXPORT void JNICALL Java_com_starlon_starvisuals_NativeHelper_visualsQuit(JNIEnv * env, jobject  obj)
{
    if(!visual_is_initialized())
        return;
    visual_video_free_buffer(v.video);
    visual_object_unref(VISUAL_OBJECT(v.video));
    visual_object_unref(VISUAL_OBJECT(v.bin));
    visual_quit();
    exit(0);
}

void app_main(int w, int h, int device, int card)
{

    int depthflag;
    VisVideoDepth depth;

    if(!visual_is_initialized())
    {
        //setenv("LVSHOWBEATS", "1", 1);
        visual_init_path_add("/data/data/com.starlon.starvisuals/lib");
        visual_log_set_verboseness (VISUAL_LOG_VERBOSENESS_HIGH);
        visual_log_set_info_handler (my_info_handler, NULL);
        visual_log_set_warning_handler (my_warning_handler, NULL);
        visual_log_set_critical_handler (my_critical_handler, NULL);
        visual_log_set_error_handler (my_error_handler, NULL);
    
        visual_init (0, NULL);
        memset(&v, 0, sizeof(v));
        memset(&pcm_ref, 0, sizeof(pcm_ref));

        v.morph_name = MORPH;
        v.actor_name = ACTOR;
        v.input_name = INPUT;

        // Check alsa device permissions.

        // Check for read permissions.
        int card = 0, device = 0;
        char fn[256];

        snprintf(fn, sizeof(fn), "/dev/snd/pcmC%uD%u%c", card, device, 'c');

        if(access(fn, R_OK) != 0)
        {
            //FIXME rigid calls this evil 
            //if(chmod(fn, S_IROTH) != 0)
                goto exit_alsa_check;
        }
        
        struct pcm *pcmstream;
        struct pcm_config config;
        config.channels = 1;
        config.rate = 44100;
        config.period_count = 4;
        config.period_size = 1024;
        config.format = PCM_FORMAT_S16_LE;
        config.stop_threshold = 0;
        pcmstream = pcm_open(device, card, PCM_IN, &config);

        if(!pcmstream) {
            goto exit_alsa_check;
        }

        pcm_close(pcmstream);

        v.input_name = "alsa";

        visual_log(VISUAL_LOG_INFO, "Choosing ALSA input plugin. Go loud.");
    } else {
        visual_video_free_buffer(v.video);
        visual_object_unref(VISUAL_OBJECT(v.video));
        visual_object_unref(VISUAL_OBJECT(v.bin));
    }

exit_alsa_check:
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

    visual_bin_switch_set_style (v.bin, VISUAL_SWITCH_STYLE_MORPH);
    visual_bin_switch_set_automatic (v.bin, 1);
    visual_bin_switch_set_steps (v.bin, 10);
    //visual_bin_set_morph_by_name (v.bin, (char *)v.morph_name);

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

    // We set it again because mic input may be default and it requires extra steps to initialize.
    set_input();

    printf ("Libvisual version %s; bpp: %d %s\n", visual_get_version(), v.video->bpp, (v.pluginIsGL ? "(GL)\n" : ""));
}

// Initialize the application's view and libvisual.
JNIEXPORT void JNICALL Java_com_starlon_starvisuals_NativeHelper_initApp(JNIEnv * env, jobject  obj, jint w, jint h, jint device, jint card)
{
    app_main(w, h, device, card);
}

// Render the view's bitmap image.
JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_render(JNIEnv * env, jobject  obj, jobject bitmap, jint dur)
{
    
    AndroidBitmapInfo  info;
    void*              pixels;
    int                ret;
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

    //visual_video_blit_overlay(vid, v.video, 0, 0, FALSE);
    visual_video_depth_transform(vid, v.video);

    visual_object_unref(VISUAL_OBJECT(vid));

    AndroidBitmap_unlockPixels(env, bitmap);

    return TRUE;
}
