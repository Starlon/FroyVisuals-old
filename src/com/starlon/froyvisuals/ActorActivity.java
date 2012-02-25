package com.starlon.froyvisuals;

import android.app.Activity;
import android.os.Bundle;
import android.view.ViewGroup;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.ScrollView;
import android.util.Log;

public class ActorActivity extends Activity
{
    private final static String TAG = "FroyVisuals/ActorActivity";
    private NativeHelper mHelper;
    private LinearLayout mLayout;
    private ScrollView mScrollView;

    // Header views
    private TextView mHeaderLongName;
    private TextView mHeaderAuthor;
    private TextView mHeaderVersion;
    private TextView mHeaderAbout;
    private TextView mHeaderHelp;
    private TextView mHeaderLicense;
    
    // Actor field views
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

        mScrollView = new ScrollView(this);

        mLayout = new LinearLayout(this);
        mLayout.setOrientation(LinearLayout.VERTICAL);
        mLayout.setOverScrollMode(View.OVER_SCROLL_ALWAYS);

        mScrollView.addView(mLayout);

        // Headers
        mHeaderLongName = new TextView(this);
        mHeaderAuthor = new TextView(this);
        mHeaderVersion = new TextView(this);
        mHeaderAbout = new TextView(this);
        mHeaderHelp = new TextView(this);
        mHeaderLicense = new TextView(this);
   
        mHeaderLongName.setTextSize(40);
        mHeaderAuthor.setTextSize(40);
        mHeaderVersion.setTextSize(40);
        mHeaderAbout.setTextSize(40);
        mHeaderHelp.setTextSize(40);
        mHeaderLicense.setTextSize(40);

           // Values
        mLongName = new TextView(this);
        mAuthor = new TextView(this);
        mVersion = new TextView(this);
        mAbout = new TextView(this);
        mHelp = new TextView(this);
        mLicense = new TextView(this);

        mLongName.setTextSize(30);
        mLongName.setHeight(45);
        mLongName.setSingleLine(false);

        mAuthor.setTextSize(30);
        mAuthor.setHeight(45);
        mAuthor.setSingleLine(false);

        mVersion.setTextSize(30);
        mVersion.setHeight(45);
        mVersion.setSingleLine(false);

        mAbout.setTextSize(30);
        mAbout.setHeight(45);
        mAbout.setSingleLine(false);

        mHelp.setTextSize(30);
        mHelp.setHeight(45);
        mHelp.setSingleLine(false);

        mLicense.setTextSize(30);
        mLicense.setHeight(45);
        mLicense.setSingleLine(false);
    

        // Stack text views on layout
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
        mHeaderLongName.setText(R.string.title_plugin_long_name);
        mHeaderAuthor.setText(R.string.title_plugin_author);
        mHeaderVersion.setText(R.string.title_plugin_version);
        mHeaderAbout.setText(R.string.title_plugin_about);
        mHeaderHelp.setText(R.string.title_plugin_help);
        mHeaderLicense.setText(R.string.title_plugin_license);

        int current = mHelper.actorGetCurrent();

        String long_name_string = mHelper.actorGetLongName(current);
        String author_string = mHelper.actorGetAuthor(current);
        String version_string = mHelper.actorGetVersion(current);
        String about_string = mHelper.actorGetAbout(current);
        String help_string = mHelper.actorGetHelp(current);
        String license_string = mHelper.actorGetLicense(current);

        int len =long_name_string.length();
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

        setContentView(mScrollView);
        
    }
}
