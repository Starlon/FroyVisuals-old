
#ifndef __DRV_NIFTY_H__
#define __DRV_NIFTY_H__

#include <QObject>
#include <QThread>
#include <string>
#include <libnifty/libnifty.h>
#include <libnifty/mutex.h>
#include <libnifty/client/remote.h>
#include <libnifty/client/input.h>


#include "LCDGraphic.h"
#include "LCDCore.h"
#include "LCDControl.h"
#include "RGBA.h"
#include "debug.h"

#define SCREEN_H 64
#define SCREEN_W 256

namespace LCD {

class DrvNifty : public LCDCore, public LCDGraphic {

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
    DrvNifty(std::string name, LCDControl *v,
        Json::Value *config, int layers);
    ~DrvNifty();    
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
