#pragma once
#include"SeeAisa.h"
#include"dxdevice.h"
#include"objmanager.h"
#include"texturemanager.h"
#include"scenenmanager.h"
#include"materialsmanager.h"


class BasicManager {

public:

	DxDevice dxDevice;
	ObjManager objManager;
	TextureManager textureManager;
	SceneManager sceneManager;
	MaterialsManager materialsManager;

	BasicManager(){}

	~BasicManager() {}

	bool StartUp(WindowsDevice &winDevice);

	void ShutUp();




};
