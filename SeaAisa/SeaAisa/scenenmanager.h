#pragma once
#include"SeeAisa.h"
#include"dxscene.h"

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



};