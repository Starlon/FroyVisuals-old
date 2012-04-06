
#ifndef __LCDPRIV__
#define __LCDPRIV__

namespace LCD {

class LCDControl;

typedef struct {
    VisPalette pal;
	VisBuffer	pcm;
    VisTimer timer;
    VisThread *thread;
    LCDControl *control;
    VisEventQueue *events;
    uint32_t *pixels;
} LCDPrivate;

}

#endif
