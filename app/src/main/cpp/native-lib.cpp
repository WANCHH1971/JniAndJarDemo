#include <jni.h>
#include <string>
#include "AFanalysis.h"
extern "C" JNIEXPORT jint JNICALL Java_com_ecg_ProcessAfb_getNewAfb(JNIEnv *env, jobject thiz, jfloatArray ft_data, jint len) {

    jfloat *featureData1 =env->GetFloatArrayElements(ft_data, NULL);
    int afb=afibanalysis(featureData1,len);
    return afb;
}