// From AOSP: frameworks/base/core/jni/android/graphics/GraphicsJNI.h

#ifndef GraphicsJNI_DEFINED
#define GraphicsJNI_DEFINED

#include "SkPoint.h"
#include "SkRect.h"
#include "SkBitmap.h"
#include "SkCanvas.h"
#include "SkPicture.h"
#include <jni.h>

class GraphicsJNI {
public:
  // returns true if an exception is set (and dumps it out to the Log)
  static bool hasException(JNIEnv*);

  static void get_jrect(JNIEnv*, jobject jrect, int* L, int* T, int* R, int* B);
  static void irect_to_jrect(const SkIRect&, JNIEnv*, jobject jrect);
  static SkRect* jrect_to_rect(JNIEnv*, jobject jrect, SkRect*);
  static void ipoint_to_jpoint(const SkIPoint& point, JNIEnv*, jobject jpoint);
  static SkCanvas* getNativeCanvas(JNIEnv*, jobject canvas);
  static SkBitmap* getNativeBitmap(JNIEnv*, jobject bitmap);
  static SkPicture* getNativePicture(JNIEnv*, jobject picture);
  static SkRegion* getNativeRegion(JNIEnv*, jobject region);
  static jobject createBitmap(JNIEnv* env, SkBitmap* bitmap, bool isMutable,
                              jbyteArray ninePatch, int density = -1);
};

#endif
