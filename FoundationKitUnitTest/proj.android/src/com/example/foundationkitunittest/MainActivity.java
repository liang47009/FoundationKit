package com.example.foundationkitunittest;
import com.losemymind.foundationkit.AndroidJavaBridge;

import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Context;
import android.os.Build;
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
		
		@SuppressWarnings("deprecation")
		String version_sdk = Build.VERSION.SDK; // 设备SDK版本  
		String version_release = Build.VERSION.RELEASE; // 设备的系统版本 
		Log.d("TAG","===== version_sdk " + version_sdk);
		Log.d("TAG","===== version_release" + version_release);
		
		final int version_code = android.os.Build.VERSION.SDK_INT;
		if(version_code >= Build.VERSION_CODES.ICE_CREAM_SANDWICH)
		
		foundationInit((Context)this);
		char[] arr = {'a','b','c','d','e'};
		AndroidJavaBridge.getInstance().invoke("getTestValue", 10, 999, "This is test", arr);
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
