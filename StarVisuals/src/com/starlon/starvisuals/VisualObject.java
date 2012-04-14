package com.starlon.starvisuals;

import android.graphics.Bitmap;
import android.util.Log;

import org.libvisual.android.VisVideo;
import org.libvisual.android.VisBin;
import org.libvisual.android.VisActor;
import org.libvisual.android.VisInput;
import org.libvisual.android.VisMorph;

class VisualObject {
    private final String TAG = "StarVisuals/VisualObject";
    public VisVideo mVideo = null;
    public VisBin mBin = null;
    public VisActor mActor = null;
    public VisInput mInput = null;
    public VisMorph mMorph = null;
    public Bitmap mBitmap;
    private boolean mDisposed = false;

    private static native void renderVisual(Bitmap bitmap, int binPtr, int videoPtr);
    private static native void fpsInit();

    public VisualObject(int w, int h, String actor, String input, String morph)
    {
        mVideo = new VisVideo();
        mBin = new VisBin();
        mActor = new VisActor(actor);
        mInput = new VisInput(input);
        mMorph = new VisMorph(morph);

        mBin.setSupportedDepth(VisVideo.VISUAL_VIDEO_DEPTH_ALL);
        mBin.setPreferredDepth(VisVideo.VISUAL_VIDEO_DEPTH_32BIT);
        mBin.setMorph(morph);

        mBin.setVideo(mVideo.VisVideo);

        mBin.connect(mActor.VisActor, mInput.VisInput);

        mBitmap = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);

        if(mBitmap.getConfig() != Bitmap.Config.ARGB_8888)
        {
            Log.e(TAG, "Bitmap format is not RGBA_8888 ! ");
            return; 
        }

        initVideo(w, h, mBitmap.getRowBytes());

        fpsInit();
    }

    public void dispose()
    {
        if(mDisposed)
            return;
        mDisposed = true;
        mVideo.finalize();
        mVideo = null;
        mBin.finalize();
        mBin = null;
        mActor.finalize();
        mActor = null;
        mInput.finalize();
        mInput = null;
        mMorph.finalize();
        mMorph = null;
    }

    /** initialize VisVideo for actor + buffer bitmap */
    void initVideo(int width, int height, int stride)
    {
        if(mDisposed)
            return;
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
        mVideo = new VisVideo();
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

    public void draw()
    {
        if(mDisposed)
            return;
        renderVisual(mBitmap, mBin.VisBin, mVideo.VisVideo);
    }

    public Bitmap getBitmap()
    {
        if(mDisposed)
            return null;
        return mBitmap;
    }
}
