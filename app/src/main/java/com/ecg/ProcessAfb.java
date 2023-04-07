package com.ecg;


import android.util.Log;


/**
 *
 * @author ligang
 */
public class ProcessAfb {
	static {
		
		try {
			System.loadLibrary("jniandjardemo");
		} catch (UnsatisfiedLinkError ule) {
			Log.e("JNI", "WARNING: Could not load lib jniandjardemo.so");
		}
	}


	/**
	 * 樊工针对房颤修正的算法
	 * @param arrayList      以心电数组的形式，将心电数据转换为Float数组传入
	 * @param length          数组长度
	 * @return                     返回：房颤检测结果：（0：表示无房颤）、（1：表示有房颤）
	 */
	public native int getNewAfb(float[] arrayList,int length);

}


