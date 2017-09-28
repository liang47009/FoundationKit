package com.example.test;
import com.losemymind.foundationkit.AndroidJavaBridge;

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
		String version_sdk = Build.VERSION.SDK; // è®¾å¤‡SDKç‰ˆæœ¬  
		String version_release = Build.VERSION.RELEASE; // è®¾å¤‡çš„ç³»ç»Ÿç‰ˆæœ? 
		Log.e("TAG","===== version_sdk " + version_sdk);
		Log.e("TAG","===== version_release" + version_release);
		
		final int version_code = android.os.Build.VERSION.SDK_INT;
		if(version_code >= Build.VERSION_CODES.ICE_CREAM_SANDWICH)
		
		foundationInit((Context)this);
		char[] arr = {'a','b','c','d','e'};
		AndroidJavaBridge.getInstance().invoke("CallAndroidJavaBridge", 10, "This is test");
	}
	
   @Override  
    public void onBackPressed() {  
        super.onBackPressed();  
        System.out.println("°´ÏÂÁËback¼ü   onBackPressed()"); 
        this.finish();
        //android.os.Process.killProcess(android.os.Process.myPid());
        System.exit(0);
    } 
	   
	public void debug_Print(int line, String fileName, String message)
	{
		Log.e("TAG", "=====debugPrint line:" + line + " file:" + fileName + " msg:"+message);
	}
	
	public static String getClassName()
	{
		return TAG;
	}
	
	public native void  foundationInit(Context context);

    static 
    {
    	System.loadLibrary("foundationkit");
        System.loadLibrary("foundationkittest");
    }
}
