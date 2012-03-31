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

#ifndef __PLUGIN_KEYBOARD_H__
#define __PLUGIN_KEYBOARD_H__

#include <X11/Xlib.h>

namespace LCD {

class LCDText;
class LCDGraphic;

class LCDCore;

class PluginKeyboardThread;

class PluginKeyboard {
    Q_OBJECT
    LCDCore *visitor_;
    PluginKeyboardThread *poll_thread_;
    bool started_;
    Display *display_;
    Window window_;
    int screen_;

    public:
    PluginKeyboard(LCDCore *v);
    ~PluginKeyboard();
    void PluginPoll();

    signals:
    void _KeyPressed(const char key);
    void _KeyReleased(const char key);
};


class PluginKeyboardThread : public QThread {
    Q_OBJECT
    PluginKeyboard *visitor_;

    protected:
    void run() { visitor_->PluginPoll(); }

    public:
    PluginKeyboardThread(PluginKeyboard *v) { visitor_ = v; }
};

}; // End namespace

#endif
