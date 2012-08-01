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

import android.content.Context;
import android.view.View;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.util.Log;




class LibVisualBitmapView extends LibVisualView 
{
    private final static String TAG = "LibVisualBitmapView"; 
    private VisualObject mVisualObject;
        
    /** constructor */
    public LibVisualBitmapView(Context context, VisualObject obj)
    {
        super(context);

        mVisualObject = obj;
            
        /* initialize actor + input */
        //fpsInit();
    }

            
    /** This is called during layout when the size of this view has changed. 
        If you were just added to the view hierarchy, you're called with the 
        old values of 0. */
    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh)
    {
            
        mVisualObject.onSizeChanged(w, h, oldw, oldh);
    }

       
    /** called whenever the contents of this view are drawn */
    @Override 
    protected void onDraw(Canvas canvas) 
    {
            
        Bitmap bitmap = mVisualObject.run();

        canvas.drawBitmap(bitmap, 0, 0, null);
            
        invalidate();
    }
}
