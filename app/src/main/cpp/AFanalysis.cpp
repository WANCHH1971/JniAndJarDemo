#include "filter.h"
#include "QRSsearch.h"
#include "AFsearch.h"
#include "model2c.h"

int len = 7500;

int afibanalysis(float* src_float, int len)
{
	QRSsearch jianbo;
	int det[200];
	int Rnum = 0;
	float* dst_native = new float[len];
	float* dsttemp = new float[len];
	filter_native lvbo;
	lvbo.Smooth_SG(src_float, dsttemp, len);
	lvbo.Medfilt(dsttemp, dst_native, 0.6, len, 250);
	jianbo.qrs_det(dst_native, len, det, &Rnum, 250);

	AFanalysis afanalysis;
	double af_result = afanalysis.AFdiagnose(dst_native, len, det, Rnum);
	delete(dst_native); 
	delete(dsttemp); 
	if (af_result > 0.8)
		return 0;
	else
		return 1;
}

int afibanalysis_test(int af_result)
{

	if (af_result > 0.8)
		return 0;
	else
		return 1;
}