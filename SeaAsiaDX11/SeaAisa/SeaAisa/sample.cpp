#include "sample.h"

void Jittere::GenerateSample()
{
	int n = (int)sqrt(sampleNum);

	for (int p = 0; p < setNum; p++)
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
			{
				Point sampleP = Point((j + dis(generator)) / n,
					(i + dis(generator)) / n,
					(0.f));
				samples.push_back(sampleP);
			}
}

Point Sampler::GetSquareSample()
{	
	if (currentSample%sampleNum == 0)
		jump = (((int)(65535*dis(generator)))%setNum)*sampleNum;
	return(samples[jump + indices[jump + currentSample++%sampleNum]]);
	/*return(samples[(int)(dis(generator)*(sampleNum-1))]);*/
}

void Sampler::SetupShuffledIndices()
{
	indices.reserve(sampleNum*setNum);
	vector<int> indices2;

	for (int i = 0; i < sampleNum; i++)
		indices2.push_back(i);

	for (int j = 0; j < setNum; j++)
	{
		random_shuffle(indices2.begin(), indices2.end());

		for (int k = 0; k < sampleNum; k++)
			indices.push_back(indices2[k]);
	}

	indices2.clear();
	vector<int>().swap(indices2);
}

int Sampler::GetSampleNum()
{
	return sampleNum;
}

void Sampler::MapSquareToHemisphere(float e)
{
	int size = samples.size();
	hemiSphereSamples.reserve(sampleNum*setNum);

	for (int i = 0; i < size; i++)
	{
		float cos_phi = cos(2.0*M_PI*samples[i].x);
		float sin_phi = sin(2.0*M_PI*samples[i].y);
		float cos_theta = pow((1.0 - samples[i].y), 1.0 / (e + 1.0));
		float sin_theta = sqrt(1.0 - cos_theta*cos_theta);
		float pu = sin_theta*cos_phi;
		float pv = sin_theta*sin_phi;
		float pw = cos_theta;

		hemiSphereSamples.push_back(Point(pu, pv, pw));
	}

	currentHmeiSphereSample = 0;

}

Point Sampler::GetHmieSphereSample()
{
	if (currentHmeiSphereSample%sampleNum == 0)
		jump = ((int)(dis(generator)*(setNum-1)))*sampleNum;

	return(hemiSphereSamples[jump + indices[jump + currentHmeiSphereSample++%sampleNum]]);
}

void NRooks::GenerateSample()
{
	for (int p = 0; p < setNum; p++)
		for (int i = 0; i < sampleNum; i++)
		{
			Point sp;
			sp.x = (i + dis(generator)) / sampleNum;
			sp.y = (i + dis(generator)) / sampleNum;
			sp.z = 0.f;
		}

	Shaflle_y_coordinate();
	Shaflle_x_coordinate();
}

void NRooks::Shaflle_x_coordinate()
{
	for (int p = 0; p < setNum; p++)
		for (int i = 0; i < sampleNum - 1; i++)
		{
			int target = (int)(dis(generator)*sampleNum) + p*sampleNum;
			float temp = samples[i + p*sampleNum + 1].x;
			samples[i + p*sampleNum + 1].x = samples[target].x;
			samples[target].x = temp;
		}
}

void NRooks::Shaflle_y_coordinate()
{
	for (int p = 0; p < setNum; p++)
		for (int i = 0; i < sampleNum - 1; i++)
		{
			int target = (int)(dis(generator)*sampleNum) + p*sampleNum;
			float temp = samples[i + p*sampleNum + 1].y;
			samples[i + p*sampleNum + 1].x = samples[target].y;
			samples[target].y = temp;
		}
}