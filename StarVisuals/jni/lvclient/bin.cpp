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

/** VisBin.binNew() */
JNIEXPORT jobject JNICALL Java_org_libvisual_android_VisBin_binNew(JNIEnv * env, jobject  jobj)
{
    LOGI("VisBin.binNew()");

    VisBin *bin = visual_bin_new();
    jobject obj;
    jclass tempClass;

    tempClass = env->FindClass("org/libvisual/android/CPtr");

    obj = env->AllocObject( tempClass );
    if (obj)
    {
        env->SetLongField( obj, env->GetFieldID( tempClass, "peer", "J" ), (jlong)bin);
    }
    return obj;

}


/** VisBin.binUnref() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisBin_binUnref(JNIEnv * env, jobject  obj, jobject cptr)
{
    LOGI("VisBin.binUnref()");

    VisBin *bin = getObjectFromCPtr<VisBin *>(env, cptr);
    visual_object_unref(VISUAL_OBJECT(bin));        
}


/** VisBin.setDepth() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisBin_binSetDepth(JNIEnv * env, jobject  obj, jobject bin, jint depth)
{    
    VisBin *b = getObjectFromCPtr<VisBin *>(env, bin);
    visual_bin_set_depth(b, depth);
}


/** VisBin.setSupportedDepth() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisBin_binSetSupportedDepth(JNIEnv * env, jobject  obj, jobject bin, jint depth)
{    
    VisBin *b = getObjectFromCPtr<VisBin *>(env, bin);
    visual_bin_set_supported_depth(b, depth);
}


/** VisBin.setSupportedDepth() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisBin_binSetPreferredDepth(JNIEnv * env, jobject  obj, jobject bin, jint depth)
{    
    VisBin *b = getObjectFromCPtr<VisBin *>(env, bin);
    visual_bin_set_preferred_depth(b, (VisBinDepth)depth);
}


/** VisBin.setVideo() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisBin_binSetVideo(JNIEnv * env, jobject  obj, jobject bin, jobject video)
{    
    VisBin *b = getObjectFromCPtr<VisBin *>(env, bin);
    VisVideo *v = getObjectFromCPtr<VisVideo *>(env, video);
    visual_bin_set_video(b, v);
}


/** VisBin.realize() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisBin_binRealize(JNIEnv * env, jobject  obj, jobject bin)
{    
    VisBin *b = getObjectFromCPtr<VisBin *>(env, bin);
    visual_bin_realize(b);
}


/** VisBin.sync() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisBin_binSync(JNIEnv * env, jobject  obj, jobject bin, jboolean noevent)
{    
    VisBin *b = getObjectFromCPtr<VisBin *>(env, bin);
    visual_bin_sync(b, noevent);
}


/** VisBin.depthChanged() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisBin_binDepthChanged(JNIEnv * env, jobject  obj, jobject bin)
{    
    VisBin *b = getObjectFromCPtr<VisBin *>(env, bin);
    visual_bin_depth_changed(b);
}


/** VisBin.connect() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisBin_binConnect(JNIEnv * env, jobject  obj, jobject bin, jobject actor, jobject input)
{    
    VisBin *b = getObjectFromCPtr<VisBin *>(env, bin);
    VisActor *a = getObjectFromCPtr<VisActor *>(env, actor);
    VisInput *i = getObjectFromCPtr<VisInput *>(env, input);
    visual_bin_connect(b, a, i);
}


/** VisBin.setMorphByName() */
JNIEXPORT int JNICALL Java_org_libvisual_android_VisBin_binSetMorphByName(JNIEnv * env, jobject  obj, jobject bin, jstring name)
{    
    jboolean isCopy;  
    const char *n = env->GetStringUTFChars(name, &isCopy);  
        
    VisBin *b = getObjectFromCPtr<VisBin *>(env, bin);
    int r = visual_bin_set_morph_by_name(b, (char *)n);

    env->ReleaseStringUTFChars(name, n);

    return r;
}


/** VisBin.switchActor() */
JNIEXPORT int JNICALL Java_org_libvisual_android_VisBin_binSwitchActorByName(JNIEnv * env, jobject  obj, jobject bin, jstring name)
{
    jboolean isCopy;  
    const char *n = env->GetStringUTFChars(name, &isCopy);  

    VisBin *b = getObjectFromCPtr<VisBin *>(env, bin);
    int r = visual_bin_switch_actor_by_name(b, (char *)n);

    env->ReleaseStringUTFChars(name, n);

    return r;
}

}
