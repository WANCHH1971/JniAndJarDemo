package com.ecg;


import android.util.Log;


/**
 *
 * @author ligang
 */
public class process {
	public final static String name = "jt_ecgprogram";
	private static boolean hasLoadLib = false;
	static {

		try {
			System.loadLibrary(name);
			Log.i("JNI", "loadLibrary lib"+name+".so");

			hasLoadLib = true;
		} catch (UnsatisfiedLinkError ule) {
			hasLoadLib = false;
			Log.e("JNI", "WARNING: Could not load lib"+name+".so");
		}
	}


	/************************************************************************************************************/
	/***********************************************************************************************************/

	public void temp_filter(){
		int[]filter_buffer = new int[10000];
		int version = version();
		createFilter();
		for(int i= 0;i < 10000;i++){
			int ecgdata = filter(filter_buffer[i]);
		}
		deleteFilter();
	}

	/************************************************************************************************************/
	/***********************************************************************************************************/

	public void temp_ecg(){
		int[]ecg_buffer = new int[10000];
		int sve_th = 15;
		int ve_th = 15;
		createEcg(sve_th,ve_th);
		int []interval_result = new int[1];
		for(int i= 0;i < 10000;i++){
			int HR = ecgProcessing(ecg_buffer[i],interval_result);
			if(interval_result[0] > 0){
				int abn = getadn();
				int af = atrFib();
			}
		}
		deleteEcg();
	}

	/************************************************************************************************************/
	/***********************************************************************************************************/

	public void temp_HRV(){
		int[]RR = new int[500];
		double[]hrvfreq = new double[5];
		double[]hrvtime = new double[5];
		createHRV();
		hrv(RR,500,hrvfreq,hrvtime);
		deleteHRV();
	}



	public native int version();

	public native void createFilter();

	public native int filter(int ecgdata);

	public native void deleteFilter();

	public native void createEcg(int sve_th,int ve_th);

	public native int ecgProcessing(int ecgdata,int[]interval_result);

	public native int getadn();

	public native int atrFib();

	public native void deleteEcg();

	public native void createHRV();

	public native void hrv(int[]RR,int rr_length,double[]hrvfreq,double[]hrvtime);

	public native void deleteHRV();
}


