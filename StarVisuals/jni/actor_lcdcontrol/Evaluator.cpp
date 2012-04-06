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

#include "Evaluator.h"
#include "SpecialChar.h"
#include "debug.h"

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
    state_ = lua_open();
    luaJIT_setmode(state_, 0, LUAJIT_MODE_ENGINE|LUAJIT_MODE_ON);
    LUAJIT_VERSION_SYM();  // linker-enforced version check 
    lua_gc(state_, LUA_GCSTOP, 0);  // stop collector during initialization 
    luaopen_io(state_);
    luaopen_base(state_);
    luaopen_table(state_);
    luaopen_string(state_);
    luaopen_math(state_);
    if(luaL_loadfile(state_, "/data/data/com.starlon.starvisuals/libstub.lua") || lua_pcall(state_, 0, 0, 0));
    if(luaL_loadfile(state_, "/data/data/com.starlon.starvisuals/pluginmath.lua") || lua_pcall(state_, 0, 0, 0));
    lua_gc(state_, LUA_GCRESTART, -1);
}

Evaluator::~Evaluator() {
    lua_close(state_);
    state_ = NULL;
}

std::string Evaluator::Eval(std::string str) 
{
    const char *ret = (const char *)"<null>";
    int s = luaL_dostring(state_, str.c_str());
    if (s == 0)
    {
        s = lua_pcall(state_, 0, LUA_MULTRET, 0);
        ret = lua_tostring(state_, -1);
        lua_settop(state_, 0);
    }
    return ret;
}


