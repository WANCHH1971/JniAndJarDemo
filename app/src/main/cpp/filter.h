#ifndef FILTER_H
#define FILTER_H

#define dnum 1
#define Dnum 2

class filter_native
{
		int pre_MedGrp;
		int MedResUp;
		int MedResDown;
		int MedFlag;
		int HistLen;
		int Half;
		int Calzhongzhi(int* histogram, int thresh, int len);
		int* Hist;

		int chengfang(int a, int b);
		bool Gauss(float A[][Dnum], float B[][Dnum], int n);
		float** matrix_zhuanzhi(float** arrA, int row, int col);
		float** matrix_multiply(float** arrA, float** arrB, int row1, int col1, int col2);
		int matrix_del(float** arrA, int row, int col);

public:
	filter_native(void);
	~filter_native(void);
	void Medfilt(float* src, float* dst, float wnd, int length, int fs);
	int Smooth_SG(float* ecg0, float* dst, int datalen);
	int Smooth_means(float* ecg0, float* dst, int wnd, int datalen);

};

#endif