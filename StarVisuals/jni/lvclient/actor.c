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



/** VisLog -> android Log glue */
static void _log_handler(VisLogSeverity severity, const char *msg, const VisLogSource *source, void *priv)
{

    switch(severity)
    {
        case VISUAL_LOG_DEBUG:
            LOGI("(debug) %s(): %s", source->func, msg);
            break;
        case VISUAL_LOG_INFO:
            LOGI("(info) %s", msg);
            break;
        case VISUAL_LOG_WARNING:
            LOGW("(WARNING) %s", msg);
            break;
        case VISUAL_LOG_ERROR:
            LOGE("(ERROR) (%s:%d) %s(): %s", source->file, source->line, source->func, msg);
            break;
        case VISUAL_LOG_CRITICAL:
            LOGE("(CRITICAL) (%s:%d) %s(): %s", source->file, source->line, source->func, msg);
            break;
    }
}


/******************************************************************************
 ******************************************************************************/

/** VisActor.actorNew() */
JNIEXPORT jint JNICALL Java_org_libvisual_android_VisActor_actorNew(JNIEnv * env, jobject  obj, jstring name)
{
    LOGI("VisActor.actorNew()");


    /* result */
    VisActor *a = NULL;

    /* get name string */
    jboolean isCopy;  
    const char *actorName = (*env)->GetStringUTFChars(env, name, &isCopy);  

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
    VisPluginData    *plugin_data = visual_actor_get_plugin(a);
    VisRandomContext *r_context   = visual_plugin_get_random_context (plugin_data);
    visual_random_context_set_seed(r_context, time(NULL));

_van_exit:
    (*env)->ReleaseStringUTFChars(env, name, actorName);
    return (jint) a;
}


/** VisActor.actorUnref() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisActor_actorUnref(JNIEnv * env, jobject  obj, jint actor)
{
    LOGI("VisActor.actorUnref()");

    VisActor *a = (VisActor *) actor;
    visual_object_unref(VISUAL_OBJECT(actor));
}


/** VisActor.actorGetSupportedDepth() */
JNIEXPORT jint JNICALL Java_org_libvisual_android_VisActor_actorGetSupportedDepth(JNIEnv * env, jobject  obj, jint actor)
{
    VisActor *a = (VisActor *) actor;
    return visual_actor_get_supported_depth(a);
}

/** VisActor.videoNegotiate() */
JNIEXPORT int JNICALL Java_org_libvisual_android_VisActor_actorVideoNegotiate(JNIEnv * env, jobject  obj, jint actor, jint rundepth, jboolean noevent, jboolean forced)
{
    VisActor *a = (VisActor *) actor;
            
    return visual_actor_video_negotiate(a, rundepth, noevent, forced);
}



