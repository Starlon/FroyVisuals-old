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

#ifndef __PLUGIN_INTERFACE__
#define __PLUGIN_INTERFACE__

#include "LCDCore.h"

namespace LCD {

class Evaluator;

class PluginInterface {
    public:
    virtual ~PluginInterface() {}
    virtual void Connect(Evaluator *visitor) = 0;
    virtual void Disconnect() = 0;
};

}; // End namespace

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(LCD::PluginInterface, "LCDControl.PluginInterface/1.0");
QT_END_NAMESPACE

#endif
