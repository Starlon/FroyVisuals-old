/* $Id$
 * $URL$
 *
 * Copyright (C) 2009 Scott Sibley <scott@starlon.net>
 *
 * This file is part of LCDControl.
 *
 * LCDControl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LCDControl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LCDControl.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <exception>
#include <cstdlib>

#include <string>
#include <jni.h>
#include <luascript/luascript.h>
#include <libvisual/libvisual.h>

#include <iostream>
#include <libvisual/libvisual.h>

#include "Evaluator.h"
#include "SpecialChar.h"
#include "debug.h"
#include "CPtr.h"

using namespace LCD;

/*
QScriptValue toSpecialChar(QScriptEngine *engine, const SpecialChar &ch) {
    QScriptValue obj = engine->newObject();
    QScriptValue array = engine->newArray(ch.Size());
    std::vector<int> matrix = ch.Vector();
    for(int i = 0; i < ch.Size(); i++)
        array.setProperty(i, QScriptValue(engine, matrix[i]));
    obj.setProperty("chars", array);
    return obj;
}
*/

/*
void fromSpecialChar(const QScriptValue &obj, SpecialChar &ch) {
    for(int i = 0; obj.property(i).isValid(); i++ )
        ch.AddChar(obj.property(i).toUInt16());
}
*/


Evaluator::Evaluator()
{
    mScript = new lua();
    mCpuinfo = new PluginCpuinfo(mScript);
    mLoadavg = new PluginLoadavg(mScript);
    mProcStat = new PluginProcStat(mScript);
    mUptime = new PluginUptime(mScript);
}

Evaluator::~Evaluator()
{
    delete mScript;
    delete mCpuinfo;
    delete mLoadavg;
    delete mProcStat;
    delete mUptime;
}

std::string Evaluator::Eval(std::string str)
{
    std::string val;
    try {
        mScript->exec("function __wrap__() " + str + " end; __out__ = __wrap__()");
        val = mScript->get_variable<lua::string_arg_t>("__out__").value();
    } catch (lua::exception &e)
    {
        visual_log(VISUAL_LOG_ERROR, "Lua error: %s, line: %d", e.error().c_str(), e.line());
    }

    return val;

}

/////////////////////// JNI glue

extern "C" {

JNIEXPORT jstring JNICALL Java_com_starlon_libscriptable_UtilsEvaluator_evaluate(
    JNIEnv *env, jclass clazz, jobject obj, jstring str)
{
    Evaluator *eval = getObjectFromCPtr<Evaluator *>( env, obj );

    jboolean isCopy;
    const char * _str = env->GetStringUTFChars(str, &isCopy);

    std::string val = eval->Eval((std::string)_str);

    env->ReleaseStringUTFChars(str, _str);
    return env->NewStringUTF(val.c_str());
}

JNIEXPORT jobject JNICALL Java_com_starlon_libscriptable_UtilsEvaluator_evaluatorNew(
    JNIEnv *env, jclass clazz)
{
    std::set_terminate(std::abort);

    Evaluator *eval = new Evaluator();

    jclass tempClass;
    jmethodID mid;
    jobject obj;

    tempClass = env->FindClass("com/starlon/libscriptable/CPtr");

    mid = env->GetMethodID(tempClass, "<init>", "()V");

    obj = env->NewObject(tempClass, mid);

    if (obj)
    {
        env->SetLongField( obj, env->GetFieldID( tempClass, "peer", "J" ), (jlong)eval);
    }
    return obj;
}

JNIEXPORT void JNICALL Java_com_starlon_libscriptable_UtilsEvaluator_evaluatorDel(
    JNIEnv *env, jclass clazz, jobject obj)
{
    Evaluator *eval = getObjectFromCPtr<Evaluator *>(env, obj);

    delete eval;
}

}//extern
