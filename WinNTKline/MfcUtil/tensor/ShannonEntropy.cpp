#include "ShannonEntropy.h"

ShannonEntropy* ShannonEntropy::m_entropy = nullptr;

ShannonEntropy * ShannonEntropy::getInstance()
{
	if (m_entropy == nullptr)
		m_entropy = new ShannonEntropy();
	return m_entropy;
}

double ShannonEntropy::SingleEntropy(cv::Mat img)
{
	double result = 0;
	double temp[256] = { 0.0 };
	// ����ÿ�����ص��ۻ�ֵ
	for (int m = 0; m<img.rows; m++)
	{
		// ��Ч��������
		const uchar* t = img.ptr<uchar>(m);
		for (int n = 0; n<img.cols; n++)
		{
			int i = t[n];
			temp[i] = temp[i] + 1;
		}
	}
	// ����ÿ�����صĸ���
	for (int i = 0; i<256; i++)
	{
		temp[i] = temp[i] / (img.rows*img.cols);
	}
	// ������Ϣ��
	for (int i = 0; i<256; i++)
	{
		if (temp[i] == 0.0)
			result = result;
		else
			result = result - temp[i] * (log(temp[i]) / log(2.0));
	}
	return result;

}

double ShannonEntropy::CommonEntropy(cv::Mat img1, cv::Mat img2, double img1_entropy, double img2_entropy)
{
	double result = 0.0;
	double temp[256][256] = { 0.0 };
	// ��������ͼ�����ص��ۻ�ֵ
	for (int m1 = 0, m2 = 0; m1 < img1.rows, m2 < img2.rows; m1++, m2++)
	{
		const uchar* t1 = img1.ptr<uchar>(m1);
		const uchar* t2 = img2.ptr<uchar>(m2);
		for (int n1 = 0, n2 = 0; n1 < img1.cols, n2 < img2.cols; n1++, n2++)
		{
			int i = t1[n1], j = t2[n2];
			temp[i][j] = temp[i][j] + 1;
		}
	}
	// ����ÿ���������صĸ���
	for (int i = 0; i < 256; i++)
	{
		for (int j = 0; j < 256; j++)

		{
			temp[i][j] = temp[i][j] / (img1.rows*img1.cols);
		}
	}
	for (int i = 0; i < 256; i++)
	{
		for (int j = 0; j < 256; j++)

		{
			if (temp[i][j] == 0.0)
				result = result;
			else
				result = result - temp[i][j] * (log(temp[i][j]) / log(2.0));
		}
	}
	// �õ�����ͼ��Ļ���Ϣ��
	img1_entropy = SingleEntropy(img1);
	img2_entropy = SingleEntropy(img2);
	result = img1_entropy + img2_entropy - result;
	return result;
}
