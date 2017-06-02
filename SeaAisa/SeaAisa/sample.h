#pragma once
#include"SeeAisa.h"
#include"geometry.h"

class Sampler
{
public:
	int sampleNum;
	int setNum;

	vector<Point> samples;
	vector<int> indices;
	vector<Point> hemiSphereSamples;

	int currentSample;
	int currentHmeiSphereSample;
	int jump;

	virtual void GenerateSample() = 0;
	Point GetSquareSample();
	void SetupShuffledIndices();
	int GetSampleNum();
	void MapSquareToHemisphere(float e);
	Point GetHmieSphereSample();

};

class Jittere : public Sampler
{
public:
	Jittere(int n) { sampleNum = n; setNum = 1; currentSample = 0; jump = 0; GenerateSample(); }

	void GenerateSample();
};

class NRooks :public Sampler
{
public:
	NRooks(int n){ sampleNum = n; setNum = 1; GenerateSample(); }
	void GenerateSample();
	void Shaflle_x_coordinate();
	void Shaflle_y_coordinate();
};