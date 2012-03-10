package com.starlon.froyvisuals;

import android.content.BroadcastReceiver;
import android.content.Intent;
import android.content.Context;
import android.util.Log;

public class FroyVisualsReceiver extends BroadcastReceiver
{
    @Override
    public void onReceive(Context context, Intent intent)
    {
            Log.e("WTFWTF", "WTFWTFWTFW");
        String action = intent.getAction();
        if(action.equals("com.starlon.froyvisuals.PREFS_UPDATE"))
        {
            ((FroyVisuals)context).updatePrefs();
        }
    } 
}
