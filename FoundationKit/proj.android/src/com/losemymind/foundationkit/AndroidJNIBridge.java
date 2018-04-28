package com.losemymind.foundationkit;
import java.lang.Class;
import java.lang.reflect.*;
import android.util.Log;

public final class AndroidJNIBridge {
	static native Object InvokeProxy(long ptr, Class<?> clazz, Method method, Object[] args);
	static native void   DeleteProxy(long ptr);

	static Object NewProxyInterface(final long ptr, final Class<?>[] interfaces){
		return Proxy.newProxyInstance(AndroidJNIBridge.class.getClassLoader(), interfaces, new InterfaceProxy(ptr));
	}

	static void DisableProxyInterface(final Object proxy){
		if (proxy != null)
			((InterfaceProxy) Proxy.getInvocationHandler(proxy)).disable();
	}

	private static class InterfaceProxy implements InvocationHandler{
		private Object InvocationLock = new Object[0];
		private long   NativePointer = 0;
		//private Constructor<MethodHandles.Lookup> MethodConstructor;
		
		public InterfaceProxy(final long ptr){
			NativePointer = ptr;

//			try{
//				MethodConstructor = MethodHandles.Lookup.class.getDeclaredConstructor(Class.class, Integer.TYPE);
//				MethodConstructor.setAccessible(true);
//			}
//			// MethodHandles.Lookup was added in Android Oreo, we get NoClassDefFoundError on devices with older OS versions
//			catch (NoClassDefFoundError e){
//				MethodConstructor = null;
//			}
//			catch (NoSuchMethodException e){
//				MethodConstructor = null;
//			}
		}

		private Object invokeDefault(Object proxy, Throwable t, Method m, Object[] args) throws Throwable{
			if (args == null){
				args = new Object[0];
			}
//			Class<?> k = m.getDeclaringClass();
//			MethodHandles.Lookup lookup = MethodConstructor.newInstance(k, MethodHandles.Lookup.PRIVATE);
//			return lookup.in(k).unreflectSpecial(m, k).bindTo(proxy).invokeWithArguments(args);
			return null;
		}

		public Object invoke(Object proxy, Method method, Object[] args) throws Throwable{
			synchronized (InvocationLock){
				if (NativePointer == 0)
					return null;

				try{
					return AndroidJNIBridge.InvokeProxy(NativePointer, method.getDeclaringClass(), method, args);
				}
				catch (NoSuchMethodError e){
//					if (MethodConstructor == null){
//						System.err.println("JNIBridge error: Java interface default methods are only supported since Android Oreo");
//						throw e;
//					}
					// isDefault() is only available since API 24, instead use getModifiers to check if a method has default implementation
					if ((method.getModifiers() & Modifier.ABSTRACT) == 0)
						return invokeDefault(proxy, e, method, args);
					else
						throw e;
				}
			}
		}

		public void finalize(){
			synchronized (InvocationLock){
				if (NativePointer == 0)
					return;
				AndroidJNIBridge.DeleteProxy(NativePointer);
			}
		}

		public void disable(){
			synchronized (InvocationLock){
				NativePointer = 0;
			}
		}
	}
	
	private static String getSignature(Class<?> clazz) throws Exception{
		if (clazz == null)
			return "V";
		if (clazz.isArray())
			return "[" + getSignature(clazz.getComponentType());
		if (clazz.isPrimitive()){
			if (clazz.equals(byte.class))	return "B";
			if (clazz.equals(short.class))	return "S";
			if (clazz.equals(int.class))	return "I";
			if (clazz.equals(long.class))	return "J";
			if (clazz.equals(float.class))	return "F";
			if (clazz.equals(double.class))	return "D";
			if (clazz.equals(char.class))	return "C";
			if (clazz.equals(boolean.class))return "Z";
			if (clazz.equals(void.class))	return "V";
			throw new Exception("Unknown primitive: " + clazz);
		}
		return "L" + clazz.getName().replace('.', '/') + ";";
	}
	
	public static <T> String getSignature(T arg) throws Exception{
		return getSignature(arg.getClass());
	}

	public static String getSignature(Member member) throws Exception{
		if(member instanceof Field)
			return getSignature(((Field)member).getType());
		if(member instanceof Method)
			return String.format("(%s)%s", (getSignature(((Method)member).getParameterTypes())), getSignature(((Method)member).getReturnType()));
		return String.format("(%s)V", getSignature(((Constructor<?>)member).getParameterTypes()));
	}

	public static String getSignature(Class<?>... clazzes) throws Exception{
		StringBuilder signature = new StringBuilder();
		for (Class<?> clazz : clazzes)
			signature.append(getSignature(clazz));
		return signature.toString();
	}
	
	public static String getSignature(Object... Objectes) throws Exception{
		StringBuilder signature = new StringBuilder();
		for (Object obj : Objectes)
			signature.append(getSignature(obj.getClass()));
		return signature.toString();
	}

	
	private static String getSimpleSignature(Class<?> clazz) throws Exception{
		if (clazz == null)
			return "V";
		if (clazz.isPrimitive()){
			if (clazz.equals(boolean.class))return "Z";
			if (clazz.equals(byte.class))	return "B";
			if (clazz.equals(char.class))	return "C";
			if (clazz.equals(short.class))	return "S";
			if (clazz.equals(int.class))	return "I";
			if (clazz.equals(long.class))	return "J";
			if (clazz.equals(float.class))	return "F";
			if (clazz.equals(double.class))	return "D";
			if (clazz.equals(void.class))	return "V";
			throw new Exception("Unknown primitive: " + clazz);
		}
		if(clazz.equals(java.lang.Boolean.class))   return "Z";
		if(clazz.equals(java.lang.Byte.class))      return "B";
		if(clazz.equals(java.lang.Character.class)) return "C";
		if(clazz.equals(java.lang.Short.class))     return "S";
		if(clazz.equals(java.lang.Integer.class))   return "I";
		if(clazz.equals(java.lang.Long.class))      return "J";
		if(clazz.equals(java.lang.Float.class))     return "F";
		if(clazz.equals(java.lang.Double.class))    return "D";
		if(clazz.equals(java.lang.String.class))    return "T";
		return "O";
	}
	
	public static String getSimpleSignature(Object... Objectes) throws Exception{
		StringBuilder signature = new StringBuilder();
		for (Object obj : Objectes)
			signature.append(getSimpleSignature(obj.getClass()));
		return signature.toString();
	}
	
	public static void sendMessage(String name, Object... args){
		String strSignature = "";
		try {
			strSignature = getSimpleSignature(args);
		} catch (Exception e) {
			e.printStackTrace();
		}
		NativeSendMeeage(name, strSignature, args);
	}
	static native void NativeSendMeeage(final String funName, final String sig, final Object[] args);
	
}
