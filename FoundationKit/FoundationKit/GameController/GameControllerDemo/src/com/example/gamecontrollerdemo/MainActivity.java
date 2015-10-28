package com.example.gamecontrollerdemo;

import com.gamecontroller.lib.GameControllerActivity;

import com.gamecontroller.lib.GameControllerAdapter;
import com.gamecontroller.lib.GameControllerManager;
import com.gamecontroller.lib.GameControllerDelegate.ControllerEventListener;

import android.annotation.TargetApi;
import android.app.Activity;
import android.os.Build;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.MotionEvent;

// 第一种方式


public class MainActivity extends GameControllerActivity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		// 如果需要自己处理事件，那么要设置一个控制器的事件监听器
		this.setControllerEventListener(mControllerEventListener);
		
		this.connectController(DRIVERTYPE_NIBIRU);
		this.connectController(DRIVERTYPE_MOGA);
		this.connectController(DRIVERTYPE_OUYA);
		
		setContentView(R.layout.activity_main);
		
	}
	
	// 如果需要自己处理事件，那么要设置一个控制器的事件监听器
	// 事件监听器的定义
    ControllerEventListener mControllerEventListener = new ControllerEventListener() {
		
		@Override
		public void onButtonEvent(String vendorName, int controller, int button,
				boolean isPressed, float value, boolean isAnalog) {
			GameControllerAdapter.onButtonEvent(vendorName, controller, button, isPressed, value, isAnalog);
		}
		
		@Override
		public void onAxisEvent(String vendorName, int controller, int axisID,
				float value, boolean isAnalog) {
			GameControllerAdapter.onAxisEvent(vendorName, controller, axisID, value, isAnalog);
		}

		@Override
		public void onConnected(String vendorName, int controller) {
			GameControllerAdapter.onConnected(vendorName, controller);
		}

		@Override
		public void onDisconnected(String vendorName, int controller) {
			GameControllerAdapter.onDisconnected(vendorName, controller);
		}
	};
}

/*
// 第二种方式
public class MainActivity extends Activity {

	public static GameControllerManager  gameControllerManager = null;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		gameControllerManager = new GameControllerManager(this);
		gameControllerManager.setControllerEventListener(mControllerEventListener);
		gameControllerManager.connectController(GameControllerManager.DRIVERTYPE_NIBIRU);
		gameControllerManager.connectController(GameControllerManager.DRIVERTYPE_MOGA);
		gameControllerManager.connectController(GameControllerManager.DRIVERTYPE_OUYA);
		setContentView(R.layout.activity_main);
	}
	
	@Override
	public boolean dispatchKeyEvent(KeyEvent event) {

		if(gameControllerManager != null){
			return gameControllerManager.dispatchKeyEvent(event) || super.dispatchKeyEvent(event);
		}else{
			return super.dispatchKeyEvent(event);
		}
	}

	@TargetApi(Build.VERSION_CODES.HONEYCOMB_MR1)
	@Override
	public boolean dispatchGenericMotionEvent(MotionEvent event) {
		if(gameControllerManager != null){
			return gameControllerManager.dispatchGenericMotionEvent(event)||super.dispatchGenericMotionEvent(event);
		}else{
			return super.dispatchGenericMotionEvent(event);
		}
	}
	
	@Override
	protected void onResume() {
		super.onResume();
		if(gameControllerManager != null){
			gameControllerManager.onResume();
		}
	}
	
	@Override
	protected void onPause() {
		if(gameControllerManager != null){
			gameControllerManager.onPause();
		}
		super.onPause();
	}
	
	@Override
	protected void onDestroy() {
		if(gameControllerManager != null){
			gameControllerManager.onDestroy();
		}
		super.onDestroy();
	}
	
	ControllerEventListener mControllerEventListener = new ControllerEventListener() {
		
		@Override
		public void onButtonEvent(String vendorName, int controller, int button,
				boolean isPressed, float value, boolean isAnalog) {
			GameControllerAdapter.onButtonEvent(vendorName, controller, button, isPressed, value, isAnalog);
		}
		
		@Override
		public void onAxisEvent(String vendorName, int controller, int axisID,
				float value, boolean isAnalog) {
			GameControllerAdapter.onAxisEvent(vendorName, controller, axisID, value, isAnalog);
		}

		@Override
		public void onConnected(String vendorName, int controller) {
			GameControllerAdapter.onConnected(vendorName, controller);
		}

		@Override
		public void onDisconnected(String vendorName, int controller) {
			GameControllerAdapter.onDisconnected(vendorName, controller);
		}
	};
	
}
*/
