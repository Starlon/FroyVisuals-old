package com.starlon.froyvisuals;

import android.content.Context;
import android.view.View;
import android.view.WindowManager;
import android.view.Display;
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


public class FroyVisualsView extends View {
    private final String TAG = "FroyVisuals/FroyVisualsView";
    public Bitmap mBitmap;
    private FroyVisuals mActivity;
    private Stats mStats;
    private final int WIDTH = 256;
    private final int HEIGHT = 256;
    private Paint mPaint;
    private Matrix mMatrix;
    private Thread mThread;
    private Display mDisplay;
    private boolean mActive = false;
    private boolean mDoBeat = false;
    private final ReentrantLock mLock = new ReentrantLock();

    public FroyVisualsView(Context context) {
        super(context);

        Log.e(TAG, "FroyVisualsVIew constructor");

        mActivity = (FroyVisuals)context;

        mPaint = new Paint();
        mPaint.setTextSize(30);
        mPaint.setAntiAlias(true);
        mPaint.setTypeface(Typeface.create(Typeface.SERIF, Typeface.ITALIC));
        mPaint.setStyle(Paint.Style.STROKE);
        mPaint.setStrokeWidth(1);
        mPaint.setColor(Color.WHITE);

        NativeHelper.initApp(WIDTH, HEIGHT, 0, 0);

        mStats = new Stats();
        mStats.statsInit();

        final int delay = 1000;
        final int period = 300;

        final Timer timer = new Timer();

        TimerTask task = new TimerTask() {
            public void run() {
                mActivity.warn(mStats.getText());
            }
        };

        timer.scheduleAtFixedRate(task, delay, period);

        mBitmap = Bitmap.createBitmap(WIDTH, HEIGHT, Bitmap.Config.RGB_565);

        mDisplay = ((WindowManager) mActivity.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();

    }

    @Override protected void onSizeChanged(int w, int h, int oldw, int oldh)
    {
        mMatrix = new Matrix();
        mMatrix.setScale(w/(float)WIDTH, h/(float)HEIGHT);
    }

    @Override protected void onDraw(Canvas canvas) 
    {
        drawScene(canvas);
    }

    public void stopThread()
    {
        if(mThread != null)
        {
            mActive = false;
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
                mActive = true;
                while(mActive)
                {
                    try {
                        updateBitmap();
                        mThread.sleep(2);
                    } 
                    catch(Exception e)
                    {
                        mActive = false;
                    }
                }
            }
        }, "Update Bitmap");

        mThread.start();
    }

    private void updateBitmap()
    {
        synchronized(mBitmap)
        {
            mStats.startFrame();
            //mLock.lock();
            NativeHelper.render(mBitmap);
            //mLock.unlock();
            mStats.endFrame();
        }

    }

    private void drawScene(Canvas canvas)
    {
            synchronized(mBitmap)//if(mLock.tryLock())
            {
                // Draw bitmap on canvas
                canvas.drawBitmap(mBitmap, mMatrix, mPaint);
                
                // Do we have text to show?
                String text = mActivity.getDisplayText();
        
                if(text != null)
                {
                    float canvasWidth = getWidth();
                    float textWidth = mPaint.measureText(text);
                    float startPositionX = (canvasWidth / 2 - textWidth / 2);
            
                    canvas.drawText(text, startPositionX, getHeight()-50, mPaint);
        
                    if(mDoBeat)
                    {
                        int bpm = NativeHelper.getBPM();
                        int confidence = NativeHelper.getBPMConfidence();
                        boolean isBeat = NativeHelper.isBeat();
            
                        if(bpm > 0)
                            text = bpm + "bpm (" + confidence + "%) " + (isBeat ? "*" : "");
                        else
                            text = "Learning...";
            
                        textWidth = mPaint.measureText(text);
                        startPositionX = (canvasWidth / 2 - textWidth / 2);
                        canvas.drawText(text, startPositionX, getHeight()-100, mPaint);
                    }
                }
        
                if(mActivity.mAlbumArt != null)
                {
                    int width = mActivity.mAlbumArt.getWidth();
                    int height = mActivity.mAlbumArt.getHeight();
                    canvas.drawBitmap(mActivity.mAlbumArt, 50.0f, 50.0f, mPaint);
                }
        
                invalidate();
            }
    }

    public void switchScene(int prev)
    {
        synchronized(mBitmap)
        {
            NativeHelper.finalizeSwitch(prev);
        }
    }

}


