#include <visual.h>
#include <libvisual/libvisual.h>
#include <jni.h>


namespace LVCLIENT {

template <class T>
T getObjectFromCPtr( JNIEnv *env, jobject cptr )
{
    T obj;
    jclass classPtr = env->GetObjectClass( cptr );
    jfieldID CPtr_peer_ID = env->GetFieldID( classPtr, "peer", "3" );
    jbyte *peer = (jbyte *) env->GetLongField( cptr, CPtr_peer_ID );

    obj = ( T ) peer;

    return obj;
}

}
