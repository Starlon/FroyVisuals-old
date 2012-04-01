
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
#include <math.h>
#include <sys/types.h>

#define DEVICE_DEPTH VISUAL_VIDEO_DEPTH_32BIT

#define  LOG_TAG    "StarVisuals"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

// Initial plugins. Preferences should override these.
#define MORPH "alphablend"
#define ACTOR "lv_analyzer"
#define INPUT "dummy"

#define URL_GPLv2 "http://www.gnu.org/licenses/gpl-2.0.txt"
#define URL_GPLv3 "http://www.gnu.org/licenses/gpl-3.0.txt"
#define URL_LGPL "http://www.gnu.org/licenses/lgpl-3.0.txt"
#define URL_BSD "http://www.opensource.org/licenses/bsd-license.php"

#define MAX_PCM 1024

struct {
    int16_t pcm_data[MAX_PCM]; // FIXME grow this with audio buffer size taken from java-side. later.
    int size;
    VisAudioSampleRateType rate;
    VisAudioSampleChannelType channels;
    VisAudioSampleFormatType encoding;
    int min_beat_hold;
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

static void my_log_handler (VisLogSeverity severity, const char *msg, const VisLogSource *source, void *priv)
{
    return;
    switch(severity)
    {
        case VISUAL_LOG_DEBUG:
            LOGI("lvDEBUG: (%s) line # %d (%s) : %s\n", source->file, source->line, source->func, msg);
            break;
        case VISUAL_LOG_INFO:
            LOGI("lvINFO: %s: %s\n", __lv_progname, msg);
            break;
        case VISUAL_LOG_WARNING:
            LOGW("lvWARNING: %s: %s\n", __lv_progname, msg);
            break;
        case VISUAL_LOG_ERROR:
            LOGE("lvERROR: (%s) line # %d (%s) : %s\n", source->file, source->line, source->func, msg);
            break;
        case VISUAL_LOG_CRITICAL:
            LOGE("lvCRITICAL: (%s) line # %d (%s) : %s\n", source->file, source->line, source->func, msg);
            break;
    }
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

JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_setMinBeat(JNIEnv *env, jobject obj, jint timemil)
{
    pcm_ref.min_beat_hold = (int)timemil;

    return 0;
}

static int v_upload_callback (VisInput* input, VisAudio *audio, void* unused)
{

    static VisTimer *timer = NULL;

    if(timer == NULL)
    {
        timer = visual_timer_new();
        visual_timer_start(timer);
    }

    visual_return_val_if_fail(input != NULL, VISUAL_ERROR_GENERAL);
    visual_return_val_if_fail(audio != NULL, VISUAL_ERROR_GENERAL);
    visual_return_val_if_fail(pcm_ref.pcm_data != NULL, VISUAL_ERROR_GENERAL);

    VisParamContainer *paramcontainer = visual_plugin_get_params(input->plugin);

    VisBuffer buf;

    visual_buffer_init( &buf, pcm_ref.pcm_data, pcm_ref.size/2, NULL );
    visual_audio_samplepool_input( audio->samplepool, &buf, pcm_ref.rate, pcm_ref.encoding, pcm_ref.channels);

    if(paramcontainer != NULL && FALSE)
    {
        VisParamEntry *entry = visual_param_container_get(paramcontainer, "isBeat");
        if(entry == NULL)
        {
            entry = visual_param_entry_new("isBeat");
            visual_param_container_add(paramcontainer, entry);
        }

        unsigned char scaled[pcm_ref.size];
        visual_mem_set(scaled, 0, sizeof(scaled));

        int i, isBeat;

        for(i = 0; i < pcm_ref.size && i < MAX_PCM; i++)
        {
            scaled[i] = pcm_ref.pcm_data[i] / (float)FLT_MAX * UCHAR_MAX;
        }
        isBeat = visual_audio_is_beat_with_data(audio, VISUAL_BEAT_ALGORITHM_PEAK, scaled, MAX_PCM);
        if(visual_timer_elapsed_msecs(timer) > pcm_ref.min_beat_hold && isBeat)
        {
            visual_param_entry_set_integer(entry, isBeat);
        }
    }
    return 0;
}


// ---------- INPUT ----------

// Get the VisInput at the requested index.
VisPluginRef *get_input(int index)
{
    VisList *list = visual_input_get_list();

    int count = visual_list_count(list);

    visual_return_val_if_fail(index < count, NULL);


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

    if(v.bin->input)
        visual_object_unref(VISUAL_OBJECT(v.bin->input));

    VisInput *input = visual_input_new(v.input_name);

    if(strstr(v.input_name, "mic"))
    {
    	if (visual_input_set_callback (input, v_upload_callback, NULL) < 0) {
    	    visual_log(VISUAL_LOG_CRITICAL, "Unable to set mic input callback.");	
    	}
    }

    visual_input_realize(input);
    visual_bin_set_input(v.bin, input);
    visual_bin_sync(v.bin, TRUE);

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
        return TRUE;
    }
    return FALSE;
}


// Get the input's plugin name.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_inputGetName(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_input(index);

    visual_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->plugname));
}

// Get the input's plugin longname.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_inputGetLongName(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_input(index);

    visual_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->name));
}

// Get the input's plugin author.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_inputGetAuthor(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_input(index);

    visual_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->author));
}

// Get the input's plugin version.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_inputGetVersion(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_input(index);

    visual_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->version));
}

// Get the input's plugin about string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_inputGetAbout(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_input(index);

    visual_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->about));
}

// Get the input's plugin help string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_inputGetHelp(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_input(index);

    visual_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->help));
}

// Get the input's plugin license string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_inputGetLicense(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_input(index);

    char text[256];

    visual_return_val_if_fail(ref != NULL, NULL);

    const char *license = ref->info->license;

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

    visual_return_val_if_fail(cont != NULL, NULL);

    int count = visual_list_count(&cont->entries);

    visual_return_val_if_fail(index < count, NULL);

    VisParamEntry *entry = (VisParamEntry *)visual_list_get(&cont->entries, index);

    visual_return_val_if_fail(entry != NULL, NULL);

    return entry;
}

JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_inputParamGetCount(JNIEnv *env, jobject obj)
{
    VisParamContainer *cont = visual_plugin_get_params(visual_input_get_plugin(v.bin->input));

    visual_return_val_if_fail(cont != NULL, 0);

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

    visual_return_val_if_fail(entry != NULL, NULL);

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
    visual_return_val_if_fail(chars != NULL, NULL);

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

    visual_return_val_if_fail(param_name != NULL, FALSE);
    visual_return_val_if_fail(new_string != NULL, FALSE);

    VisParamContainer *cont = visual_plugin_get_params(visual_input_get_plugin(v.bin->input));

    VisParamEntry *entry = visual_param_container_get(cont, param_name);

    int ret = visual_param_entry_set_string(entry, (char *)new_string);

    return !ret;
}

JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_inputParamGetInteger(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *string = ((*env)->GetStringUTFChars(env, name, &iscopy));
    visual_return_val_if_fail(string != NULL, 0);

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

    visual_return_val_if_fail(param_name != NULL, FALSE);

    VisParamContainer *cont = visual_plugin_get_params(visual_input_get_plugin(v.bin->input));

    VisParamEntry *entry = visual_param_container_get(cont, param_name);

    int ret = visual_param_entry_set_integer(entry, new_int);

    return !ret;
}

JNIEXPORT jfloat JNICALL Java_com_starlon_starvisuals_NativeHelper_inputParamGetFloat(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *string = ((*env)->GetStringUTFChars(env, name, &iscopy));
    visual_return_val_if_fail(string != NULL, 0.0f);

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

    visual_return_val_if_fail(param_name != NULL, FALSE);

    VisParamContainer *cont = visual_plugin_get_params(visual_input_get_plugin(v.bin->input));

    VisParamEntry *entry = visual_param_container_get(cont, param_name);

    int ret = visual_param_entry_set_float(entry, new_float);

    return !ret;
}

JNIEXPORT jdouble JNICALL Java_com_starlon_starvisuals_NativeHelper_inputParamGetDouble(JNIEnv *env, jobject obj, jstring name)
{    
    jboolean iscopy;
    const char *string = ((*env)->GetStringUTFChars(env, name, &iscopy));

    visual_return_val_if_fail(string != NULL, 0.0);

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

    visual_return_val_if_fail(param_name != NULL, FALSE);

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

    visual_return_val_if_fail(index >= 0 && index < count, NULL);

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

    visual_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->plugname));
}

// Get the morph plugin's long name string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_morphGetLongName(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_morph(index);

    visual_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->name));
}

// Get the morph plugin's author string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_morphGetAuthor(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_morph(index);

    visual_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->author));
}

// Get the morph plugin's version string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_morphGetVersion(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_morph(index);

    visual_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->version));

}

// Get the morph plugin's about string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_morphGetAbout(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_morph(index);

    visual_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->about));
}

// Get the morph plugin's help string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_morphGetHelp(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_morph(index);

    visual_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->help));
}

// Get the morph plugin's license string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_morphGetLicense(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_morph(index);

    char text[256];

    visual_return_val_if_fail(ref != NULL, NULL);

    const char *license = ref->info->license;

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

    visual_return_val_if_fail(cont != NULL, NULL);

    int count = visual_list_count(&cont->entries);

    visual_return_val_if_fail(index < count, NULL);

    VisParamEntry *entry = (VisParamEntry *)visual_list_get(&cont->entries, index);

    visual_return_val_if_fail(entry != NULL, NULL);

    return entry;
}

JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_morphParamGetCount(JNIEnv *env, jobject obj)
{
    VisParamContainer *cont = visual_plugin_get_params(visual_morph_get_plugin(v.bin->morph));

    visual_return_val_if_fail(cont != NULL, 0);

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

    visual_return_val_if_fail(entry != NULL, NULL);

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
    visual_return_val_if_fail(chars != NULL, NULL);

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

    visual_return_val_if_fail(param_name != NULL, FALSE);
    visual_return_val_if_fail(new_string != NULL, FALSE);

    VisParamContainer *cont = visual_plugin_get_params(visual_morph_get_plugin(v.bin->morph));

    VisParamEntry *entry = visual_param_container_get(cont, param_name);

    int ret = visual_param_entry_set_string(entry, (char *)new_string);

    return !ret;
}

JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_morphParamGetInteger(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *string = ((*env)->GetStringUTFChars(env, name, &iscopy));
    visual_return_val_if_fail(string != NULL, 0);

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

    visual_return_val_if_fail(param_name != NULL, FALSE);

    VisParamContainer *cont = visual_plugin_get_params(visual_morph_get_plugin(v.bin->morph));

    VisParamEntry *entry = visual_param_container_get(cont, param_name);

    int ret = visual_param_entry_set_integer(entry, new_int);

    return !ret;
}

JNIEXPORT jfloat JNICALL Java_com_starlon_starvisuals_NativeHelper_morphParamGetFloat(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *string = ((*env)->GetStringUTFChars(env, name, &iscopy));
    visual_return_val_if_fail(string != NULL, 0.0f);

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

    visual_return_val_if_fail(param_name != NULL, FALSE);

    VisParamContainer *cont = visual_plugin_get_params(visual_morph_get_plugin(v.bin->morph));

    VisParamEntry *entry = visual_param_container_get(cont, param_name);

    int ret = visual_param_entry_set_float(entry, new_float);

    return !ret;
}

JNIEXPORT jdouble JNICALL Java_com_starlon_starvisuals_NativeHelper_morphParamGetDouble(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *string = ((*env)->GetStringUTFChars(env, name, &iscopy));
    visual_return_val_if_fail(string != NULL, 0.0);

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

    visual_return_val_if_fail(param_name != NULL, FALSE);

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

    visual_return_val_if_fail(index >= 0 && index < count, NULL);

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

    visual_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->plugname));
}

// Get the actor's long name.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_actorGetLongName(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_actor(index);
    const char *name;

    visual_return_val_if_fail(ref != NULL, NULL);
    name = ref->info->name;

    return ((*env)->NewStringUTF(env, (char *)name));
}

// Get the actor's author.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_actorGetAuthor(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_actor(index);

    visual_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->author));
}

// Get the actor's version string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_actorGetVersion(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_actor(index);

    visual_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->version));
}

// Get the actor's about string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_actorGetAbout(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_actor(index);

    visual_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->about));
}

// Get the actor's help string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_actorGetHelp(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_actor(index);

    visual_return_val_if_fail(ref != NULL, NULL);

    return ((*env)->NewStringUTF(env, ref->info->help));
}

// Get the actor's license string.
JNIEXPORT jstring JNICALL Java_com_starlon_starvisuals_NativeHelper_actorGetLicense(JNIEnv *env, jobject obj, jint index)
{
    VisPluginRef *ref = get_actor(index);
    char text[256];

    visual_return_val_if_fail(ref != NULL, NULL);

    const char *license = ref->info->license;

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

    visual_return_val_if_fail(cont != NULL, NULL);

    int count = visual_list_count(&cont->entries);

    visual_return_val_if_fail(index < count, NULL);

    VisParamEntry *entry = (VisParamEntry *)visual_list_get(&cont->entries, index);

    visual_return_val_if_fail(entry != NULL, NULL);

    return entry;
}

JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_actorParamGetCount(JNIEnv *env, jobject obj)
{
    VisParamContainer *cont = visual_plugin_get_params(visual_actor_get_plugin(v.bin->actor));

    visual_return_val_if_fail(cont != NULL, 0);

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

    visual_return_val_if_fail(entry != NULL, NULL);

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
    visual_return_val_if_fail(chars != NULL, NULL);

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

    visual_return_val_if_fail(param_name != NULL, FALSE);
    visual_return_val_if_fail(new_string != NULL, FALSE);

    VisParamContainer *cont = visual_plugin_get_params(visual_actor_get_plugin(v.bin->actor));

    VisParamEntry *entry = visual_param_container_get(cont, param_name);

    int ret = visual_param_entry_set_string(entry, (char *)new_string);

    return !ret;
}


JNIEXPORT jint JNICALL Java_com_starlon_starvisuals_NativeHelper_actorParamGetInteger(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *string = ((*env)->GetStringUTFChars(env, name, &iscopy));
    visual_return_val_if_fail(string != NULL, 0);

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

    visual_return_val_if_fail(param_name != NULL, FALSE);

    VisParamContainer *cont = visual_plugin_get_params(visual_actor_get_plugin(v.bin->actor));

    VisParamEntry *entry = visual_param_container_get(cont, param_name);

    int ret = visual_param_entry_set_integer(entry, new_int);

    return !ret;
}

JNIEXPORT jfloat JNICALL Java_com_starlon_starvisuals_NativeHelper_actorParamGetFloat(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *string = ((*env)->GetStringUTFChars(env, name, &iscopy));
    visual_return_val_if_fail(string != NULL, 0.0f);

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

    visual_return_val_if_fail(param_name != NULL, FALSE);

    VisParamContainer *cont = visual_plugin_get_params(visual_actor_get_plugin(v.bin->actor));

    VisParamEntry *entry = visual_param_container_get(cont, param_name);

    float ret = visual_param_entry_set_float(entry, new_float);

    return !ret;
}

JNIEXPORT jdouble JNICALL Java_com_starlon_starvisuals_NativeHelper_actorParamGetDouble(JNIEnv *env, jobject obj, jstring name)
{
    jboolean iscopy;
    const char *string = ((*env)->GetStringUTFChars(env, name, &iscopy));
    visual_return_val_if_fail(string != NULL, 0.0);

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

    visual_return_val_if_fail(param_name != NULL, FALSE);

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
    for(i = 0; i < len && i < pcm_ref.size / sizeof(int16_t); i++)
    {
        pcm_ref.pcm_data[i] = pcm[i];
    }
    (*env)->ReleaseShortArrayElements(env, data, pcm, 0);
}

// Reinitialize audio fields.
JNIEXPORT void JNICALL Java_com_starlon_starvisuals_NativeHelper_resizePCM(jint size, jint samplerate, jint channels, jint encoding)
{
    //if(pcm_ref.pcm_data != NULL)
    //    visual_mem_free(pcm_ref.pcm_data);
    pcm_ref.size = MAX_PCM;//size;
    //pcm_ref.pcm_data = visual_mem_malloc(pcm_ref.size * sizeof(int16_t));

    //D/StarVisuals/StarVisualsActivity( 1102): Opened mic: 44100Hz, bits: 2, channel: 12, buffersize:8192
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
        default:
            pcm_ref.rate = VISUAL_AUDIO_SAMPLE_RATE_44100;
        break;
    }

    // It seems lv only supports stereo? hmm
    pcm_ref.channels = VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO;

    // According to documentation 16BIT sample size is guaranteed to be supported.
    pcm_ref.encoding = VISUAL_AUDIO_SAMPLE_FORMAT_S16;
}

// Increment or decrement actor and morph
// Variable 'prev' is used to shift morph plugin around. 
// 0=left, 1=right, 2=up, 3=down, 4=cycle.. Any other and the current value is used.
JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_finalizeSwitch(JNIEnv * env, jobject  obj, jint prev)
{

    VisMorph *bin_morph = visual_bin_get_morph(v.bin);
    const char *morph = v.morph_name;

    
    if(bin_morph && !visual_morph_is_done(bin_morph))
        return FALSE;



    if(prev == -1) {
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
    else if(prev == -2)
    {
        v.morph_name = "slide_down";
    } 
    else if(prev == 0)
    {
        v_cycleMorph((int)prev);
    }
    else 
    {
        v.morph_name = "alphablend";
    }

    visual_log(VISUAL_LOG_INFO, "Switching actors %s -> %s", morph, v.morph_name);

    visual_bin_set_morph_by_name (v.bin, (char *)v.morph_name);

    v_cycleActor((int)prev);
    visual_bin_switch_actor_by_name(v.bin, (char *)v.actor_name);

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


JNIEXPORT void JNICALL Java_com_starlon_starvisuals_NativeHelper_visualsQuit(JNIEnv * env, jobject  obj, jboolean toExit)
{

    if(v.video != NULL)
    {
        visual_video_free_buffer(v.video);
        visual_object_unref(VISUAL_OBJECT(v.video));
    }

    if(v.bin != NULL && v.bin->actor != NULL)
        visual_object_unref(VISUAL_OBJECT(v.bin->actor));

    if(v.bin != NULL && v.bin->input != NULL)
        visual_object_unref(VISUAL_OBJECT(v.bin->input));

    if(v.bin != NULL)
        visual_object_unref(VISUAL_OBJECT(v.bin));

    if(visual_is_initialized())
        visual_quit();
}

void app_main(int w, int h)
{

#if defined(USLEEP)
    usleep(USLEEP); 
#endif
    int depthflag;
    VisVideoDepth depth;

    if(!visual_is_initialized())
    {
        //setenv("LVSHOWBEATS", "1", 1);
        visual_init_path_add("/data/data/com.starlon.starvisuals/lib");
        visual_log_set_verbosity (VISUAL_LOG_DEBUG);
        visual_log_set_handler (VISUAL_LOG_DEBUG, my_log_handler, NULL);
        visual_log_set_handler (VISUAL_LOG_INFO, my_log_handler, NULL);
        visual_log_set_handler (VISUAL_LOG_WARNING, my_log_handler, NULL);
        visual_log_set_handler (VISUAL_LOG_CRITICAL, my_log_handler, NULL);
        visual_log_set_handler (VISUAL_LOG_ERROR, my_log_handler, NULL);
    
        visual_init (0, NULL);
        memset(&v, 0, sizeof(v));
        memset(&pcm_ref, 0, sizeof(pcm_ref));

        v.morph_name = MORPH;
        v.actor_name = ACTOR;
        v.input_name = INPUT;
        pcm_ref.min_beat_hold = 300;
    } else {
        visual_video_free_buffer(v.video);
        visual_object_unref(VISUAL_OBJECT(v.video));
        v.video = NULL; // Will thread activity blowup in native_render() if we do this? Speifically free_buffer(). hmm This whole block of code is wrong.

        visual_object_unref(VISUAL_OBJECT(v.bin->input));
        visual_object_unref(VISUAL_OBJECT(v.bin->actor));
        //FIXME We need valgrind pronto... 
        //visual_object_unref(VISUAL_OBJECT(v.bin));
    }

    v.bin    = visual_bin_new ();

    if (!visual_actor_valid_by_name (v.actor_name)) {
        v_cycleActor(1);
        visual_log(VISUAL_LOG_CRITICAL, ("Actor plugin not found! Choosing %s instead."), v.actor_name);
    }

    visual_bin_set_supported_depth (v.bin, VISUAL_VIDEO_DEPTH_ALL);
    visual_bin_set_preferred_depth(v.bin, VISUAL_VIDEO_DEPTH_32BIT);

    VisActor *actor = visual_actor_new((char*)v.actor_name);
    VisInput *input = visual_input_new((char*)v.input_name);

    if(strstr(v.input_name, "mic"))
    {
    	if (visual_input_set_callback (input, v_upload_callback, NULL) < 0) {

    	    visual_log(VISUAL_LOG_CRITICAL, "Unable to set mic input callback.");	

            visual_object_unref(VISUAL_OBJECT(input));

            v.input_name = "dummy";

            input = visual_input_new((char *)v.input_name);
    	} else {
        }
    }

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
    visual_bin_switch_set_steps (v.bin, 3);

    visual_bin_connect(v.bin, actor, input);
    if((v.pluginIsGL = (visual_bin_get_depth (v.bin) == VISUAL_VIDEO_DEPTH_GL)))
    {
        visual_video_set_depth(v.video, VISUAL_VIDEO_DEPTH_GL);
        visual_video_free_buffer(v.video);
        visual_video_allocate_buffer(v.video);
    }
    visual_bin_realize (v.bin);
    visual_bin_sync (v.bin, FALSE);
    visual_bin_depth_changed(v.bin);

    printf ("Libvisual version %s; bpp: %d %s\n", visual_get_version(), v.video->bpp, (v.pluginIsGL ? "(GL)\n" : ""));
}

// Initialize the application's view and libvisual.
JNIEXPORT void JNICALL Java_com_starlon_starvisuals_NativeHelper_initApp(JNIEnv * env, jobject  obj, jint w, jint h)
{
    app_main(w, h);
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

void swap_video_BGR(VisVideo *vid1, VisVideo *vid2)
{
    int8_t *d = visual_video_get_pixels(vid1);
    int8_t *s = visual_video_get_pixels(vid2);
    int i;

    for(i = 0; i < vid1->width * vid1->height * sizeof(int32_t); i+=4)
    {
        d[i] = s[i+2];
        d[i+1] = s[i+1];
        d[i+2] = s[i];
        d[i+3] = 0xff;
    }
}

// Render the view's bitmap image.
JNIEXPORT jboolean JNICALL Java_com_starlon_starvisuals_NativeHelper_render(JNIEnv * env, jobject  obj, jobject bitmap, jboolean do_swap)
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
        if(v.video != NULL && visual_video_get_pixels(v.video))
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

    visual_video_depth_transform(vid, v.video);

    if(do_swap && FALSE) // FIXME
    {
        int32_t data[vid->pitch * vid->height];
        VisVideo *swap = visual_video_new();
        visual_video_clone(swap, vid);
        visual_video_set_buffer(swap, data);
        visual_video_blit_overlay(swap, vid, 0, 0, FALSE);

        swap_video_BGR(vid, swap);

        visual_object_unref(VISUAL_OBJECT(swap));
    }

    visual_object_unref(VISUAL_OBJECT(vid));

    AndroidBitmap_unlockPixels(env, bitmap);

    return TRUE;
}
