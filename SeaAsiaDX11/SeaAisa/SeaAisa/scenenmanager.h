#pragma once
#include"SeeAisa.h"
#include"dxscene.h"
#include "objmanager.h"

class SceneManager {

public:

	DxScene* sceneList;

	int currentSceneId;
	int endSceneId;
	int scenenNum;

	SceneManager(){}

	~SceneManager() {}

	void CreatSceneMemory(int num);

	void StartUp();

	void ShutUp();

	void ComputeSceneBounds(DxScene& scene, ObjManager& objManager);
};