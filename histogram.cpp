#include "histogram.h"
#include <cstring>
#include <assert.h>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <fstream>
//���캯������Ĭ��ֵ   Histogram(int _dimSize = 16, int _range = 256);
Histogram::Histogram(int _dimSize, int _range)
{
    dimSize = _dimSize;
    range = _range;
}

//����ֵ
//input: ͼ������ͨ���ĻҶ�ͼ��ÿ����Ӧ���ص�Ȩ��
//output: data   ���д��������Ҷ�ͼ��Ӧ���ص�ֵ��Ȩ��
//				  data�������Ϊһ��ֱ��ͼ��x���СΪ16*16*16��Ĭ�ϣ���Ϊ�����Ҷ�ͼ��Ӧ��һ�����ص��
//				  ��������ֵ�����ض����������һ��ֵ��y���Ӧλ��������ص��Ȩ������Ȩ���е�Ȩ��
//				  ����һ�����Ȩ�أ���Ϊһ��0~1�е�ֵ��
void Histogram::insertValues(std::vector<int> & data1, std::vector<int> & data2, std::vector<int> & data3, std::vector<double> &weight)
{
    if (data.size() < (unsigned int)dimSize*dimSize*dimSize)
        data.resize(dimSize*dimSize*dimSize);

    bool useWeights = true;
    if (weight.size() != data1.size())
        useWeights = false;

    rangePerBin = range/dimSize;				//int rangePerBin;  ����Ĭ��ֵΪ16
    rangePerBinInv = 1./(float)rangePerBin;		//float rangePerBinInv;  ����Ĭ��ֵΪ0.0625��1/16��
    double sum = 0;
	
	//
    for (unsigned int i=0; i < data1.size(); ++i){
        int id1 = rangePerBinInv*data1[i];
        int id2 = rangePerBinInv*data2[i];
        int id3 = rangePerBinInv*data3[i];
        int id = id1*dimSize*dimSize + id2*dimSize + id3;	//һ�����ص�����ͨ����ֵ�ֱ����һ��Ȩ�����	
			//id���ֵΪ4368   256*16+256+16
        double w = useWeights ? weight[i] : 1;		//���weigth��data1��������ȣ���һһ��Ӧ������ȫ����Ϊ1

		//data�Ĵ�СΪ16*16*16,����id �����ֵҪ�������
        data[id] += w;
        sum += w;				//sum��weight�ĺ�
    }

	//��һ������data�е�Ȩֵ��ӣ�ÿ��Ȩֵ����Ϊ  ֮ǰ��ÿ��Ȩֵ��Ȩֵ���еı��أ�
    normalize();
}
//�������ƶȡ�����ֱ��ͼ��  ���ж�Ӧ��data[i]�Ļ� ��ƽ����  �ĺ�
double Histogram::computeSimilarity(Histogram * hist)
{
    double conf = 0;
    for (unsigned int i=0; i < data.size(); ++i) {
        conf += sqrt(data[i]*hist->data[i]);
    }
    return conf;
}
//��ȡ��Ӧ��Ȩֵ
double Histogram::getValue(int val1, int val2, int val3)
{
    int id1 = rangePerBinInv*val1;
    int id2 = rangePerBinInv*val2;
    int id3 = rangePerBinInv*val3;
    int id = id1*dimSize*dimSize + id2*dimSize + id3;
    return data[id];
}

void Histogram::transformToWeights()
{
    double min = 0;
/*    std::ifstream alfa_file;
    alfa_file.open("param.txt");
    if (alfa_file.is_open()){
        double sum = 0;
        for (unsigned int i=0; i < data.size(); ++i) {
            sum += data[i];
        }
        double alfa;
        alfa_file >> alfa;
        min = (alfa/100.0)*sum;
        alfa_file.close();
    }else*/
        min = getMin();			//�ҳ�data�е���Сֵ

    transformByWeight(min);
}
//�ٹ�һ����data�е���Сֵ�ֱ����data�е�����
void Histogram::transformByWeight(double min)
{
    for (unsigned int i=0; i < data.size(); ++i){
        if (data[i] > 0){
            data[i] = min/data[i];
            if (data[i] > 1)
                data[i] = 1;
        }else
            data[i] = 1;
    }

}

void Histogram::multiplyByWeights(Histogram * hist)
{
    double sum = 0;
    for (unsigned int i=0; i < data.size(); ++i) {
        data[i] *= hist->data[i];
        sum += data[i];
    }

    normalize();
}
void Histogram::clear()
{
    for (unsigned int i=0; i < data.size(); ++i)
        data[i] = 0;
}

//��׼������һ����
void Histogram::normalize()
{
    double sum = 0;
    for (unsigned int i=0; i < data.size(); ++i)
        sum += data[i];
    for (unsigned int i=0; i < data.size(); ++i)
        data[i] /= sum;
}
//�ҳ�data�е���Сֵ
double Histogram::getMin()
{
    double min = 1;
    for (unsigned int i=0; i < data.size(); ++i) {
        if (data[i] < min && data[i] != 0)
            min = data[i];
    }
    return min;
}

void Histogram::addExpHist(double alpha, Histogram & hist)
{
    double beta = 1-alpha;
    for (unsigned int i=0; i < data.size(); ++i){
        data[i] = beta*data[i] + alpha*hist.data[i];
    }
}

