package com.example.test;
import com.losemymind.foundationkit.AndroidJavaBridge;

import android.app.Activity;
import android.content.Context;
import android.net.ConnectivityManager;
import android.os.Bundle;
import android.telephony.TelephonyManager;
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
	
		TelephonyManager manager = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
		Log.e("getDeviceId",manager.getDeviceId());//deviceID
		Log.e("getNetworkOperatorName",manager.getNetworkOperatorName());//运营商名称
		Log.e("isNetworkRoaming",manager.isNetworkRoaming()+"");//是否漫游
		Log.e("getNetworkCountryIso",manager.getNetworkCountryIso());//国家代码
		Log.e("getNetworkType",manager.getNetworkType()+"");//当前网络类型，2G\3G\4G
		Log.e("getSimState",manager.getSimState()+"");//默认SIM卡状态
		Log.e("getSimOperator",manager.getSimOperator());//SIM卡经营者
		Log.e("getSimOperatorName",manager.getSimOperatorName());//SIM卡经营者名称
		Log.e("getSimCountryIso",manager.getSimCountryIso());//SIM卡经营者所在国家代码
		Log.e("getSimSerialNumber",manager.getSimSerialNumber());//SIM卡序列号
		Log.e("getLine1Number",manager.getLine1Number());//手机号
		Log.e("Android version is ",android.os.Build.VERSION.RELEASE );
		Log.e("Android manufacturer is ",android.os.Build.MANUFACTURER );
		Log.e("Android model is ",android.os.Build.MODEL );
		//Log.e("Build.SERIAL", android.os.Build.getSerial());
		foundationInit((Context)this);
	}

   @Override  
    public void onBackPressed() {  
        super.onBackPressed();  
        System.out.println("按下了back键   onBackPressed()"); 
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
