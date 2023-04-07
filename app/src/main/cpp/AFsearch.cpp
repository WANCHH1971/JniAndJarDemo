#include "AFsearch.h"
#include "string.h"
#include <math.h>
#include "model2c.h"

AFanalysis::AFanalysis()
{
	mean_rr = 0;
	median_rr = 0;
	SD = 0;
	per25 = 0;
	per75 = 0;
}

AFanalysis::~AFanalysis()
{

}

int AFanalysis::RRfeature(int* rr,int len)
{
	float sum = 0;
	for (int i = 0; i < len; i++)
	{
		sum += rr[i];
	}
	mean_rr = sum / len;
	sum = 0;
	for (int i = 0; i < len; i++)
	{
		sum += (rr[i] - mean_rr)*(rr[i] - mean_rr);
	}
	sum = sum / (len - 1);
	SD = sqrt(sum);

	int* rr_sort = new int[len];
	memcpy(rr_sort, rr, len*sizeof(int));
	quick_sort(rr_sort, 0, len - 1);
	median_rr = rr_sort[(int)(len / 2)];
	per25 = rr_sort[(int)(len / 4)];
	per75 = rr_sort[(int)(len * 0.75)];
	delete(rr_sort); rr_sort = NULL;
	return 0;
}

void AFanalysis::quick_sort(int s[], int l, int r)
{
	if (l < r)
	{
		int i = l, j = r;
		int x = s[l];
		while (i<j)
		{
			while (i<j && s[j] >= x)
				j--;
			if (i<j)
				s[i++] = s[j];
			while (i<j && s[i] <= x)
				i++;
			if (i<j)
				s[j--] = s[i];
		}
		s[i] = x;
		quick_sort(s, l, i - 1);
		quick_sort(s, i + 1, r);
	}
}

double AFanalysis::AFdiagnose(float* ecg, int len, int* Rpos, int Rnum)
{
	if (Rnum < 10)
	{
		return -1;
	}

	int* rr = new int[Rnum];
	int num = 0;
	for (int i = 0; i < Rnum; i++)
	{
		if (Rpos[i]>200 && Rpos[i] < len - 200)
		{
			rr[num++] = Rpos[i];
		}
	}

	for (int i = 1; i < num; i++)
	{
		rr[i - 1] = rr[i] - rr[i - 1];
	}
	num--;

	double input[3];
	RRfeature(rr, num);

	int rrsame = 0;
	int* rrtem = new int[num];
	for (int i = 0; i < num; i++)
	{
		if (fabs(rr[i] - median_rr) < median_rr*0.3)
		{
			rrtem[rrsame++] = rr[i];
		}
	}
	memcpy(rr, rrtem, rrsame*sizeof(int));
	num = rrsame;
	input[0] = (double)rcvFunc(rr, num);
	input[1] = (double)skpFunc(rr, num);
	input[2] = (double)hrSynLempelZivFunc(rr, num);
	model2c modeltest;
	double result = modeltest.score(input);
	return result;
}

float AFanalysis::rcvFunc(int* rr, int len)
{
	int IQR = per75 - per25;
	float RCV = 0;
	if (median_rr == 0)
	{
		RCV = (float)(IQR*0.7413) / mean_rr;
	}
	else
	{
		RCV = (float)(IQR*0.7413) / median_rr;
	}
	return RCV;
}

float AFanalysis::skpFunc(int* rr, int len)
{
	float sum = 0;
	float SKP = 0;
	if (SD == 0)
	{
		SKP = 0;
	}
	else
	{
		float sd3 = pow(SD, 3);
		sum = 0;
		for (int i = 0; i < len; i++)
		{
			sum += pow(rr[i] - mean_rr, 3) / sd3;
		}
		SKP = sum / (len - 1);
	}
	return SKP;
}

//int AFanalysis::hrSynLempelZivFunc(int* rr, int len)
//{
//	int* s = new int[len];
//	for (int i = 0; i < len; i++)
//	{
//		if (rr[i]>mean_rr)
//		{
//			s[i] = 1;
//		}
//		else
//		{
//			s[i] = 0;
//		}
//	}
//	int cn = lz_complexity(s, len);
//	delete(s); s = NULL;
//	return cn;
//}

int AFanalysis::hrSynLempelZivFunc(int* rr, int len)
{
	int* s = new int[len];
	for (int i = 0; i < len; i++)
	{
		if (abs(rr[i] - mean_rr)>0.15*(mean_rr))
		{
			s[i] = 1;
		}
		else
		{
			s[i] = 0;
		}
	}
	int cn = lz_complexity(s, len);
	delete(s); s = NULL;
	return cn;
}


int AFanalysis::lz_complexity(int* s, int len)
{
	int i = 0; int k = 1; int l = 1; int k_max = 1;
	int n = len - 1; int c = 1;
	while (1)
	{
		if (s[i + k - 1] == s[l + k - 1])
		{
			k++;
			if (l + k >= n - 1)
			{
				c++;
				break;
			}
		}
		else
		{
			if (k > k_max)
			{
				k_max = k;
			}
			i++;
			if (i == l)
			{
				c++;
				l += k_max;
				if (l + 1 > n)
				{
					break;
				}
				else
				{
					i = 0;
					k = 1;
					k_max = 1;
				}
			}
			else
			{
				k = 1;
			}
		}
	}
	return c;
}


