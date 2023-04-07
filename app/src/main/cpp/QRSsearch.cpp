#include "QRSsearch.h"
#include "math.h"
#include "string.h"
#include <stdlib.h>
#include "string.h"
#include <iostream>
#include <vector>
#include "filter.h"

QRSsearch::QRSsearch()
{

}

QRSsearch::~QRSsearch()
{

}

int QRSsearch::qrs_det(float* ecg, int length, int* det, int* Rnum, int fs)
{
	//for (int i = 0; i < length; i++)
	//{
	//	ecg[i] = ecg[i] / 400;
	//}
	int slice = 10;
	int stage = length / slice;
	float slen = 0.2;
	int buyingqi = (int)(slen*fs);
	float* diff = (float*)malloc((length - 1 + buyingqi)*sizeof(float));
	memset(diff, 0, (length - 1 + buyingqi)*sizeof(float));
	//diff_det(ecg, diff, length, 8);
	for (int i = 1; i < length; i++)
	{
		diff[i - 1] = ecg[i] - ecg[i - 1];
	}
	/*for (int i = 1; i < length; i++)
	{
	diff[i - 1] = ecg[i] - ecg[i - 1];
	if (diff[i - 1] < 0)
	diff[i - 1] = -diff[i - 1];
	}*/
	for (int i = 0; i < length - 1; i++)
	{
		if (diff[i] == 0)
		{
			diff[i] = 0.0001;
		}
		if (diff[i] < 0)
		{
			diff[i] = -diff[i];
		}

	}
	//for (int i = 0; i < length - 1; i++)
	//{
	//	diff[i] = diff[i] * log(diff[i]);
	//}

	float th = 0.5;
	float* m_index = (float*)malloc(slice*sizeof(float));

	float max_f = 0;
	int max_num = 0;
	for (int i = 0; i < slice; i++)
	{
		max_f = max_det(diff + stage*i, stage - 1);
		if (max_f > 0.1)
		{
			m_index[max_num] = max_f;
			max_num++;
		}
	}
	quick_sort_float(m_index, 0, max_num - 1);
	float m = th*m_index[max_num / 2];
	float m_ini = m;

	float m_ini1 = m_index[max_num / 2];

	//for (int i = 0; i < length; i++)
	//{
	//	diff[i] = diff[i] / (2 * m_index[max_num / 2]);
	//	diff[i] = abs(diff[i]) - diff[i] * diff[i];
	//	diff[i] = diff[i] * (2 * m_index[max_num / 2]);
	//}
	//WriteFile(".\\diff.txt", diff, length);

	float c = m;
	int index = 1;
	int b = 0;

	int* temp_index = (int*)malloc(length*sizeof(int));
	int maxcnt = 1;

	while (index < length - slen*fs + buyingqi)
	{
		if (diff[index - 1]>m)
		{
			temp_index[maxcnt - 1] = index;
			int loc = 0;
			//float M = maxloc_det(diff + index - 1, slen*fs + 1, &loc);
			if (1)
				/*if ((diff[index - 1]<10 * m_ini) && ((loc >= 1 && loc <= 14) || (loc == 0 && M> th*m_ini)))*/
			{
				m = m_ini;
				//if (maxcnt>1 && m>(1.2*diff[temp_index[maxcnt - 2] - 1]))
				//	m = 1.0*diff[temp_index[maxcnt - 2] - 1];
				c = m;
				b = 0;
				index = index + slen*fs;
				maxcnt++;
			}
			else
			{
				index++;
				b++;
				m = (1 - 0.3*b / (1.2*fs))*c;
				if (m < 0.25*c)
					m = 0.25*c;
			}

		}
		else
		{
			index++;
			b++;
			m = (1 - 0.3*b / (1.2*fs))*c;
			if (m < 0.25*c)
				m = 0.25*c;
		}
	}
	maxcnt--;
	for (int i = 1; i <= maxcnt; i++)
	{
		int qj = 10;
		int qin = 0;
		int qin1 = 0;
		int qin2 = 0;
		float v1 = 0;
		float v2 = 0;
		int ab1 = temp_index[i - 1] - qj;
		int ab2 = temp_index[i - 1] + qj;
		if ((ab1 >= 1) && (ab2 <= length))
		{
			v1 = maxloc_det(ecg + ab1, ab2 - ab1 + 1, &qin1);
			v2 = minloc_det(ecg + ab1, ab2 - ab1 + 1, &qin2);
			if (abs_float(v2) > 3 * abs_float(v1))
				qin = qin2;
			else
				qin = qin1;
		}
		else if (ab1 < 1)
		{
			ab1 = 1;
			v1 = maxloc_det(ecg + ab1, ab2 - ab1 + 1, &qin1);
			v2 = minloc_det(ecg + ab1, ab2 - ab1 + 1, &qin2);
			if (abs_float(v2) > 3 * abs_float(v1))
				qin = qin2;
			else
				qin = qin1;
		}
		else
		{
			ab2 = length;
			v1 = maxloc_det(ecg + ab1, ab2 - ab1 + 1, &qin1);
			v2 = minloc_det(ecg + ab1, ab2 - ab1 + 1, &qin2);
			if (abs_float(v2) > 3 * abs_float(v1))
				qin = qin2;
			else
				qin = qin1;
		}
		temp_index[i - 1] = ab1 + qin;
	}
	for (int i = 0; i < maxcnt; i++)
	{
		temp_index[i] = temp_index[i] + 1;
	}

	//int* Rpos = new int[maxcnt]; int rrnum0 = 0;
	//memset(Rpos, 0, maxcnt*sizeof(int));
	//for (int i = 0; i < maxcnt; i++)
	//{
	//	if (  abs(  abs(ecg[temp_index[i]] - ecg[temp_index[i] - 3] ) - abs( ecg[temp_index[i]] - ecg[temp_index[i] + 3] )  ) < 500  )
	//	{
	//		Rpos[rrnum0++] = temp_index[i];
	//	}
	//}
	/*int Rnum1 = rrnum0;*/
	/*memcpy(temp_index, Rpos, rrnum0*sizeof(int));*/

	int Rnum1 = maxcnt;
	memcpy(temp_index, temp_index, maxcnt*sizeof(int));


	if (Rnum1 == 0)
	{
		if (diff != NULL)
		{
			free(diff);
			diff = NULL;
		}
		if (temp_index != NULL)
		{
			free(temp_index);
			temp_index = NULL;
		}
		if (m_index != NULL)
		{
			free(m_index);
			m_index = NULL;
		}
		*Rnum = 0;
		return 1;
	}
	else
	{
		memcpy(det, temp_index, Rnum1*sizeof(float));
		*Rnum = Rnum1;
		if (diff != NULL)
		{
			free(diff);
			diff = NULL;
		}
		if (temp_index != NULL)
		{
			free(temp_index);
			temp_index = NULL;
		}
		if (m_index != NULL)
		{
			free(m_index);
			m_index = NULL;
		}
		return 1;
	}

	int* det1 = (int*)malloc(Rnum1*sizeof(int));
	memcpy(det1, temp_index, Rnum1*sizeof(float));
	for (int i = 0; i < Rnum1; i++)
	{
		if (det1[i] < 40)
		{
			for (int j = 0; j < det1[i] + 40; j++)
				diff[j] = 0;
		}
		else if (det1[i] > length - 40)
		{
			for (int j = det1[i] - 40; j < length; j++)
				diff[j] = 0;
		}
		else
		{
			for (int j = det1[i] - 40; j < det1[i] + 40; j++)
				diff[j] = 0;
		}
	}

	for (int i = 0; i < slice; i++)
	{
		m_index[i] = max_det(diff + stage*i, 2.5*fs - 1);
	}
	int n = 0;
	float nsum = 0;
	for (int i = 0; i < slice; i++)
	{
		if (m_index[i]>m_ini1*0.3)
		{
			m_index[n] = m_index[i];
			n++;
		}
	}

	n = 0;				///******************************************************************************************************************************************************///
	if (n == 0)
	{
		memcpy(det, det1, Rnum1*sizeof(int));
		*Rnum = Rnum1;
		if (det1 != NULL)
		{
			free(det1);
			det1 = NULL;
		}
		if (diff != NULL)
		{
			free(diff);
			diff = NULL;
		}
		if (temp_index != NULL)
		{
			free(temp_index);
			temp_index = NULL;
		}
		if (m_index != NULL)
		{
			free(m_index);
			m_index = NULL;
		}
		return 1;
	}

	quick_sort_float(m_index, 0, n - 1);
	m = th*m_index[n / 2];
	m_ini = m;

	c = m;
	index = 1;
	b = 0;
	maxcnt = 1;
	slen = 0.2;

	while (index < length - slen*fs + buyingqi)
	{
		if (diff[index - 1]>m)
		{
			temp_index[maxcnt - 1] = index;
			int loc = 0;
			float M = maxloc_det(diff + index - 1, slen*fs + 1, &loc);
			/*    if ((diff[index - 1]<10 * m_ini) && ((loc >= 1 && loc <= 14) || (loc == 0 && M> 0.6*m_ini)))*/
			if (M>m_ini && (diff[index - 1]<10 * m_ini) && ((loc >= 1 && loc <= 14) || (loc == 0 && M> th*m_ini)))
			{
				m = th*M;
				if (maxcnt>1 && m>(1.2*diff[temp_index[maxcnt - 2] - 1]))
					m = 1.0*diff[temp_index[maxcnt - 2] - 1];
				c = m;
				b = 0;
				index = index + slen*fs;
				maxcnt++;
			}
			else
			{
				index++;
				b++;
				m = (1 - 0.3*b / (1.2*fs))*c;
				if (m < 0.25*c)
					m = 0.25*c;
			}
		}
		else
		{
			index++;
			b++;
			m = (1 - 0.3*b / (1.2*fs))*c;
			if (m < 0.25*c)
				m = 0.25*c;
		}
	}
	maxcnt--;

	for (int i = 1; i <= maxcnt; i++)
	{
		int qj = 10;
		int qin = 0;
		int qin1 = 0;
		int qin2 = 0;
		float v1 = 0;
		float v2 = 0;
		int ab1 = temp_index[i - 1] - qj;
		int ab2 = temp_index[i - 1] + qj;
		if ((ab1 >= 1) && (ab2 <= length))
		{
			v1 = maxloc_det(ecg + ab1, ab2 - ab1 + 1, &qin1);
			v2 = minloc_det(ecg + ab1, ab2 - ab1 + 1, &qin2);
			if (abs_float(v2) > 2 * abs_float(v1))
				qin = qin2;
			else
				qin = qin1;
		}
		else if (ab1 < 1)
		{
			ab1 = 1;
			v1 = maxloc_det(ecg + ab1, ab2 - ab1 + 1, &qin1);
			v2 = minloc_det(ecg + ab1, ab2 - ab1 + 1, &qin2);
			if (abs_float(v2) > 2 * abs_float(v1))
				qin = qin2;
			else
				qin = qin1;
		}
		else
		{
			ab2 = length;
			v1 = maxloc_det(ecg + ab1, ab2 - ab1 + 1, &qin1);
			v2 = minloc_det(ecg + ab1, ab2 - ab1 + 1, &qin2);
			if (abs_float(v2) > 2 * abs_float(v1))
				qin = qin2;
			else
				qin = qin1;
		}
		temp_index[i - 1] = ab1 + qin;
	}
	int Rnum2 = 0;

	int* det2 = (int*)malloc(length*sizeof(int));
	int dis2 = 0;
	for (int i = 0; i < maxcnt; i++)
	{
		int j = 0;
		while (det1[j] < temp_index[i] && j<Rnum1)
		{
			j++;
		}

		if (j == Rnum1)
		{
			if (ecg[temp_index[i]] > 0.3*ecg[det1[Rnum1 - 1]])
			{
				det2[Rnum2] = temp_index[i];
				Rnum2++;
			}
		}
		else if (j == 0)
		{
			if (abs_float(ecg[temp_index[i]]) > 0.3*abs_float(ecg[det1[0]]))
			{
				det2[Rnum2] = temp_index[i];
				Rnum2++;
			}
		}
		else
		{
			dis2 = temp_index[i] - det1[j - 1];
			if (ecg[temp_index[i]] > 0.3*ecg[det1[j - 1]] && dis2 >75)
			{
				det2[Rnum2] = temp_index[i];
				Rnum2++;
			}
		}
	}
	int ord1 = 0;
	int ord2 = 0;
	int ord3 = 0;
	int* det3 = (int*)malloc((Rnum1 + Rnum2)*sizeof(int));
	while (ord1 < Rnum1 && ord2 < Rnum2)
	{
		if (det1[ord1] < det2[ord2])
		{
			det3[ord3] = det1[ord1];
			ord1++;
			ord3++;
		}
		else
		{
			det3[ord3] = det2[ord2];
			ord2++;
			ord3++;
		}
	}
	if (ord1 == Rnum1)
	{
		for (int i = ord2; i < Rnum2; i++)
		{
			det3[ord3] = det2[i];
			ord3++;
		}
	}
	else
	{
		for (int i = ord1; i < Rnum1; i++)
		{
			det3[ord3] = det1[i];
			ord3++;
		}
	}
	if (det1 != NULL)
	{
		free(det1);
		det1 = NULL;
	}
	if (det2 != NULL)
	{
		free(det2);
		det2 = NULL;
	}
	if (diff != NULL)
	{
		free(diff);
		diff = NULL;
	}
	if (temp_index != NULL)
	{
		free(temp_index);
		temp_index = NULL;
	}

	for (int i = 0; i < ord3; i++)
	{
		det3[i] = det3[i] + 2;
	}

	int* det3_dif = (int*)malloc((ord3 - 1)*sizeof(int));
	for (int i = 1; i < ord3; i++)
	{
		det3_dif[i - 1] = det3[i] - det3[i - 1];
	}
	int n3 = 1;
	det[0] = det3[0];
	for (int i = 0; i < ord3 - 1; i++)
	{
		if (det3_dif[i]>50)
		{
			det[n3] = det3[i + 1];
			n3++;
		}
		else
		{
			if (i < ord3 - 2)
				det3_dif[i + 1] = det3_dif[i + 1] + det3_dif[i];
		}
	}
	*Rnum = n3;

	if (det3 != NULL)
	{
		free(det3);
		det3 = NULL;
	}
	if (det3_dif != NULL)
	{
		free(det3_dif);
		det3_dif = NULL;
	}
	if (m_index != NULL)
	{
		free(m_index);
		m_index = NULL;
	}
	return 1;

}
float QRSsearch::maxloc_det(float* ecg, int length, int* loc)
{
	float temp = ecg[0];
	int num = 0;
	for (int i = 1; i < length; i++)
	{
		if (temp <= ecg[i])
		{
			temp = ecg[i];
			num = i;
		}
	}
	*loc = num;
	return temp;
}
double QRSsearch::maxloc_det(double* ecg, int length, int* loc)
{
	float temp = ecg[0];
	int num = 0;
	for (int i = 1; i < length; i++)
	{
		if (temp <= ecg[i])
		{
			temp = ecg[i];
			num = i;
		}
	}
	*loc = num;
	return temp;
}
float QRSsearch::minloc_det(float* ecg, int length, int* loc)
{
	float temp = ecg[0];
	int num = 0;
	for (int i = 1; i < length; i++)
	{
		if (temp >= ecg[i])
		{
			temp = ecg[i];
			num = i;
		}
	}
	*loc = num;
	return temp;
}
void QRSsearch::quick_sort_float(float s[], int l, int r)
{
	if (l < r)
	{
		int i = l, j = r;
		float x = s[l];
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
		quick_sort_float(s, l, i - 1);
		quick_sort_float(s, i + 1, r);
	}
}
void QRSsearch::quick_sort(int s[], int l, int r)
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
float QRSsearch::max_det(float* ecg, int length)
{
	float temp = ecg[0];
	for (int i = 1; i < length; i++)
	{
		if (temp <= ecg[i])
			temp = ecg[i];
	}
	return temp;
}
float QRSsearch::abs_float(float a)
{
	if (a >= 0)
		return a;
	else
		return a*(-1);
}

int QRSsearch::diff_det(float* src, float* dst, int length, int len)
{
	for (int i = len; i < length - len; i++)
	{
		dst[i] = (2 * src[i] - src[i - len] - src[i + len]) / 2;
		if (dst[i] < 0)
			dst[i] = -dst[i];
	}
	return 0;
}


//int QRSsearch::smooth(float* src, float* dst, int length, int len)
//{
//	for (int i = len; i < length - len; i++)
//	{
//		dst[i] = (2 * src[i] - src[i - len] - src[i + len]) / 2;
//		if (dst[i] < 0)
//			dst[i] = -dst[i];
//	}
//	return 0;
//}


