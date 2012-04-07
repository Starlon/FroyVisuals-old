
#ifndef __LCDPRIV__
#define __LCDPRIV__

namespace LCD {

class LCDControl;

#define PCM_SIZE	1024

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
