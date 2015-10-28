package com.gamecontroller.lib;

import java.util.ArrayList;

//import android.app.AlertDialog;

public class GameControllerAdapter {
	private static ArrayList<Runnable> sRunnableFrameStartList = null;
	
	public static void addRunnableToFrameStartList(Runnable runnable) {
		if (sRunnableFrameStartList == null)
			sRunnableFrameStartList = new ArrayList<Runnable>();
		sRunnableFrameStartList.add(runnable);
	}

	public static void removeRunnableFromFrameStartList(Runnable runnable) {
		if (sRunnableFrameStartList != null)
			sRunnableFrameStartList.remove(runnable);
	}

	public static void onDrawFrameStart() {
		if (sRunnableFrameStartList != null)
		{
			int size = sRunnableFrameStartList.size();
			for (int i = 0; i < size; ++i) {
				sRunnableFrameStartList.get(i).run();
			}
		}
	}

	public static void onConnected(final String vendorName, final int controller)
	{
		nativeControllerConnected(vendorName, controller);

		// GameControllerActivity
		// GameControllerManager
		
//		AlertDialog ad = new AlertDialog.Builder(GameControllerActivity.getActivity()).setTitle("标题" )
//				.setMessage("onConnected" )
//				.setPositiveButton("确定" ,  null )
//				.show();
		
	}
	
	public static void onDisconnected(final String vendorName, final int controller)
	{
		nativeControllerDisconnected(vendorName, controller);

//		AlertDialog ad = new AlertDialog.Builder(GameControllerActivity.getActivity()).setTitle("标题" )
//				.setMessage("onDisconnected" )
//				.setPositiveButton("确定" ,  null )
//				.show();
	}
	
	public static void onButtonEvent(final String vendorName, final int controller, final int button, final boolean isPressed, final float value, final boolean isAnalog)
	{
		nativeControllerButtonEvent(vendorName, controller, button, isPressed, value, isAnalog);

		
//		AlertDialog ad = new AlertDialog.Builder(GameControllerActivity.getActivity()).setTitle("标题" )
//				.setMessage("onButtonEvent vendorName = " + vendorName + 
//						" controller="+controller +
//						" button="+button +
//						" isPressed="+isPressed +
//						" value="+value+
//						" isAnalog="+isAnalog )
//				.setPositiveButton("确定" ,  null )
//				.show();
	}
	
	public static void onAxisEvent(final String vendorName, final int controller, final int axisID, final float value, final boolean isAnalog)
	{
		nativeControllerAxisEvent(vendorName, controller, axisID, value, isAnalog);

//		AlertDialog ad = new AlertDialog.Builder(GameControllerActivity.getActivity()).setTitle("标题" )
//				.setMessage("onAxisEvent vendorName = " + vendorName + 
//						" controller="+controller +
//						" axisID="+axisID +
//						" value="+value +
//						" isAnalog="+isAnalog )
//				.setPositiveButton("确定" ,  null )
//				.show();
	}
	
	private static native void nativeControllerConnected(final String vendorName, final int controller);
	private static native void nativeControllerDisconnected(final String vendorName, final int controller);
	private static native void nativeControllerButtonEvent(final String vendorName, final int controller, final int button, final boolean isPressed, final float value, final boolean isAnalog);
	private static native void nativeControllerAxisEvent(final String vendorName, final int controller, final int axisID, final float value, final boolean isAnalog);
}
