package com.starlon.starvisuals;

import android.content.Context;
import android.view.View;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Typeface;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Matrix;
import android.util.Log;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.locks.ReentrantLock;
import java.util.concurrent.TimeUnit;
import java.lang.Thread;
import java.lang.Double;
import java.lang.NumberFormatException;
import java.nio.IntBuffer;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.text.DecimalFormat;



public class StarVisualsView extends View {
    private final String TAG = "StarVisuals/StarVisualsView";
    private final int INT_BYTES = 4;
    private final Bitmap.Config RGB = Bitmap.Config.ARGB_8888;
    public Bitmap mBitmap = null;
    public Bitmap mBitmapSecond = null;
    public IntBuffer mIntBuffer = null;
    private StarVisuals mActivity = null;
    private Stats mStatsNative = null;
    private Stats mStatsCanvas = null;
    private int WIDTH = 128;
    private int HEIGHT = 128;
    private Paint mPaint = null;
    private Matrix mMatrix = null;
    //private Display mDisplay = null;
    public Thread mThread = null;
    private final ReentrantLock mLock = new ReentrantLock();
    public final Object mSynch = new Object();

    public StarVisualsView(Context context) {
        super(context);

        Log.e(TAG, "StarVisualsVIew constructor");

        mActivity = (StarVisuals)context;

        mStatsNative = new Stats();
        mStatsCanvas = new Stats();
        mStatsNative.statsInit();
        mStatsCanvas.statsInit();

        mPaint = new Paint();
        mPaint.setTextSize(15);
        mPaint.setAntiAlias(true);
        mPaint.setTypeface(Typeface.create(Typeface.SERIF, Typeface.ITALIC));
        mPaint.setStyle(Paint.Style.STROKE);
        mPaint.setStrokeWidth(1);
        mPaint.setColor(Color.WHITE);

        final int delay = 1000;
        final int period = 300;

        final Timer timer = new Timer();


        TimerTask task = new TimerTask() {
            double roundTwoDecimals(double d) {
                double val = 0.00;
                try {
                    DecimalFormat twoDForm = new DecimalFormat("#.##");
                    val = (double)Double.valueOf(twoDForm.format(d));
                } catch(NumberFormatException e) {
                    val = 0.00;
                }
                return val;
            }
            public void run() {
                mActivity.warn(roundTwoDecimals((mStatsCanvas.mAvgFrame + mStatsNative.mAvgFrame) / 2) + "fps");
            }
        };

        timer.scheduleAtFixedRate(task, delay, period);

        mBitmap = Bitmap.createBitmap(WIDTH, HEIGHT, RGB );
        mBitmapSecond = Bitmap.createBitmap(WIDTH, HEIGHT, RGB );

        ByteBuffer ibb = ByteBuffer.allocateDirect(WIDTH * HEIGHT * INT_BYTES);
        ibb.order(ByteOrder.nativeOrder());
        mIntBuffer = IntBuffer.wrap( new int[WIDTH * HEIGHT] );
        mIntBuffer.position(0);

        //mDisplay = ((WindowManager) mActivity.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();

        initVisual();
    }

    public void initVisual(int w, int h)
    {
        WIDTH = w;
        HEIGHT = h;
        initVisual();
    }

    public void initVisual()
    {
        stopThread();
        mLock.lock();
        synchronized(mSynch)
        {
    
            if(mBitmap != null) 
            {
                mBitmap.recycle();
                mBitmap = null;
                mBitmap = Bitmap.createBitmap(WIDTH, HEIGHT, Bitmap.Config.ARGB_8888);
            }

            if(mBitmapSecond != null)
            {
                mBitmapSecond.recycle();
                mBitmapSecond = null;
                mBitmapSecond = Bitmap.createBitmap(WIDTH, HEIGHT, Bitmap.Config.ARGB_8888);
            }
    
    
            mActivity.setPlugins(false);
            NativeHelper.initApp(WIDTH, HEIGHT);
            // Make this the last thing we do here since it's synchronized.
            // Hopefully it doesn't blow up.
        }
        mLock.unlock();
    }

    @Override protected void onSizeChanged(int w, int h, int oldw, int oldh)
    {
        mMatrix = new Matrix();
        mMatrix.setScale(w/(float)WIDTH, h/(float)HEIGHT);
    }

    @Override protected void onDraw(Canvas canvas) 
    {
        mStatsCanvas.startFrame();

        short[] data = mActivity.getMicData();
        if(data != null)
            NativeHelper.uploadAudio(data);

        drawScene(canvas);

        mStatsCanvas.endFrame();
    }

    public void stopThread()
    {
        if(mThread != null)
        {
            mActivity.setIsActive(false);
            mThread.interrupt();
            try {
                mThread.join();
            } catch (InterruptedException e) {
                // Do nothing
            }
        }
    }

    public void startThread()
    {
        stopThread();
        mThread = new Thread(new Runnable() {
            public void run() {
                mActivity.setIsActive(true);
                while(mActivity.getIsActive() == true)
                {
                    try {
                        double then;
                        double now;
                        double delta;
                        double avg;
                        double diff;
                        synchronized(mSynch)
                        {
                            then = mStatsCanvas.nowMil();
                            mStatsNative.startFrame();
                            mLock.lock();
                            NativeHelper.renderBitmap(mBitmap, mActivity.getDoSwap());
                            mLock.unlock();
                            mStatsNative.endFrame();

                            now = mStatsNative.nowMil();
                            delta = (now - then);
                            avg = mStatsNative.mAvgFrame;
                            diff = (avg / mActivity.getMaxFPS());

                        }
                        Thread.sleep((int)(delta + diff * 60));
                    } 
                    catch(Exception e)
                    {
                        mActivity.warn(e.toString(), true);
                        NativeHelper.setIsActive(false);
                    }
                }
            }
        }, "Update Bitmap");

        mThread.start();
    }

    private void drawScene(Canvas canvas)
    {
            if(mLock.tryLock())
            {
                // Draw bitmap on canvas
                canvas.drawBitmap(mBitmap, mMatrix, mPaint);

                mIntBuffer.position(0);
                mBitmap.copyPixelsToBuffer(mIntBuffer);

                mLock.unlock();

            } else {
                mIntBuffer.position(0);
                mBitmapSecond.copyPixelsFromBuffer(mIntBuffer);

                canvas.drawBitmap(mBitmapSecond, mMatrix, mPaint);
            }
                
            // Do we have text to show?
    

            if(mActivity.getShowText())
            {
                String text = mActivity.getDisplayText();
                float canvasWidth = getWidth();
                float textWidth = mPaint.measureText(text);
                float startPositionX = (canvasWidth / 2 - textWidth / 2) - canvasWidth/4;
        
                canvas.drawText(text, startPositionX, 50, mPaint);
            }
    
            if(mActivity.getDoBeat())
            {
                String text;
                int bpm = NativeHelper.getBPM();
                int confidence = NativeHelper.getBPMConfidence();
                boolean isBeat = NativeHelper.isBeat();

                if(isBeat)
                    mActivity.setDoSwap(!mActivity.getDoSwap());
    
                if(bpm > 0 && confidence > 0)
                    text = bpm + "bpm (" + confidence + "%) " + (isBeat ? "*" : " ");
                else
                    text = "Learning... (" + confidence + "%)";

                float canvasWidth = getWidth();
                float textWidth = mPaint.measureText(text);
                float startPositionX = (canvasWidth / 2 - textWidth / 2);
   
                startPositionX = (canvasWidth / 2 - textWidth / 2) - canvasWidth/4;
                canvas.drawText(text, startPositionX, 100, mPaint);
            }
    
            if(mActivity.getShowArt() && mActivity.mAlbumArt != null)
            {
                int width = mActivity.mAlbumArt.getWidth();
                int height = mActivity.mAlbumArt.getHeight();
                canvas.drawBitmap(mActivity.mAlbumArt, getWidth()-width-50.0f, 50.0f, mPaint);
            }
            invalidate();
    }

    public void switchScene(int prev)
    {
        mLock.lock();
        Log.i(TAG, "Switch scene....");
        NativeHelper.finalizeSwitch(prev);
        mLock.unlock();
    }

}


