/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.starlon.froyvisuals;

import android.app.Activity;
import android.os.Bundle;
import android.content.Context;
import android.view.View;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Point;
import android.view.Display;
import android.view.WindowManager;

public class FroyVisuals extends Activity
{
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(new FroyVisualsView(this));
    }

    /* load our native library */
    static {
        System.loadLibrary("main");
    }
}

class FroyVisualsView extends View {
    private Bitmap mBitmap;
    private Context mCtx;
    private long mStartTime;
    private int width = -1;
    private int height = -1;
    /* implementend by libplasma.so */
    private static native void render(Bitmap  bitmap, long time_ms);
    private static native void screenResize(int w, int y);
    //private static native void uploadAudio(short data);
    private static native void switchActor(int prev);
    private static native void mouseMotion(float x, float y);
    private static native void mouseButton(int button, float x, float y);
    private static native void visualsQuit();
    private static native void initApp(int w, int h);

    public FroyVisualsView(Context context) {
        super(context);

        mCtx = context;

        width = 32;
        height = 32;

        mBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.RGB_565);
        mStartTime = System.currentTimeMillis();
        initApp(width, height);
    }

    @Override protected void onDraw(Canvas canvas) {
        WindowManager mWinMgr = (WindowManager)mCtx.getSystemService(Context.WINDOW_SERVICE);
        int W = mWinMgr.getDefaultDisplay().getWidth();
        int H = mWinMgr.getDefaultDisplay().getHeight();

        if(width != W || height != H) {
            width = W;
            height = H;
            mBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.RGB_565);
            screenResize(width, height);
        }

        //canvas.drawColor(0xFFCCCCCC);
        render(mBitmap, System.currentTimeMillis() - mStartTime);
        canvas.drawBitmap(mBitmap, 0, 0, null);
        // force a redraw, with a different time-based pattern.
        invalidate();
    }
}
