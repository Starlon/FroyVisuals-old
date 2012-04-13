package com.starlon.starvisuals;

import java.util.Timer;
import java.util.TimerTask;

import java.nio.FloatBuffer;
import java.nio.ShortBuffer;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.util.Random;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.openglesbook.common.ESShader;
import com.starlon.starvisuals.R;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.SystemClock;

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


public class StarVisualsRenderer implements Renderer {
    private Visual vis;
    private int mSurfaceWidth;
    private int mSurfaceHeight;
    private Stats mStats;
    private StarVisuals mActivity;
    private NativeHelper mNativeHelper;
    private boolean mInited = false;

    public StarVisualsRenderer(Context context) {
        vis = new Visual((StarVisuals)context);
        mStats = new Stats();
        mStats.statsInit();
        mActivity = (StarVisuals)context;
        mInited = true;
    }

    public void destroy()
    {
        if(!mInited) return;
        vis.destroy();
        vis = null;
        mStats = null;
        mActivity = null;
    }

    ///
    //  Load texture from resource
    //
    private int loadTexture ( InputStream is )
    {
        int[] textureId = new int[1];
        Bitmap bitmap;
        bitmap = BitmapFactory.decodeStream(is);
        byte[] buffer = new byte[bitmap.getWidth() * bitmap.getHeight() * 3];

        for ( int y = 0; y < bitmap.getHeight(); y++ )
            for ( int x = 0; x < bitmap.getWidth(); x++ )
            {
                int pixel = bitmap.getPixel(x, y);
                buffer[(y * bitmap.getWidth() + x) * 3 + 0] = (byte)((pixel >> 16) & 0xFF);
                buffer[(y * bitmap.getWidth() + x) * 3 + 1] = (byte)((pixel >> 8) & 0xFF);
                buffer[(y * bitmap.getWidth() + x) * 3 + 2] = (byte)((pixel >> 0) & 0xFF);
            }

        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(bitmap.getWidth() * bitmap.getHeight() * 3);
        byteBuffer.put(buffer).position(0);

        GLES20.glGenTextures ( 1, textureId, 0 );
        GLES20.glBindTexture ( GLES20.GL_TEXTURE_2D, textureId[0] );

        GLES20.glTexImage2D ( GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGB, bitmap.getWidth(), bitmap.getHeight(), 0,
                              GLES20.GL_RGB, GLES20.GL_UNSIGNED_BYTE, byteBuffer );

        GLES20.glTexParameteri ( GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR );
        GLES20.glTexParameteri ( GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR );
        GLES20.glTexParameteri ( GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE );
        GLES20.glTexParameteri ( GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE );

        return textureId[0];
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);

        GLES20.glUseProgram(mProgramObject);

        // Load the vertex attributes
        mParticles.position(0);
        GLES20.glVertexAttribPointer ( mLifetimeLoc, 1, GLES20.GL_FLOAT,
                                       false, PARTICLE_SIZE * 4,
                                       mParticles );

        mParticles.position(1);
        GLES20.glVertexAttribPointer ( mEndPositionLoc, 3, GLES20.GL_FLOAT,
                                       false, PARTICLE_SIZE * 4,
                                       mParticles );

        mParticles.position(4);
        GLES20.glVertexAttribPointer ( mStartPositionLoc, 3, GLES20.GL_FLOAT,
                                       false, PARTICLE_SIZE * 4,
                                       mParticles );


        GLES20.glEnableVertexAttribArray ( mLifetimeLoc );
        GLES20.glEnableVertexAttribArray ( mEndPositionLoc );
        GLES20.glEnableVertexAttribArray ( mStartPositionLoc );

        // Blend particles
        GLES20.glEnable ( GLES20.GL_BLEND );
        GLES20.glBlendFunc ( GLES20.GL_SRC_ALPHA, GLES20.GL_ONE );

        // Bind the texture
        GLES20.glActiveTexture ( GLES20.GL_TEXTURE0 );
        GLES20.glBindTexture ( GLES20.GL_TEXTURE_2D, mTextureId );
        GLES20.glEnable ( GLES20.GL_TEXTURE_2D );

        // Set the sampler texture unit to 0
        GLES20.glUniform1i ( mSamplerLoc, 0 );

        GLES20.glDrawArrays( GLES20.GL_POINTS, 0, NUM_PARTICLES );


        mStats.startFrame();
        vis.performFrame(gl10, mSurfaceWidth, mSurfaceHeight);
        mStats.endFrame();

    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int width, int height) {
        vis.initialize(gl10, width, height);
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

       String vShaderStr =
            "uniform float u_time;                                \n" +
            "uniform vec3 u_centerPosition;                       \n" +
            "attribute float a_lifetime;                          \n" +
            "attribute vec3 a_startPosition;                      \n" +
            "attribute vec3 a_endPosition;                        \n" +
            "varying float v_lifetime;                            \n" +
            "void main()                                          \n" +
            "{                                                    \n" +
            "  if ( u_time <= a_lifetime )                        \n" +
            "  {                                                  \n" +
            "    gl_Position.xyz = a_startPosition +              \n" +
            "                      (u_time * a_endPosition);      \n" +
            "    gl_Position.xyz += u_centerPosition;             \n" +
            "    gl_Position.w = 1.0;                             \n" +
            "  }                                                  \n" +
            "  else                                               \n" +
            "     gl_Position = vec4( -1000, -1000, 0, 0 );       \n" +
            "  v_lifetime = 1.0 - ( u_time / a_lifetime );        \n" +
            "  v_lifetime = clamp ( v_lifetime, 0.0, 1.0 );       \n" +
            "  gl_PointSize = ( v_lifetime * v_lifetime ) * 40.0; \n" +
            "}";


        String fShaderStr =
            "precision mediump float;                             \n" +
            "uniform vec4 u_color;                                \n" +
            "varying float v_lifetime;                            \n" +
            "uniform sampler2D s_texture;                         \n" +
            "void main()                                          \n" +
            "{                                                    \n" +
            "  vec4 texColor;                                     \n" +
            "  texColor = texture2D( s_texture, gl_PointCoord );  \n" +
            "  gl_FragColor = vec4( u_color ) * texColor;         \n" +
            "  gl_FragColor.a *= v_lifetime;                      \n" +
            "}                                                    \n";

        // Load the shaders and get a linked program object
        mProgramObject = ESShader.loadProgram(vShaderStr, fShaderStr);

        // Get the attribute locations
        mLifetimeLoc = GLES20.glGetAttribLocation(mProgramObject, "a_lifetime");
        mStartPositionLoc = GLES20.glGetAttribLocation(mProgramObject, "a_startPosition" );
        mEndPositionLoc = GLES20.glGetAttribLocation(mProgramObject, "a_endPosition" );

        // Get the uniform locations
        mTimeLoc = GLES20.glGetUniformLocation ( mProgramObject, "u_time" );
        mCenterPositionLoc = GLES20.glGetUniformLocation ( mProgramObject, "u_centerPosition" );
        mColorLoc = GLES20.glGetUniformLocation ( mProgramObject, "u_color" );
        mSamplerLoc = GLES20.glGetUniformLocation ( mProgramObject, "s_texture" );

        GLES20.glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );

        // Fill in particle data array
        Random generator = new Random();

        for ( int i = 0; i < NUM_PARTICLES; i++ )
        {
           // Lifetime of particle
           mParticleData[i * 7 + 0] = generator.nextFloat();

           // End position of particle
           mParticleData[i * 7 + 1] = generator.nextFloat() * 2.0f - 1.0f;
           mParticleData[i * 7 + 2] = generator.nextFloat() * 2.0f - 1.0f;
           mParticleData[i * 7 + 3] = generator.nextFloat() * 2.0f - 1.0f;

           // Start position of particle
           mParticleData[i * 7 + 4] = generator.nextFloat() * 0.25f - 0.125f;
           mParticleData[i * 7 + 5] = generator.nextFloat() * 0.25f - 0.125f;
           mParticleData[i * 7 + 6] = generator.nextFloat() * 0.25f - 0.125f;
        }
        mParticles = ByteBuffer.allocateDirect(mParticleData.length * 4)
            .order(ByteOrder.nativeOrder()).asFloatBuffer();
        mParticles.put(mParticleData).position(0);

        // Initialize time to cause reset on first update
        mTime = 1.0f;

        // Load particle texture
        mTextureId = loadTexture ( mContext.getResources().openRawResource( R.raw.smoke ) );

    }

    // Particles
    // Handle to a program object
    private int mProgramObject;

    // Attribute locations
    private int mLifetimeLoc;
    private int mStartPositionLoc;
    private int mEndPositionLoc;

    // Uniform location
    private int mTimeLoc;
    private int mColorLoc;
    private int mCenterPositionLoc;
    private int mSamplerLoc;

    // Texture handle
    private int mTextureId;

    // Update time
    private float mTime;
    private long mLastTime;

    // Additional member variables
    private int mWidth;
    private int mHeight;
    private FloatBuffer mParticles;
    private Context mContext;

    private final int NUM_PARTICLES = 1000;
    private final int PARTICLE_SIZE = 7;

    private final float[] mParticleData = new float[NUM_PARTICLES * PARTICLE_SIZE];


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
    private StarVisuals mActivity;
    private Bitmap mBitmap;
    private Paint mPaint;
    private Canvas mCanvas;
    private GL10 mGL10 = null;

    private FloatBuffer mVertexBuffer;   // buffer holding the vertices
    private float vertices[] = {
            -1.0f, -1.0f,  0.0f,        // V1 - bottom left
            -1.0f,  1.0f,  0.0f,        // V2 - top left
             1.0f, -1.0f,  0.0f,        // V3 - bottom right
             1.0f,  1.0f,  0.0f         // V4 - top right
    };

    private FloatBuffer mTextureBuffer;  // buffer holding the texture coordinates
    private float texture[] = {         
            // Mapping coordinates for the vertices
            0.0f, 1.0f,     // top left     (V2)
            0.0f, 0.0f,     // bottom left  (V1)
            1.0f, 1.0f,     // top right    (V4)
            1.0f, 0.0f      // bottom right (V3)
    };

    public Visual(StarVisuals activity) {
        mActivity = activity;

        // a float has 4 bytes so we allocate for each coordinate 4 bytes
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(vertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        
        // allocates the memory from the byte buffer
        mVertexBuffer = byteBuffer.asFloatBuffer();
        
        // fill the mVertexBuffer with the vertices
        mVertexBuffer.put(vertices);
        
        // set the cursor position to the beginning of the buffer
        mVertexBuffer.position(0);
        
        byteBuffer = ByteBuffer.allocateDirect(texture.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        mTextureBuffer = byteBuffer.asFloatBuffer();
        mTextureBuffer.put(texture);
        mTextureBuffer.position(0);

        mNativeHelper.initApp(mTextureWidth, mTextureHeight);

        mActivity.setPlugins(true);
    }

    public void initialize(GL10 gl, int surfaceWidth, int surfaceHeight) {

        mGL10 = gl;

        mTextureWidth = 128;
        mTextureHeight = 128;


        textureCrop[0] = 0;
        textureCrop[1] = 0;
        textureCrop[2] = mTextureWidth;
        textureCrop[3] = mTextureHeight;

        mCanvas = new Canvas();
        
        mBitmap = Bitmap.createBitmap(mTextureWidth, mTextureHeight, Bitmap.Config.ARGB_8888);

        mCanvas.setBitmap(mBitmap);

        mPaint = new Paint();
        mPaint.setAntiAlias(true);
        mPaint.setTextSize(10);
        mPaint.setTypeface(Typeface.create(Typeface.SERIF, Typeface.ITALIC));
        mPaint.setStyle(Paint.Style.STROKE);
        mPaint.setStrokeWidth(1);
        mPaint.setColor(Color.WHITE);
        mPaint.setTextAlign(Paint.Align.CENTER);

        // init the pixel buffer
        mPixelBuffer = ByteBuffer.allocate(mTextureWidth * mTextureHeight * bytesPerPixel);

        // init the GL settings
        if (glInited) {
            resetGl();
        }

        initGl(surfaceWidth, surfaceHeight);

        // init the GL texture
        initGlTexture();

    }   

    public void resetGl() {
        if(!glInited || mGL10 == null) return;

        glInited = false;

        mGL10.glMatrixMode(GL10.GL_PROJECTION);
        mGL10.glPopMatrix();
        mGL10.glMatrixMode(GL10.GL_TEXTURE);
        mGL10.glPopMatrix();
        mGL10.glMatrixMode(GL10.GL_MODELVIEW);
        mGL10.glPopMatrix();
    }

    public void destroy()
    {
        if(!glInited) return;

        resetGl();

        mBitmap.recycle();
        mBitmap = null;
        mPixelBuffer = null;
        mPaint = null;
        mCanvas = null;
        mVertexBuffer = null;
        mTextureBuffer = null;
        releaseTexture();
        glInited = false;
        mGL10 = null; // This needs to be last.
    }
    

    public void initGl(int surfaceWidth, int surfaceHeight) {
        if(glInited || mGL10 == null) return;

        mGL10.glViewport(0, 0, surfaceWidth, surfaceHeight);

        mGL10.glShadeModel(GL10.GL_FLAT);
        mGL10.glFrontFace(GL10.GL_CCW);
        mGL10.glEnable(GL10.GL_TEXTURE_2D);

        mGL10.glMatrixMode(GL10.GL_PROJECTION);
        mGL10.glLoadIdentity();
        mGL10.glPushMatrix();

        mGL10.glMatrixMode(GL10.GL_MODELVIEW);
        mGL10.glLoadIdentity();
        mGL10.glOrthof(-1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f);
        mGL10.glPushMatrix();

        glInited = true;
    }

    public void updatePixels()
    {
        // Fill the bitmap with black.
        mBitmap.eraseColor(Color.BLACK);

        // Pass bitmap to be rendered by native function.
        mNativeHelper.renderBitmap(mBitmap, mActivity.getDoSwap());


        // If StarVisuals has text to display, then use a canvas and paint brush to display it.
        String text = mActivity.getDisplayText();
        if(text != null)
        {
            // Give the bitmap a canvas so we can draw on it.
    

            float canvasWidth = mCanvas.getWidth();
            float textWidth = mPaint.measureText(text);
            float startPositionX = (canvasWidth - textWidth / 2) / 2;
    
            mCanvas.drawText(text, startPositionX, mTextureWidth-12, mPaint);
        }

        // Copy bitmap pixels into buffer.
        mPixelBuffer.rewind();

        mBitmap.copyPixelsToBuffer(mPixelBuffer);
    }

    private void releaseTexture() {
        if(mGL10 == null)
            return;

        if (mTextureId != -1) {
            mGL10.glDeleteTextures(1, new int[] { mTextureId }, 0);
        }       
    }

    private void initGlTexture() {

        releaseTexture();

        int[] textures = new int[1];
        mGL10.glGenTextures(1, textures, 0);
        mTextureId = textures[0];

        // we want to modify this texture so bind it
        mGL10.glBindTexture(GL10.GL_TEXTURE_2D, mTextureId);

        // GL_LINEAR gives us smoothing since the texture is larger than the screen
        mGL10.glTexParameterf(GL10.GL_TEXTURE_2D, 
                           GL10.GL_TEXTURE_MAG_FILTER,
                           GL10.GL_LINEAR);        

        mGL10.glTexParameterf(GL10.GL_TEXTURE_2D, 
                           GL10.GL_TEXTURE_MIN_FILTER,
                           GL10.GL_LINEAR);

        // repeat the edge pixels if a surface is larger than the texture
        mGL10.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_S,
                           GL10.GL_CLAMP_TO_EDGE);

        mGL10.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_T,
                           GL10.GL_CLAMP_TO_EDGE); 

        // now, let's init the texture with pixel values
        //updatePixels();

        // and init the GL texture with the pixels
        mGL10.glTexImage2D(GL10.GL_TEXTURE_2D, 0, GL10.GL_RGBA, mTextureWidth, mTextureHeight,
                0, GL10.GL_RGBA, GL10.GL_UNSIGNED_BYTE, mPixelBuffer);        


        // at this point, we are OK to further modify the texture
        // using glTexSubImage2D
    }

    


    public void performFrame(GL10 gl, int surfaceWidth, int surfaceHeight) {

        if(mGL10 != gl)
            mGL10 = gl;

        if(mGL10 == null)
            return;

        // Draw
        updatePixels();

        // Clear the surface
        mGL10.glClearColorx(0, 0, 0, 0);
        mGL10.glClear(GL10.GL_COLOR_BUFFER_BIT);

        // Point to our buffers
        mGL10.glEnableClientState(GL10.GL_VERTEX_ARRAY);
        mGL10.glEnableClientState(GL10.GL_TEXTURE_COORD_ARRAY);
        
        // Set the face rotation
        mGL10.glFrontFace(GL10.GL_CW);
        
        // Point to our vertex buffer
        mGL10.glVertexPointer(3, GL10.GL_FLOAT, 0, mVertexBuffer);
        mGL10.glTexCoordPointer(2, GL10.GL_FLOAT, 0, mTextureBuffer);
 

        // Choose the texture
        mGL10.glBindTexture(GL10.GL_TEXTURE_2D, mTextureId);

        // Update the texture
        mGL10.glTexSubImage2D(GL10.GL_TEXTURE_2D, 0, 0, 0, mTextureWidth, mTextureHeight, 
                           GL10.GL_RGBA, GL10.GL_UNSIGNED_BYTE, mPixelBuffer);
        
        // Draw the vertices as triangle strip
        mGL10.glDrawArrays(GL10.GL_TRIANGLE_STRIP, 0, vertices.length / 3);

        //Disable the client state before leaving
        mGL10.glDisableClientState(GL10.GL_VERTEX_ARRAY);
        mGL10.glDisableClientState(GL10.GL_TEXTURE_COORD_ARRAY);

    }
}

