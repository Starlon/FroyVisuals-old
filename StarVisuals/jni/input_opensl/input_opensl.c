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
#define BUFFERS 1

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

void assert(int test)
{
    visual_return_if_fail(test == 1);
}

static int inp_opensl_init (VisPluginData *plugin);
static int inp_opensl_cleanup (VisPluginData *plugin);
static int inp_opensl_upload (VisPluginData *plugin, VisAudio *audio);

static void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context);
static void stopRecording(inp_opensl_priv *priv);
static void startRecording(inp_opensl_priv *priv);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
    static VisInputPlugin input[] = {{
        .upload = inp_opensl_upload
    }};

    static VisPluginInfo info[] = {{
        .type = VISUAL_PLUGIN_TYPE_INPUT,

        .plugname = "opensl",
        .name = "OpenSL Microphone Input",
        .author = "Scott Sibley <sisibley@gmail.com>",
        .version = "0.1",
        .about = ("OpenSL mic input plugin for libvisual"),
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
    visual_return_val_if_fail(plugin != NULL, -1);

    inp_opensl_priv *priv = visual_mem_new0 (inp_opensl_priv, 1);

    visual_return_val_if_fail(priv != NULL, -1);

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

    //End init, setup recorder

    SLDataLocator_AndroidSimpleBufferQueue loc_bq;
    loc_bq.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
    loc_bq.numBuffers = BUFFERS;

    SLDataFormat_PCM pcm_format;
    pcm_format.formatType = SL_DATAFORMAT_PCM;
    pcm_format.numChannels = 1; // Mono sound.
    pcm_format.samplesPerSec = SL_SAMPLINGRATE_44_1;
    pcm_format.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
    pcm_format.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
    pcm_format.channelMask = SL_SPEAKER_FRONT_CENTER;
    pcm_format.endianness = SL_BYTEORDER_LITTLEENDIAN;

    SLDataLocator_IODevice loc_dev;
    loc_dev.locatorType = SL_DATALOCATOR_IODEVICE;
    loc_dev.deviceType = SL_IODEVICE_AUDIOINPUT;
    loc_dev.deviceID = SL_DEFAULTDEVICEID_AUDIOINPUT;
    loc_dev.device = NULL;

    SLDataSource audioSrc;
    audioSrc.pLocator = &loc_dev;
    audioSrc.pFormat = NULL;

    SLDataSink audioSnk;
    audioSnk.pLocator = &loc_bq;
    audioSnk.pFormat = &pcm_format;

    // create audio recorder
    // (requires the RECORD_AUDIO permission)
    const SLuint32 count = 2;
    const SLInterfaceID id[] = {SL_IID_RECORD, SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
    const SLboolean req[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    result = (*priv->engineEngine)->CreateAudioRecorder(priv->engineEngine, 
            &priv->recorderObject, &audioSrc,
            &audioSnk, count, id, req);

    if (SL_RESULT_SUCCESS != result) {
        return JNI_FALSE;
    }

    // realize the audio recorder
    result = (*priv->recorderObject)->Realize(priv->recorderObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        return JNI_FALSE;
    }

    // get the record interface
    result = (*priv->recorderObject)->GetInterface(priv->recorderObject, 
        SL_IID_RECORD, &priv->recorderRecord);
    assert(SL_RESULT_SUCCESS == result);

    // get the buffer queue interface
    result = (*priv->recorderObject)->GetInterface(priv->recorderObject, 
            SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
            &priv->recorderBufferQueue);
    assert(SL_RESULT_SUCCESS == result);

    // register callback on the buffer queue
    result = (*priv->recorderBufferQueue)->RegisterCallback(
        priv->recorderBufferQueue, bqRecorderCallback, priv);
    assert(SL_RESULT_SUCCESS == result);

    result = (*priv->recorderRecord)->SetCallbackEventsMask(
        priv->recorderRecord, SL_RECORDEVENT_BUFFER_FULL);
    assert(SL_RESULT_SUCCESS == result);

    startRecording(priv);

    return 0;
}

int inp_opensl_cleanup (VisPluginData *plugin)
{
    visual_return_val_if_fail(plugin != NULL, -1);

    inp_opensl_priv *priv = visual_object_get_private(VISUAL_OBJECT(plugin));
    visual_return_val_if_fail(priv != NULL, -1);

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
    int f =  (priv->currentFrame) % 2;
    if(inc) 
        priv->currentFrame++;
    return f;
}

// this callback handler is called every time a buffer finishes recording
void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
    inp_opensl_priv *priv = context;

    visual_return_if_fail(bq == priv->recorderBufferQueue);

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
    SLuint32 recorderState;

    if(priv->recordingState != RECORDING_STATE_STOPPED)
    {
        return;
    }

    result = (*priv->recorderObject)->GetState(priv->recorderObject, &recorderState);
    visual_return_if_fail(recorderState == SL_OBJECT_STATE_REALIZED);

    // in case already recording, stop recording and clear buffer queue
    result = (*priv->recorderRecord)->SetRecordState(priv->recorderRecord, SL_RECORDSTATE_STOPPED);
    visual_return_if_fail(SL_RESULT_SUCCESS == result);

    result = (*priv->recorderBufferQueue)->Clear(priv->recorderBufferQueue);
    visual_return_if_fail(SL_RESULT_SUCCESS == result);

    // enqueue buffers
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


    // Start recording
    result = (*priv->recorderRecord)->SetRecordState(priv->recorderRecord,
            SL_RECORDSTATE_RECORDING);

    visual_return_if_fail(result == SL_RESULT_SUCCESS);

    priv->currentFrame = 0;
    priv->recordingState = RECORDING_STATE_RECORDING;

    return;
}
