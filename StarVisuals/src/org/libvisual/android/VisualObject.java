package org.libvisual.android;

import android.graphics.Bitmap;
import android.graphics.Color;
import android.util.Log;

public class VisualObject {
    private final String TAG = "LibVisual/VisualObject";
    public VisVideo mVideo = null;
    public VisBin mBin = null;
    public VisActor mActor = null;
    public VisInput mInput = null;
    public VisMorph mMorph = null;
    public Bitmap mBitmap;
    private static boolean inited = false;
    private boolean mVideoInitialized;

    private static native void renderVisual(Bitmap bitmap, CPtr binPtr, CPtr videoPtr);
    private static native void fpsInit();
    private static native void init();
    private static native void deinit();

    public VisualObject(int w, int h, String actor, String input, String morph)
    {
        if(!inited)
        {
            init();
            inited=true;
        }

        mVideo = new VisVideo();
        mBin = new VisBin();
        mInput = new VisInput(input);
        mActor = new VisActor(actor);
        //mMorph = new VisMorph(morph);

        mBin.setSupportedDepth(VisVideo.VISUAL_VIDEO_DEPTH_ALL);
        mBin.setPreferredDepth(VisVideo.VISUAL_VIDEO_DEPTH_32BIT);

        mBitmap = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);

        if(mBitmap.getConfig() != Bitmap.Config.ARGB_8888)
        {
            Log.e(TAG, "Bitmap format is not RGBA_8888 ! ");
            return; 
        }

        fpsInit();

        // We initialize VisBin here.
        onSizeChanged(w, h, w, h);
    }

    /** initialize VisVideo for actor + buffer bitmap */
    void initVideo(int width, int height, int stride)
    {
        /* get depth of current actor */
        int depth;
        int depthflag = mActor.getSupportedDepth();
        if(depthflag == VisVideo.VISUAL_VIDEO_DEPTH_GL)
        {
            depth = VisVideo.depthGetHighest(depthflag);
        }
        else
        {
            depth = VisVideo.depthGetHighestNoGl(depthflag);
        }

        /* set depth of bin */
        mBin.setDepth(depth);

        /* connect actor & input to bin */
        mBin.connect(mActor.VisActor, mInput.VisInput);

        /* create new VisVideo object for this bitmap */
        mVideo.setAttributes(width, height,
                             stride,
                             VisVideo.VISUAL_VIDEO_DEPTH_32BIT);

        /* get depth from actor */
        int actorDepth = mActor.getSupportedDepth();
        int videoDepth;
        if(actorDepth == VisVideo.VISUAL_VIDEO_DEPTH_GL)
        {
            videoDepth = VisVideo.depthGetHighest(actorDepth);
        }
        else
        {
            videoDepth = VisVideo.depthGetHighestNoGl(actorDepth);
        }

        /* create new VisVideo for actor */
        VisVideo avideo = new VisVideo();
        avideo.setAttributes(width, height,
                             width*VisVideo.bppFromDepth(videoDepth),
                             videoDepth);
        avideo.allocateBuffer();

        /* set video for bin */
        mBin.setVideo(avideo.VisVideo);
    }

    public Bitmap run()
    {
        mBitmap.eraseColor(Color.RED);
        renderVisual(mBitmap, mBin.VisBin, mVideo.VisVideo);
        return mBitmap;
    }

    public Bitmap getBitmap()
    {
        return mBitmap;
    }

    public void onSizeChanged(int w, int h, int oldw, int oldh)
    {
        /* free previous Bitmap */
        if(mBitmap != null)
        {
            mBitmap.recycle();
            mBitmap = null;
        }

        /* create bitmap */
        mBitmap = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);

        /* validate bitmap */
        if(mBitmap.getConfig() != Bitmap.Config.ARGB_8888)
        {
            Log.e(TAG, "Bitmap format is not RGBA_8888 !");
            return;
        }

        Log.i(TAG, "onSizeChanged(): "+w+"x"+h+" stride: "+mBitmap.getRowBytes()+" (prev: "+oldw+"x"+oldh+")");

        if(!mVideoInitialized)
        {
            initVideo(w, h, mBitmap.getRowBytes());
            mVideoInitialized = true;
        }
        else
        {
            mVideo.freeBuffer();

            mVideo.setAttributes(w, h,
                                    mBitmap.getRowBytes(),
                                    VisVideo.VISUAL_VIDEO_DEPTH_32BIT);
            mVideo.allocateBuffer();

            /* create new VisVideo object for this bitmap */
            mBin.setVideo(mVideo.VisVideo);
            mActor.videoNegotiate(VisVideo.VISUAL_VIDEO_DEPTH_32BIT, false, false);
        }

        /* realize bin */
        mBin.realize();
        mBin.sync(false);
        mBin.depthChanged();

    }

}
