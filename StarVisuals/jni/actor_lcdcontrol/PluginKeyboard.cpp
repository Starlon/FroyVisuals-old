/* $Id$
 * $URL$
 *
 * Copyright (C) 2009 Scott Sibley <scott@starlon.net>
 *
 * This file is part of LCDControl.
 *
 * LCDControl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LCDControl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LCDControl.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string>
#include <json/json.h>

#include "LCDCore.h"
#include "PluginKeyboard.h"

#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
//#define VROOT

using namespace LCD;

PluginKeyboard::PluginKeyboard(LCDCore *v) {
    visitor_ = v;

    //display_ = XOpenDisplay(NULL);
    //window_ = DefaultRootWindow(display_);

    started_ = true;

    poll_thread_ = new PluginKeyboardThread(this);

    poll_thread_->start();
}

PluginKeyboard::~PluginKeyboard() {
    started_ = false;
    sleep(1);
    delete poll_thread_;
}

void PluginKeyboard::PluginPoll() {
    Display *display = XOpenDisplay(NULL);
    if(!display) {
        LCDError("PluginKeyboard: Unable to open display.");
        return;
    }   

    int screen = DefaultScreen(display);

    Window window = DefaultRootWindow(display);
    XSelectInput(display, window, KeyPressMask|KeyReleaseMask);
    //int r  = XGrabKeyboard(display, window, True, GrabModeAsync, GrabModeAsync, CurrentTime);
    int r = XGrabKey(display, XKeysymToKeycode(display, XK_a), 
        0, window, False, 0, GrabModeAsync);
    while(started_) {

        //if(!XPending(display))
        //    continue;

        XEvent event;
        char str[256+1];
        KeySym ks;
        char keycode;
        int c;

        XNextEvent(display, &event);

        if(event.type == KeyPress) {
            c = XLookupString((XKeyEvent *)&event, str, 256, &ks, NULL);

            LCDError("KeyPressed: c: %d, ks: %d", c, ks);

            keycode = XKeysymToKeycode(display, ks); 

            emit _KeyPressed(keycode);
            LCDError("Keypressed: %d", keycode);
        }

        if(event.type == KeyRelease) {
            c = XLookupString((XKeyEvent *)&event, str, 256, &ks, NULL);

            keycode = XKeysymToKeycode(display, ks);

            emit _KeyReleased(keycode); 
        }
    }
    XCloseDisplay(display);
}


