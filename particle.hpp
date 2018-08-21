#ifndef PARTICLE_HPP_INCLUDED
#define PARTICLE_HPP_INCLUDED

#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

class Particle{
private:
	int particleX;
	int particleY;
	int particleVX;
	int particleVY;
	double weight;
public:
	Particle();
	//Particle(int x, int y);
	void setX(int x);
	void setY(int y);
	void setWeight(double likelihood);
	int getX();
	int getY();
	double getWeight();
};

#endif // PARTICLE_HPP_INCLUDED