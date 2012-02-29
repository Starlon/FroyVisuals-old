package com.starlon.froyvisuals;

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
import javax.microedition.khronos.opengles.GL11;
import javax.microedition.khronos.opengles.GL11Ext;

import java.util.Timer;
import java.util.TimerTask;

//import java.io.IOException;
//import java.io.InputStream;
import java.nio.ShortBuffer;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class FroyVisualsRenderer implements Renderer {
    private Visual vis;
    private int mSurfaceWidth;
    private int mSurfaceHeight;
    private Stats mStats;
    private FroyVisuals mActivity;
    private NativeHelper mNativeHelper;
    private boolean mInited = false;

    public FroyVisualsRenderer(Context context) {
        vis = new Visual();
        mStats = new Stats();
        mStats.statsInit();
        mActivity = (FroyVisuals)context;
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        mStats.startFrame();
        vis.performFrame((GL11) gl10, mSurfaceWidth, mSurfaceHeight);
        mStats.endFrame();
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int width, int height) {
        vis.initialize(width, height, (GL11) gl10, mActivity);
        mSurfaceWidth = width;
        mSurfaceHeight = height;
    }

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglconfig) {
        if (! (gl10 instanceof GL11Ext)) {
            throw new RuntimeException("GL11Ext not supported");
        }

        // FPS stats

        final int delay = 0;
        final int period = 300;

        final Timer timer = new Timer();

        TimerTask task = new TimerTask() {
            public void run() {
                mActivity.mTextDisplay = mStats.getText();
            }
        };

        timer.scheduleAtFixedRate(task, delay, period);

    }



}

final class Visual {
    private int mTextureWidth;
    private int mTextureHeight;
    //private static final int seedLines = 3;
    //private byte[] intensityMap;
    private ByteBuffer mPixelBuffer;
    private static final int bytesPerPixel = 4;
    //private int[] colors;
    private int mTextureId = -1;
    private int[] textureCrop = new int[4]; 
    private boolean glInited = false;
    //private static final int iterationsPerFrame = 1;
    //private long lastSeedTime = -1L;
    //private static final long targetFrameInterval = 1000L / 10L; // target 10 FPS 
    //private static final long seedInterval = 175L;
    //private long lastFpsTime = -1L;
    //private static final long fpsInterval = 1000L * 5;
    //private int frameCounter = 0;
    private NativeHelper mNativeHelper;
    private FroyVisuals mActivity;

    public Visual() {
    }

    public void initialize(int surfaceWidth, int surfaceHeight, GL11 gl, FroyVisuals activity) {

        mActivity = activity;

        // TODO: choose values smarter
        // but remember they have to be powers of 2
        if (surfaceWidth < surfaceHeight) {
            mTextureWidth = 256;
            mTextureHeight = 256;
        } else {
            mTextureWidth = 256;
            mTextureHeight = 256;
        }

        mNativeHelper.initApp(mTextureWidth, mTextureHeight, 0, 0);

        textureCrop[0] = 0;
        textureCrop[1] = 0;
        textureCrop[2] = mTextureWidth;
        textureCrop[3] = mTextureHeight;
        
        // init the intensity map
        //intensityMap = new byte[mTextureWidth * mTextureHeight];

        // init the pixel buffer
        mPixelBuffer = ByteBuffer.allocate(mTextureWidth * mTextureHeight * bytesPerPixel);

        // init the GL settings
        if (glInited) {
            resetGl(gl);
        }

        initGl(gl, surfaceWidth, surfaceHeight);

        // init the GL texture
        initGlTexture(gl);
    }   

    private void resetGl(GL11 gl) {
        gl.glMatrixMode(GL10.GL_PROJECTION);
        gl.glPopMatrix();
        gl.glMatrixMode(GL10.GL_MODELVIEW);
        gl.glPopMatrix();               
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
        glInited = false;
    }

    public void updatePixels()
    {

        Bitmap bitmap = Bitmap.createBitmap(mTextureWidth, mTextureHeight, Bitmap.Config.ARGB_8888);

        Canvas canvas = new Canvas(bitmap);

        Paint paint = new Paint();
        paint.setAntiAlias(true);
        paint.setTextSize(10);
        paint.setTypeface(Typeface.create(Typeface.SERIF, Typeface.ITALIC));
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(1);
        paint.setColor(Color.WHITE);

        if(!mNativeHelper.render(bitmap)) return;

        String text = mActivity.mTextDisplay;
        if(text != null)
        {
    
            float canvasWidth = canvas.getWidth();
            float textWidth = paint.measureText(text);
            float startPositionX = (canvasWidth - textWidth / 2) / 2;
    
            paint.setTextAlign(Paint.Align.CENTER);
            canvas.drawText(text, startPositionX, mTextureWidth-50, paint);
        }

        // We need to flip the textures vertically:
        Matrix flip = new Matrix();
        flip.postScale(1f, -1f);

        Bitmap temp = Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), flip, true);

        mPixelBuffer.rewind();

        temp.copyPixelsToBuffer(mPixelBuffer);

        temp.recycle();
        bitmap.recycle();
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

        // now, let's init the texture with pixel values
        //updatePixels();

        // and init the GL texture with the pixels
        gl.glTexImage2D(GL11.GL_TEXTURE_2D, 0, GL11.GL_RGBA, mTextureWidth, mTextureHeight,
                0, GL11.GL_RGBA, GL11.GL_UNSIGNED_BYTE, mPixelBuffer);        

        // at this point, we are OK to further modify the texture
        // using glTexSubImage2D
    }

    

    public void dispose(GL11 gl) {
        releaseTexture(gl);
    }

    public void performFrame(GL11 gl, int surfaceWidth, int surfaceHeight) {

        updatePixels();

        // Clear the surface
        gl.glClearColorx(0, 0, 0, 0);
        gl.glClear(GL11.GL_COLOR_BUFFER_BIT);

        // Choose the texture
        gl.glBindTexture(GL11.GL_TEXTURE_2D, mTextureId);

        // Update the texture
        gl.glTexSubImage2D(GL11.GL_TEXTURE_2D, 0, 0, 0, mTextureWidth, mTextureHeight, 
                           GL11.GL_RGBA, GL11.GL_UNSIGNED_BYTE, mPixelBuffer);

        // Draw the texture on the surface
        gl.glTexParameteriv(GL10.GL_TEXTURE_2D, GL11Ext.GL_TEXTURE_CROP_RECT_OES, textureCrop, 0);

        ((GL11Ext) gl).glDrawTexiOES(0, 0, 0, surfaceWidth, surfaceHeight);

    }
}
