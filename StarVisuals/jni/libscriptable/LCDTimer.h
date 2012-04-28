
#ifndef __LCDTIMER
#define __LCDTIMER

#include <string>
#include <vector>

#include <libvisual/libvisual.h>
#include "LCDEvent.h"

namespace LCD 
{

class LCDTimer
{
    private:
    LCDEvent *mEvent;
    
    int mDuration;
    int mStartTime;
    bool mRepeating;
    bool mActive;
    std::string mName;
    VisTimer mTimer;
    VisEventQueue *mEvents;
    
    public:

    LCDTimer(VisEventQueue *queue, LCDEventFunc func, void *data, int duration, bool repeating)
    {

        mEvent = new LCDEvent(queue, func, data);
        mDuration = duration;
        mRepeating = repeating;
        mStartTime = 0;
        mEvents = queue;

        visual_timer_init(&mTimer);
        visual_object_ref(VISUAL_OBJECT(queue));
        
    }

    ~LCDTimer()
    {
        delete mEvent;
        visual_object_unref(VISUAL_OBJECT(mEvents));
    }

    void Start(int duration, void *data, LCDEventFunc func)
    {
        visual_timer_start(&mTimer);
    }

    void Stop()
    {
        visual_timer_stop(&mTimer);
    }

    void Tick()
    {
        if(visual_timer_elapsed_msecs(&mTimer) > mDuration)
        {
            mEvent->Fire();
            if(mRepeating)
            {
                visual_timer_reset(&mTimer);
                visual_timer_start(&mTimer);
            }
            else
            {
                visual_timer_stop(&mTimer);
            }
        }
    }
};

class LCDTimerBin {
    private:
    std::vector<LCDTimer *> mTimers;
    VisTime mTime;
    VisEventQueue *mEvents;
    
    public:
    LCDTimerBin(VisEventQueue *events)
    {
        mEvents = events;
        visual_time_init(&mTime);
        visual_object_ref(VISUAL_OBJECT(events));
    }
    ~LCDTimerBin()
    {
        visual_object_unref(VISUAL_OBJECT(mEvents));
    }

    LCDTimer *AddTimer(LCDEventFunc func, void *data, int duration, bool repeating)
    {
        int n = mTimers.size();
        mTimers[n] = new LCDTimer(mEvents, func, data, duration, repeating);
        return mTimers[n];
    }

    void Tick()
    {
        unsigned int i;
        for(i = 0; i < mTimers.size(); i++)
        {
            mTimers[i]->Tick();
        }
    }

    void Stop()
    {
        unsigned int i;
        for(i = 0; i < mTimers.size(); i++)
        {
            mTimers[i]->Stop();
        }
    }
};

}; // End namespace
#endif
