package com.starlon.froyvisuals;

import android.content.Intent;
import android.preference.CheckBoxPreference;
import android.preference.ListPreference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceCategory;
import android.preference.PreferenceScreen;
import android.preference.PreferenceManager;
import android.os.Bundle;


public class PreferencesActivity extends PreferenceActivity
{
        private final static String TAG = "FroyVisuals/PreferencesActivity";
        private final static String PREFS = "FroyVisualsPrefs";
        
        /** called by OS when app is created initially */
        @Override
        public void onCreate(Bundle savedInstanceState)
        {
            super.onCreate(savedInstanceState);

            /** create prefs from xml */
            //addPreferencesFromResource(R.xml.preferences);

            PreferenceManager prefManager = getPreferenceManager();
            prefManager.setSharedPreferencesName(PREFS); 

            setPreferenceScreen(createPreferenceHierarchy());
        }

        /** another activity comes over this activity */
        @Override
        public void onPause()
        {
            super.onPause();
            Intent i = new Intent(this, FroyVisualsReceiver.class);
            i.setAction("com.starlon.froyvisuals.PREFS_UPDATE");
            sendBroadcast(i);
        }

        private PreferenceScreen createPreferenceHierarchy()
        {
            CharSequence[] entries;
            CharSequence[] entryValues;
            int count;
            int current;
            PreferenceScreen root = getPreferenceManager().createPreferenceScreen(this);
            
            PreferenceCategory inlinePrefCat = new PreferenceCategory(this);
            inlinePrefCat.setTitle(R.string.prefs_title_general);
            root.addPreference(inlinePrefCat);

            CheckBoxPreference checkboxPref = new CheckBoxPreference(this);
            checkboxPref.setKey("prefs_morph_enabled");
            checkboxPref.setTitle(R.string.prefs_morph_enabled);
            checkboxPref.setSummary(R.string.prefs_summary_morph_enabled);
            inlinePrefCat.addPreference(checkboxPref);

            current = NativeHelper.actorGetCurrent();
            count = NativeHelper.actorCount();
            entries = new CharSequence[count];
            entryValues = new CharSequence[count];
            for(int i = 0; i < count; i++)
            {
                entryValues[i] = NativeHelper.actorGetName(i);
                entries[i] = NativeHelper.actorGetLongName(i);
            }
            ListPreference actorPref = new ListPreference(this);
            actorPref.setEntries(entries);
            actorPref.setEntryValues(entryValues);
            actorPref.setDialogTitle(R.string.prefs_actor_selection);
            actorPref.setKey("prefs_actor_selection");
            actorPref.setTitle(R.string.prefs_actor_selection);
            actorPref.setSummary(R.string.prefs_actor_selection_summary);
            //if(current >= 0)
            //    actorPref.setValueIndex(current);
            inlinePrefCat.addPreference(actorPref);

            current = NativeHelper.inputGetCurrent();
            count = NativeHelper.inputCount();
            entries = new CharSequence[count];
            entryValues = new CharSequence[count];
            for(int i = 0; i < count; i++)
            {
                entries[i] = NativeHelper.inputGetName(i);
                entryValues[i] = NativeHelper.inputGetLongName(i);
            }
            ListPreference inputPref = new ListPreference(this);
            inputPref.setEntries(entries);
            inputPref.setEntryValues(entryValues);
            inputPref.setDialogTitle(R.string.prefs_input_selection);
            inputPref.setKey("prefs_input_selection");
            inputPref.setTitle(R.string.prefs_input_selection);
            inputPref.setSummary(R.string.prefs_input_selection_summary);
            //if(current >= 0)
            //    inputPref.setValueIndex(current);
            inlinePrefCat.addPreference(inputPref);

            current = NativeHelper.morphGetCurrent();
            count = NativeHelper.morphCount();
            entries = new CharSequence[count];
            entryValues = new CharSequence[count];
            for(int i = 0; i < count; i++)
            {
                entries[i] = NativeHelper.morphGetName(i);
                entryValues[i] = NativeHelper.morphGetLongName(i);
            }
            ListPreference morphPref = new ListPreference(this);
            morphPref.setEntries(entries);
            morphPref.setEntryValues(entryValues);
            morphPref.setDialogTitle(R.string.prefs_morph_selection);
            morphPref.setKey("prefs_morph_selection");
            morphPref.setTitle(R.string.prefs_morph_selection);
            morphPref.setSummary(R.string.prefs_morph_selection_summary);
            //if(current >= 0)
            //    morphPref.setValueIndex(current);
            inlinePrefCat.addPreference(morphPref);

            return root;
        }
        
}
