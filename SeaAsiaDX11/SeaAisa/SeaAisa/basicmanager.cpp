#include "basicmanager.h"

bool BasicManager::StartUp(WindowsDevice &winDevice)
{
	if (FAILED(dxDevice.Init(winDevice)))	return 0;
	objManager.StartUp();
	textureManager.StartUp(dxDevice);
	sceneManager.StartUp();
	materialsManager.StartUp();
	return true;
}

void BasicManager::ShutUp()
{	
	objManager.ShutUp();
	textureManager.ShutUp();
	sceneManager.ShutUp();
	materialsManager.ShutUp();
}
