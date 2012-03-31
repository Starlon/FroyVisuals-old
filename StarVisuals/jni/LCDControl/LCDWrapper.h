// This file is not used without QT
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

#ifndef __WRAPPER_H__
#define __WRAPPER_H__

#include "debug.h"

namespace LCD {

class LCDCore;

class LCDText;

class LCDInterface {
    public:
    virtual ~LCDInterface() {}
    // Slots
    virtual void TextSetSpecialChars() = 0;
    virtual void LayoutChangeBefore() = 0;
    virtual void LayoutChangeAfter() = 0;
    virtual void TextSpecialCharChanged(int i) = 0;
    virtual void ChangeLayout() = 0;
    virtual void LayoutTransition() = 0;
    virtual void TransitionFinished() = 0;
    virtual void KeypadEvent(const int) = 0;
};

class LCDEvents {
    public:

    // Signals
    virtual void _TextSpecialCharsSet() = 0;
    virtual void _TransitionFinished() = 0;
    virtual void _SpecialCharChanged(int ch) = 0;
    virtual void _LayoutChangeBefore() = 0;
    virtual void _LayoutChangeAfter() = 0;
    virtual void _DisplayDisconnectedBefore() = 0;
    virtual void _DisplayDisconnectedAfter() = 0;
    virtual void _DisplayConnected() = 0;
    virtual void _KeypadEvent(const int) = 0;
    virtual void _ResizeLCD(int rows, int cols, int old_rows, int old_cols) = 0;
    virtual void _ResizeBefore(int rows, int cols) = 0;
    virtual void _ResizeAfter() = 0;
};

class LCDWrapper : public LCDInterface, public LCDEvents {
    LCDInterface *wrappedObject;

    public:
    LCDWrapper(LCDInterface *v, QObject *parent);

    public slots:
    void TextSetSpecialChars() { wrappedObject->TextSetSpecialChars(); };
    void LayoutChangeBefore() { wrappedObject->LayoutChangeBefore(); };
    void LayoutChangeAfter() { wrappedObject->LayoutChangeAfter(); };
    void TextSpecialCharChanged(int i) { 
        wrappedObject->TextSpecialCharChanged(i); };
    void ChangeLayout() { wrappedObject->ChangeLayout(); }
    void LayoutTransition() { wrappedObject->LayoutTransition(); }
    void TransitionFinished() { wrappedObject->TransitionFinished(); }
    void KeypadEvent(const int k) { wrappedObject->KeypadEvent(k); }

    signals:
    void _TextSpecialCharsSet();
    void _TransitionFinished();
    void _SpecialCharChanged(int ch);
    void _LayoutChangeBefore();
    void _LayoutChangeAfter();
    void _DisplayDisconnectedBefore();
    void _DisplayDisconnectedAfter();
    void _DisplayConnected();
    void _KeypadEvent(const int key);
    void _ResizeLCD(int rows, int cols, int old_rows, int old_cols);
    void _ResizeBefore(int rows, int cols);
    void _ResizeAfter();

   
};

}; // End namespace

#endif
