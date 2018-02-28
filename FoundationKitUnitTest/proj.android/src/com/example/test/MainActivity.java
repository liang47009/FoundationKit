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
		Log.e("getNetworkOperatorName",manager.getNetworkOperatorName());//��Ӫ������
		Log.e("isNetworkRoaming",manager.isNetworkRoaming()+"");//�Ƿ�����
		Log.e("getNetworkCountryIso",manager.getNetworkCountryIso());//���Ҵ���
		Log.e("getNetworkType",manager.getNetworkType()+"");//��ǰ�������ͣ�2G\3G\4G
		Log.e("getSimState",manager.getSimState()+"");//Ĭ��SIM��״̬
		Log.e("getSimOperator",manager.getSimOperator());//SIM����Ӫ��
		Log.e("getSimOperatorName",manager.getSimOperatorName());//SIM����Ӫ������
		Log.e("getSimCountryIso",manager.getSimCountryIso());//SIM����Ӫ�����ڹ��Ҵ���
		Log.e("getSimSerialNumber",manager.getSimSerialNumber());//SIM�����к�
		Log.e("getLine1Number",manager.getLine1Number());//�ֻ���
		Log.e("Android version is ",android.os.Build.VERSION.RELEASE );
		Log.e("Android manufacturer is ",android.os.Build.MANUFACTURER );
		Log.e("Android model is ",android.os.Build.MODEL );
		//Log.e("Build.SERIAL", android.os.Build.getSerial());
		foundationInit((Context)this);
	}

   @Override  
    public void onBackPressed() {  
        super.onBackPressed();  
        System.out.println("������back��   onBackPressed()"); 
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
