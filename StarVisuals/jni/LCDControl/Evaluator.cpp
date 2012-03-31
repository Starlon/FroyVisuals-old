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
#include "lua.h"

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
/*
    engine_ = new QScriptEngine();
    qScriptRegisterMetaType(engine_, toSpecialChar, fromSpecialChar);
    LoadPlugins();
    QScriptValue val = Eval("diskstats.Diskstats('sd.', 'read_sectors', 500);");

    LCDError("script %d", val.toInt32());
*/
}

/*
Evaluator::Evaluator(QScriptEngine *engine) {
    engine_ = engine;
    qScriptRegisterMetaType(engine_, toSpecialChar, fromSpecialChar);
    LoadPlugins();
}
*/

void Evaluator::LoadPlugins() {

/*
    QDir pluginsDir(qApp->applicationDirPath());
    pluginsDir.cd("plugins");
    foreach(string fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if(plugin) {
            PluginInterface *obj = qobject_cast<PluginInterface *>(plugin);
            if(obj) {
                obj->Connect(this);
                plugins_.push_back(obj);
            }
        } else {
            LCDError("Plugin Error: %s", 
                pluginLoader.errorString().toStdString().c_str());
        }
    }
*/
}

Evaluator::~Evaluator() {
/*
    for(std::list<PluginInterface *>::iterator it = plugins_.begin();
        it != plugins_.end(); it++) {
        (*it)->Disconnect();
    }

    delete engine_;
*/
}

std::string Evaluator::Eval(std::string str) {
/*
    QScriptValue tmp = engine_->evaluate(string(str.c_str()));
    if(tmp.isError()) LCDError("Error in evaluation: %s", tmp.toString().toStdString().c_str());
*/
    return "";
}


/*
void Evaluator::AddAccessor(std::string name, 
    QScriptValue (*func)(QScriptContext *ctx, QScriptEngine *eng), 
    QFlags<QScriptValue::PropertyFlag> flags) {
    std::cout << name << std::endl;
    engine_->globalObject().setProperty(string(name.c_str()), 
        engine_->newFunction(func), flags);
}
*/

/*
void Evaluator::AddValue(std::string name, int value) {
    LCDInfo("AddValue: %s, %f", name.c_str(), (double)val.toNumber());
    engine_->globalObject().setProperty(string(name.c_str()), val);
}
*/

/*
QScriptEngine Evaluator::Engine() {
    return engine_;
}*/

