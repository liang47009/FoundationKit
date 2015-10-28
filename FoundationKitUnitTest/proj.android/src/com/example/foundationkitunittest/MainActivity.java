package com.example.foundationkitunittest;

import java.util.Arrays;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

public class MainActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) 
	{
		super.onCreate(savedInstanceState);
		foundationInit((Context)this);
	
		String str= "sjfldsfjdls";
		str += '\0';

		
		
		byte[] key = str.getBytes();
		
	
		
		byte[] randout = str.getBytes();

   

		A3A8(key, randout);
		
		 String value = new String(key);
		 String value1 = new String(randout);
		
		Log.d("MainActivity", "======:" + value + " | " + value1);
		
	}
	
	
	
	
	public native void  foundationInit(Context context);
	
	public static native void A3A8(byte[] key, byte[] randout);
	
    static 
    {
    	//System.loadLibrary("jemalloc");
        System.loadLibrary("foundationkittest");
    }
}
