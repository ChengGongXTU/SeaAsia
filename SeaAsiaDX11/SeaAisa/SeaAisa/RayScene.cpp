#include "RayScene.h"

void RayUnity::CreateDxUnity(Unity & dxUnity)
{
	if (dxUnity.type == TriangleType)
		shape = new RayTriangleMesh[1];
	if (dxUnity.type == SphereType)
		shape = new RaySphere[1];
	if (dxUnity.type == RectangleType)
		shape = new RayRectangle[1];
	if (dxUnity.type == EmissiveType)
		shape = new RayRectangle[1];
}

void RayScene::SetSample(Sampler* sp)
{
	if (samplePTR != NULL)
	{
		delete samplePTR;
		samplePTR = NULL;
	}

	sampleNum = sp->GetSampleNum();
	samplePTR = sp;
}

void RayScene::SetSamples(int n)
{
	sampleNum = n;

	if (samplePTR != NULL)
	{
		delete samplePTR;
		samplePTR = NULL;
	}

	samplePTR =new Jittere(n);
}


