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

import java.nio.FloatBuffer;
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
    private ByteBuffer mPixelBuffer;
    private static final int bytesPerPixel = 4;
    private int mTextureId = -1;
    private int[] textureCrop = new int[4]; 
    private boolean glInited = false;
    private NativeHelper mNativeHelper;
    private FroyVisuals mActivity;
    private Canvas mCanvas;
    private Bitmap mBitmap;
    private Paint mPaint;

    private FloatBuffer vertexBuffer;   // buffer holding the vertices
    private float vertices[] = {
            -1.0f, -1.0f,  0.0f,        // V1 - bottom left
            -1.0f,  1.0f,  0.0f,        // V2 - top left
             1.0f, -1.0f,  0.0f,        // V3 - bottom right
             1.0f,  1.0f,  0.0f         // V4 - top right
    };

    private FloatBuffer textureBuffer;  // buffer holding the texture coordinates
    private float texture[] = {         
            // Mapping coordinates for the vertices
            0.0f, 1.0f,     // top left     (V2)
            0.0f, 0.0f,     // bottom left  (V1)
            1.0f, 1.0f,     // top right    (V4)
            1.0f, 0.0f      // bottom right (V3)
    };

    public Visual() {
        mCanvas = new Canvas();

        // a float has 4 bytes so we allocate for each coordinate 4 bytes
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(vertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        
        // allocates the memory from the byte buffer
        vertexBuffer = byteBuffer.asFloatBuffer();
        
        // fill the vertexBuffer with the vertices
        vertexBuffer.put(vertices);
        
        // set the cursor position to the beginning of the buffer
        vertexBuffer.position(0);
        
        byteBuffer = ByteBuffer.allocateDirect(texture.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        textureBuffer = byteBuffer.asFloatBuffer();
        textureBuffer.put(texture);
        textureBuffer.position(0);

    }

    public void initialize(int surfaceWidth, int surfaceHeight, GL11 gl, FroyVisuals activity) {

        mActivity = activity;

        mTextureWidth = 256;
        mTextureHeight = 256;

        mNativeHelper.initApp(mTextureWidth, mTextureHeight, 0, 0);

        textureCrop[0] = 0;
        textureCrop[1] = 0;
        textureCrop[2] = mTextureWidth;
        textureCrop[3] = mTextureHeight;
        
        mBitmap = Bitmap.createBitmap(mTextureWidth, mTextureHeight, Bitmap.Config.ARGB_8888);

        mPaint = new Paint();

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
        gl.glMatrixMode(GL10.GL_TEXTURE);
        gl.glPopMatrix();
        gl.glMatrixMode(GL10.GL_MODELVIEW);
        gl.glPopMatrix();
        glInited = false;
    }
    

    private void initGl(GL11 gl, int surfaceWidth, int surfaceHeight) {

        gl.glShadeModel(GL11.GL_FLAT);
        gl.glFrontFace(GL11.GL_CCW);
        gl.glEnable(GL11.GL_TEXTURE_2D);

        gl.glMatrixMode(GL11.GL_PROJECTION);
        gl.glLoadIdentity();
        gl.glPushMatrix();

        gl.glMatrixMode(GL10.GL_MODELVIEW);
        gl.glLoadIdentity();
        gl.glOrthof(-1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f);
        gl.glPushMatrix();

        glInited = true;
    }

    public void updatePixels()
    {
        // Fill the bitmap with black.
        mBitmap.eraseColor(Color.BLACK);

        // Pass bitmap to be rendered by native function.
        if(!mNativeHelper.render(mBitmap)) return;


        // If FroyVisuals has text to display, then use a canvas and paint brush to display it.
        String text = mActivity.mTextDisplay;
        if(text != null)
        {
            // Give the bitmap a canvas so we can draw on it.
            mCanvas.setBitmap(mBitmap);
    
            mPaint.setAntiAlias(true);
            mPaint.setTextSize(10);
            mPaint.setTypeface(Typeface.create(Typeface.SERIF, Typeface.ITALIC));
            mPaint.setStyle(Paint.Style.STROKE);
            mPaint.setStrokeWidth(1);
            mPaint.setColor(Color.WHITE);
            mPaint.setTextAlign(Paint.Align.CENTER);

            float canvasWidth = mCanvas.getWidth();
            float textWidth = mPaint.measureText(text);
            float startPositionX = (canvasWidth - textWidth / 2) / 2;
    
            mCanvas.drawText(text, startPositionX, mTextureWidth-12, mPaint);
        }

        // Copy bitmap pixels into buffer.
        mPixelBuffer.rewind();

        mBitmap.copyPixelsToBuffer(mPixelBuffer);
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

        // Draw
        updatePixels();

        // Clear the surface
        gl.glClearColorx(0, 0, 0, 0);
        gl.glClear(GL11.GL_COLOR_BUFFER_BIT);

        // Point to our buffers
        gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
        gl.glEnableClientState(GL10.GL_TEXTURE_COORD_ARRAY);
        
        // Set the face rotation
        gl.glFrontFace(GL10.GL_CW);
        
        // Point to our vertex buffer
        gl.glVertexPointer(3, GL10.GL_FLOAT, 0, vertexBuffer);
        gl.glTexCoordPointer(2, GL10.GL_FLOAT, 0, textureBuffer);
 

        // Choose the texture
        gl.glBindTexture(GL11.GL_TEXTURE_2D, mTextureId);

        // Update the texture
        gl.glTexSubImage2D(GL11.GL_TEXTURE_2D, 0, 0, 0, mTextureWidth, mTextureHeight, 
                           GL11.GL_RGBA, GL11.GL_UNSIGNED_BYTE, mPixelBuffer);

        // Draw the texture on the surface
        gl.glTexParameteriv(GL10.GL_TEXTURE_2D, GL11Ext.GL_TEXTURE_CROP_RECT_OES, textureCrop, 0);

        //((GL11Ext) gl).glDrawTexiOES(0, 0, 0, surfaceWidth, surfaceHeight);
        
        // Draw the vertices as triangle strip
        gl.glDrawArrays(GL10.GL_TRIANGLE_STRIP, 0, vertices.length / 3);

        //Disable the client state before leaving
        gl.glDisableClientState(GL10.GL_VERTEX_ARRAY);
        gl.glDisableClientState(GL10.GL_TEXTURE_COORD_ARRAY);

    }
}

