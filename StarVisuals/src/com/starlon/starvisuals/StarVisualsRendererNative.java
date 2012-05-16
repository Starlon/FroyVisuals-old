package com.starlon.starvisuals;

import android.content.Context;
import android.util.Log;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Typeface;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Matrix;

import android.opengl.GLU;
import android.opengl.GLSurfaceView.Renderer;
import android.opengl.GLUtils;
import android.opengl.GLES20;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import java.util.Timer;
import java.util.TimerTask;

import java.nio.FloatBuffer;
import java.nio.ShortBuffer;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import org.libvisual.android.VisualObject;

public class StarVisualsRendererNative implements Renderer {
    public VisualObject mVisualObject;
    private int mSurfaceWidth;
    private int mSurfaceHeight;
    private Stats mStats;
    private StarVisuals mActivity;

    public StarVisualsRendererNative(Context context) {
        mStats = new Stats();
        mStats.statsInit();
        mActivity = (StarVisuals)context;
        mSurfaceWidth = 128;
        mSurfaceHeight = 128;
        String actor = mActivity.mActor;
        String input = mActivity.mInput;
        String morph = mActivity.mMorph;
        mVisualObject = new VisualObject(mSurfaceHeight, mSurfaceHeight, actor, input, morph);
    }

    public void destroy()
    {
        mStats = null;
        mActivity = null;
    }
    @Override
    public void onDrawFrame(GL10 gl10) {
        mStats.startFrame();
        //
        mStats.endFrame();
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int width, int height) {
        mVisualObject.onSizeChanged(width, height, mSurfaceWidth, mSurfaceHeight);
        mSurfaceWidth = width;
        mSurfaceHeight = height;
    }

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglconfig) {
        final int delay = 0;
        final int period = 300;

        final Timer timer = new Timer();

        TimerTask task = new TimerTask() {
            public void run() {
                mActivity.warn(mStats.getText(), true);
            }
        };

        timer.scheduleAtFixedRate(task, delay, period);

    }



}


