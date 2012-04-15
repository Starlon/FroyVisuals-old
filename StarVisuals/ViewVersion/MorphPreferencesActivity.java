package com.starlon.starvisuals;

import android.content.Intent;
import android.widget.ListView;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.SimpleAdapter;
import android.widget.ArrayAdapter;
import android.view.LayoutInflater;
import android.view.View;
import android.app.ListActivity;
import android.preference.PreferenceActivity;
import android.preference.PreferenceFragment;
import android.content.res.Resources;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import java.util.List;
import java.util.ListIterator;
import java.util.ArrayList;
import java.util.HashMap;


public class MorphPreferencesActivity extends ListActivity
{
        private final static String TAG = "StarVisuals/PreferencesActivity";
        private final static String PREFS = "StarVisualsPrefs";
        
        /** called by OS when app is created initially */
        @Override
        public void onCreate(Bundle savedInstanceState)
        {
            super.onCreate(savedInstanceState);

            setContentView(R.layout.about_morphs);

            ListView listView = (ListView) findViewById(android.R.id.list);

            int current = NativeHelper.morphGetCurrent();

            String long_name_string = NativeHelper.morphGetLongName(current);
            String author_string = NativeHelper.morphGetAuthor(current);
            String version_string = NativeHelper.morphGetVersion(current);
            String about_string = NativeHelper.morphGetAbout(current);
            String help_string = NativeHelper.morphGetHelp(current);

            String[] values = new String[]{"Name: " + long_name_string,
                                     "Author: " + author_string, 
                                    "Version: " + version_string, 
                                    "About: " +  about_string,
                                    "Help: " + help_string};

            
            View view = (ListView) findViewById(android.R.id.list);


            final ArrayAdapter<String> adapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_list_item_1, android.R.id.text1, values);

            setListAdapter(adapter);

        }

}
