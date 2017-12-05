#include "region.h"
#include <algorithm>

using namespace cv;
using namespace std;
/*
void drawBox(Mat& image, CvRect box, Scalar color, int thick) {
	rectangle(image, cvPoint(box.x, box.y), cvPoint(box.x + box.width, box.y + box.height), color, thick);
}
*/

void BBox::setTrack(bool flag) {

	tracked = flag;
}

void BBox::setBBox(double _x, double _y, double _width, double _height, double _accuracy, double _normCross){
	accuracy = _accuracy;
	height = _height;
	width = _width;
	x = _x;
	y = _y;
    normCross = _normCross;
}


//��x,y,width,heigth��ֵ����ret��������ret
double * BBox::getTopLeftWidthHeight()
{
    double * ret = new double[4];
    ret[0] = x;
    ret[1] = y;
    ret[2] = width;
    ret[3] = height;

    return ret;
}

//��TM��Ȼ�Ǹ��������Ҷ�û��Ӧ����������������   int xxx() {return int;}
std::vector<BBox *> BBox::bbOverlap(std::vector<BBox *> & vect, double overLap)
{
    std::vector<BBox *> ret;		//�洢overlap<0.7��BBox
    std::vector<BBox *> retP;		//�洢overlap>0.7��BBox
    double x1, y1;
    BBox * tmp;		//tmp��һ��ָ��BBox���ָ��
    double intersection;
    double over = overLap;

    if (over == 0)		//ͷ�ļ�����ʱ��overlap=0.0����˼����Ĭ����0.7��
        over = 0.7;

		//ʹ�õ���������������vector<BBox *>
    for(std::vector<BBox *>::iterator it = vect.begin(); it != vect.end(); ++it){
        tmp = *it;
        x1 =  std::min(x + width, tmp->x + tmp->width) - std::max(x, tmp->x) + 1;
        if (x1 <= 0)
        {
            ret.push_back(*it);
            continue;
        }

        y1 =  std::min(y + height, tmp->y + tmp->height) - std::max(y, tmp->y) + 1;

        if (y1 <= 0)
        {
            ret.push_back(*it);
            continue;
        }

        intersection = x1 * y1;

		//�����ص��ȣ�������ֵ�ģ�����retP��������ֵ�ģ�����ret
        if ( (intersection / (width * height + tmp->width * tmp->height - intersection)) >= over)
            retP.push_back(*it);
        else
            ret.push_back(*it);
    }
	
	//vect��ȫ��Ϊ��������
    vect = retP;

	//���ظ�������
    return (ret);
}
//����tmp  ������box�����ú���ʱ���Ǹ����е�box��Ϣ�����ص���
double BBox::bbOverlap(BBox * tmp)
{
    double x1, y1;
    double intersection;

    x1 =  std::min(x + width, tmp->x + tmp->width) - std::max(x, tmp->x) + 1;
    if (x1 <= 0)
        return 0;
    y1 =  std::min(y + height, tmp->y + tmp->height) - std::max(y, tmp->y) + 1;

    if (y1 <= 0)
        return 0;

    intersection = x1 * y1;
	double area1 = (width+1)*(height+1);
	double area2 = (tmp->width+1)*(tmp->height+1);
    return (intersection / (area1 + area2 - intersection));
}

//����tmp�� ����Box�����ú���ʱ���Ǹ����е�box��Ϣ�����ص�����
double BBox::bbCoverage(BBox * tmp)
{
    double x1, y1;
    double intersection;

    x1 =  std::min(x + width, tmp->x + tmp->width) - std::max(x, tmp->x) + 1;
    if (x1 <= 0)
        return 0;
    y1 =  std::min(y + height, tmp->y + tmp->height) - std::max(y, tmp->y) + 1;

    if (y1 <= 0)
        return 0;

    intersection = x1 * y1;

    return (intersection);

}
//��BB���о��ࡣ
//���дκ������ȰѺ�BB[0]���ƵĹ��࣬���ƽ��ֵ��Ȼ����ʣ�µ��ٹ�����ƽ��ֵ��
//���᷵��һ��ret(std::vector<BBox *>)������Ǹ�����ľ�ֵ��ͬʱBB����ա�
std::vector<BBox *> BBox::clusterBBoxes(std::vector<BBox *> & BB)
{
    std::vector<BBox *> ret;
    std::vector<BBox *> tmpV1;
    std::vector<BBox *> tmpV2;
    std::vector<BBox *> tmpV3;

    BBox * tmp;

    if (BB.size() == 0)
        return ret;

    while(1){
        tmp = BB[0];
        tmpV1 = tmp->bbOverlap(BB);			//tmpv1�д���tmp��BB[0]����BB��box���ص���С����ֵ��0.7���ĸ�����
        tmpV3 = BB;							//BB��ʣ������Ϊ������������tmpV3

		//��������BB�������������ֱ��ڸ������ٴαȶԣ�tmpV3���������ֵ����������tmpV1����tmpV2���������бȶ���ʣ��ĸ�����
        for (std::vector<BBox *>::iterator it = BB.begin(); it != BB.end(); ++it){
            tmpV2 = (*it)->bbOverlap(tmpV1);
            for (unsigned int i=0; i<tmpV1.size(); ++i)
                tmpV3.push_back(tmpV1[i]);
            tmpV1.swap(tmpV2);
        }

		
        if (tmpV3.size() != 0){
            BBox * bbox = new BBox();
            bbox->setBBox(0,0,0,0,0);
            bbox->normCross = 0;
			//�ҳ���������tmpV3�У�normCross��accuracy�����ֵ������bbox�У���x,y,width,heigthȫ���ۼ�
			
            for (std::vector<BBox *>::iterator it = tmpV3.begin(); it != tmpV3.end(); ++it){
                bbox->x += (*it)->x;
                bbox->y += (*it)->y;
                bbox->width += (*it)->width;
                bbox->height += (*it)->height;
                if ((*it)->normCross > bbox->normCross)
                    bbox->normCross = (*it)->normCross;
                if ((*it)->accuracy > bbox->accuracy)
                    bbox->accuracy = (*it)->accuracy;
                delete *it;
            }
			//�������������x,y,width,heigth��ƽ��ֵ
            bbox->x /= tmpV3.size();
            bbox->y /= tmpV3.size();
            bbox->width /= tmpV3.size();
            bbox->height /= tmpV3.size();

			//bbox����ret��
            ret.push_back(bbox);
			//����������⣨tmpV3���͸������⣨tmpV2������������V1û�����
            tmpV3.clear();
            tmpV2.clear();
        }
        BB.swap(tmpV1);		//BB�д�����Ǹ�����

        if (BB.size() == 0)
            break;
    }

    return ret;
}
//A��B�е�BOX���бȶԡ���A�к�B��BOX�ظ���ɾ�������ص�ret����
std::vector<BBox *> BBox::findDiff(std::vector<BBox *> & A, std::vector<BBox *> & B)
{
    bool equal;
    std::vector<BBox *> ret;
    if (B.size()==0){
        ret = A;
        return (ret);
    }

    for(std::vector<BBox *>::iterator it = A.begin(); it != A.end(); ++it){
        equal = false;
        for (std::vector<BBox *>::iterator it2 = B.begin(); it2 != B.end(); ++it2)
            if (*it == *it2) {
                equal = true;
                break;
            }

        if (!equal)
            ret.push_back(*it);
    }

    return (ret);
}


