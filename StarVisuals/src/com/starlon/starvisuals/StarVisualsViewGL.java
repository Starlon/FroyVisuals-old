package com.starlon.starvisuals;

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
import java.util.concurrent.locks.ReentrantLock;
import java.util.concurrent.TimeUnit;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;


class StarVisualsViewGL extends GLSurfaceView {
    private static final String TAG = "StarVisuals/StarVisualsView";
    private StarVisuals mActivity;
    private Stats mStats;
    private float mLastX = -1.0f;
    private float mLastY = -1.0f;
    private int mSize = 0;
    private int direction = -1;
    private final ReentrantLock mLock = new ReentrantLock();
    public final Object mSynch = new Object();

    //AudioRecord recorder = findAudioRecord();
    public StarVisualsViewGL(Context context) {
        super(context);

        mActivity = (StarVisuals)context;

        init(true, 0, 0);
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
            break;
            case MotionEvent.ACTION_UP:
                Log.w(TAG, "MotionEvent.ACTION_UP direction=" + direction);
            break;
            case MotionEvent.ACTION_MOVE:
                float x = event.getX();
                float y = event.getY();
                Log.w(TAG, "MotionEvent.ACTION_MOVE x=" + x + " y=" + y);

                mLock.lock();
                //NativeHelper.mouseMotion(x, y);
                mLock.unlock();
            break;
        }
        return true;    
    }
}





        

