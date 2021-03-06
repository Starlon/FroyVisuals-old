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

/** VisMorph.morphNew() */
JNIEXPORT jint JNICALL Java_org_libvisual_android_VisMorph_morphNew(JNIEnv * env, jobject  obj, jstring name)
{
    LOGI("VisMorph.morphNew()");

    /* result */
    VisMorph *m = NULL;
        
    /* get name string */
    jboolean isCopy;  
    const char *morphName = env->GetStringUTFChars(name, &isCopy);  

    /* plugin valid ? */
    if(!visual_actor_valid_by_name(morphName))
    //if(!(visual_plugin_registry_has_plugin(VISUAL_PLUGIN_TYPE_MORPH, morphName)))
    {
            LOGE("Invalid morph-plugin: \"%s\"", morphName);
            goto _vin_exit;
    }

    /* create new morph */
    m = visual_morph_new(morphName);
        
_vin_exit:
    env->ReleaseStringUTFChars(name, morphName);
    return (jint) m;
}


/** VisMorph.morphUnref() */
JNIEXPORT void JNICALL Java_org_libvisual_android_VisMorph_morphUnref(JNIEnv * env, jobject  obj, jobject morph)
{
    LOGI("VisMorph.morphUnref()");

    VisMorph *m = getObjectFromCPtr<VisMorph *>(env, morph);
    visual_object_unref(VISUAL_OBJECT(m));        
}

JNIEXPORT jobject JNICALL Java_org_libvisual_android_VisMorph_morphGetPlugin(JNIEnv *env, jclass clazz, jobject morph)
{
    VisMorph *m = getObjectFromCPtr<VisMorph *>(env, morph);
    const VisPluginInfo *info = m->plugin->info;

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


/******************************************************************************/

}
