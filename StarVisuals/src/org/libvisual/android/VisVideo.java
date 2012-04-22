/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004-2006 Dennis Smit <ds@nerds-incorporated.org>
 * Copyright (C) 2012 Daniel Hiepler <daniel-lva@niftylight.de>         
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *          Daniel Hiepler <daniel-lva@niftylight.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

package org.libvisual.android;

import android.util.Log;
import java.lang.Exception;



/** VisVideo wrapper */
public class VisVideo
{
    private final static String TAG = "VisVideo";
    public final static int VISUAL_VIDEO_DEPTH_NONE	   = 0;	/**< No video surface flag. */
    public final static int VISUAL_VIDEO_DEPTH_8BIT	   = 1;	/**< 8 bits indexed surface flag. */
    public final static int VISUAL_VIDEO_DEPTH_16BIT   = 2;	/**< 16 bits 5-6-5 surface flag. */
    public final static int VISUAL_VIDEO_DEPTH_24BIT   = 4;	/**< 24 bits surface flag. */
    public final static int VISUAL_VIDEO_DEPTH_32BIT   = 8;	/**< 32 bits surface flag. */
    public final static int VISUAL_VIDEO_DEPTH_GL	   = 16;	/**< openGL surface flag. */
    public final static int VISUAL_VIDEO_DEPTH_ENDLIST = 32;	/**< Used to mark the end of the depth list. */
    public final static int VISUAL_VIDEO_DEPTH_ERROR   = -1;	/**< Used when there is an error. */
    public final static int VISUAL_VIDEO_DEPTH_ALL	   = VISUAL_VIDEO_DEPTH_8BIT
                                | VISUAL_VIDEO_DEPTH_16BIT
                                | VISUAL_VIDEO_DEPTH_24BIT
                                | VISUAL_VIDEO_DEPTH_32BIT
                                | VISUAL_VIDEO_DEPTH_GL; /**< All graphical depths. */
        

    /** implemented by visual.c */
    private native CPtr videoNew();
    private native void videoUnref(CPtr videoPtr);
    private native void videoAllocateBuffer(CPtr videoPtr);
    private native void videoSetAttributes(CPtr videoPtr, int width, int height, int stride, int depth); 
    private static native int videoGetHighestDepth(int depth);
    private static native int videoGetHighestDepthNoGl(int depth);    
    private static native int videoBppFromDepth(int depth);
    private static native int videoGetBoundary();
    private static native int videoEnumFromValue(int depthvalue);
    private static native int videoRegionSub(CPtr src, CPtr rect);
    private static native int videoRegionSubByValues(int x, int y, int width, int height);
    private static native int videoRegionSubAll(int src);
    private static native int videoRegionSubWithBoundary(CPtr drect, CPtr src, CPtr srect);
     
    private static native int videoCompositeSetType(int type);
    private static native int videoSetColorkey(int color);
    private static native int videoSetSurface(int alpha);
    private static native int videoBlitOverlayRectangle(CPtr drect, CPtr src, CPtr srect, int alpha);
    private static native int videoBlitOverlayRectangleScale(CPtr drect, CPtr src, CPtr srect);
    private static native int videoBlitOverlay(CPtr src, int x, int y, int alpha);

    private static native int videoFillAlphaColor(int color, int density);
    private static native int videoFillAlpha(int density);
    private static native int videoFillAlphaRectangle(int density, int rectangle);
    private static native int videoFillColor(int color);
    private static native int videoFillColorRectangle(int color, int rectangle);
    private static native int videoFlipPixelBytes(CPtr src);
    private static native int videoRotate(CPtr src, int degrees);
    private static native int videoRotateNew(CPtr src, int degrees);
    private static native int videoMirror(CPtr src, int degrees);
    private static native int videoMirrorNew(CPtr src, int orient);
    private static native int videoDepthTransform(CPtr src);
    private static native int videoZoomNew(int CPtr, int scale_method, float zoom_factor);

    /* current VisVideo object */
    public CPtr VisVideo;    
        
    public VisVideo()
    {
        VisVideo = videoNew();
    }


    public void setAttributes(int width, int height, int stride, int depth)
    {
        videoSetAttributes(VisVideo, width, height, stride, depth);
    }

    public static int depthGetHighest(int depth)
    {
        return videoGetHighestDepth(depth);
    }

    public static int depthGetHighestNoGl(int depth)
    {
        return videoGetHighestDepthNoGl(depth);
    }

    public static int bppFromDepth(int depth)
    {
        return videoBppFromDepth(depth);
    }

    public void allocateBuffer()
    {
        videoAllocateBuffer(VisVideo);
    }
        
    @Override
    public void finalize()
    {
        videoUnref(VisVideo);
    }
}

