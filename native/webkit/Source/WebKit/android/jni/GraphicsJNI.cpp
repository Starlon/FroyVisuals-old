// From AOSP: frameworks/base/core/jni/android/graphics/Graphics.cpp

#include "config.h"

#include "GraphicsJNI.h"
#include "utils/Log.h"

bool GraphicsJNI::hasException(JNIEnv *env) {
    if (env->ExceptionCheck() != 0) {
        LOGE("*** Uncaught exception returned from Java call!\n");
        env->ExceptionDescribe();
        return true;
    }
    return false;
}

static jclass   gRect_class;
static jfieldID gRect_leftFieldID;
static jfieldID gRect_topFieldID;
static jfieldID gRect_rightFieldID;
static jfieldID gRect_bottomFieldID;

static jclass   gPoint_class;
static jfieldID gPoint_xFieldID;
static jfieldID gPoint_yFieldID;

static jclass   gBitmap_class;
static jfieldID gBitmap_nativeInstanceID;
static jmethodID gBitmap_constructorMethodID;

static jclass   gCanvas_class;
static jfieldID gCanvas_nativeInstanceID;

static jclass   gPicture_class;
static jfieldID gPicture_nativeInstanceID;

static jclass   gRegion_class;
static jfieldID gRegion_nativeInstanceID;
static jmethodID gRegion_constructorMethodID;

bool graphics_init_done = false;

int register_android_graphics_Graphics(JNIEnv* env);

void check_graphics_init(JNIEnv *env)
{
  if (!graphics_init_done) {
    register_android_graphics_Graphics(env);
    graphics_init_done = true;
  }
}
        
void GraphicsJNI::get_jrect(JNIEnv* env, jobject obj, int* L, int* T, int* R, int* B)
{
    check_graphics_init(env);
    SkASSERT(env->IsInstanceOf(obj, gRect_class));

    *L = env->GetIntField(obj, gRect_leftFieldID);
    *T = env->GetIntField(obj, gRect_topFieldID);
    *R = env->GetIntField(obj, gRect_rightFieldID);
    *B = env->GetIntField(obj, gRect_bottomFieldID);
}

void GraphicsJNI::irect_to_jrect(const SkIRect& ir, JNIEnv* env, jobject obj)
{
    check_graphics_init(env);
    SkASSERT(env->IsInstanceOf(obj, gRect_class));

    env->SetIntField(obj, gRect_leftFieldID, ir.fLeft);
    env->SetIntField(obj, gRect_topFieldID, ir.fTop);
    env->SetIntField(obj, gRect_rightFieldID, ir.fRight);
    env->SetIntField(obj, gRect_bottomFieldID, ir.fBottom);
}


SkRect* GraphicsJNI::jrect_to_rect(JNIEnv* env, jobject obj, SkRect* r)
{
    check_graphics_init(env);
    SkASSERT(env->IsInstanceOf(obj, gRect_class));
    
    r->set(SkIntToScalar(env->GetIntField(obj, gRect_leftFieldID)),
           SkIntToScalar(env->GetIntField(obj, gRect_topFieldID)),
           SkIntToScalar(env->GetIntField(obj, gRect_rightFieldID)),
           SkIntToScalar(env->GetIntField(obj, gRect_bottomFieldID)));
    return r;
}

void GraphicsJNI::ipoint_to_jpoint(const SkIPoint& ir, JNIEnv* env, jobject obj)
{
    check_graphics_init(env);
    SkASSERT(env->IsInstanceOf(obj, gPoint_class));

    env->SetIntField(obj, gPoint_xFieldID, ir.fX);
    env->SetIntField(obj, gPoint_yFieldID, ir.fY);
}

// pmk: FIXME: some of these functions call into private APIs

SkCanvas* GraphicsJNI::getNativeCanvas(JNIEnv* env, jobject canvas) {
    check_graphics_init(env);
    SkASSERT(env);
    SkASSERT(canvas);
    SkASSERT(env->IsInstanceOf(canvas, gCanvas_class));
    SkCanvas* c = (SkCanvas*)env->GetIntField(canvas, gCanvas_nativeInstanceID);
    SkASSERT(c);
    return c;
}

SkBitmap* GraphicsJNI::getNativeBitmap(JNIEnv* env, jobject bitmap) {
    check_graphics_init(env);
    SkASSERT(env);
    SkASSERT(bitmap);
    SkASSERT(env->IsInstanceOf(bitmap, gBitmap_class));
    SkBitmap* b = (SkBitmap*)env->GetIntField(bitmap, gBitmap_nativeInstanceID);
    SkASSERT(b);
    return b;
}

SkPicture* GraphicsJNI::getNativePicture(JNIEnv* env, jobject picture)
{
    check_graphics_init(env);
    SkASSERT(env);
    SkASSERT(picture);
    SkASSERT(env->IsInstanceOf(picture, gPicture_class));
    SkPicture* p = (SkPicture*)env->GetIntField(picture, gPicture_nativeInstanceID);
    SkASSERT(p);
    return p;
}

SkRegion* GraphicsJNI::getNativeRegion(JNIEnv* env, jobject region)
{
    check_graphics_init(env);
    SkASSERT(env);
    SkASSERT(region);
    SkASSERT(env->IsInstanceOf(region, gRegion_class));
    SkRegion* r = (SkRegion*)env->GetIntField(region, gRegion_nativeInstanceID);
    SkASSERT(r);
    return r;
}

jobject GraphicsJNI::createBitmap(JNIEnv* env, SkBitmap* bitmap, bool isMutable,
                                  jbyteArray ninepatch, int density)
{
    check_graphics_init(env);
    SkASSERT(bitmap != NULL);
    SkASSERT(NULL != bitmap->pixelRef());
    
    jobject obj = env->AllocObject(gBitmap_class);
    if (obj) {
        env->CallVoidMethod(obj, gBitmap_constructorMethodID,
                            (jint)bitmap, isMutable, ninepatch, density);
        if (hasException(env)) {
            obj = NULL;
        }
    }
    return obj;
}

static jclass make_globalref(JNIEnv* env, const char classname[])
{
    jclass c = env->FindClass(classname);
    SkASSERT(c);
    return (jclass)env->NewGlobalRef(c);
}

static jfieldID getFieldIDCheck(JNIEnv* env, jclass clazz,
                                const char fieldname[], const char type[])
{
    jfieldID id = env->GetFieldID(clazz, fieldname, type);
    SkASSERT(id);
    return id;
}

int register_android_graphics_Graphics(JNIEnv* env)
{
    gRect_class = make_globalref(env, "android/graphics/Rect");
    gRect_leftFieldID = getFieldIDCheck(env, gRect_class, "left", "I");
    gRect_topFieldID = getFieldIDCheck(env, gRect_class, "top", "I");
    gRect_rightFieldID = getFieldIDCheck(env, gRect_class, "right", "I");
    gRect_bottomFieldID = getFieldIDCheck(env, gRect_class, "bottom", "I");

    gPoint_class = make_globalref(env, "android/graphics/Point");
    gPoint_xFieldID = getFieldIDCheck(env, gPoint_class, "x", "I");
    gPoint_yFieldID = getFieldIDCheck(env, gPoint_class, "y", "I");

    gBitmap_class = make_globalref(env, "android/graphics/Bitmap");
    gBitmap_nativeInstanceID = getFieldIDCheck(env, gBitmap_class, "mNativeBitmap", "I");    
    gBitmap_constructorMethodID = env->GetMethodID(gBitmap_class, "<init>",
                                            "(IZ[BI)V");

    gCanvas_class = make_globalref(env, "android/graphics/Canvas");
    gCanvas_nativeInstanceID = getFieldIDCheck(env, gCanvas_class, "mNativeCanvas", "I");

    gPicture_class = make_globalref(env, "android/graphics/Picture");
    gPicture_nativeInstanceID = getFieldIDCheck(env, gPicture_class, "mNativePicture", "I");

    gRegion_class = make_globalref(env, "android/graphics/Region");
    gRegion_nativeInstanceID = getFieldIDCheck(env, gRegion_class, "mNativeRegion", "I");
    gRegion_constructorMethodID = env->GetMethodID(gRegion_class, "<init>",
        "(II)V");
}
