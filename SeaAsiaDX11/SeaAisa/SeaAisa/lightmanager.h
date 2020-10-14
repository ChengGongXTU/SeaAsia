#pragma once
#include"SeeAisa.h"
#include"dxlight.h"
#include"dxdevice.h"
#include "geometry.h"
#include "dxscene.h"
class LightManager {
public:

	int endDirLightID;
	int endPointLightID;
	int endSpotLightID;

	int dirLightCount;
	int pointLightCount;
	int spotLightCount;

	ID3D11Buffer** DirLightBuffer;
	ID3D11Buffer** PointLightBuffer;
	ID3D11Buffer** SpotLightBuffer;

	int DirLightType;
	int PointLightType;
	int SpotLightType;

	ID3D11Buffer* DirLightTypeBuffer;
	ID3D11Buffer* PointLightTypeBuffer;
	ID3D11Buffer* SpotLightTypeBuffer;

	ID3D11ShaderResourceView** DirLightSRV;
	ID3D11ShaderResourceView** PointLightSRV;
	ID3D11ShaderResourceView** SpotLightSRV;

	DxDirLight* dxDirLights;
	DxPointLight* dxPointLights;
	DxSpotLight* dxSpotLights;

	LightManager(){}
	~LightManager(){}

	void StartUp();
	void ShutUp();


	bool CreateBuffer(DxDevice &dev,DxDirLight &dl);
	bool CreateBuffer(DxDevice &dev, DxPointLight &pl);
	bool CreateBuffer(DxDevice & dev, DxSpotLight & pl);
	bool SetDirLight(DxDevice &dev,DxDirLight &dirLight);

	bool AddDirLight(DxDevice &dev, XMFLOAT4 color, float intensity, XMFLOAT4 dir);
	bool AddPointLight(DxDevice & dev, XMFLOAT4 color, float intensity, XMFLOAT4 wPos, float range);
	bool AddPointLight(DxDevice & dev, XMFLOAT4 color, float intensity, XMFLOAT4 dir, XMFLOAT4 wPos, float range, float inner_angle, float outer_angle);
};