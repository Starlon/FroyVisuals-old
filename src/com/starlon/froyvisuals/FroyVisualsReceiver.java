package com.starlon.froyvisuals;

import android.content.BroadcastReceiver;
import android.content.Intent;
import android.content.Context;

public class FroyVisualsReceiver extends BroadcastReceiver
{
    @Override
    public void onReceive(Context context, Intent intent)
    {
        String action = intent.getAction();
        if(action.equals("com.starlon.froyvisuals.PREFS_UPDATE"))
        {
            ((FroyVisuals)context).updatePrefs();
        }
    } 
}
