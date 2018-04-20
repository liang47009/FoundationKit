package com.losemymind.foundationkit;

import java.util.HashMap;

public class AndroidJavaBridge {

	private HashMap<Class<?>, Character> _foundationTypeMap = null;
	
	private static class SingletonHolder {  
        private static AndroidJavaBridge instance = new AndroidJavaBridge();  
    }
	
    public static AndroidJavaBridge getInstance() {  
        return SingletonHolder.instance;  
    }
	
	private AndroidJavaBridge() {  
		_foundationTypeMap = new HashMap<Class<?>, Character>();
		_foundationTypeMap.put(java.lang.Boolean.class, 'z');
		_foundationTypeMap.put(java.lang.Byte.class, 'b');
		_foundationTypeMap.put(java.lang.Character.class, 'c');
		_foundationTypeMap.put(java.lang.Short.class, 's');
		_foundationTypeMap.put(java.lang.Integer.class, 'i');
		_foundationTypeMap.put(java.lang.Long.class, 'j');
		_foundationTypeMap.put(java.lang.Float.class, 'f');
		_foundationTypeMap.put(java.lang.Double.class, 'd');
		_foundationTypeMap.put(java.lang.Boolean[].class, 'Z');
		_foundationTypeMap.put(java.lang.Byte[].class, 'B');
		_foundationTypeMap.put(java.lang.Character[].class, 'C');
		_foundationTypeMap.put(java.lang.Short[].class, 'S');
		_foundationTypeMap.put(java.lang.Integer[].class, 'I');
		_foundationTypeMap.put(java.lang.Long[].class, 'J');
		_foundationTypeMap.put(java.lang.Float[].class, 'F');
		_foundationTypeMap.put(java.lang.Double[].class, 'D');
		_foundationTypeMap.put(boolean[].class, 'Z');
		_foundationTypeMap.put(byte[].class, 'B');
		_foundationTypeMap.put(char[].class, 'C');
		_foundationTypeMap.put(short[].class, 'S');
		_foundationTypeMap.put(int[].class, 'I');
		_foundationTypeMap.put(long[].class, 'J');
		_foundationTypeMap.put(float[].class, 'F');
		_foundationTypeMap.put(double[].class, 'D');
		_foundationTypeMap.put(String.class, 'T');
		//_foundationTypeMap.put(null, 'N');
		
		//瀵硅薄榛樿灏辨槸 Object 鍜孫bject鏁扮粍锛屽湪getArgumentSignature涓�
		//鍋氬鐞嗭紝杩欓噷涓嶅繀淇濆瓨銆�
		//_foundationTypeMap.put(Object.class, 'l');
		//_foundationTypeMap.put(Object[].class, 'L');
    }
	
	private String getArgumentSignature(Object... args)
	{
		String strSignature = "";
		for(Object obj : args)
		{
			char szSig = 'l';
			if(obj==null)
				szSig = 'N';
			else if(_foundationTypeMap.containsKey(obj.getClass()))
				szSig= _foundationTypeMap.get(obj.getClass());
			else if(obj.getClass().isArray())
				szSig = 'L';
			strSignature += szSig;
		}
		return strSignature;
	}
  
	public void invoke(String name, Object... args){
		String strSignature = getArgumentSignature(args);
		nativeInvoke(name, strSignature, args);
	}
	
	public native void nativeInvoke(final String funName, final String sig, final Object... args);
}
