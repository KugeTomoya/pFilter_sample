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
		//ParticleのクラスをPARTICLE_NAMの数だけ呼び出す（構造体）
		//int x = rand() % this->image->cols - 1;
		//int y = rand() % this->image->rows - 1;
		//printf("%d\n", i);
		Particle* p = new Particle();
		particle.push_back(p);

		//particle[i]->setX(rand() % this->image->cols-1);
		//particle[i]->setY(rand() % this->image->rows-1);
	}
	//imshow("Capture1", particle_img); // 表示
}

//パーティクルを配置する
void Pfilter::setParticle()
{
	int pre_x, pre_y;

	for (int i = 0; i < PARTICLE_NUM; i++){
		//printf("ok");
		NOISE n;
		//RAND_MAX=そういうやつ　（stdlibにdefinされてた謎のやつ）
		//下の式は、(noise.x*2-noise.x)の範囲でrand()の値が決定するという式
		//式は「これ」としか言えない…
		n.x = (int)(((double)rand() / ((double)RAND_MAX + 1)) * (double)noise.x * 2 - (double)noise.x);
		n.y = (int)(((double)rand() / ((double)RAND_MAX + 1)) * (double)noise.y * 2 - (double)noise.y);

		//パーティクルごとの座標の更新
		//今の座標=前の座標+noise
		pre_x = particle[i]->getX() + n.x;
		particle[i]->setX(pre_x);
		pre_y = particle[i]->getY() + n.y;
		particle[i]->setY(pre_y);

		//表示するウインドウのサイズに合っているかどうかの判断を行う
		//下限より小さいとき、下限の値とする((0,0)以下は(0,0))
		if (particle[i]->getX() < 0) particle[i]->setX(0);
		if (particle[i]->getY() < 0) particle[i]->setY(0);

		//上限より大きいとき、上限の値とする(幅(cols)と高さ(rows)の最大値より上は最大値に合わせる)
		if (particle[i]->getX() > image->cols - 1) particle[i]->setX(image->cols - 1);
		if (particle[i]->getY() > image->rows - 1) particle[i]->setY(image->rows - 1);
		
		
		/*int x = rand() % this->image->cols + 1;
		int y = rand() % this->image->rows + 1;

		particle[i]->setX(x);
		particle[i]->setY(y);*/
	}
	//printf("x = %d y = %d\n", particle[0]->getX(), particle[0]->getY());
}

//尤度を求めて重さを求める
void Pfilter::setWeight()
{
	risetFlag = true;
	double dist = 0.0, sigma = 50.0, result = 0.0;

	for (int i = 0; i < PARTICLE_NUM; i++){
		//各パーティクルの色を確認
		int pixel = image->step*particle[i]->getY() + (particle[i]->getX() * 3);

		int b = image->data[pixel];
		int g = image->data[pixel + 1];
		int r = image->data[pixel + 2];

		//printf("B = %d G = %d R = %d\n", b, g, r);
		//ユークリッド距離を図る
		dist = sqrt((askB - b)*(askB - b) + (askG - g)*(askG - g) + (askR - r)*(askR - r));// 赤色らしさをユークリッド距離として求める

		//この時近いやつはriset_flagを落とす

		if (dist <= RISET_DIST)risetFlag = false;

		//printf("%f\n", dist);
		//ユークリッド距離を用いて重み付け(尤度関数というらしい…)
		result = 1.0 / (sqrt(2.0*CV_PI)*sigma) * expf(-dist*dist / (2.0*sigma*sigma));// 距離(dist)を平均、sigmaを分散として持つ、正規分布を尤度関数とする
		//printf("%f\n", result);
		//重みをparticleクラスに送る
		particle[i]->setWeight(result);
	}
	//printf("\n");

	//正規化
	//正規化する
	double sum = 0.0;
	for (int i = 0; i < PARTICLE_NUM; i++){
		sum += particle[i]->getWeight();
	}
	//重みを正規化する
	//正規化とは-->データを使いやすく整理すること らしい…
	for (int i = 0; i < PARTICLE_NUM; i++){
		particle[i]->setWeight(particle[i]->getWeight() / sum);
		//printf("%f\n", particle[i]->getWeight());
	}
}


//パーティクルの重みつき平均を推定結果
void Pfilter::measure(Particle* result)
{
	//重みの平均を出す…！？
	//
	double x = 0, y = 0, vx = 0, vy = 0;
	for (int i = 0; i < PARTICLE_NUM; i++){
		x += (double)particle[i]->getX() * particle[i]->getWeight();
		y += (double)particle[i]->getY() * particle[i]->getWeight();
	}
	//x,y更新
	result->setX((int)x);
	result->setY((int)y);
}


//リサンプリングする
void Pfilter::resample()
{	
	//distがすべて規定値以上なら完全ランダムで座標を出す
	//要するに全員ダメなパーティクルだった時の処理
	//世代交代☆大失敗
	if (risetFlag == true){
		for (int i = 0; i < PARTICLE_NUM; i++){
			particle[i]->setX(rand() % this->image->cols);
			particle[i]->setY(rand() % this->image->rows);
			particle[i]->setWeight(0.0);
		}
	}
	//一つでも近いパーティクルがあったときにこっちの処理になる
	//ルーレット選択によって決める
	//正規化した重みを使っている
	else{
		double *w = new double[PARTICLE_NUM];
		//やってることは↓と同じ
		//double w[PARTICLE_NUM] = {};
		w[0] = particle[0]->getWeight();
		//w[i]=w[i-1]+particle[i]->getWeight -> ルーレット選択での累積　(順に足していく動作を事前にやっている)(ロボWikiみて)
		for (int i = 1; i < PARTICLE_NUM; i++){
			w[i] = w[i - 1] + particle[i]->getWeight();
		}
		//ルーレット選択の動作
		for (int i = 0; i < PARTICLE_NUM; i++){
			//正規化したデータ -> ０～１の間
			//((double)rand() / ((double)RAND_MAX + 1))で得られるデータも０～１
			//この二つでルーレット選択できる
			//w[i]=w[i-1]+particle[i]->getWeightより累積したデータが得られる
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
		//wの初期化
		delete w;
		//こちらも↓と同じ動作
		/*for (int i = 0; i < PARTICLE_NUM; i++){
			w[i] = 0.0;
		}*/
	}
}

//パーティクルを出力する
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