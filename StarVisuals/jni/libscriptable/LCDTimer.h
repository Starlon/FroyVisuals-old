
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
    
    int mID;
    int mDuration;
    int mStartTime;
    bool mRepeating;
    bool mActive;
    std::string mName;
    VisTimer mTimer;
    VisEventQueue *mEvents;
    
    public:

    LCDTimer(VisEventQueue *queue, int id, LCDEventFunc func, void *data, int duration, bool repeating)
    {

        mID = id;
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

    int GetID()
    {
        return mID;
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
    std::vector<LCDTimer> mTimers;
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

    int AddTimer(LCDEventFunc func, void *data, int duration, bool repeating)
    {
        visual_time_get(&mTime);
        visual_random_set_seed(visual_time_get_msecs(&mTime));
        int rand = visual_random_int();
        mTimers[mTimers.size()] = LCDTimer(mEvents, rand, func, data, duration, repeating);
        return rand;
    }

    void Tick()
    {
        unsigned int i;
        for(i = 0; i < mTimers.size(); i++)
        {
            mTimers[i].Tick();
        }
    }

    void Stop()
    {
        unsigned int i;
        for(i = 0; i < mTimers.size(); i++)
        {
            mTimers[i].Stop();
        }
    }

    void Stop(int id)
    {
        unsigned int i;
        for(i = 0; i < mTimers.size(); i++)
        {
            if(id == mTimers[i].GetID())
                mTimers[i].Stop();
        }
    }
};

}; // End namespace
#endif
