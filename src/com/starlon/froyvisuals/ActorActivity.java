package com.starlon.froyvisuals;

import android.app.Activity;
import android.os.Bundle;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.util.Log;

public class ActorActivity extends Activity
{
    private final static String TAG = "FroyVisuals/ActorActivity";
    private NativeHelper mHelper;
    private LinearLayout mLayout;

    // Header views
    private TextView mHeaderName;
    private TextView mHeaderLongName;
    private TextView mHeaderAuthor;
    private TextView mHeaderVersion;
    private TextView mHeaderAbout;
    private TextView mHeaderHelp;
    private TextView mHeaderLicense;
    
    // Actor field views
    private TextView mName;
    private TextView mLongName;
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

        mLayout = new LinearLayout(this);
        mLayout.setOrientation(LinearLayout.VERTICAL);

        // Headers
        mHeaderName = new TextView(this);
        mHeaderLongName = new TextView(this);
        mHeaderAuthor = new TextView(this);
        mHeaderVersion = new TextView(this);
        mHeaderAbout = new TextView(this);
        mHeaderHelp = new TextView(this);
        mHeaderLicense = new TextView(this);
   
        mHeaderName.setTextSize(20);
        mHeaderLongName.setTextSize(20);
        mHeaderAuthor.setTextSize(20);
        mHeaderVersion.setTextSize(20);
        mHeaderAbout.setTextSize(20);
        mHeaderHelp.setTextSize(20);
        mHeaderLicense.setTextSize(20);

           // Values
        mName = new TextView(this);
        mLongName = new TextView(this);
        mAuthor = new TextView(this);
        mVersion = new TextView(this);
        mAbout = new TextView(this);
        mHelp = new TextView(this);
        mLicense = new TextView(this);

        mName.setTextSize(15);
        mLongName.setTextSize(15);
        mAuthor.setTextSize(15);
        mVersion.setTextSize(15);
        mAbout.setTextSize(15);
        mHelp.setTextSize(15);
        mLicense.setTextSize(15);
    

        // Static text views on layout
        mLayout.addView(mHeaderName);
        mLayout.addView(mName);
        mLayout.addView(mHeaderLongName);
        mLayout.addView(mLongName);
        mLayout.addView(mHeaderAuthor);
        mLayout.addView(mAuthor);
        mLayout.addView(mHeaderVersion);
        mLayout.addView(mVersion);
        mLayout.addView(mHeaderAbout);
        mLayout.addView(mAbout);
        mLayout.addView(mHeaderHelp);
        mLayout.addView(mHelp);
        mLayout.addView(mHeaderLicense);
        mLayout.addView(mLicense);

        // Set headers
        mHeaderName.setText(R.string.title_plugin_name);
        mHeaderLongName.setText(R.string.title_plugin_long_name);
        mHeaderAuthor.setText(R.string.title_plugin_author);
        mHeaderVersion.setText(R.string.title_plugin_version);
        mHeaderAbout.setText(R.string.title_plugin_about);
        mHeaderHelp.setText(R.string.title_plugin_help);
        mHeaderLicense.setText(R.string.title_plugin_license);

        int current = mHelper.actorGetCurrent();

        String name_string = mHelper.actorGetName(current);

        String long_name_string = mHelper.actorGetLongName(current);
        String author_string = mHelper.actorGetAuthor(current);
        String version_string = mHelper.actorGetVersion(current);
        String about_string = mHelper.actorGetAbout(current);
        String help_string = mHelper.actorGetHelp(current);
        String license_string = mHelper.actorGetLicense(current);

        int len = name_string.length();
        char[] name_chars = name_string.toCharArray();
        mName.setText(name_chars, 0, len);

        len =long_name_string.length();
        char[]long_name_chars =long_name_string.toCharArray();
        mLongName.setText(long_name_chars, 0, len);

        len = author_string.length();
        char[] author_chars = author_string.toCharArray();
        mAuthor.setText(author_chars, 0, len);

        len = version_string.length();
        char[] version_chars = version_string.toCharArray();
        mVersion.setText(version_chars, 0, len);

        len = about_string.length();
        char[] about_chars = about_string.toCharArray();
        mAbout.setText(about_chars, 0, len);

        len = help_string.length();
        char[] help_chars = help_string.toCharArray();
        mHelp.setText(help_chars, 0, len);

        len = license_string.length();
        char[] license_chars = license_string.toCharArray();
        mVersion.setText(license_chars, 0, len);

        setContentView(mLayout);
        
    }
}
