package com.starlon.froyvisuals;

import android.app.Activity;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Looper;
import android.os.AsyncTask;
import android.os.ParcelFileDescriptor;
import android.content.Context;
import android.content.ContentUris;
import android.content.res.Resources;
import android.content.res.Configuration;
import android.content.SharedPreferences;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.ViewGroup;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.MotionEvent;
import android.view.Display;
import android.view.Surface;
import android.view.View.OnTouchListener;
import android.view.View.OnClickListener;
import android.view.GestureDetector;
import android.view.GestureDetector.SimpleOnGestureListener;
import android.view.ViewConfiguration;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Typeface;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Matrix;
import android.media.AudioRecord;
import android.media.AudioFormat;
import android.media.MediaRecorder;
import android.util.Log;
import android.util.TypedValue;
import android.net.Uri;
import java.util.Timer;
import java.util.TimerTask;
import java.io.FileDescriptor;


public class FroyVisualsView extends View {
    private final String TAG = "FroyVisuals/FroyVisualsView";
    public Bitmap mBitmap;
    private NativeHelper mNativeHelper;
    private FroyVisuals mActivity;
    private Stats mStats;
    private final int WIDTH = 256;
    private final int HEIGHT = 256;
    private Paint mPaint;
    private Matrix mMatrix;
    private Display mDisplay;
    private Thread mThread;
    private boolean mActive = false;

    public FroyVisualsView(Context context) {
        super(context);

        Log.e(TAG, "FroyVisualsVIew constructor");

        mActivity = (FroyVisuals)context;

        mPaint = new Paint();
        mPaint.setAntiAlias(true);
        mPaint.setTextSize(30);
        mPaint.setTypeface(Typeface.create(Typeface.SERIF, Typeface.ITALIC));
        mPaint.setStyle(Paint.Style.STROKE);
        mPaint.setStrokeWidth(1);
        mPaint.setColor(Color.WHITE);

        mNativeHelper.initApp(WIDTH, HEIGHT, 0, 0);

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
                        mThread.sleep(5);
                    } catch (InterruptedException e) {
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
            mNativeHelper.render(mBitmap);
            mStats.endFrame();
        }

    }

    private void drawScene(Canvas canvas)
    {
        synchronized(mBitmap)
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
            }

            invalidate();
        }

        if(mActivity.mAlbumArt != null)
        {
            int width = mActivity.mAlbumArt.getWidth();
            int height = mActivity.mAlbumArt.getHeight();
            canvas.drawBitmap(mActivity.mAlbumArt, 50.0f, 50.0f, mPaint);
            canvas.drawText("lollllz", 100, 80, mPaint);
        }
       
    }

    public void switchScene(int prev)
    {
        synchronized(mBitmap)
        {
            mNativeHelper.finalizeSwitch(prev);
        }
    }

}


