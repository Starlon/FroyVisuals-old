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

#ifndef __EVALUATOR__
#define __EVALUATOR__

#include <string>

#include "SpecialChar.h"
#include "PluginCpuinfo.h"
#include <luascript/luascript.h>

namespace LCD {

class Evaluator {
    private:
    PluginCpuinfo *mCpuinfo;
    lua mScript;

    public:
    Evaluator();
    virtual ~Evaluator();
    virtual std::string Eval(std::string str);
/*
    void AddAccessor(std::string name, QScriptValue (*func)(QScriptContext *ctx, 
        QScriptEngine *eng), QFlags<QScriptValue::PropertyFlag>);
    void AddValue(std::string, QScriptValue val);
    QScriptEngine *GetEngine() { return engine_; };
*/
};

};

#endif
