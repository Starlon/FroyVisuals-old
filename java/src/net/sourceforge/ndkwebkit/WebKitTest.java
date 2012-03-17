package net.sourceforge.ndkwebkit;

import net.sourceforge.ndkwebkit.*;

import android.app.*;
import android.content.*;
import android.content.res.*;
import android.os.*;
import android.view.*;
import android.view.View.*;
import android.view.ViewGroup.*;
import android.view.inputmethod.*;
import android.webkit.*;
import android.widget.*;
import android.widget.AdapterView.*;

import java.io.*;
import java.util.*;

public class WebKitTest extends Activity
{
  NDKView ndkView;

  public void onCreate(Bundle savedInstanceState)
  {
    super.onCreate(savedInstanceState);
    ndkView = new NDKView(this);
    setContentView(ndkView);
  }
}
