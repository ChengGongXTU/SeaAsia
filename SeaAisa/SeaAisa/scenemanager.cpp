#include"scenenmanager.h"

void SceneManager::CreatSceneMemory(int num) {
	sceneList = new DxScene[num];
	for (int i = 0; i < num; i++)
	{
		sceneList[i].unityList = NULL; sceneList[i].endUnityId = 0; sceneList[i].currentUnityId = 0;
		sceneList[i].cameraList = NULL; sceneList[i].endCameraId = 0; sceneList[i].currentCameraId = 0;
		sceneList[i].plList = NULL; sceneList[i].endPlId = 0; sceneList[i].currentPlId = 0;
		sceneList[i].dlList = NULL; sceneList[i].endDlId = 0; sceneList[i].currentDlId = 0;
	}
}

void SceneManager::StartUp() {
	currentSceneId = 0;
	endSceneId = 24;
	CreatSceneMemory(24);
}

void SceneManager::ShutUp() {
	delete sceneList;
	currentSceneId = 0;
	endSceneId = 0;
}