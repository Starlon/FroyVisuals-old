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
import java.nio.ShortBuffer;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import java.util.Random;


import android.util.Log;

public class FroyVisualsRenderer implements GLSurfaceView.Renderer {
    private Visual vis;
    private int mSurfaceWidth;
    private int mSurfaceHeight;
    private Stats mStats;
    private FroyVisuals mActivity;
    private NativeHelper mNativeHelper;
    private boolean mInited = false;

    public FroyVisualsRenderer(Context context) {
        vis = new Visual();
        //mStats = new Stats();
        //mStats.statsInit();
        mActivity = (FroyVisuals)context;
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        //mStats.startFrame();
        vis.performFrame((GL11) gl10, mSurfaceWidth, mSurfaceHeight);
        //mStats.endFrame();
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int width, int height) {
        vis.initialize(width, height, (GL11) gl10);
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
                //mActivity.mTextDisplay = mStats.getText();
            }
        };

        timer.scheduleAtFixedRate(task, delay, period);

    }



}

final class Visual {
    private static Random random = new Random();
    private int mTextureWidth;
    private int mTextureHeight;
    private static final int seedLines = 3;
    private byte[] intensityMap;
    private ByteBuffer mPixelBuffer;
    private static final int bytesPerPixel = 4;
    private int[] colors;
    private int mTextureId = -1;
    private int[] textureCrop = new int[4]; 
    private boolean glInited = false;
    private static final int iterationsPerFrame = 1;
    private long lastSeedTime = -1L;
    private static final long targetFrameInterval = 1000L / 10L; // target 10 FPS 
    private static final long seedInterval = 175L;
    private long lastFpsTime = -1L;
    private static final long fpsInterval = 1000L * 5;
    private int frameCounter = 0;
    private NativeHelper mNativeHelper;

    public Visual() {
        //generateColors();
    }

/*
    private void generateColors() {
        colors = new int[256];
        float gg = 200.0f;

        for (int i = 0xff; i >= 0x00; i--) {
            int r = 0;
            r = i << 16;
            int g = 0;

            if (gg > 0) {
                g = ((int) gg) << 8;
                if (i < 0xe0) {
                    gg -= 2.0f;
                } else {
                    gg -= 1.0f;
                }
            }

            int b = 0;

            colors[i] = (r | g | b);
        }
    }   
*/

    public void initialize(int surfaceWidth, int surfaceHeight, GL11 gl) {
        // TODO: choose values smarter
        // but remember they have to be powers of 2
        if (surfaceWidth < surfaceHeight) {
            mTextureWidth = 256;
            mTextureHeight = 64;
        } else {
            mTextureWidth = 64;
            mTextureHeight = 256;
        }

        mNativeHelper.initApp(mTextureWidth, mTextureHeight, 0, 0);

        textureCrop[0] = 0;
        textureCrop[1] = seedLines;
        textureCrop[2] = mTextureWidth;
        textureCrop[3] = mTextureHeight;
        
        // init the intensity map
        intensityMap = new byte[mTextureWidth * mTextureHeight];

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

    private static int nextRandomInt(int max) {
        return max == 0 ? 0 : random.nextInt(max);
    }

    private void seedIntensity() {
        int y = intensityMap.length - mTextureWidth;

        for (int line = 0; line < seedLines; ++line) {
            boolean on = random.nextBoolean();
            for (int x = 0; x < mTextureWidth; ++x) {
                // magic settings for good looking fire             
                if ((on && nextRandomInt(20 / (line + 1)) == 0) 
                        || ((! on) && nextRandomInt(9) == 0)) {
                    on = ! on;
                }               

                intensityMap[y + x] = (byte) (on ? 0xff : 0x00);
            }

            y -= mTextureWidth;

        }

    }

    private void iterateIntensity() {
        int y = intensityMap.length - mTextureWidth * 2;

        int v1, v2, v3, v4;

        while (y > 0) {
            for (int x = 0; x < mTextureWidth; ++x) {
                // now take the current value, the values from both sides
                // and the one from the bottom              
                v1 = unsignedByte(intensityMap[y + x]);
                if (x < mTextureWidth - 1) {
                    v2 = unsignedByte(intensityMap[y + x + 1]);
                } else {
                    v2 = 0;
                }

                if (x > 0) {
                    v3 = unsignedByte(intensityMap[y + x - 1]);
                } else {
                    v3 = 0;
                }

                v4 = unsignedByte(intensityMap[y + mTextureWidth + x]);

                int v = (v1 + v2 + v3 + v4) / (((x != 0) && (x != mTextureWidth - 1)) ? 4 : 3);
                // magic values - needed for good decay
                v = v - 2 * (255 - v) / 128;

                // now clip the value
                if (v > 0xff) {
                    v = 0xff;
                } else if (v < 0x00) {
                    v = 0;
                }

                intensityMap[y + x] = (byte) v;
            }

            y -= mTextureWidth;
        }
    }

    private static int unsignedByte(byte v) {
        return (v >= 0 ? v : (256 + v));
    }

    public void updatePixels()
    {
        Bitmap bitmap = Bitmap.createBitmap(mTextureWidth, mTextureHeight, Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(bitmap);

        Paint paint = new Paint();
        paint.setAntiAlias(true);
        paint.setTextSize(30);
        paint.setTypeface(Typeface.create(Typeface.SERIF, Typeface.ITALIC));
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(1);
        paint.setColor(Color.WHITE);

        if(!mNativeHelper.render(bitmap)) return;

        //String text = mActivity.mTextDisplay;
/*
        String text = "HELLO WORLD";
        if(text != null)
        {
    
            float canvasWidth = canvas.getWidth();
            float textWidth = paint.measureText(text);
            float startPositionX = (canvasWidth - textWidth / 2) / 2;
    
            paint.setTextAlign(Paint.Align.CENTER);
            canvas.drawText(text, startPositionX, mTextureWidth-50, paint);
        }
*/

        mPixelBuffer.rewind();
        bitmap.copyPixelsToBuffer(mPixelBuffer);
        bitmap.recycle();
    }

   
/*

    private void updatePixels() {

        mPixelBuffer.rewind();



        // we need to output the pixels upside down due to glDrawTex peculiarities

        for (int y = intensityMap.length - mTextureWidth; y > 0; y -= mTextureWidth) {

            for (int x = 0; x < mTextureWidth; ++x) {

                int pixel = pixelFromIntensity(intensityMap[y + x]);

                mPixelBuffer.put((byte) (pixel >> 16));

                mPixelBuffer.put((byte) ((pixel >> 8) & 0xff));

                mPixelBuffer.put((byte) (pixel & 0xff));

            }

        }

    }
*/
    

/*
    private int pixelFromIntensity(byte intensity) {

        return colors[unsignedByte(intensity)];

    }
*/


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
        long frameStartTime = System.currentTimeMillis();
        if (lastSeedTime == -1L || (frameStartTime - lastSeedTime) >= seedInterval) {
            seedIntensity();
            lastSeedTime = frameStartTime;
        }
        
        for (int i = 0; i < iterationsPerFrame; ++i) {
            iterateIntensity();
        }

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

        // Sleep the extra time

        long frameEndTime = System.currentTimeMillis();
        long delta = frameEndTime - frameStartTime;

        if (targetFrameInterval - delta > 10L) {
            try {
                Thread.sleep(targetFrameInterval - delta);
            } catch (InterruptedException e) {}
        }

        // Output FPS if necessary
        frameCounter++;
        if (lastFpsTime == -1L) {
            lastFpsTime = frameEndTime;
        } else if ((frameEndTime - lastFpsTime) >= fpsInterval) {
            float fps = frameCounter / ((frameEndTime - lastFpsTime) / 1000.0f);
            Log.d("FPS", String.format("%1.0f", fps));
            
            frameCounter = 0;
            lastFpsTime = frameEndTime;
        }
    }
}
