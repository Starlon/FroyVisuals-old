package com.starlon.starvisuals;

import android.content.Intent;
import android.widget.ListView;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.SimpleAdapter;
import android.widget.ListAdapter;
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


public class InputPreferencesActivity extends ListActivity
{
        private final static String TAG = "StarVisuals/PreferencesActivity";
        private final static String PREFS = "StarVisualsPrefs";
        
        /** called by OS when app is created initially */
        @Override
        public void onCreate(Bundle savedInstanceState)
        {
            super.onCreate(savedInstanceState);

            /** create prefs from xml */

            //PreferenceManager prefManager = getPreferenceManager();
            //prefManager.setSharedPreferencesName(PREFS); 

            //setPreferenceScreen(createPreferenceHierarchy());


/*
            int current = NativeHelper.inputGetCurrent();

            String long_name_string = NativeHelper.inputGetLongName(current);
            String author_string = NativeHelper.inputGetAuthor(current);
            String version_string = NativeHelper.inputGetVersion(current);
            String about_string = NativeHelper.inputGetAbout(current);
            String help_string = NativeHelper.inputGetHelp(current);

            List<HashMap<String, String>> list = new ArrayList<HashMap<String, String>>();

            HashMap<String, String> map = new HashMap<String, String>();

            map.put(" ", long_name_string);
            map.put(" ", author_string);
            map.put(" ", version_string);
            map.put(" ", about_string);
            map.put(" ", help_string);

            list.add(map);

            String[] from = new String[]{"Name", "Author", "Version", "About", "Help"};

            int[] to = new int[]{
                R.id.input_long_name,
                R.id.input_author,
                R.id.input_version,
                R.id.input_about,
                R.id.input_help
            };

            
            LayoutInflater inflater = getLayoutInflater();

            View view = (View)inflater.inflate(R.layout.about_inputs, null, false);

            SimpleAdapter adapter = new SimpleAdapter(
                this, list, R.id.input_info_list, from, to); 

            setListAdapter(adapter);

            setContentView(R.layout.about_inputs);
*/
        }

        @Override
        protected void onListItemClick(ListView l, View v, int position, long id)
        {

        }
}
