#ifndef AFSEARCH_H
#define AFSEARCH_H


class AFanalysis
{
	float mean_rr;
	float SD;
	int median_rr;
	int per25;
	int per75;
public:
	double AFdiagnose(float* ecg, int len, int* Rpos, int Rnum);
	float rcvFunc(int* rr,int len);
	float skpFunc(int* rr, int len);
	int hrSynLempelZivFunc(int* rr, int len);
	int lz_complexity(int* s, int len);
	int RRfeature(int* rr, int len);
	void quick_sort(int s[], int l, int r);
	AFanalysis();
	~AFanalysis();
};

#endif