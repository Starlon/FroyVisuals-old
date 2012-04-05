package com.starlon.starvisuals;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.LayoutInflater;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.ArrayAdapter;
import android.app.ListActivity;
import android.util.Log;
import android.content.Intent;


public class AboutPluginsActivity extends ListActivity
{
    private final static String TAG = "StarVisuals/AboutPluginsActivity";

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.about_plugins);
        ListView listView = (ListView) findViewById(android.R.id.list);

        final LayoutInflater factory = getLayoutInflater();

        String[] names = new String[] {
            "Actors", "Inputs", "Morphs"
        };

        final ArrayAdapter<String> adapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_list_item_1, android.R.id.text1, names);

        listView.setAdapter(adapter);
    }

    @Override
    protected void onListItemClick(ListView listView, View view, int position, long id)
    {
        String item = (String) listView.getAdapter().getItem(position);
        if(item.equals("Actors"))
                startActivity(new Intent(this, ActorPreferencesActivity.class));
        else if(item.equals("Inputs"))
                startActivity(new Intent(this, InputPreferencesActivity.class));
        else if(item.equals("Morphs"))
                startActivity(new Intent(this, MorphPreferencesActivity.class));
    }
}

