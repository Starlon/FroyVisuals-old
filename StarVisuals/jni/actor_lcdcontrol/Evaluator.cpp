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
#include <iostream>
#include <lua/lua.hpp>
#include <libvisual/libvisual.h>

#include "Evaluator.h"
#include "SpecialChar.h"
#include "debug.h"
#include "PluginCpuinfo.h"

using namespace LCD;

/*
Q_DECLARE_METATYPE(SpecialChar)

QScriptValue toSpecialChar(QScriptEngine *engine, const SpecialChar &ch) {
    QScriptValue obj = engine->newObject();
    QScriptValue array = engine->newArray(ch.Size());
    std::vector<int> matrix = ch.Vector();
    for(int i = 0; i < ch.Size(); i++)
        array.setProperty(i, QScriptValue(engine, matrix[i]));
    obj.setProperty("chars", array);
    return obj;
}
*/

/*
void fromSpecialChar(const QScriptValue &obj, SpecialChar &ch) {
    for(int i = 0; obj.property(i).isValid(); i++ )
        ch.AddChar(obj.property(i).toUInt16());
}
*/

Evaluator::Evaluator() {
    mCpuinfo = new PluginCpuinfo(mLua);
}

Evaluator::~Evaluator() {
    delete mCpuinfo;
}

std::string Evaluator::Eval(std::string str) 
{
    mLua.executeCode("function ____wrap____() " + str + " end");
    return mLua.callLuaFunction<std::string>("____wrap____");
}


