package com.starlon.starvisuals;

import android.appwidget.AppWidgetManager;
import android.content.Intent;
import android.content.BroadcastReceiver;
import android.preference.CheckBoxPreference;
import android.preference.EditTextPreference;
import android.preference.ListPreference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceCategory;
import android.preference.PreferenceScreen;
import android.preference.SwitchPreference;
import android.preference.PreferenceManager;
import android.view.LayoutInflater;
import android.os.Bundle;
import android.util.Log;




public class EditSettingsActivity extends PreferenceActivity
{
        private final static String TAG = "StarVisuals/EditSettingsActivity";
        private final static String PREFS = "StarVisualsPrefs";
        
        private NativeHelper mNativeHelper;
        
        
        /** called by OS when app is created initially */
        @Override
        public void onCreate(Bundle savedInstanceState)
        {
            super.onCreate(savedInstanceState);

            PreferenceManager prefManager = getPreferenceManager();
            prefManager.setSharedPreferencesName(PREFS); 

            setPreferenceScreen(createPreferenceHierarchy());
            addPreferencesFromResource(R.xml.preferences);
        }

        /** another activity comes over this activity */
        @Override
        public void onPause()
        {
            super.onPause();
/*
            Intent i = new Intent(this, BroadcastReceiver.class);
            i.setAction("com.starlon.froyvisuals.PREFS_UPDATE");
            sendBroadcast(i);
*/
        }

        private PreferenceScreen createPreferenceHierarchy()
        {
            CharSequence[] entries;
            CharSequence[] entryValues;
            int count;
            int current;

            PreferenceScreen root = getPreferenceManager().createPreferenceScreen(this);
            
            PreferenceCategory inlinePrefCat = new PreferenceCategory(this);
            inlinePrefCat.setTitle(R.string.prefs_text_title_general);
            root.addPreference(inlinePrefCat);

            current = mNativeHelper.actorGetCurrent();
            count = mNativeHelper.actorCount();
            entries = new CharSequence[count];
            entryValues = new CharSequence[count];
            for(int i = 0; i < count; i++)
            {
                entryValues[i] = mNativeHelper.actorGetName(i);
                entries[i] = mNativeHelper.actorGetLongName(i);
            }
            ListPreference actorPref = new ListPreference(this);
            actorPref.setEntries(entries);
            actorPref.setEntryValues(entryValues);
            actorPref.setDialogTitle(R.string.prefs_text_actor_selection);
            actorPref.setKey("prefs_actor_selection");
            actorPref.setTitle(R.string.prefs_text_actor_selection);
            actorPref.setSummary(R.string.prefs_text_actor_selection_summary);
            if(current >= 0)
                actorPref.setValueIndex(current);
            inlinePrefCat.addPreference(actorPref);

            current = mNativeHelper.inputGetCurrent();
            count = mNativeHelper.inputCount();
            entries = new CharSequence[count];
            entryValues = new CharSequence[count];
            for(int i = 0; i < count; i++)
            {
                entryValues[i] = mNativeHelper.inputGetName(i);
                entries[i] = mNativeHelper.inputGetLongName(i);
            }
            ListPreference inputPref = new ListPreference(this);
            inputPref.setEntries(entries);
            inputPref.setEntryValues(entryValues);
            inputPref.setDialogTitle(R.string.prefs_text_input_selection);
            inputPref.setKey("prefs_input_selection");
            inputPref.setTitle(R.string.prefs_text_input_selection);
            inputPref.setSummary(R.string.prefs_text_input_selection_summary);
            if(current >= 0)
                inputPref.setValueIndex(current);
            inlinePrefCat.addPreference(inputPref);

            current = mNativeHelper.morphGetCurrent();
            count = mNativeHelper.morphCount();
            entries = new CharSequence[count];
            entryValues = new CharSequence[count];
            for(int i = 0; i < count; i++)
            {
                entryValues[i] = mNativeHelper.morphGetName(i);
                entries[i] = mNativeHelper.morphGetLongName(i);
            }
            ListPreference morphPref = new ListPreference(this);
            morphPref.setEntries(entries);
            morphPref.setEntryValues(entryValues);
            morphPref.setDialogTitle(R.string.prefs_text_morph_selection);
            morphPref.setKey("prefs_morph_selection");
            morphPref.setTitle(R.string.prefs_text_morph_selection);
            morphPref.setSummary(R.string.prefs_text_morph_selection_summary);
            if(current >= 0)
                morphPref.setValueIndex(current);
            inlinePrefCat.addPreference(morphPref);

            return root;
        }
        
}
