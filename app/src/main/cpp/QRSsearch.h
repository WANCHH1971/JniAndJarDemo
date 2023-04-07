#ifndef QRSSEARCH_H
#define QRSSEARCH_H

class QRSsearch
{
public:
	QRSsearch();
	~QRSsearch();
	int qrs_det(float* ecg, int length, int* det, int* Rnum, int fs);
	float maxloc_det(float* ecg, int length, int* loc);
	double maxloc_det(double* ecg, int length, int* loc);
	float minloc_det(float* ecg, int length, int* loc);
	void quick_sort_float(float s[], int l, int r);
	void quick_sort(int s[], int l, int r);
	float max_det(float* ecg, int length);
	float abs_float(float a);
	int diff_det(float* src, float* dst, int length, int len);
};

#endif