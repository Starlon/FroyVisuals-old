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




/** VisPlugin wrapper */
public class VisPlugin
{
    public CPtr VisPlugin;

    /** implemented by visual.c */
    private native String pluginGetName(CPtr pluginPtr);
    private native String pluginGetPlugname(CPtr pluginPtr);
    private native String pluginGetAuthor(CPtr pluginPtr);
    private native String pluginGetVersion(CPtr pluginPtr);
    private native String pluginGetAbout(CPtr pluginPtr);
    private native String pluginGetHelp(CPtr pluginPtr);
    private native String pluginGetLicense(CPtr pluginPtr);
        
        
    public VisPlugin(CPtr pluginPtr)
    {
        VisPlugin = pluginPtr;
    }

    public String getName()
    {
        return pluginGetName(VisPlugin);
    }
        
    public String getPlugname()
    {
        return pluginGetPlugname(VisPlugin);
    }

    public String getAuthor()
    {
        return pluginGetAuthor(VisPlugin);
    }

    public String getVersion()
    {
        return pluginGetVersion(VisPlugin);
    }

    public String getAbout()
    {
        return pluginGetAbout(VisPlugin);
    }

    public String getHelp()
    {
        return pluginGetHelp(VisPlugin);
    }

    public String getLicense()
    {
        return pluginGetLicense(VisPlugin);
    }
}

