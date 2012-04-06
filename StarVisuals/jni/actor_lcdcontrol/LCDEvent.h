#ifndef __LCDEVENT
#define __LCDEVENT

#include <libvisual/libvisual.h>

typedef void (*LCDEventFunc) (void *data);

namespace LCD 
{
class LCDEvent {
    private:
    VisEventQueue *mQueue;

    public:
    LCDEventFunc mFunc;
    void *mData;

    LCDEvent(VisEventQueue *queue, LCDEventFunc func, void *data)
    {
        mQueue = queue;
        mFunc = func;
        mData = data;
    }

    ~LCDEvent()
    {
    }

    void Fire()
    {
        visual_event_queue_add_generic(mQueue, 0, 0, this);
    }
};
};

#endif

