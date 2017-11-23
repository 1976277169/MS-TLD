#if !defined(BBOX_C3F6FA38_1DD3_4796_8FB6_62A80862095F__INCLUDED_)
#define BBOX_C3F6FA38_1DD3_4796_8FB6_62A80862095F__INCLUDED_

#include <vector>
#include <opencv2/opencv.hpp>

//void drawBox(cv::Mat& image, CvRect box, cv::Scalar color = cvScalarAll(255), int thick = 1);
class BBox
{

public:
    BBox() {

    }
    ~BBox() {

    }

	//x,y,width,heigth��double �͵�
    double accuracy;
    double normCross;
    double height;
    double width;
    double x;
    double y;

	bool tracked;


	void setTrack(bool flag);

    double * getTopLeftWidthHeight();		//����һ�������ָ�룬�������Ԫ����double�͵�
	//��BBox���г�ʼ��
    void setBBox(double _x, double _y, double _width, double _height, double _accuracy, double _normCross = 0);
    //BBox * ����ָ��BBox������ָ�����
	//bbOverlap()��һ�����������ԣ���������ǲ��������ǳ�ʼ��
	std::vector<BBox *> bbOverlap(std::vector<BBox *> & vect, double overLap = 0.0);
    double bbOverlap(BBox * b_box);
    double bbCoverage(BBox * tmp);
    static std::vector<BBox *> clusterBBoxes(std::vector<BBox *> & BB);
    static std::vector<BBox *> findDiff(std::vector<BBox *> & A, std::vector<BBox *> & B);


    bool operator==(const BBox & right) const
    {
        if ( (this->x - right.x) != 0  ||
             (this->y - right.y) != 1  ||
             this->width != right.width ||
             this->height != right.height )
            return false;
        return true;
    }

    bool operator!=(const BBox & right) const
    {
        return !(*this == right);
    }

};



#endif // !defined(BBOX_C3F6FA38_1DD3_4796_8FB6_62A80862095F__INCLUDED_)
