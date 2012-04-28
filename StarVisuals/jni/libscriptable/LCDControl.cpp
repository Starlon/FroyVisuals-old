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

#include <iostream>
#include <vector>
#include <map>
#include <json/json.h>

#include "Property.h"
#include "LCDControl.h"
#include "Evaluator.h"
#include "debug.h"
#include "DrvFB.h"

using namespace LCD;

LCDControl::LCDControl(void *priv) {
    priv_ = priv;
    //timers_ = new LCDTimerBin(priv->events);
    visual_mutex_init(&mutex_);
    active_ = false;
}

LCDControl::~LCDControl() {
    active_ = false;
/*
    Shutdown();
    for(std::vector<std::string>::iterator it = display_keys_.begin();
        it != display_keys_.end(); it++) {
        if(devices_.find(*it) != devices_.end() && devices_[*it])
            delete devices_[*it];
    }
*/
}

int LCDControl::Start() {
    CFG_Init("config.js");
    ConfigSetup();
    active_ = true;
    while(active_)
    {
        visual_mutex_lock(&mutex_);

        timers_->Tick();

        visual_mutex_unlock(&mutex_);

        visual_time_usleep(0.2 * VISUAL_USEC_PER_SEC);
    }
    return 1;
}

void LCDControl::Stop() {
    active_ = false;
}

/*
void LCDControl::ProcessVariables(Json::Value *config, Evaluator *ev) {
    Json::Value *vars = CFG_Fetch_Raw(config, "variables");
    if(vars) {
        Json::Value::Members keys = vars->getMemberNames();
        for(std::vector<std::string>::iterator it = keys.begin(); it != keys.end(); it++) {
            Json::Value *val = CFG_Fetch_Raw(vars, *it);
            if(!val)
                continue;
                LCDInfo("Adding variable <%s>", (*it).c_str());
            QScriptValue *scriptVal;
            switch(val->type()) {
                case Json::intValue:
                    LCDInfo("int value");
                    scriptVal = new QScriptValue((int)val->asInt());
                    break;
                case Json::uintValue:
                    scriptVal = new QScriptValue((uint32_t)val->asUInt());
                    break;
                case Json::realValue:
                    scriptVal = new QScriptValue((double)val->asDouble());
                    break;
                case Json::stringValue:
                    scriptVal = new QScriptValue(val->asCString());
                    break;
                case Json::booleanValue:
                    scriptVal = new QScriptValue(val->asBool());
                    break;
                case Json::arrayValue:
                {
                    QScriptValue array = GetEngine()->newArray();
                    int i = 0;
                    while(true) {
                        Json::Value m = val[i];
                        switch(m.type()) {
                            case Json::intValue:
                                array.setProperty(i, (int)m.asInt());
                                break;
                            case Json::uintValue:
                                array.setProperty(i, (uint32_t)m.asUInt());
                                break;
                            case Json::realValue:
                                array.setProperty(i, (double)m.asDouble());
                                break;
                            case Json::stringValue:
                                array.setProperty(i, m.asCString());
                                break;
                            case Json::booleanValue:
                                array.setProperty(i, m.asBool());
                                break;
                            case Json::nullValue:
                                goto exit_loop;
                            default:
                                break;
                        }
                    }
                    exit_loop:

                    scriptVal = new QScriptValue(array);
                    break;
                }
                default:
                    break;
            }
            ev->AddValue(*it, *scriptVal);
        }
        delete vars;
    }
}
*/

void LCDControl::ConfigSetup() {
    if(!CFG_Get_Root()) return;

    Json::Value::Members keys = CFG_Get_Root()->getMemberNames();

    for(std::vector<std::string>::iterator it = keys.begin(); it != keys.end(); it++ ) {
        if(it->find("display_", 0) != std::string::npos) {
            Json::Value *display = CFG_Fetch_Raw(CFG_Get_Root(), it->c_str());
            Json::Value *driver = CFG_Fetch_Raw(display, "driver");
            if(!driver) {
                LCDError("CFG: Must specify driver <%s>", it->c_str());
                continue;
            }
            Json::Value *rows = CFG_Fetch(display, "rows", new Json::Value(0));
            /*if(!rows->isNumeric() || rows->asInt() == -1) {
                LCDError("Display <%s> requires number of rows to initialize.", it->c_str());
                delete display;
                delete driver;
                continue;
            }*/
            Json::Value *cols = CFG_Fetch(display, "cols", new Json::Value(0));
            /*if(!cols->isNumeric() || rows->asInt() == -1) {
                LCDError("Display <%s> requires number of columns to initialize.", it->c_str());
                delete display;
                delete driver;
                delete rows;
                continue;
            }*/
            Json::Value *layers = CFG_Fetch(display, "layers", new Json::Value(1));

            Json::Value *model = CFG_Fetch_Raw(display, "model");
            if(driver->asString() == "crystalfontz") {
/*
                if(model) {
                    devices_[*it] = DrvCrystalfontz::Get(*it, this, 
                        CFG_Get_Root(), model->asString(), layers->asInt());
                } else {
                    LCDError("Device <%s> requires a model.", it->c_str());
                    delete display;
                    delete driver;
                    delete rows;
                    delete cols;
                    continue;
                }
*/
            } else if(driver->asString() == "fb") {
                devices_[*it] = new DrvFB(*it, this, CFG_Get_Root(), layers->asInt());
/*
            } else if(driver->asString() == "qt") {
                devices_[*it] = new DrvQt(*it, this, CFG_Get_Root(), 
                    rows->asInt(), cols->asInt(), layers->asInt());
            } else if(driver->asString() == "qtgraphic") {
                devices_[*it] = new DrvQtGraphic(*it, this, CFG_Get_Root(),
                    rows->asInt(), cols->asInt(), layers->asInt());
            } else if(driver->asString() == "pertelian") {
                devices_[*it] = new DrvPertelian(*it, this, CFG_Get_Root(), 
                    layers->asInt());
            } else if(driver->asString() == "picographic") {
                devices_[*it] = new DrvPicoGraphic(*it, this, CFG_Get_Root(), 
                    layers->asInt());
            } else if(driver->asString() == "sdl") {
                devices_[*it] = new DrvSDL(*it, this, CFG_Get_Root(),
                    layers->asInt());
            } else if(driver->asString() == "lcdproc") {
                devices_[*it] = new DrvLCDProc(*it, this, CFG_Get_Root(), layers->asInt());
            */
            } else {
                continue;
            }
            if(model) delete model;
            delete display;
            delete driver;
            delete rows;
            delete cols;
        }
        
    }

/*
    for(std::map<std::string, LCDCore *>::iterator it = 
        devices_.begin(); it != devices_.end(); it++) {
        display_keys_.push_back(it->first);
        LCDError("Starting <%s> %p", it->first.c_str(), it->second);
        LCDCore *device = it->second;
        device->CFGSetup();
        device->SetupDevice();
        device->Connect();
        device->BuildLayouts();
        device->StartLayout();
    }
*/
}

void LCDControl::Shutdown() {
/*
    for(std::map<std::string, LCDCore *>::iterator it =
        devices_.begin(); it != devices_.end(); it++ ) {
        it->second->TakeDown();
    }
*/
}

LCDCore *LCDControl::FindDisplay(std::string name) {
    std::map<std::string, LCDCore *>::iterator dev = devices_.find(name);
    if(dev == devices_.end())
        return NULL;
    return dev->second;
}

