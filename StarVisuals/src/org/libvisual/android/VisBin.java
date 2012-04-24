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




/** VisBin wrapper */
public class VisBin
{
    public CPtr VisBin;
    public VisActor actor;
    public VisInput input;
    public VisMorph morph;

    /** implemented by visual.c */
    private native CPtr binNew();
    private native int binUnref(CPtr binPtr);
    private native int binSetDepth(CPtr binPtr, int depth);
    private native int binSetSupportedDepth(CPtr binPtr, int depth);
    private native int binSetPreferredDepth(CPtr binPtr, int depth);
    private native int binSetVideo(CPtr binPtr, CPtr videoPtr);
    private native int binRealize(CPtr binPtr);
    private native int binSync(CPtr binPtr, boolean noevent);
    private native int binDepthChanged(CPtr binPtr);
    private native int binConnect(CPtr binPtr, CPtr actorPtr, CPtr inputPtr);
    private native int binSetMorphByName(CPtr binPtr, String name);
    private native int binSwitchActorByName(CPtr binPtr, String name);
    private native CPtr binGetMorph(CPtr binPtr);
    private native CPtr binGetActor(CPtr binPtr);
                
        
    public VisBin()
    {
        VisBin = binNew();
    }

    public void setDepth(int depth)
    {
        binSetDepth(VisBin, depth);
    }
        
    public void setSupportedDepth(int depth)
    {
        binSetSupportedDepth(VisBin, depth);
    }

    public void setPreferredDepth(int depth)
    {
        binSetPreferredDepth(VisBin, depth);
    }

    public void setVideo(CPtr videoPtr)
    {
        binSetVideo(VisBin, videoPtr);
    }

    public void realize()
    {
        binRealize(VisBin);
    }

    public void sync(boolean noevent)
    {
        binSync(VisBin, noevent);
    }

    public void depthChanged()
    {
        binDepthChanged(VisBin);
    }

    public void connect(CPtr actorPtr, CPtr inputPtr)
    {
        binConnect(VisBin, actorPtr, inputPtr);
        actor = new VisActor(actorPtr);
        input = new VisInput(inputPtr); 
        //morph = new VisMorph(morphPtr);
    }

    public void setMorph(String name)
    {
        binSetMorphByName(VisBin, name);
    }

    public void switchActor(String name)
    {
        binSwitchActorByName(VisBin, name);
    }

    public VisActor getActor()
    {
        return actor;
    }

    public VisInput getInput()
    {
        return input;
    }

    public VisMorph getMorph()
    {
        return morph;
    }
        
    @Override
    public void finalize()
    {
        binUnref(VisBin);
    }
}

