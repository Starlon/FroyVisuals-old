package com.starlon.froyvisuals;

import android.app.Activity;
import android.os.Bundle;
import android.view.ViewGroup;
import android.view.View;
import android.view.LayoutInflater;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.ScrollView;
import android.util.Log;

public class ActorActivity extends Activity
{
    private final static String TAG = "FroyVisuals/ActorActivity";
    private NativeHelper mHelper;
    private static TextView mLongName;
    private TextView mAuthor;
    private TextView mVersion;
    private TextView mAbout;
    private TextView mHelp;
    private TextView mLicense;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        final LayoutInflater factory = getLayoutInflater();

        final View view = factory.inflate(R.layout.actor, null);

        mLongName = (TextView)view.findViewById(R.id.actor_long_name);
        mAuthor = (TextView)view.findViewById(R.id.actor_author);
        mVersion = (TextView)view.findViewById(R.id.actor_version);
        mAbout = (TextView)view.findViewById(R.id.actor_about);
        mHelp = (TextView)view.findViewById(R.id.actor_help);
        mLicense = (TextView)view.findViewById(R.id.actor_license);

        int current = mHelper.actorGetCurrent();

        if(current < 0)
        {
            Log.w(TAG, "No actor plugin available to display.");
            return;
        }

        String long_name_string = mHelper.actorGetLongName(current);
        String author_string = mHelper.actorGetAuthor(current);
        String version_string = mHelper.actorGetVersion(current);
        String about_string = mHelper.actorGetAbout(current);
        String help_string = mHelper.actorGetHelp(current);
        String license_string = mHelper.actorGetLicense(current);

        if(long_name_string != null)
        {
            int len =long_name_string.length();
            char[]long_name_chars =long_name_string.toCharArray();
            mLongName.setText(long_name_chars, 0, len);
        }

        if(author_string != null)
        {
            int len = author_string.length();
            char[] author_chars = author_string.toCharArray();
            mAuthor.setText(author_chars, 0, len);
        }

        if(version_string != null)
        {
            int len = version_string.length();
            char[] version_chars = version_string.toCharArray();
            mVersion.setText(version_chars, 0, len);
        }

        if(about_string != null)
        {
            int len = about_string.length();
            char[] about_chars = about_string.toCharArray();
            mAbout.setText(about_chars, 0, len);
        }

        if(help_string != null)
        {
            int len = help_string.length();
            char[] help_chars = help_string.toCharArray();
            mHelp.setText(help_chars, 0, len);
        }

        if(license_string != null)
        {
            int len = license_string.length();
            char[] license_chars = license_string.toCharArray();
            mVersion.setText(license_chars, 0, len);
        }

        setContentView(view);
        
    }
}
