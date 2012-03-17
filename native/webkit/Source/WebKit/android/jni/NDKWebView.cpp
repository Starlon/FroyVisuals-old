/*
 * Copyright 2009, The Android Open Source Project
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define LOG_TAG "webcoreglue"

#include "config.h"

#include "BackForwardList.h"
#include "ChromeClientAndroid.h"
#include "ContextMenuClientAndroid.h"
#include "CookieClient.h"
#include "DeviceMotionClientAndroid.h"
#include "DeviceOrientationClientAndroid.h"
#include "DragClientAndroid.h"
#include "EditorClientAndroid.h"
#include "FocusController.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "FrameLoaderClientAndroid.h"
#include "FrameView.h"
#include "GraphicsContext.h"
#include "HistoryItem.h"
#include "InspectorClientAndroid.h"
#include "IntRect.h"
#include "JavaSharedClient.h"
#include "Page.h"
#include "PlatformGraphicsContext.h"
#include "ResourceRequest.h"
#include "ScriptController.h"
#include "SecurityOrigin.h"
#include "SelectionController.h"
#include "Settings.h"
#include "SharedBuffer.h"
#include "SkBitmap.h"
#include "SkCanvas.h"
#include "SkImageEncoder.h"
#include "SubstituteData.h"
#include "TimerClient.h"
#include "TextEncoding.h"
#include "WebCoreViewBridge.h"
#include "WebFrameView.h"
#include "WebViewCore.h"
#include "benchmark/Intercept.h"
#include "benchmark/MyJavaVM.h"
#include "EmptyClients.h"
#include "Text.h"
#include "wtf/text/CString.h"

#include "JNIHelp.h"
#include <JNIUtility.h>
#include <jni.h>
#include <utils/Log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

namespace android {

class MyJavaSharedClient : public TimerClient, public CookieClient {
public:
    MyJavaSharedClient() : m_hasTimer(false) {}
    virtual void setSharedTimer(long long timemillis) { m_hasTimer = true; }
    virtual void stopSharedTimer() { m_hasTimer = false; }
    virtual void setSharedTimerCallback(void (*f)()) { m_func = f; }
    virtual void signalServiceFuncPtrQueue() {}

    // Cookie methods that do nothing.
    virtual void setCookies(const KURL&, const String&) {}
    virtual String cookies(const KURL&) { return ""; }
    virtual bool cookiesEnabled() { return false; }

    bool m_hasTimer;
    void (*m_func)();
};

static void historyItemChanged(HistoryItem* i) {
    if (i->bridge())
        i->bridge()->updateHistoryItem(i);
}

class NativeView {
public:
  NativeView();
  ~NativeView();
  void load(const char* url);
  void draw(SkCanvas &canvas);

  MyJavaSharedClient client;
  ChromeClientAndroid* chrome;
  EditorClientAndroid* editor;
  DeviceMotionClientAndroid* deviceMotion;
  DeviceOrientationClientAndroid* deviceOrientation;

  WebCore::Page::PageClients pageClients;
  WebCore::Page* page;
  MyWebFrame* webFrame;
  FrameLoaderClientAndroid* loader;
  RefPtr<Frame> frame;
  WebViewCore* webViewCore;
  RefPtr<FrameView> frameView;
  WebFrameView* webFrameView;

  int scrollOffsetX;
  int scrollOffsetY;
  int count;
};

NativeView::NativeView()
{
    __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** NativeView::NativeView()");

    scrollOffsetX = 0;
    scrollOffsetY = 0;
    count = 0;

    ScriptController::initializeThreading();

    // Setting this allows data: urls to load from a local file.
    SecurityOrigin::setLocalLoadPolicy(SecurityOrigin::AllowLocalLoadsForAll);

    // Create the fake JNIEnv and JavaVM
    InitializeJavaVM();

    // The real function is private to libwebcore but we know what it does.
    notifyHistoryItemChanged = historyItemChanged;

    // Implement the shared timer callback
    JavaSharedClient::SetTimerClient(&client);
    JavaSharedClient::SetCookieClient(&client);
    __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** NativeView::NativeView() 1");

    // Create the page with all the various clients
    chrome = new ChromeClientAndroid;
    editor = new EditorClientAndroid;
    deviceMotion = new DeviceMotionClientAndroid;
    deviceOrientation = new DeviceOrientationClientAndroid;
    pageClients.chromeClient = chrome;
    pageClients.contextMenuClient = new ContextMenuClientAndroid;
    pageClients.editorClient = editor;
    pageClients.dragClient = new DragClientAndroid;
    pageClients.inspectorClient = new InspectorClientAndroid;
    pageClients.deviceMotionClient = deviceMotion;
    pageClients.deviceOrientationClient = deviceOrientation;
    __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** NativeView::NativeView() 2");
    page = new WebCore::Page(pageClients);
    __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** NativeView::NativeView() 3");
    editor->setPage(page);
    __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** NativeView::NativeView() 4");

    __android_log_print(ANDROID_LOG_DEBUG,
                        "NDKWebKit","**** JSC::Bindings::getJNIEnv() = %p",
                        JSC::Bindings::getJNIEnv());
    

    // Create MyWebFrame that intercepts network requests
    webFrame = new MyWebFrame(page);
    __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** NativeView::NativeView() 5");
    webFrame->setUserAgent("Performance testing"); // needs to be non-empty
    chrome->setWebFrame(webFrame);
    __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** NativeView::NativeView() 6");
    // ChromeClientAndroid maintains the reference.
    Release(webFrame);
    __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** NativeView::NativeView() 7");

    // Create the Frame and the FrameLoaderClient
    loader = new FrameLoaderClientAndroid(webFrame);
    frame = Frame::create(page, NULL, loader);
    loader->setFrame(frame.get());

    // Build our View system, resize it to the given dimensions and release our
    // references. Note: We keep a referenec to frameView so we can layout and
    // draw later without risk of it being deleted.
    webViewCore = new WebViewCore(JSC::Bindings::getJNIEnv(),
            MY_JOBJECT, frame.get());
    __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** NativeView::NativeView() 11");
    frameView = FrameView::create(frame.get());
    __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** NativeView::NativeView() 12");
    webFrameView = new WebFrameView(frameView.get(), webViewCore);
    __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** NativeView::NativeView() 13");
    frame->setView(frameView);
    // frameView->resize(width, height);
    Release(webViewCore);
    Release(webFrameView);

    // Initialize the frame and turn of low-bandwidth display (it fails an
    // assertion in the Cache code)
    frame->init();
    __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** NativeView::NativeView() 14");
    frame->selection()->setFocused(true);
    frame->page()->focusController()->setFocused(true);
    __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** NativeView::NativeView() 15");

    deviceMotion->setWebViewCore(webViewCore);
    deviceOrientation->setWebViewCore(webViewCore);

    // Set all the default settings the Browser normally uses.
    Settings* s = frame->settings();
#ifdef ANDROID_LAYOUT
    // s->setLayoutAlgorithm(Settings::kLayoutNormal); // Normal layout for now
#endif
    s->setStandardFontFamily("sans-serif");
    s->setFixedFontFamily("monospace");
    s->setSansSerifFontFamily("sans-serif");
    s->setSerifFontFamily("serif");
    s->setCursiveFontFamily("cursive");
    s->setFantasyFontFamily("fantasy");
    s->setMinimumFontSize(8);
    s->setMinimumLogicalFontSize(8);
    s->setDefaultFontSize(16);
    s->setDefaultFixedFontSize(13);
    s->setLoadsImagesAutomatically(true);
    s->setJavaScriptEnabled(true);
    s->setDefaultTextEncodingName("latin1");
    s->setPluginsEnabled(false);
    s->setShrinksStandaloneImagesToFit(false);
#ifdef ANDROID_LAYOUT
    // s->setUseWideViewport(false);
#endif
}

void NativeView::load(const char* url)
{
    // Finally, load the actual data
    ResourceRequest req(url);
    frame->loader()->load(req, false);

    // Layout the page and service the timer
    frame->view()->layout();
    while (client.m_hasTimer) {
      client.m_func();
      JavaSharedClient::ServiceFunctionPtrQueue();
    }
    JavaSharedClient::ServiceFunctionPtrQueue();

    // Layout more if needed.
    while (frame->view()->needsLayout())
      frame->view()->layout();
    JavaSharedClient::ServiceFunctionPtrQueue();
}

void NativeView::draw(SkCanvas &canvas)
{
    // Draw into an offscreen bitmap
    // SkBitmap bmp;
    // bmp.setConfig(SkBitmap::kARGB_8888_Config, width, height);
    // bmp.allocPixels();
    // SkCanvas canvas(bmp);
    PlatformGraphicsContext ctx(&canvas, NULL);
    GraphicsContext gc(&ctx);

    canvas.translate(-scrollOffsetX,-scrollOffsetY);

    int left = scrollOffsetX;
    int top = scrollOffsetY;
    // int right = left + frame->view()->width();
    // int bottom = top + frame->view()->height();

    top += 10;
    left += 10;
    // right -= 10;
    // bottom -= 10;

    int width = frame->view()->width();
    int height = frame->view()->height();
    width -= 20;
    height -= 20;

    frame->view()->paintContents(&gc, IntRect(left,top,width,height));

    // Write the bitmap to the sdcard
    // SkImageEncoder* enc = SkImageEncoder::Create(SkImageEncoder::kPNG_Type);
    // enc->encodeFile("/sdcard/webcore_test.png", bmp, 100);
    // delete enc;
}

NativeView::~NativeView()
{
  __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** NativeView::~NativeView() 1");
  // Tear down the world.
  frame->loader()->detachFromParent();
  __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** NativeView::~NativeView() 2");
  delete page;
  __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** NativeView::~NativeView() 3");
}




















jfieldID ptrField = 0;


// extern "C" {
// EXPORT void Java_net_sourceforge_ndkwebkit_NDKView_nativeInit
//   (JNIEnv *, jobject);
// EXPORT void Java_net_sourceforge_ndkwebkit_NDKView_nativeDestroy
//   (JNIEnv *, jobject);
// EXPORT void Java_net_sourceforge_ndkwebkit_NDKView_nativeDraw
//   (JNIEnv *, jobject, jobject);
// EXPORT void Java_net_sourceforge_ndkwebkit_NDKView_nativeSetSizeScreenWidthAndScale
//   (JNIEnv *, jobject, jint, jint, jint, jfloat, jint, jint, jint, jint, jboolean);
// EXPORT void Java_net_sourceforge_ndkwebkit_NDKView_nativeSetScrollOffset
//   (JNIEnv *, jobject, jint, jboolean, jint, jint);
// EXPORT void Java_net_sourceforge_ndkwebkit_NDKView_nativeSetGlobalBounds
//   (JNIEnv *, jobject, jint, jint, jint, jint);
// EXPORT void Java_net_sourceforge_ndkwebkit_NDKView_executeJavaScript
//   (JNIEnv *, jobject, jstring);
// EXPORT void Java_net_sourceforge_ndkwebkit_NDKView_test
//   (JNIEnv *, jobject);
// }

void NDKView_nativeInit(JNIEnv *env, jobject obj)
{
  __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** nativeInit 1");

  jclass cls = env->FindClass("net/sourceforge/ndkwebkit/NDKView");
  __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** cls = %p",cls);
  ptrField = env->GetFieldID(cls, "nativePointer", "I");
  env->DeleteLocalRef(cls);
  __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** ptrField = %p",ptrField);


  NativeView *view = new NativeView();
  view->load("file:///sdcard/Documents/test.html");

  env->SetIntField(obj,ptrField,(int)view);
  __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** nativeInit done");
}

void NDKView_nativeDestroy(JNIEnv *env, jobject obj)
{
  __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** nativeDestroy 1");
  NativeView *view = (NativeView*)env->GetIntField(obj,ptrField);
  delete view;
  __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** nativeDestroy 2");
}

void NDKView_nativeDraw(JNIEnv *env, jobject obj, jobject surface)
{
  __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** nativeDraw 1");
  NativeView *view = (NativeView*)env->GetIntField(obj,ptrField);
  ANativeWindow *window = ANativeWindow_fromSurface(env,surface);
  ANativeWindow_Buffer buffer;
  bzero(&buffer,sizeof(buffer));
  int width = ANativeWindow_getWidth(window);
  int height = ANativeWindow_getHeight(window);
  ARect dirty;
  dirty.left = 0;
  dirty.top = 0;
  dirty.right = width;
  dirty.bottom = height;
  int r;
  if (0 != (r = ANativeWindow_lock(window,&buffer,&dirty))) {
    __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** ANativeWindow_lock failed: %d",r);
    ANativeWindow_release(window);
    return;
  }

  __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** nativeDraw 2");
  __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** format = %d",buffer.format);
  unsigned char *pixels = (unsigned char*)buffer.bits;
  memset(pixels,0,width*height*2);
#if 0
  int i;
  for (i = 10000*3; i < 20000*3; i++) {
    pixels[i] = 255;
  }
  for (i = 20000*3; i < 30000*3; i += 3) {
    pixels[i] = 255;
  }
  __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** nativeDraw 3");
#endif

  SkBitmap bmp;
  if ((buffer.format == WINDOW_FORMAT_RGBA_8888) ||
      (buffer.format == WINDOW_FORMAT_RGBX_8888))
    bmp.setConfig(SkBitmap::kARGB_8888_Config, width, height);
  else
    bmp.setConfig(SkBitmap::kRGB_565_Config, width, height);
  bmp.setPixels(pixels,NULL);
  SkCanvas canvas(bmp);
  view->draw(canvas);

  // SkPaint paint;
  // paint.setARGB(255,255,0,0);
  // paint.setStrokeWidth(4);
  // canvas.drawLine(0,0,width,height,paint);

  if (0 != (r = ANativeWindow_unlockAndPost(window))) {
    __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit",
                        "**** ANativeWindow_unlockAndPost failed: %d",r);
    ANativeWindow_release(window);
    return;
  }
  __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** nativeDraw 4");
  ANativeWindow_release(window);
}

void NDKView_nativeSetSizeScreenWidthAndScale
  (JNIEnv *env , jobject obj, jint width, jint height, jint textWrapWidth, jfloat scale,
   jint screenWidth, jint screenHeight, jint anchorX, jint anchorY, jboolean ignoreHeight)
{
    NativeView *view = (NativeView*)env->GetIntField(obj,ptrField);
    view->webViewCore->setSizeScreenWidthAndScale(width,height,textWrapWidth,scale,
                                                  screenWidth,screenHeight,anchorX,anchorY,
                                                  ignoreHeight);
}

void NDKView_nativeSetScrollOffset(JNIEnv *env, jobject obj, jint moveGeneration,
                                   jboolean sendScrollEvent, jint dx, jint dy)
{
    NativeView *view = (NativeView*)env->GetIntField(obj,ptrField);
    // view->webViewCore->setScrollOffset(moveGeneration,sendScrollEvent,dx,dy);
    // view->frameView->setScrollPosition(IntPoint(dx,dy));
    view->scrollOffsetX = dx;
    view->scrollOffsetY = dy;
    __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** nativeSetScrollOffset %d %d",dx,dy);
}

void NDKView_nativeSetGlobalBounds(JNIEnv *env, jobject obj, jint x, jint y, jint h, jint v)
{
    // NativeView *view = (NativeView*)env->GetIntField(obj,ptrField);
    // view->webViewCore->setGlobalBounds(x,y,h,v);
}

void NDKView_executeJavaScript(JNIEnv *env, jobject obj, jstring script)
{
  NativeView *view = (NativeView*)env->GetIntField(obj,ptrField);
}

void NDKView_test(JNIEnv *env, jobject obj)
{
  __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** test");
  NativeView *view = (NativeView*)env->GetIntField(obj,ptrField);
  __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** count = %d",view->count);
  RefPtr<Document> doc = view->frame->document();
  RefPtr<Element> docElem = doc->documentElement();
  RefPtr<Node> child = docElem->firstChild();
  while ((child != NULL) && (child->nodeName().upper() != "BODY")) {
    child = child->nextSibling();
    const char *name = child->nodeName().ascii().data();
    __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** skipping %s",name);
  }
  if (child != NULL) {
    RefPtr<Node> textNode = child->firstChild();
    RefPtr<Text> text = (Text*)textNode.get();
    const char *oldValue = text->data().ascii().data();
    __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** oldValue = %s",oldValue);
    ExceptionCode ec = 0;
    text->appendData(" F",ec);
    __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** ec = %d",ec);
    const char *newValue = text->data().ascii().data();
    __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** newValue = %s",newValue);

    while (view->frame->view()->needsLayout()) {
      __android_log_print(ANDROID_LOG_DEBUG,"NDKWebKit","**** doing relayout");
      view->frame->view()->layout();
    }

    view->count++;
  }
}

static JNINativeMethod gJavaNDKWebViewMethods[] = {
    { "nativeInit", "()V",
            (void*) NDKView_nativeInit },
    { "nativeDestroy", "()V",
            (void*) NDKView_nativeDestroy },
    { "nativeDraw", "(Landroid/view/Surface;)V",
            (void*) NDKView_nativeDraw },
    { "nativeSetSizeScreenWidthAndScale", "(IIIFIIIIZ)V",
            (void*) NDKView_nativeSetSizeScreenWidthAndScale },
    { "nativeSetScrollOffset", "(IZII)V",
            (void*) NDKView_nativeSetScrollOffset },
    { "nativeSetGlobalBounds", "(IIII)V",
            (void*) NDKView_nativeSetGlobalBounds },
    { "executeJavaScript", "(Ljava/lang/String;)V",
            (void*) NDKView_executeJavaScript },
    { "test", "()V",
            (void*) NDKView_test },
};

int registerNDKWebView(JNIEnv* env)
{
    return jniRegisterNativeMethods(env, "net/sourceforge/ndkwebkit/NDKView",
            gJavaNDKWebViewMethods, NELEM(gJavaNDKWebViewMethods));
}

}  // namespace android
