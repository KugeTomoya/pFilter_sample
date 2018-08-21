#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/opencv.hpp>
//#include <labeling/Labeling.h>
#include <conio.h>

#include <iostream>
#include <vector>

#include "template.hpp"
#include "Pfilter.hpp"

#define CAPNAME 1//"videoCollection/a.avi"//"videoCollection/capture.avi"
#define SIZE_W 320
#define SIZE_H 240

using namespace cv;
using namespace std;

int checkMode(String st){ return 0; }
int checkMode(int in){ return 1; }

Mat detectFaceInImage(Mat &image, string &cascade_file){
	CascadeClassifier cascade;
	cascade.load(cascade_file);

	vector<Rect> faces;
	cascade.detectMultiScale(image, faces, 1.1, 3, 0, Size(20, 20));

	for (int i = 0; i < faces.size(); i++){
		rectangle(image, Point(faces[i].x, faces[i].y), Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height), Scalar(0, 200, 0), 3, CV_AA);
	}
	return image;
}

int main(int argc, char *argv[])
{
	int key = -1;
	//int videoOn = 0;
	String str = "videoCollection/not_need.avi";

	if (checkMode(CAPNAME))str = "videoCollection/a.avi";

	VideoCapture cap(CAPNAME);
	// �l�X�Ȑݒ�...
	cap.set(CV_CAP_PROP_FRAME_WIDTH, SIZE_W);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, SIZE_H);
	// �J�������I�[�v���ł������̊m�F
	if (!cap.isOpened()) return -1;

	namedWindow("Capture", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	if (!checkMode(CAPNAME))namedWindow("Status", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);

	Mat frame, hsv_img,campus;
	Mat status_img(Size(SIZE_W, SIZE_H), CV_8UC3, Scalar::all(255));
	MouseParam mparam;
	int pix = 0;
	setMouseCallback("Capture", Mouse, &mparam);

	int fps = 60;
	VideoWriter writer(str, CV_FOURCC('M', 'J', 'P', 'G'), fps, Size(SIZE_W, SIZE_H), true);

	//--------------------------
	/*//�����ŏ���

	//���̏����A�c��
	//cv::BackgroundSubtractorGMG backGroundSubtractor;
	//cv::BackgroundSubtractorMOG backGroundSubtractor;
	Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
	Mat fgMaskMOG2, dst_img;
	pMOG2 = createBackgroundSubtractorMOG2(); //MOG2 approach
	//*/

	//�p�[�e�B�N���t�B���^
	NOISE noise;
	noise.x = 50;// SIZE_W * 2 / 3;
	noise.y = 50;// SIZE_H * 2 / 3;

	Pfilter pfilter(campus, noise,150,30,10);
	Particle *p = new Particle();
	//--------------------------

	while (1) {
		status_img = Scalar::all(255);
		cap >> frame;  // �L���v�`��
		writer << frame;
		cvtColor(frame, hsv_img, CV_BGR2HSV);
		campus = frame.clone();


		key = waitKey(30);
		if (key == 'q' || key == 'Q')break;

		//--------------------------
		//�����ŏ���

		//���̏����A�c��
		/*boxFilter(frame, dst_img, frame.type(), Size(5, 5), Point(-1, -1), true);
		// �}�X�N�摜�̎擾
		pMOG2->apply(dst_img, fgMaskMOG2);//*/

		//�p�[�e�B�N���t�B���^
		pfilter.setParticle();//�p�[�e�B�N���̔z�u
		pfilter.setWeight();//�ޓx�����߂ďd�ݕt��
		pfilter.measure(p);//�d�ݕt�����ς̗\��
		pfilter.resample();//���T���v�����O

		if (pfilter.getFlag() == false)circle(campus, Point(p->getX(), p->getY()), 10, Scalar(0, 255, 0), -1, CV_AA);
		//�����Z�b�g�t���O�@�p�[�e�B�N���̖ޓx���Ⴉ������false//
		pfilter.getParticle();//*/

		//--------------------------

		if (!checkMode(CAPNAME))videoPlayer(status_img, key, cap);

		checkPixel(hsv_img, status_img, &mparam, checkMode(CAPNAME)); //hsv_img
		pix = (campus.step*(campus.rows/2))+((campus.cols/2)*3);
		//printf("B:%d G:%d R:%d\n", campus.data[pix], campus.data[pix + 1], campus.data[pix + 2]);
		imshow("Status", status_img);
		imshow("Capture", campus); // �\��Status//detectFaceImage campus

	}
}

// �R�[���o�b�N�֐�
void Mouse(int event, int x, int y, int flags, void *param) // �R�[���o�b�N�֐�
{
	MouseParam *mparam = (MouseParam *)param;
	switch (event){
	case CV_EVENT_LBUTTONDOWN:
		mparam->flag = 1;
		mparam->x = x;
		mparam->y = y;
		break;
	}
}//*/