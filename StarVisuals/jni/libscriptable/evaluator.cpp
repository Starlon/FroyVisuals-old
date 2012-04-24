#include <string>
#include <jni.h>
#include <luascript/luascript.h>
#include <libvisual/libvisual.h>
#include "PluginCpuinfo.h"

namespace LCD {

class Evaluator {
    private:
    lua *mScript;
    PluginCpuinfo *mCpuinfo;
    
    public:
    Evaluator()
    {
        mScript = new lua();
        mCpuinfo = new PluginCpuinfo(mScript);
        
    }

    ~Evaluator()
    {
        delete mCpuinfo;
        delete mScript;
    }

    std::string eval(std::string str)
    {
        std::string val;
        try {
            mScript->exec("function __wrap__() " + val + " end; __out__ = __wrap__()");
            val = mScript->get_variable<lua::string_arg_t>("__out__").value();
        } catch (lua::exception &e)
        {
            visual_log(VISUAL_LOG_ERROR, "Lua error: %s, line: %d", e.error().c_str(), e.line());
        }
    
        return val;

    }
};

}

using namespace LCD;

template <class T>
T getObjectFromCPtr( JNIEnv *env, jobject cptr )
{
    T obj;
    jclass classPtr = env->GetObjectClass( cptr );
    jfieldID CPtr_peer_ID = env->GetFieldID( classPtr, "peer", "J" );
    jlong *peer = (jlong *) env->GetLongField( cptr, CPtr_peer_ID );

    obj = ( T ) peer;

    return obj;
}

extern "C" {

JNIEXPORT jstring JNICALL Java_com_starlon_libscriptable_UtilsEvaluator_evaluate(
    JNIEnv *env, jclass clazz, jobject obj, jstring str)
{
    Evaluator *eval = getObjectFromCPtr<Evaluator *>( env, obj );

    jboolean isCopy;
    std::string _str = env->GetStringUTFChars(str, &isCopy);

    std::string val = eval->eval(_str);

    return env->NewStringUTF(val.c_str());
}

JNIEXPORT jobject JNICALL Java_com_starlon_libscriptable_UtilsEvaluator_evaluatorNew(
    JNIEnv *env, jclass clazz)
{

    Evaluator *eval = new Evaluator();

    jobject obj;
    jclass tempClass;

    tempClass = env->FindClass("com/starlon/libscriptable/CPtr");

    obj = env->AllocObject( tempClass );
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
