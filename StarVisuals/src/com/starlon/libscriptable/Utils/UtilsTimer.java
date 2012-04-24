package com.starlon.libscriptable;

import java.util.Timer;
import java.util.TimerTask;
import java.util.Date;

public class UtilsTimer {
    private Timer mTimer;
    public String mName;
    private int mDuration;
    private boolean mRepeating;
    private TimerTask mTask;

    UtilsTimer(String name, int duration, boolean repeating, TimerTask task)
    {
        mTimer = new Timer(name);
        mName = name;
        mDuration = duration;
        mRepeating = repeating;
    }

    public void start()
    {
        start(mDuration, mTask);
    }

    public void start(int duration)
    {
        mDuration = duration;
        start(mDuration, mTask);
    }

    public void start(int duration, TimerTask task)
    {
        mDuration = duration;
        mTask = task;
        if(mRepeating)
            mTimer.schedule(task, duration/2, duration/2);
        else
            mTimer.schedule(task, duration);
    }

    public void set(int duration)
    {
        mDuration = duration;
    }

    public void stop()
    {
        mTimer.cancel();
        mTimer.purge();
    }
}
