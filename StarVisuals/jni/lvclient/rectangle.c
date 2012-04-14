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




/** VisRectangle.rectangleNew() */
JNIEXPORT jint JNICALL Java_org_libvisual_android_VisRectangle_rectangleNew(JNIEnv * env, jobject  obj, jint x, jint y, jint width, jint height)
{
    VisRectangle *rect = NULL;

    LOGI("VisRectangle.rectangleNew()");

    rect = visual_rectangle_new(x, y, width, height);

    return (jint) rect;
}



JNIEXPORT jint JNICALL Java_org_libvisual_android_VisRectangle_rectangleFree(JNIEnv * env, jobject  obj, jint rect)
{
    LOGI("VisRectangle.rectangleFree()");

    visual_rectangle_free((VisRectangle *)rect);

    return VISUAL_OK;
}
