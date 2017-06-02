#pragma once
#include"SeeAisa.h"
#include"camera.h"
#include"transform.h"
#include"RayScene.h"
#include"RayTraceAlgorithm.h"
#include"dxscene.h"
#include"basicmanager.h"
#include"objmanager.h"
#include"scenenmanager.h"
#include"sample.h"
#include"RayMaterial.h"

class RayTraceManager
{
public:
	RayScene scene;

	RayTraceAlgorithm* RayTrace;

	int totalSampleNum;
	void SetRayTrace(int choos);

	void SceneLoad(DxScene &dxscene, BasicManager &basicMng);

	void Render(float& pace);

	void SetSceneSample(int n);

	RayTraceManager() { totalSampleNum = 100; }
	~RayTraceManager(){}

	
};
