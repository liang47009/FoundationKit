package com.example.foundationkitunittest;
import com.losemymind.foundationkit.AndroidJavaBridge;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;

public class MainActivity extends Activity {

	public static String TAG = MainActivity.class.getSimpleName();
	public static MainActivity activity = null;
	
	public boolean isEnable = true;
	
	public int     value = 10000;
	
	public Bundle   instanceState = null;
	
	
	@Override
	protected void onCreate(Bundle savedInstanceState) 
	{
		super.onCreate(savedInstanceState);
		instanceState = savedInstanceState;
		activity = this;
		foundationInit((Context)this);
		float[]  fs={10.f,11.f};
		MainActivity[] activities = {this, this, this};
		AndroidJavaBridge.getInstance().invoke("debugPrint", 10, 10.f, 'c', "This is test", this,fs, activities);

	}
	
	public void debugPrint(int line, String fileName, String message)
	{
		Log.d("TAG", "debugPrint line:" + line + " file:" + fileName + " msg:"+message);
	}
	
	public native void  foundationInit(Context context);

    static 
    {
        System.loadLibrary("foundationkittest");
    }
}
