/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2012
 *
 * Authors: Scott Sibley <sisibley@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <jni.h>

// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include <libvisual/libvisual.h>

// Recording States
#define RECORDING_STATE_STOPPED 1
#define RECORDING_STATE_RECORDING 2
#define RECORDING_STATE_STOPPING 3

#define SAMPLES 1024
#define BUFFERS 2

typedef struct {
    VisMutex *mutex;

    SLObjectItf engineObject;
    SLEngineItf engineEngine;


    SLObjectItf recorderObject;
    SLRecordItf recorderRecord;
    SLAndroidSimpleBufferQueueItf recorderBufferQueue;

    // Samples storage
    short recordBuffer[SAMPLES * BUFFERS];
    short pcm_data[SAMPLES];
    int recordingState;
    int currentFrame;
} inp_opensl_priv;

static int inp_opensl_init (VisPluginData *plugin);
static int inp_opensl_cleanup (VisPluginData *plugin);
static int inp_opensl_upload (VisPluginData *plugin, VisAudio *audio);

static void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context);
static void stopRecording(inp_opensl_priv *priv);
static void startRecording(inp_opensl_priv *priv);
static void record(void *data);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static VisInputPlugin input[] = {{
        .upload = inp_opensl_upload
    }};

    static VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_INPUT,

        .plugname = "mic",
        .name = "Microphone Input",
        .author = "Scott Sibley <sisibley@gmail.com>",
        .version = "0.1",
        .about = ("Mic input plugin for libvisual"),
        .help = ("You sing into the mic, and LibVisual goes nuts. Sing loud. With feeling."),
        .license = VISUAL_PLUGIN_LICENSE_LGPL,

        .init = inp_opensl_init,
        .cleanup = inp_opensl_cleanup,

        .plugin = VISUAL_OBJECT (&input[0])
    }};

    *count = sizeof (info) / sizeof (*info);

    return info;
}

int inp_opensl_init (VisPluginData *plugin)
{
    visual_log_return_val_if_fail(plugin != NULL, -1);

    inp_opensl_priv *priv = visual_mem_new0 (inp_opensl_priv, 1);

    visual_log_return_val_if_fail(priv != NULL, -1);

    visual_object_set_private(VISUAL_OBJECT (plugin), priv);

    priv->mutex = visual_mutex_new();

    SLresult result;

    // create engine
    result = slCreateEngine(&priv->engineObject, 0, NULL, 0, NULL, NULL);
    assert(SL_RESULT_SUCCESS == result);

    // realize the engine
    result = (*priv->engineObject)->Realize(priv->engineObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);

    // get the engine interface, which is needed in order to create other objects
    result = (*priv->engineObject)->GetInterface(priv->engineObject, SL_IID_ENGINE, &priv->engineEngine);
    assert(SL_RESULT_SUCCESS == result);

    // configure audio source
    SLDataLocator_IODevice loc_dev = {SL_DATALOCATOR_IODEVICE, SL_IODEVICE_AUDIOINPUT,
            SL_DEFAULTDEVICEID_AUDIOINPUT, NULL};
    SLDataSource audioSrc = {&loc_dev, NULL};

    // configure audio sink
    SLDataLocator_AndroidSimpleBufferQueue loc_bq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 1, SL_SAMPLINGRATE_16,
        SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
        SL_SPEAKER_FRONT_CENTER, SL_BYTEORDER_LITTLEENDIAN};
    SLDataSink audioSnk = {&loc_bq, &format_pcm};

    // create audio recorder
    // (requires the RECORD_AUDIO permission)
    const SLInterfaceID id[1] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};
    result = (*priv->engineEngine)->CreateAudioRecorder(priv->engineEngine, 
            &priv->recorderObject, &audioSrc,
            &audioSnk, 1, id, req);
    if (SL_RESULT_SUCCESS != result) {
        return JNI_FALSE;
    }

    // realize the audio recorder
    result = (*priv->recorderObject)->Realize(priv->recorderObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        return JNI_FALSE;
    }

    // get the record interface
    result = (*priv->recorderObject)->GetInterface(priv->recorderObject, SL_IID_RECORD, &priv->recorderRecord);
    assert(SL_RESULT_SUCCESS == result);

    // get the buffer queue interface
    result = (*priv->recorderObject)->GetInterface(priv->recorderObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
            &priv->recorderBufferQueue);
    assert(SL_RESULT_SUCCESS == result);

    // register callback on the buffer queue
    result = (*priv->recorderBufferQueue)->RegisterCallback(priv->recorderBufferQueue, bqRecorderCallback,
            priv);
    assert(SL_RESULT_SUCCESS == result);

    startRecording(priv);

    return 0;
}

int inp_opensl_cleanup (VisPluginData *plugin)
{
    visual_log_return_val_if_fail(plugin != NULL, -1);

    inp_opensl_priv *priv = visual_object_get_private(VISUAL_OBJECT(plugin));
    visual_log_return_val_if_fail(priv != NULL, -1);

    stopRecording(priv);

    // destroy audio recorder object, and invalidate all associated interfaces
    if (priv->recorderObject != NULL) {
        (*priv->recorderObject)->Destroy(priv->recorderObject);
        priv->recorderObject = NULL;
        priv->recorderRecord = NULL;
        priv->recorderBufferQueue = NULL;
    }

    // destroy engine object, and invalidate all associated interfaces
    if (priv->engineObject != NULL) {
        (*priv->engineObject)->Destroy(priv->engineObject);
        priv->engineObject = NULL;
        priv->engineEngine = NULL;
    }

    return 0;
}


static int inp_opensl_upload (VisPluginData *plugin, VisAudio *audio)
{
    inp_opensl_priv *priv = visual_object_get_private(VISUAL_OBJECT(plugin));
    VisBuffer buffer;

    visual_mutex_lock(priv->mutex);

    visual_buffer_init (&buffer, priv->pcm_data, SAMPLES, NULL);
    visual_audio_samplepool_input (audio->samplepool, &buffer, 
        VISUAL_AUDIO_SAMPLE_RATE_48000,
        VISUAL_AUDIO_SAMPLE_FORMAT_S16, 
        VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);

    visual_mutex_unlock(priv->mutex);

    return 0;
}

static int frame(inp_opensl_priv *priv, int inc)
{
    if(inc) 
        priv->currentFrame++;
    return (priv->currentFrame) % 2;
}

// this callback handler is called every time a buffer finishes recording
void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
    inp_opensl_priv *priv = context;

    visual_log_return_if_fail(bq == priv->recorderBufferQueue);

    visual_mutex_lock(priv->mutex);

    (*priv->recorderBufferQueue)->Enqueue(priv->recorderBufferQueue,
        priv->recordBuffer + frame(priv, FALSE) * SAMPLES, SAMPLES * sizeof(short));

    memcpy(priv->pcm_data, priv->recordBuffer + frame(priv, TRUE) * SAMPLES, SAMPLES * sizeof(short));

    visual_mutex_unlock(priv->mutex);
}

void stopRecording(inp_opensl_priv *priv)
{
    SLresult result;

   // in case already recording, stop recording and clear buffer queue
    SLAndroidSimpleBufferQueueState state;
    (*priv->recorderBufferQueue)->GetState(priv->recorderBufferQueue, &state);
    if (state.count == 0) {
        priv->recordingState = RECORDING_STATE_STOPPED;
    } else {
        priv->recordingState = RECORDING_STATE_STOPPING;
    }

    result = (*priv->recorderRecord)->SetRecordState(priv->recorderRecord, SL_RECORDSTATE_STOPPED);
    if (SL_RESULT_SUCCESS == result) {
       // We're done
    }
}

SLresult enqueueBuffer(inp_opensl_priv *priv, int i) {
    return (*priv->recorderBufferQueue)->Enqueue(priv->recorderBufferQueue,
            priv->recordBuffer + i * SAMPLES, SAMPLES * sizeof(short));
}

// set the recording state for the audio recorder
void startRecording(inp_opensl_priv *priv)
{
    SLresult result;

    if(priv->recordingState != RECORDING_STATE_STOPPED)
    {
        return;
    }

    // in case already recording, stop recording and clear buffer queue
    result = (*priv->recorderRecord)->SetRecordState(priv->recorderRecord, SL_RECORDSTATE_STOPPED);
    visual_log_return_if_fail(SL_RESULT_SUCCESS == result);
    result = (*priv->recorderBufferQueue)->Clear(priv->recorderBufferQueue);
    visual_log_return_if_fail(SL_RESULT_SUCCESS == result);

    // enqueue an empty buffer to be filled by the recorder
    // (for streaming recording, we would enqueue at least 2 empty buffers to start things off)
    int i;
    for(i = 0; i < BUFFERS; i++)
    {
        result = enqueueBuffer(priv, i);
        if(result != SL_RESULT_SUCCESS)
        {
            if(result == SL_RESULT_BUFFER_INSUFFICIENT)
            {
    
                visual_log(VISUAL_LOG_CRITICAL, "Failed to enqueue buffer: buffer insufficient");
            }
            else
            {
                visual_log(VISUAL_LOG_CRITICAL, "Failed to enqueue buffer...");
            }
        }
    }

    // start recording
    result = (*priv->recorderRecord)->SetRecordState(priv->recorderRecord, SL_RECORDSTATE_RECORDING);
    visual_log_return_if_fail(SL_RESULT_SUCCESS == result);

    priv->currentFrame = 0;
    priv->recordingState = RECORDING_STATE_RECORDING;

    result = (*priv->recorderRecord)->SetRecordState(priv->recorderRecord,
            SL_RECORDSTATE_RECORDING);

    if (SL_RESULT_SUCCESS != result) {
        visual_log(VISUAL_LOG_CRITICAL, "Failed to start recording");
    }

    return;
}
