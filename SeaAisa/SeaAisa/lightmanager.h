#pragma once
#include"SeeAisa.h"
#include"dxlight.h"
#include"dxdevice.h"

class LightManager {
public:
	ID3D11Buffer* PointLightBuffer;
	ID3D11Buffer* DirLightBuffer;

	LightManager(){}
	~LightManager(){}

	void StartUp();
	void ShutUp();


	bool CreateBuffer(DxDevice &dev,DxDirLight &dl);
	bool CreateBuffer(DxDevice &dev, DxPointLight &pl);
	bool SetDirLight(DxDevice &dev,DxDirLight &dirLight);
};