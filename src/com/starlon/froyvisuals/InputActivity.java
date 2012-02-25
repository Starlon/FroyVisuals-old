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

public class InputActivity extends Activity
{
    private final static String TAG = "FroyVisuals/InputActivity";
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

        final LayoutInflater finputy = getLayoutInflater();

        final View view = finputy.inflate(R.layout.input, null);

        mLongName = (TextView)view.findViewById(R.id.input_long_name);
        mAuthor = (TextView)view.findViewById(R.id.input_author);
        mVersion = (TextView)view.findViewById(R.id.input_version);
        mAbout = (TextView)view.findViewById(R.id.input_about);
        mHelp = (TextView)view.findViewById(R.id.input_help);
        mLicense = (TextView)view.findViewById(R.id.input_license);

        int current = mHelper.inputGetCurrent();

        if(current < 0)
        {
            Log.w(TAG, "No input plugin available to display.");
            return;
        }

        String long_name_string = mHelper.inputGetLongName(current);
        String author_string = mHelper.inputGetAuthor(current);
        String version_string = mHelper.inputGetVersion(current);
        String about_string = mHelper.inputGetAbout(current);
        String help_string = mHelper.inputGetHelp(current);
        String license_string = mHelper.inputGetLicense(current);

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

        setContentView(view);
        
    }
}
