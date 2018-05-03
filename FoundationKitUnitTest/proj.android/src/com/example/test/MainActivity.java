package com.example.test;

import java.io.File;

import com.losemymind.foundationkit.AndroidJNIBridge;
import com.losemymind.foundationkit.AndroidJNIProxyTest;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;

public class MainActivity extends Activity {

	public static String TAG = MainActivity.class.getSimpleName();
	public static MainActivity activity = null;
	
	public static AndroidJNIProxyTest JNIProxy = null;
	

	
	@Override
	protected void onCreate(Bundle savedInstanceState) 
	{
		super.onCreate(savedInstanceState);
		activity = this;
		foundationInit((Context)this);
		
		byte B= 'B';
		short S = 10;
		int   I =10;
		long  J = 10;
		float F = 10.1f;
		double D = 10.1;
		char   C = 'C';
		boolean Z = true;
		
		Byte    b = 'b';
		Short   s = 10;
		Integer i = 10;
		Long    j = 10l;
		Float   f = 10.0f;
		Double  d = 10.1;
		Character c = 'c';
		Boolean   z = false;
		AndroidJNIBridge.sendMessage("JNIBridgeTest", B,S,I,J,F,D,C,Z, b,s,i,j,f,d,c,z);
		//Log.e("TAG", "===== JNIProxy " + JNIProxy);
		JNIProxy.OnMethodInvoke("OnMethodInvoke", I, D);
		JNIProxy.OnMethodInvoke1("OnMethodInvoke1", C, Z);
		
		AndroidJNIBridge.sendMessage("OnJavaCall", "Value", 10, 100.f, "value c");
		
	}

	public void setProxy(Object proxy){
		//Log.e("TAG", "===== setProxy " + proxy);
		JNIProxy = (AndroidJNIProxyTest)proxy;
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
        System.loadLibrary("foundationkittest");
    }
}
