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

#ifndef __LCD_CONTROL_H__
#define __LCD_CONTROL_H__

#include <map>
#include <vector>
#include <json/json.h>

#include "CFG.h"

namespace LCD {

class LCDCore;
class Evaluator;

class LCDControl : public CFG {

    bool active_;
    std::map<std::string, LCDCore *> devices_;
    std::vector<std::string> display_keys_;
    void ConfigSetup();

    public:
    LCDControl();
    ~LCDControl();
    int Start();
    void Stop();
    void Shutdown();
    LCDCore *FindDisplay(std::string name);
    void ProcessVariables(Json::Value *config, Evaluator *ev);
    bool IsActive() { return active_; }
};

}; // End namespace
#endif
