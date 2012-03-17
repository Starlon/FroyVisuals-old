package net.sourceforge.ndkwebkit;

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
import android.opengl.*;
import javax.microedition.khronos.opengles.*;
import javax.microedition.khronos.egl.*;

import java.io.*;
import java.util.*;

public class NDKView extends LinearLayout
{
  static {
    System.loadLibrary("ndkcutils");
    System.loadLibrary("ndkutils");
    System.loadLibrary("ndkjpeg");
    System.loadLibrary("ndkicuuc");
    System.loadLibrary("ndkicui18n");
    System.loadLibrary("ndksqlite");
    System.loadLibrary("ndkharfbuzz");
    System.loadLibrary("ndkemoji");
    System.loadLibrary("ndkskia");
    System.loadLibrary("ndkskiagpu");
    System.loadLibrary("ndkwebkit");
  }

  private class WebSurfaceView extends SurfaceView
  {
    public WebSurfaceView(Context context)
    {
      super(context);
    }

    protected void onSizeChanged(int w, int h, int oldw, int oldh)
    {
      super.onSizeChanged(w,h,oldw,oldh);
      System.out.println("**** onSizeChanged "+w+" "+h+" "+oldw+" "+oldh);
      int width = getWidth();
      int height = getHeight();
      nativeSetSizeScreenWidthAndScale(width,height,width,1.0f,width,height,0,0,false);
    }

    public boolean onTouchEvent(MotionEvent event)
    {
      if (event.getAction() == MotionEvent.ACTION_DOWN) {
        if (event.getY() < getHeight()/2) {
          scrollY -= 100;
        }
        else {
          scrollY += 100;
        }
        nativeSetScrollOffset(moveGeneration,true,0,scrollY);
        nativeSetGlobalBounds(0,scrollY,getWidth(),getHeight());// +scrollY?
        NDKView.this.draw();
      }
      return true;
    }
  }

  public NDKView(Context context)
  {
    super(context);

    setOrientation(VERTICAL);
    surfaceView = new WebSurfaceView(context);

    Button drawButton = new Button(context);
    drawButton.setText("Draw");
    drawButton.setOnClickListener(new View.OnClickListener() {
             public void onClick(View v) {
               draw();
             }
      });

    Button testButton = new Button(context);
    testButton.setText("Test");
    testButton.setOnClickListener(new View.OnClickListener() {
             public void onClick(View v) {
               while (true) {
                 long start = System.currentTimeMillis();
                 test();
                 long middle = System.currentTimeMillis();
                 draw();
                 long end = System.currentTimeMillis();
                 long testTime = middle - start;
                 long drawTime = end - middle;
                 long total = testTime + drawTime;
                 System.out.println("**** iteration: test "+testTime+"ms, "+
                                    "draw "+drawTime+"ms, "+
                                    "total "+total+"ms");
               }
             }
      });

    LinearLayout buttonPanel = new LinearLayout(context);
    buttonPanel.setOrientation(LinearLayout.HORIZONTAL);
    buttonPanel.addView(drawButton);
    buttonPanel.addView(testButton);

    addView(buttonPanel);
    addView(surfaceView);
  }

  @Override
  protected void onAttachedToWindow()
  {
    nativeInit();
  }
  @Override
  protected void onDetachedFromWindow()
  {
    nativeDestroy();
  }

  private void draw()
  {
    nativeDraw(surfaceView.getHolder().getSurface());
  }

  private native void nativeInit();
  private native void nativeDestroy();
  private native void nativeDraw(Surface surface);
  private native void nativeSetSizeScreenWidthAndScale(int width, int height,
                                                       int textWrapWidth, float scale,
                                                       int screenWidth, int screenHeight,
                                                       int anchorX, int anchorY,
                                                       boolean ignoreHeight);
  private native void nativeSetScrollOffset(int moveGeneration, boolean sendScrollEvent,
                                            int dx, int dy);
  private native void nativeSetGlobalBounds(int x, int y, int h, int v);
  private native void test();
  private native void executeJavaScript(String script);

  private int nativePointer;

  private WebSurfaceView surfaceView;

  private int scrollY = 0;
  private int moveGeneration = 0;

}
