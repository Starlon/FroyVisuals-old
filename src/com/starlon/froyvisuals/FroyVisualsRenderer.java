package com.starlon.froyvisuals;

import android.os.SystemClock;
import android.content.Context;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Typeface;
import android.graphics.Color;
import android.graphics.Paint;

import android.opengl.GLSurfaceView;
import android.opengl.GLU;
import android.opengl.GLSurfaceView.Renderer;
import android.opengl.GLUtils;
import android.opengl.GLES20;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.opengles.GL11;
import javax.microedition.khronos.opengles.GL11Ext;

import java.util.Timer;
import java.util.TimerTask;

import java.io.IOException;
import java.io.InputStream;
import java.nio.IntBuffer;
import java.nio.ByteOrder;


public class FroyVisualsRenderer implements Renderer {
    private Bitmap mBitmap;
    private NativeHelper mNativeHelper;
    private Stats mStats;
    private Canvas mCanvas;
    private Paint mPaint;
    private FroyVisuals mActivity;
    private int mW;
    private int mH;
    private int mTextureId;
    private IntBuffer mPixelBuffer;

    public FroyVisualsRenderer(Context context) {
        mActivity = (FroyVisuals)context;
    }

    private void initGl(GL11 gl, int surfaceWidth, int surfaceHeight) {
        gl.glShadeModel(GL11.GL_FLAT);
        gl.glFrontFace(GL11.GL_CCW);

        gl.glEnable(GL11.GL_TEXTURE_2D);

        gl.glMatrixMode(GL11.GL_PROJECTION);
        gl.glPushMatrix();
        gl.glLoadIdentity();
        gl.glOrthof(0.0f, surfaceWidth, 0.0f, surfaceHeight, 0.0f, 1.0f);

        gl.glMatrixMode(GL10.GL_MODELVIEW);
        gl.glPushMatrix();
        gl.glLoadIdentity();
    }

    private void resetGl(GL11 gl) {
        gl.glMatrixMode(GL10.GL_PROJECTION);
        gl.glPopMatrix();
        gl.glMatrixMode(GL10.GL_MODELVIEW);
        gl.glPopMatrix();
    }

    private void releaseTexture(GL11 gl) {
        if (mTextureId != -1) {
            gl.glDeleteTextures(1, new int[] { mTextureId }, 0);
        }
    }

    private void initGlTexture(GL11 gl) {
        releaseTexture(gl);

        int[] textures = new int[1];
        gl.glGenTextures(1, textures, 0);
        mTextureId = textures[0];

        // we want to modify this texture so bind it
        gl.glBindTexture(GL11.GL_TEXTURE_2D, mTextureId);

        // GL_LINEAR gives us smoothing since the texture is larger than the screen
        gl.glTexParameterf(GL10.GL_TEXTURE_2D,
                           GL10.GL_TEXTURE_MAG_FILTER,
                           GL10.GL_LINEAR);
        gl.glTexParameterf(GL10.GL_TEXTURE_2D,
                           GL10.GL_TEXTURE_MIN_FILTER,
                           GL10.GL_LINEAR);
        // repeat the edge pixels if a surface is larger than the texture
        gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_S,
                           GL10.GL_CLAMP_TO_EDGE);
        gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_T,
                           GL10.GL_CLAMP_TO_EDGE);


        // and init the GL texture with the pixels
        gl.glTexImage2D(GL11.GL_TEXTURE_2D, 0, GL11.GL_RGB, mW, mH,
                0, GL11.GL_RGB, GL11.GL_UNSIGNED_BYTE, mPixelBuffer);

        // at this point, we are OK to further modify the texture
        // using glTexSubImage2D
    }

    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        // FPS stats
        mStats = new Stats();
        mStats.statsInit();

        mPaint = new Paint();

        final int delay = 0;
        final int period = 300;

        final Timer timer = new Timer();

        TimerTask task = new TimerTask() {
            public void run() {
                mActivity.mTextDisplay = mStats.getText();
            }
        };

        timer.scheduleAtFixedRate(task, delay, period);


        /*
         * By default, OpenGL enables features that improve quality
         * but reduce performance. One might want to tweak that
         * especially on software renderer.
         */
        gl.glDisable(GL10.GL_DITHER);

        /*
         * Some one-time OpenGL initialization can be made here
         * probably based on features of this particular context
         */
        gl.glHint(GL10.GL_PERSPECTIVE_CORRECTION_HINT,
                GL10.GL_FASTEST);

        GLES20.glEnable(GLES20.GL_TEXTURE_2D);
    }

    public void onDrawFrame(GL10 gl) {
        mStats.startFrame();

        GL11 gl11 = (GL11)gl;

        /*
         * By default, OpenGL enables features that improve quality
         * but reduce performance. One might want to tweak that
         * especially on software renderer.
         */
        gl.glDisable(GL10.GL_DITHER);

        gl.glTexEnvx(GL10.GL_TEXTURE_ENV, GL10.GL_TEXTURE_ENV_MODE,
                GL10.GL_MODULATE);

        /*
         * Usually, the first thing one might want to do is to clear
         * the screen. The most efficient way of doing this is to use
         * glClear().
         */

        gl.glColor4f(0f, 0f, 0f, 1f);
        gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);

        // Set the active texture unit to texture unit 0.
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);

        mBitmap = Bitmap.createBitmap(mW, mH, Bitmap.Config.RGB_565);
        mCanvas = new Canvas(mBitmap);


        if(!mNativeHelper.render(mBitmap)) return;

        //mCanvas.drawBitmap(mBitmap, 0, 0, null);

        String text = mActivity.mTextDisplay;
        text = "--------------------------------";
        if(text != null)
        {
            mPaint.setAntiAlias(true);
            mPaint.setTextSize(30);
            mPaint.setTypeface(Typeface.create(Typeface.SERIF, Typeface.ITALIC));
            mPaint.setStyle(Paint.Style.STROKE);
            mPaint.setStrokeWidth(1);
            mPaint.setColor(Color.WHITE);
    
            float canvasWidth = mCanvas.getWidth();
            float textWidth = mPaint.measureText(text);
            float startPositionX = (canvasWidth - textWidth / 2) / 2;
    
            mPaint.setTextAlign(Paint.Align.CENTER);
            mCanvas.drawText(text, startPositionX, mH-50, mPaint);
        }
 
        mPixelBuffer.rewind();
        mBitmap.copyPixelsToBuffer(mPixelBuffer);

        gl11.glBindTexture(GL11.GL_TEXTURE_2D, mTextureId);

        gl11.glTexSubImage2D(GL11.GL_TEXTURE_2D, 0, 0, 0, mW, mH,
                           GL11.GL_RGB, GL11.GL_UNSIGNED_BYTE, mPixelBuffer);

        // Draw the texture on the surface
        ((GL11Ext) gl11).glDrawTexiOES(0, 0, 0, mW, mH);

        mBitmap.recycle();
        mStats.endFrame();
    }

    public void onSurfaceChanged(GL10 gl, int width, int height) {

        mW = width;
        mH = height;

        mPixelBuffer = IntBuffer.allocate(mW * mH);


        mNativeHelper.screenResize(width, height);

        resetGl((GL11)gl);

        initGl((GL11)gl, mW, mH);

        initGlTexture((GL11)gl);
    }

}


