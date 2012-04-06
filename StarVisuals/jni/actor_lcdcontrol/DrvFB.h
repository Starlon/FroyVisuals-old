
#ifndef __DRV_NIFTY_H__
#define __DRV_NIFTY_H__

#include <string>

#include "LCDGraphic.h"
#include "LCDCore.h"
#include "LCDControl.h"
#include "RGBA.h"
#include "debug.h"

#define SCREEN_H 64
#define SCREEN_W 256

namespace LCD {

class DrvFB : public LCDCore, public LCDGraphic {

    RGBA *drvFB;

    bool connected_;
    int update_;
    int cols_;
    int rows_;
    int bpp_;
    int depth_;

    void DrvClear();
    void DrvUpdateImg();
    void DrvUpdate();

    public:
    DrvFB(std::string name, LCDControl *v,
        Json::Value *config, int layers);
    ~DrvFB();    
    void SetupDevice();
    void TakeDown();
    void CFGSetup();
    void Connect();
    void Disconnect();
    void UpdateThread();
    void DrvBlit(const int row, const int col, const int height, const int width);

};


}; // End namespace

#endif
