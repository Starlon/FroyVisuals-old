
#ifndef __LCDTIMER
#define __LCDTIMER

#include <string>
#include <vector>

#include <libvisual/libvisual.h>

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
    
    
    public:

    LCDTimer(int id, LCDEventFunc *func, void *data, int duration, bool repeating)
    {

        mID = id;
        mEvent = new LCDEvent(func, data);
        mDuration = duration;
        mRepeating = repeating;
        mStartTime = 0;

        visual_timer_init(&mTimer);
        
    }

    ~LCDTimer()
    {
        delete mEvent;
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
}

class LCDTimerBin {
    private:
    std::vector<LCDTimer> mTimers;
    VisTime mTime;
    
    public:
    LCDTimerBin()
    {
        visual_time_init(&mTime);
    }
    ~LCDTimerBin()
    {

    }

    int AddTimer(LCDEventFunc func, void *data, int duration, bool repeating)
    {
        visual_time_get(&mTime);
        visual_random_set_seed(visual_time_get_msecs(mTime));
        int rand = visual_random_int();
        mTimers[mTimers.count()] = LCDTimer(rand, func, data, duration, repeating);
        return rand;
    }

    void Tick()
    {
        int i;
        for(i = 0; i < mTimers.count(); i++)
        {
            mTimers[i].Tick();
        }
    }

    void Stop()
    {
        int i;
        for(i = 0; i < mTimers.count(); i++)
        {
            mTimers[i].Stop();
        }
    }

    void Stop(int id)
    {
        int i;
        for(i = 0; i < mTimers.count(); i++)
        {
            if(id == mTimers[i].GetID())
                mTimers[i].Stop();
        }
    }
}
#endif
