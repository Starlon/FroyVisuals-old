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

/******************************************************************************/

/** VisBin.binNew() */
JNIEXPORT jint JNICALL Java_org_libvisual_android_VisBin_binNew(JNIEnv * env, jobject  obj)
{
    LOGI("VisBin.binNew()");

    return (jint) visual_bin_new();
}


/** VisBin.binUnref() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisBin_binUnref(JNIEnv * env, jobject  obj, jint bin)
{
    LOGI("VisBin.binUnref()");

    VisBin *b = (VisBin *) bin;
    visual_object_unref(VISUAL_OBJECT(bin));        
}


/** VisBin.setDepth() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisBin_binSetDepth(JNIEnv * env, jobject  obj, jint bin, jint depth)
{    
    VisBin *b = (VisBin *) bin;
    visual_bin_set_depth(b, depth);
}


/** VisBin.setSupportedDepth() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisBin_binSetSupportedDepth(JNIEnv * env, jobject  obj, jint bin, jint depth)
{    
    VisBin *b = (VisBin *) bin;
    visual_bin_set_supported_depth(b, depth);
}


/** VisBin.setSupportedDepth() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisBin_binSetPreferredDepth(JNIEnv * env, jobject  obj, jint bin, jint depth)
{    
    VisBin *b = (VisBin *) bin;
    visual_bin_set_preferred_depth(b, depth);
}


/** VisBin.setVideo() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisBin_binSetVideo(JNIEnv * env, jobject  obj, jint bin, jint video)
{    
    VisBin *b = (VisBin *) bin;
    VisVideo *v = (VisVideo *) video;
    visual_bin_set_video(b, v);
}


/** VisBin.realize() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisBin_binRealize(JNIEnv * env, jobject  obj, jint bin)
{    
    VisBin *b = (VisBin *) bin;
    visual_bin_realize(b);
}


/** VisBin.sync() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisBin_binSync(JNIEnv * env, jobject  obj, jint bin, jboolean noevent)
{    
    VisBin *b = (VisBin *) bin;
    visual_bin_sync(b, noevent);
}


/** VisBin.depthChanged() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisBin_binDepthChanged(JNIEnv * env, jobject  obj, jint bin)
{    
    VisBin *b = (VisBin *) bin;
    visual_bin_depth_changed(b);
}


/** VisBin.connect() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisBin_binConnect(JNIEnv * env, jobject  obj, jint bin, jint actor, jint input)
{    
    VisBin *b = (VisBin *) bin;
    VisActor *a = (VisActor *) actor;
    VisInput *i = (VisInput *) input;
    visual_bin_connect(b, a, i);
}


/** VisBin.setMorphByName() */
JNIEXPORT int JNICALL Java_org_libvisual_android_VisBin_binSetMorphByName(JNIEnv * env, jobject  obj, jint bin, jstring name)
{    
    jboolean isCopy;  
    const char *n = (*env)->GetStringUTFChars(env, name, &isCopy);  
        
    VisBin *b = (VisBin *) bin;
    int r = visual_bin_set_morph_by_name(b, n);

    (*env)->ReleaseStringUTFChars(env, name, n);

    return r;
}


/** VisBin.switchActor() */
JNIEXPORT int JNICALL Java_org_libvisual_android_VisBin_binSwitchActorByName(JNIEnv * env, jobject  obj, jint bin, jstring name)
{
    jboolean isCopy;  
    const char *n = (*env)->GetStringUTFChars(env, name, &isCopy);  

    VisBin *b = (VisBin *) bin;
    int r = visual_bin_switch_actor_by_name(b, n);

    (*env)->ReleaseStringUTFChars(env, name, n);

    return r;
}


