package com.starlon.starvisuals;

import android.content.Intent;
import android.content.Context;
import android.widget.ListView;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.SimpleAdapter;
import android.widget.ArrayAdapter;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.view.LayoutInflater;
import android.view.View;
import android.app.Activity;
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


public class AboutPluginsActivity extends ListActivity
{
        private final static String TAG = "StarVisuals/AboutPluginsActivity";
        private final static String PREFS = "StarVisualsPrefs";
        
        /** called by OS when app is created initially */
        @Override
        public void onCreate(Bundle savedInstanceState)
        {
            super.onCreate(savedInstanceState);
            setContentView(R.layout.preferences);

            final Context mContext = (Context)this;

            ListView listView = (ListView) findViewById(android.R.id.list);
            String[] values = new String[] { "About Plugins" };
            final ArrayAdapter<String> adapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_list_item_1, android.R.id.text1, values);

            listView.setAdapter(adapter);
        
            listView.setOnItemClickListener(new OnItemClickListener() {
                @Override
                public void onItemClick(AdapterView<?> parent, View view, int position, long id) 
                {
                    String item = (String) adapter.getItem(position);

                    if(item.equals("About Plugins"))
                        startActivity(new Intent(mContext, AboutPluginsActivity.class));
                    
                }
            });

        }
}

