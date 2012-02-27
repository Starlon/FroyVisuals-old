package com.starlon.froyvisuals;

import android.content.Context;
import android.graphics.Bitmap;



public class NativeHelper
{
    // VisInput native calls
    public static native int inputCount();
    public static native int inputGetCurrent();
    public static native boolean inputSetCurrent(int index);
    public static native String inputGetName(int index);
    public static native String inputGetLongName(int index);
    public static native String inputGetAuthor(int index);
    public static native String inputGetVersion(int index);
    public static native String inputGetAbout(int index);
    public static native String inputGetHelp(int index);
    public static native String inputGetLicense(int index);

    // VisMorph native calls
    public static native int morphCount();
    public static native int morphGetCurrent();
    public static native boolean morphSetCurrent(int index);
    public static native String morphGetName(int index);
    public static native String morphGetLongName(int index);
    public static native String morphGetAuthor(int index);
    public static native String morphGetVersion(int index);
    public static native String morphGetAbout(int index);
    public static native String morphGetHelp(int index);
    public static native String morphGetLicense(int index);

    // VisActor native calls
    public static native int actorCount();
    public static native int actorGetCurrent();
    public static native boolean actorSetCurrent(int index);
    public static native String actorGetName(int index);
    public static native String actorGetLongName(int index);
    public static native String actorGetAuthor(int index);
    public static native String actorGetVersion(int index);
    public static native String actorGetAbout(int index);
    public static native String actorGetHelp(int index);
    public static native String actorGetLicense(int index);

    public static native void cycleInput(int prev);
    public static native void cycleMorph(int prev);
    public static native void cycleActor(int prev);
    public static native void finalizeSwitch(int prev);
    public static native void updatePlugins();
    public static native void setMorphStyle(boolean style);
    public static native boolean render(Bitmap  bitmap);
    public static native void resizePCM(int size, int rate, int channels, int encoding);
    public static native void uploadAudio(short[] data);
    public static native void initApp(int w, int h, int device, int card);
    public static native void mouseMotion(float x, float y);
    public static native void mouseButton(int button, float x, float y);
    public static native void screenResize(int w, int h);
    public static native void visualsQuit();

    /** constructor */
    public NativeHelper(Context c)
    {
    }
}
