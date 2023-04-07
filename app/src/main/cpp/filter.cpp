#include "string.h"
#include "filter.h"
#include "math.h"


filter_native::filter_native(void)
{
	pre_MedGrp = 0;
	MedResUp = 0;
	MedResDown = 0;
	MedFlag = 1100;
	HistLen = 1025;
	Half = HistLen / 2;
}
filter_native::~filter_native(void)
{

}

void filter_native::Medfilt(float* src, float* dst, float wnd, int length, int fs)
{
	int leftValue = 0;
	int rightValue = 0;
	int diameter = fs*wnd + 1;
	int radius = (diameter - 1) / 2;

	float* srctemp = new float[length];
	for (int i = 0; i < length; i++)
	{
		srctemp[i] = src[i]/10 ;
	}
	Hist = new int[HistLen];
	memset(Hist, 0, sizeof(int)* HistLen);
	int histogramSize = HistLen;
	int thresholdValue = diameter / 2 + 1;
	int rightb = length - radius;   //

	for (int i = radius; i < rightb && i < length && i >= 0; i++)
	{
		if (i == radius)
		{
			memset(Hist, 0, histogramSize*sizeof(int));
			for (int x = i - radius; x <= i + radius && x < length; x++)
			{
				int value = (int)srctemp[x] + Half;
				if (value >= 0 && value < HistLen)
					Hist[value]++;
			}
		}
		else
		{
			int left = i - radius - 1;
			int right = i + radius;
			{
				int leftIdx = left;
				if (leftIdx < 0 || leftIdx >= length)
					continue;
				leftValue = (int)srctemp[leftIdx] + Half;
				if (leftValue >= 0 && leftValue < HistLen)
					Hist[leftValue]--;
				int rightIdx = right;
				if (rightIdx < 0 || rightIdx >= length)
					continue;
				rightValue = (int)srctemp[rightIdx] + Half;
				if (rightValue >= 0 && rightValue < HistLen)
					Hist[rightValue]++;


				if (leftValue < pre_MedGrp)
				{
					if (rightValue < pre_MedGrp)
					{
						MedFlag = 0;
					}
					else if (rightValue == pre_MedGrp)
					{
						MedFlag = 0;
						MedResDown++;
					}
					else
					{
						if (MedResUp == 0)
						{
							MedResDown = 0;
							for (int j = pre_MedGrp + 1; j < HistLen-1; j++)
							{
								if (Hist[j] != 0)
								{
									MedFlag = j - pre_MedGrp;
									MedResUp = Hist[j] - 1;
									break;
								}
							}
						}
						else
						{
							MedFlag = 0;
							MedResUp--;
							MedResDown++;
						}
					}
				}
				else if (leftValue == pre_MedGrp)
				{
					if (rightValue < pre_MedGrp)
					{
						if (MedResDown == 0)
						{
							MedResUp = 0;
							for (int j = pre_MedGrp - 1; j >0; j--)
							{
								if (Hist[j] != 0)
								{
									MedFlag = j - pre_MedGrp;
									MedResDown = Hist[j] - 1;
									break;
								}
							}
						}
						else
						{
							MedFlag = 0;
							MedResDown--;
						}
					}
					else if (rightValue == pre_MedGrp)
					{
						MedFlag = 0;
					}
					else
					{
						if (MedResUp == 0)
						{
							MedResDown = 0;
							for (int j = pre_MedGrp + 1; j < HistLen-1; j++)
							{
								if (Hist[j] != 0)
								{
									MedFlag = j - pre_MedGrp;
									MedResUp = Hist[j] - 1;
									break;
								}
							}
						}
						else
						{
							MedFlag = 0;
							MedResUp--;
						}
					}
				}
				else
				{
					if (rightValue < pre_MedGrp)
					{
						if (MedResDown == 0)
						{
							MedResUp = 0;
							for (int j = pre_MedGrp - 1; j >0; j--)
							{
								if (Hist[j] != 0)
								{
									MedFlag = j - pre_MedGrp;
									MedResDown = Hist[j] - 1;
									break;
								}
							}
						}
						else
						{
							MedFlag = 0;
							MedResUp++;
							MedResDown--;
						}
					}
					else if (rightValue == pre_MedGrp)
					{
						MedFlag = 0;
						MedResUp++;
					}
					else
					{
						MedFlag = 0;
					}
				}

			}

		}
		int filterValue = Calzhongzhi(Hist, thresholdValue, HistLen);
		dst[i] = filterValue - Half;
	}

	for (int i = 0; i < radius; i++)
	{
		dst[i] = (int)srctemp[i];
	}
	for (int i = length - radius; i < length; i++)
	{
		dst[i] = (int)srctemp[i];
	}
	for (int i = 0; i < length; i++)
	{
		dst[i] = srctemp[i] - dst[i];
	}

	for (int i = 0; i < length; i++)
	{
		dst[i] = 10 * dst[i];
	}
	delete(srctemp); srctemp = NULL;
	delete(Hist); Hist = NULL;
}

int filter_native::Calzhongzhi(int* histogram, int thresh, int len)
{
	if (MedFlag == 1100)
	{
		int sum = 0;
		for (int i = 0; i < len; ++i)
		{
			sum += histogram[i];
			if (sum >= thresh)
			{
				if (sum == thresh)
				{
					MedResUp = 0;
					MedResDown = histogram[i] - 1;
				}
				else
				{
					MedResUp = sum - thresh;
					MedResDown = histogram[i] - 1 - MedResUp;
				}
				pre_MedGrp = i;
				return i;
			}
		}
		return len - 1;
	}
	else
	{
		if (MedFlag == 0)
		{
			return pre_MedGrp;
		}
		else if (MedFlag > 0)
		{
			pre_MedGrp = pre_MedGrp + MedFlag;
			return pre_MedGrp;
		}
		else
		{
			pre_MedGrp = pre_MedGrp + MedFlag;
			return pre_MedGrp;
		}
	}
}


int filter_native::Smooth_SG(float* ecg0, float* dst, int datalen)
{
	if (ecg0 == NULL)
		return 0;
	int N = 9;
	int d = dnum;
	int M = (N - 1) / 2;
	float** S = new float*[N];
	for (int i = 0; i < N; ++i)
	{
		S[i] = new float[d + 1];
	}
	for (int m = -M; m <= M; m++)
	{
		for (int i = 0; i <= d; i++)
		{
			S[m + M][i] = chengfang(m, i);
		}
	}

	float** SZ = matrix_zhuanzhi(S, N, d + 1);
	float** F = matrix_multiply(SZ, S, d + 1, N, d + 1);
	float A[Dnum][Dnum];
	for (int i = 0; i < Dnum; i++)
	{
		for (int j = 0; j < Dnum; j++)
		{
			A[i][j] = F[i][j];
		}
	}
	float FN[Dnum][Dnum];
	for (int i = 0; i < Dnum; i++)
	{
		for (int j = 0; j < Dnum; j++)
		{
			FN[i][j] = 0;
		}
	}

	Gauss(A, FN, Dnum);


	float** FR = new float*[Dnum];
	for (int i = 0; i < Dnum; i++)
	{
		FR[i] = new float[Dnum];
	}
	for (int i = 0; i < Dnum; i++)
	{
		for (int j = 0; j < Dnum; j++)
		{
			FR[i][j] = FN[i][j];
		}
	}
	float** C = matrix_multiply(S, FR, N, d + 1, Dnum);
	float** B = matrix_multiply(C, SZ, N, d + 1, N);

	float** temp1 = new float*[1];
	for (int i = 0; i < 1; i++)
	{
		temp1[i] = new float[N];
	}
	float** temp2 = new float*[N];
	for (int i = 0; i < N; i++)
	{
		temp2[i] = new float[1];
	}

	for (int i = 1; i <= M + 1; ++i)
	{
		for (int j = 0; j < N; j++)
		{
			temp1[0][j] = B[j][i - 1];
		}
		for (int j = 0; j < N; j++)
		{
			temp2[j][0] = ecg0[j];
		}
		float** result = matrix_multiply(temp1, temp2, 1, N, 1);
		dst[i - 1] = result[0][0];
		matrix_del(result, 1, 1);
	}


	for (int i = M + 2; i <= datalen - M - 1; ++i)
	{
		for (int j = 0; j < N; j++)
		{
			temp1[0][j] = B[j][M];
		}
		for (int j = 0; j < N; j++)
		{
			temp2[j][0] = ecg0[i - M - 1 + j];
		}
		float** result = matrix_multiply(temp1, temp2, 1, N, 1);
		dst[i - 1] = result[0][0];
		matrix_del(result, 1, 1);
	}

	//�ұ߽�
	for (int i = 0; i <= M; ++i)
	{
		for (int j = 0; j < N; j++)
		{
			temp1[0][j] = B[j][M + i];
		}
		for (int j = 0; j < N; j++)
		{
			temp2[j][0] = ecg0[datalen - N + j];
		}
		float** result = matrix_multiply(temp1, temp2, 1, N, 1);
		dst[datalen - M + i - 1] = result[0][0];
		matrix_del(result, 1, 1);
	}

	matrix_del(S, N, Dnum);
	matrix_del(SZ, Dnum, N);
	matrix_del(F, Dnum, Dnum);
	matrix_del(FR, Dnum, Dnum);
	matrix_del(C, N, Dnum);
	matrix_del(B, N, N);
	matrix_del(temp1, 1, N);
	matrix_del(temp2, N, 1);

	return 1;

}
float** filter_native::matrix_zhuanzhi(float** arrA, int row, int col)
{
	float** res = new float*[col];
	for (int i = 0; i < col; i++)
	{
		res[i] = new float[row];
		for (int j = 0; j < row; j++)
		{
			res[i][j] = arrA[j][i];
		}
	}
	return     res;
}
float** filter_native::matrix_multiply(float** arrA, float** arrB, int row1, int col1, int col2)
{
	float** res = new float*[row1];
	for (int i = 0; i < row1; i++)
	{
		res[i] = new float[col2];
	}
	for (int i = 0; i < row1; i++)
	{
		for (int j = 0; j < col2; j++)
			res[i][j] = 0;
	}
	for (int i = 0; i < row1; ++i)
	{
		for (int j = 0; j < col2; ++j)
		{
			for (int k = 0; k < col1; ++k)
			{
				res[i][j] += arrA[i][k] * arrB[k][j];
			}
		}
	}
	return res;
}
int filter_native::matrix_del(float** arrA, int row, int col)
{
	for (int i = 0; i < row; i++)
	{
		if (arrA[i] != NULL)
		{
			delete arrA[i];
			arrA[i] = NULL;
		}
	}
	if (arrA != NULL)
	{
		delete arrA;
		arrA = NULL;
	}
	return 1;
}
int filter_native::chengfang(int a, int b)
{
	int c = 1;
	if (b == 0)
		return 1;
	else
	{
		for (int i = 0; i < b; i++)
			c = c * a;
		return c;
	}
}
bool filter_native::Gauss(float A[][Dnum], float B[][Dnum], int n)
{
	int i, j, k;
	float max, temp;
	float t[Dnum][Dnum];
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			t[i][j] = A[i][j];
		}
	}
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			B[i][j] = (i == j) ? (float)1 : 0;
		}
	}
	for (i = 0; i < n; i++)
	{
		max = t[i][i];
		k = i;
		for (j = i + 1; j < n; j++)
		{
			if (fabs(t[j][i]) > fabs(max))
			{
				max = t[j][i];
				k = j;
			}
		}
		if (k != i)
		{
			for (j = 0; j < n; j++)
			{
				temp = t[i][j];
				t[i][j] = t[k][j];
				t[k][j] = temp;
				//B���潻��
				temp = B[i][j];
				B[i][j] = B[k][j];
				B[k][j] = temp;
			}
		}
		if (t[i][i] == 0)
		{
			return false;
		}
		temp = t[i][i];
		for (j = 0; j < n; j++)
		{
			t[i][j] = t[i][j] / temp;
			B[i][j] = B[i][j] / temp;
		}
		for (j = 0; j < n; j++)
		{
			if (j != i)
			{
				temp = t[j][i];
				for (k = 0; k < n; k++)
				{
					t[j][k] = t[j][k] - t[i][k] * temp;
					B[j][k] = B[j][k] - B[i][k] * temp;
				}
			}
		}
	}
	return true;
}

int filter_native::Smooth_means(float* ecg0, float* dst, int wnd, int datalen)
{
	for (int i = 0; i < datalen; i++)
	{
		if (i>4 & i < datalen - 1 - 4)
		{

		}
	}
	return 0;
}