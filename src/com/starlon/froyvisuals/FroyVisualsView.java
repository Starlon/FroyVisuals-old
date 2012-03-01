package com.starlon.froyvisuals;

import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Typeface;
import android.graphics.Color;
import android.graphics.Paint;

import android.opengl.GLU;
import android.opengl.GLSurfaceView.Renderer;
import android.opengl.GLUtils;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

import java.util.Timer;
import java.util.TimerTask;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;


class FroyVisualsView extends GLSurfaceView {
    private static final String TAG = "FroyVisuals/FroyVisualsView";
    private NativeHelper mNativeHelper;
    private FroyVisuals mActivity;
    private Stats mStats;
    private float mLastX = -1.0f;
    private float mLastY = -1.0f;
    private int mSize = 0;
    private int direction = -1;

    //AudioRecord recorder = findAudioRecord();
    public FroyVisualsView(Context context) {
        super(context);

        mActivity = (FroyVisuals)context;

        init(false, 0, 0);
    }

    public FroyVisualsView(Context context, boolean translucent, int depth, int stencil) {
        super(context);

        mActivity = (FroyVisuals)context;

        translucent = true;
        init(translucent, depth, stencil);
    }


    private void init(boolean translucent, int depth, int stencil)
    {
        if (translucent) {
            this.getHolder().setFormat(PixelFormat.TRANSLUCENT);
        }

    }

    @Override
    public void onPause()
    {
        super.onPause();    
    }

    @Override
    public void onResume()
    {
        super.onResume();
    }

    @Override public boolean onTouchEvent (MotionEvent event) 
    {
        int action = event.getAction();
        switch(action)
        {
            case MotionEvent.ACTION_DOWN:
                Log.w(TAG, "MotionEvent.ACTION_DOWN");
                direction = -1;
                mLastX = -1;
                mSize = 0;
            break;
            case MotionEvent.ACTION_UP:
                Log.w(TAG, "MotionEvent.ACTION_UP direction=" + direction);
                if(direction >= 0) {
                    Log.w(TAG, "Switching actor: " + direction);
                    mNativeHelper.finalizeSwitch(direction);
                }
            break;
            case MotionEvent.ACTION_MOVE:
                mSize = mSize + 1;
                float x = event.getX();
                float y = event.getY();
                if(mSize > 2 && mLastX != -1.0f)
                {
                    if(mLastX < x)
                    {
                        direction = 0;
                    }
                    else
                    {
                        direction = 1;
                    }
                }
                mLastX = x;
                Log.w(TAG, "MotionEvent.ACTION_MOVE x=" + x + " y=" + y + " size=" + mSize + " direction=" + direction);
                mNativeHelper.mouseMotion(x, y);
            break;
        }
        return true;    
    }
}





        

