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



/** local variables */
static struct
{

}_l;


extern "C" {

/******************************************************************************/

/** VisVideo.videoNew() */
JNIEXPORT jobject JNICALL Java_org_libvisual_android_VisVideo_videoNew(JNIEnv * env, jobject  jobj)
{
    LOGI("VisVideo.videoNew()");

    VisVideo *vid = visual_video_new();

    jobject obj;
    jclass tempClass;

    tempClass = env->FindClass("org/libvisual/android/CPtr");
    
    obj = env->AllocObject( tempClass );
    if (obj)
    {
        env->SetLongField( obj, env->GetFieldID( tempClass, "peer", "J" ), (jlong)vid);
    }
    return obj;
}


/** VisVideo.videoUnref() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisVideo_videoUnref(JNIEnv * env, jobject  obj, jobject cptr)
{
    LOGI("VisVideo.videoUnref()");

    VisVideo *v = getObjectFromCPtr<VisVideo *>(env, cptr);
    visual_object_unref(VISUAL_OBJECT(v));        
}


/** VisVideo.setAttributes() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisVideo_videoSetAttributes(JNIEnv * env, 
                                                                              jobject  obj, 
                                                                              jobject cptr, 
                                                                              jint width, 
                                                                              jint height, 
                                                                              jint stride, 
                                                                              jint depth)
{
    LOGI("VisVideo.videoSetAttributes()");
        
    VisVideo *v = getObjectFromCPtr<VisVideo *>(env, cptr);

    visual_video_set_attributes(v, width, height, stride, (VisVideoDepth)depth);
}


/** VisVideo.depthGetHighest() */
JNIEXPORT jint JNICALL Java_org_libvisual_android_VisVideo_videoGetHighestDepth(JNIEnv * env, jobject  obj, jint depth)
{
    LOGI("VisVideo.videoGetHighestDepth()");

    return visual_video_depth_get_highest(depth);
}


/** VisVideo.depthGetHighestNoGl() */
JNIEXPORT jint JNICALL Java_org_libvisual_android_VisVideo_videoGetHighestDepthNoGl(JNIEnv * env, jobject  obj, jint depth)
{
    LOGI("VisVideo.videoGetHighestDepthNoGl()");

    return visual_video_depth_get_highest_nogl(depth);
}


/** VisVideo.depthGetHighestNoGl() */
JNIEXPORT jint JNICALL Java_org_libvisual_android_VisVideo_videoBppFromDepth(JNIEnv * env, jobject  obj, jint depth)
{
    LOGI("VisVideo.videoBppFromDepth()");
    return visual_video_bpp_from_depth((VisVideoDepth)depth);
}


/** VisVideo.videoAllocateBuffer() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisVideo_videoAllocateBuffer(JNIEnv * env, jobject  obj, jobject videoPtr)
{
    LOGI("VisVideo.videoAllocateBuffer()");
    VisVideo *v = getObjectFromCPtr<VisVideo *>(env, videoPtr);
    visual_video_allocate_buffer(v);
}
}
