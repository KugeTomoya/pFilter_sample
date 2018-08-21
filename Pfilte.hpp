#ifndef PFILTER_HPP_INCLUDED
#define PFILTER_HPP_INCLUDED

#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "particle.hpp"
#include <time.h>

#define PARTICLE_NUM 5000
#define NOISE_X 30
#define NOISE_Y 30
#define RISET_DIST 100

using namespace cv;
using namespace std;

typedef struct {
	int x, y;
} NOISE;

class Pfilter{
private:
	bool risetFlag;
	int askB;
	int askG;
	int askR;
	vector<Particle*> particle;
	Mat *image;//image(�摜�f�[�^)�̃|�C���^�������Ă��邱�Ƃ��|�C���g
	Mat particle_img;
	Mat campus;
	NOISE noise;

public:
	Pfilter(Mat &image, NOISE noise, int b, int g, int r);
	void setParticle();
	void setWeight();
	void resample();
	void measure(Particle* result);


	void getParticle();
	bool getFlag();
};

#endif // PFILTER_HPP_INCLUDED