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

#include <libusb-1.0/libusb.h>

#include "DrvFB.h"
using namespace LCD;

// LCDGraphic RealBlit
void DrvFBBlit(LCDGraphic *lcd, const int row, const int col,
    const int height, const int width) {
    ((DrvFB *)lcd)->DrvBlit(row, col, height, width);
}


// Constructor
DrvFB::DrvFB(std::string name, LCDControl *v,
    Json::Value *config, int layers) :
    LCDCore(v, name, config, LCD_GRAPHIC, (LCDGraphic *)this),
    LCDGraphic((LCDCore *)this) {
    LCDError("DrvFB");

    GraphicRealBlit = DrvFBBlit;

    Json::Value *val = CFG_Fetch(config, name + ".cols", new Json::Value(SCREEN_W));
    cols_ = val->asInt();
    delete val;

    val = CFG_Fetch(config, name + ".rows", new Json::Value(SCREEN_H));
    rows_ = val->asInt();
    delete val;

    val = CFG_Fetch(config, name + ".update", new Json::Value(10));
    update_ = val->asInt();
    delete val;

    val = CFG_Fetch(config, name + ".depth", new Json::Value(8));
    depth_ = val->asInt();
    delete val;

    val = CFG_Fetch(config, name + ".bpp", new Json::Value(8*3));
    bpp_ = val->asInt();
    delete val;

    GraphicInit(rows_, cols_, 8, 6, layers);

    drvFB = new RGBA[rows_*cols_];

}

// Destructor
DrvFB::~DrvFB() {
    update_thread_->wait();
    delete []drvFB;
}

// Initialize device and libusb
void DrvFB::SetupDevice() {
    if(update_ < 0)
        return;
}

// Deinit driver
void DrvFB::TakeDown() {
    Disconnect();
}

// Configuration setup
void DrvFB::CFGSetup() {
    LCDCore::CFGSetup();
}

// Connect -- generic method called from main code
void DrvFB::Connect() {
    if(update_ < 0)
        return;
    connected_ = true;
    GraphicClear();
    GraphicStart();
}

// Disconnect -- deinit
void DrvFB::Disconnect() {
    connected_ = false;
}

void DrvFB::DrvUpdateImg() {
        unsigned int size = cols_*rows_*3;
        uint8_t data[size];
    
        for(int i = 0; i < cols_*rows_; i++) {
            data[i*3] = drvFB[i].R;
                data[i*3+1] = drvFB[i].G;
                data[i*3+2] = drvFB[i].B;
        }
    
}

// Driver-side blit method
void DrvFB::DrvBlit(const int row, const int col, 
    const int height, const int width) {
    for(int r = row; r < row + height; r++) {
        for(int c = col; c < col + width; c++) {
            drvFB[r * cols_ + c] = GraphicRGB(r, c);
        }
    }
}

// Clear the LCD
void DrvFB::DrvClear() {
}

