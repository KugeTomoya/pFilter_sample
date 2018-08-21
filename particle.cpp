#include "particle.hpp"

Particle::Particle()
{
	particleX = 0;
	particleY = 0;
	weight = 0.0;
}

/*Particle::Particle(int x, int y)
{
	particleX = x;
	particleY = y;
}*/

void Particle::setX(int x){ particleX = x; }

void Particle::setY(int y){ particleY = y; }

void Particle::setWeight(double likelihood){ this->weight = likelihood; }


int Particle::getX(){ return particleX; }

int Particle::getY(){ return particleY; }

double Particle::getWeight(){ return weight; }