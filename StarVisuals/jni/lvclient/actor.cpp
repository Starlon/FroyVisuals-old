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

/******************************************************************************
 ******************************************************************************/

extern "C" {

/** VisActor.actorNew() */
JNIEXPORT jobject JNICALL Java_org_libvisual_android_VisActor_actorNew(JNIEnv * env, jobject  jobj, jstring name)
{
    LOGI("VisActor.actorNew()");


    /* result */
    VisActor *a = NULL;
    VisPluginData *plugin_data;
    VisRandomContext *r_context;

    /* get name string */
    jboolean isCopy;  
    const char *actorName = env->GetStringUTFChars(name, &isCopy);  

    /* actor valid ? */
    //if(!(visual_plugin_registry_has_plugin(VISUAL_PLUGIN_TYPE_ACTOR, actorName)))
    if(!visual_actor_valid_by_name(actorName))
    {
            LOGE("Invalid actor-plugin: \"%s\"", actorName);
            goto _van_exit;
    }

    /* create new actor */
    a = visual_actor_new(actorName);

    /* set random seed */
    plugin_data = visual_actor_get_plugin(a);
    r_context   = visual_plugin_get_random_context (plugin_data);
    visual_random_context_set_seed(r_context, time(NULL));

_van_exit:
    env->ReleaseStringUTFChars(name, actorName);

    jobject obj;
    jclass tempClass;
    
    tempClass = env->FindClass("org/libvisual/android/CPtr");

    obj = env->AllocObject(tempClass );
    if (obj)
    {
        env->SetLongField(obj, env->GetFieldID(tempClass, "peer", "J" ), (jlong)a);
    }
    
    return obj;
}


/** VisActor.actorUnref() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisActor_actorUnref(JNIEnv * env, jobject  obj, jobject actor)
{
    LOGI("VisActor.actorUnref()");

    VisActor *a = getObjectFromCPtr<VisActor *>(env, actor);
    visual_object_unref(VISUAL_OBJECT(a));
}


/** VisActor.actorGetSupportedDepth() */
JNIEXPORT jint JNICALL Java_org_libvisual_android_VisActor_actorGetSupportedDepth(JNIEnv * env, jobject  obj, jobject actor)
{
    VisActor *a = getObjectFromCPtr<VisActor *>(env, actor);
    return visual_actor_get_supported_depth(a);
}

/** VisActor.videoNegotiate() */
JNIEXPORT int JNICALL Java_org_libvisual_android_VisActor_actorVideoNegotiate(JNIEnv * env, jobject  obj, jobject actor, jint rundepth, jboolean noevent, jboolean forced)
{
    VisActor *a = getObjectFromCPtr<VisActor *>(env, actor);
            
    return visual_actor_video_negotiate(a, (VisVideoDepth)rundepth, noevent, forced);
}

JNIEXPORT jobject JNICALL Java_org_libvisual_android_VisActor_actorGetPlugin(JNIEnv *env, jclass clazz, jobject actor)
{
    VisActor *a = getObjectFromCPtr<VisActor *>(env, actor);
    const VisPluginInfo *info = a->plugin->info;

    jobject obj;
    jclass tempClass;

    tempClass = env->FindClass("org/libvisual/android/CPtr");

    obj = env->AllocObject( tempClass );
    if (obj)
    {
        env->SetLongField( obj, env->GetFieldID(tempClass, "peer", "J" ), (jlong)info);
    }
    return obj;

}

}
