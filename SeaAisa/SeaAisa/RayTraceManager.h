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

class RayTraceManager
{
public:
	RayScene scene;

	RayTraceAlgorithm* RayTrace;

	void SetRayTrace(int choos);

	void SceneLoad(DxScene &dxscene, BasicManager &basicMng);

	void Render(float& pace);

	RayTraceManager(){}
	~RayTraceManager(){}

	
};
