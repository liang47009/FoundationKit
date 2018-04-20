package com.losemymind.foundationkit;

public interface AndroidJNIProxy {
	public abstract void OnMethodInvoke(String MethodName, int a, double b);
	public abstract void OnMethodInvoke1(String MethodName, char a, boolean b);
}
