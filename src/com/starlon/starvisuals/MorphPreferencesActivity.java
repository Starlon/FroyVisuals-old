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


public class MorphPreferencesActivity extends ListActivity
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
            int current = NativeHelper.morphGetCurrent();

            String long_name_string = NativeHelper.morphGetLongName(current);
            String author_string = NativeHelper.morphGetAuthor(current);
            String version_string = NativeHelper.morphGetVersion(current);
            String about_string = NativeHelper.morphGetAbout(current);
            String help_string = NativeHelper.morphGetHelp(current);

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
                R.id.morph_long_name,
                R.id.morph_author,
                R.id.morph_version,
                R.id.morph_about,
                R.id.morph_help
            };

            
            LayoutInflater inflater = getLayoutInflater();

            View view = (View)inflater.inflate(R.layout.about_morphs, null, false);

            SimpleAdapter adapter = new SimpleAdapter(
                this, list, R.id.morph_info_list, from, to); 

            setListAdapter(adapter);

            setContentView(R.layout.about_morphs);
*/
        }

        @Override
        protected void onListItemClick(ListView l, View v, int position, long id)
        {

        }
}
