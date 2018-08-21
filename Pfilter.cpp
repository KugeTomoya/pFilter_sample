#include "Pfilter.hpp"

Pfilter::Pfilter(Mat &image, NOISE noise,int b,int g,int r)
{
	risetFlag = false;
	askB = b;
	askG = g;
	askR = r;
	srand(clock());
	this->image = &image;
	this->noise = noise;
	particle_img = this->image->clone();
	campus = this->image->clone();

	for (int i = 0; i < PARTICLE_NUM; i++){
		//Particle�̃N���X��PARTICLE_NAM�̐������Ăяo���i�\���́j
		//int x = rand() % this->image->cols - 1;
		//int y = rand() % this->image->rows - 1;
		//printf("%d\n", i);
		Particle* p = new Particle();
		particle.push_back(p);

		//particle[i]->setX(rand() % this->image->cols-1);
		//particle[i]->setY(rand() % this->image->rows-1);
	}
	//imshow("Capture1", particle_img); // �\��
}

//�p�[�e�B�N����z�u����
void Pfilter::setParticle()
{
	int pre_x, pre_y;

	for (int i = 0; i < PARTICLE_NUM; i++){
		//printf("ok");
		NOISE n;
		//RAND_MAX=����������@�istdlib��defin����Ă���̂�j
		//���̎��́A(noise.x*2-noise.x)�͈̔͂�rand()�̒l�����肷��Ƃ�����
		//���́u����v�Ƃ��������Ȃ��c
		n.x = (int)(((double)rand() / ((double)RAND_MAX + 1)) * (double)noise.x * 2 - (double)noise.x);
		n.y = (int)(((double)rand() / ((double)RAND_MAX + 1)) * (double)noise.y * 2 - (double)noise.y);

		//�p�[�e�B�N�����Ƃ̍��W�̍X�V
		//���̍��W=�O�̍��W+noise
		pre_x = particle[i]->getX() + n.x;
		particle[i]->setX(pre_x);
		pre_y = particle[i]->getY() + n.y;
		particle[i]->setY(pre_y);

		//�\������E�C���h�E�̃T�C�Y�ɍ����Ă��邩�ǂ����̔��f���s��
		//������菬�����Ƃ��A�����̒l�Ƃ���((0,0)�ȉ���(0,0))
		if (particle[i]->getX() < 0) particle[i]->setX(0);
		if (particle[i]->getY() < 0) particle[i]->setY(0);

		//������傫���Ƃ��A����̒l�Ƃ���(��(cols)�ƍ���(rows)�̍ő�l����͍ő�l�ɍ��킹��)
		if (particle[i]->getX() > image->cols - 1) particle[i]->setX(image->cols - 1);
		if (particle[i]->getY() > image->rows - 1) particle[i]->setY(image->rows - 1);
		
		
		/*int x = rand() % this->image->cols + 1;
		int y = rand() % this->image->rows + 1;

		particle[i]->setX(x);
		particle[i]->setY(y);*/
	}
	//printf("x = %d y = %d\n", particle[0]->getX(), particle[0]->getY());
}

//�ޓx�����߂ďd�������߂�
void Pfilter::setWeight()
{
	risetFlag = true;
	double dist = 0.0, sigma = 50.0, result = 0.0;

	for (int i = 0; i < PARTICLE_NUM; i++){
		//�e�p�[�e�B�N���̐F���m�F
		int pixel = image->step*particle[i]->getY() + (particle[i]->getX() * 3);

		int b = image->data[pixel];
		int g = image->data[pixel + 1];
		int r = image->data[pixel + 2];

		//printf("B = %d G = %d R = %d\n", b, g, r);
		//���[�N���b�h������}��
		dist = sqrt((askB - b)*(askB - b) + (askG - g)*(askG - g) + (askR - r)*(askR - r));// �ԐF�炵�������[�N���b�h�����Ƃ��ċ��߂�

		//���̎��߂����riset_flag�𗎂Ƃ�

		if (dist <= RISET_DIST)risetFlag = false;

		//printf("%f\n", dist);
		//���[�N���b�h������p���ďd�ݕt��(�ޓx�֐��Ƃ����炵���c)
		result = 1.0 / (sqrt(2.0*CV_PI)*sigma) * expf(-dist*dist / (2.0*sigma*sigma));// ����(dist)�𕽋ρAsigma�𕪎U�Ƃ��Ď��A���K���z��ޓx�֐��Ƃ���
		//printf("%f\n", result);
		//�d�݂�particle�N���X�ɑ���
		particle[i]->setWeight(result);
	}
	//printf("\n");

	//���K��
	//���K������
	double sum = 0.0;
	for (int i = 0; i < PARTICLE_NUM; i++){
		sum += particle[i]->getWeight();
	}
	//�d�݂𐳋K������
	//���K���Ƃ�-->�f�[�^���g���₷���������邱�� �炵���c
	for (int i = 0; i < PARTICLE_NUM; i++){
		particle[i]->setWeight(particle[i]->getWeight() / sum);
		//printf("%f\n", particle[i]->getWeight());
	}
}


//�p�[�e�B�N���̏d�݂����ς𐄒茋��
void Pfilter::measure(Particle* result)
{
	//�d�݂̕��ς��o���c�I�H
	//
	double x = 0, y = 0, vx = 0, vy = 0;
	for (int i = 0; i < PARTICLE_NUM; i++){
		x += (double)particle[i]->getX() * particle[i]->getWeight();
		y += (double)particle[i]->getY() * particle[i]->getWeight();
	}
	//x,y�X�V
	result->setX((int)x);
	result->setY((int)y);
}


//���T���v�����O����
void Pfilter::resample()
{	
	//dist�����ׂċK��l�ȏ�Ȃ犮�S�����_���ō��W���o��
	//�v����ɑS���_���ȃp�[�e�B�N�����������̏���
	//�����す�厸�s
	if (risetFlag == true){
		for (int i = 0; i < PARTICLE_NUM; i++){
			particle[i]->setX(rand() % this->image->cols);
			particle[i]->setY(rand() % this->image->rows);
			particle[i]->setWeight(0.0);
		}
	}
	//��ł��߂��p�[�e�B�N�����������Ƃ��ɂ������̏����ɂȂ�
	//���[���b�g�I���ɂ���Č��߂�
	//���K�������d�݂��g���Ă���
	else{
		double *w = new double[PARTICLE_NUM];
		//����Ă邱�Ƃ́��Ɠ���
		//double w[PARTICLE_NUM] = {};
		w[0] = particle[0]->getWeight();
		//w[i]=w[i-1]+particle[i]->getWeight -> ���[���b�g�I���ł̗ݐρ@(���ɑ����Ă�����������O�ɂ���Ă���)(���{Wiki�݂�)
		for (int i = 1; i < PARTICLE_NUM; i++){
			w[i] = w[i - 1] + particle[i]->getWeight();
		}
		//���[���b�g�I���̓���
		for (int i = 0; i < PARTICLE_NUM; i++){
			//���K�������f�[�^ -> �O�`�P�̊�
			//((double)rand() / ((double)RAND_MAX + 1))�œ�����f�[�^���O�`�P
			//���̓�Ń��[���b�g�I���ł���
			//w[i]=w[i-1]+particle[i]->getWeight���ݐς����f�[�^��������
			double roulette = ((double)rand() / ((double)RAND_MAX + 1));
			for (int j = 0; j < PARTICLE_NUM; j++){
				if (roulette > w[j]){
					continue;
				}
				else{
					particle[i]->setX(particle[j]->getX());
					particle[i]->setY(particle[j]->getY());
					particle[i]->setWeight(0.0);
					break;
				}
			}
		}
		//w�̏�����
		delete w;
		//����������Ɠ�������
		/*for (int i = 0; i < PARTICLE_NUM; i++){
			w[i] = 0.0;
		}*/
	}
}

//�p�[�e�B�N�����o�͂���
void Pfilter::getParticle()
{
	for (int i = 0; i < PARTICLE_NUM; i++){
		//printf("x = %d y = %d\n", particle[i]->checkX(), particle[i]->checkY());
		int pixel = image->step*particle[i]->getY() + (particle[i]->getX() * 3);

		image->data[pixel] = 255;
		image->data[pixel + 1] = 0;
		image->data[pixel + 2] = 0;
	}
}

bool Pfilter::getFlag(){ return risetFlag; }