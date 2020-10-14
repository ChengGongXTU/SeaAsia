#pragma once

#include"SeeAisa.h"

enum MaterialType
{
	Opaque
};

struct MaterialParameter {
	XMFLOAT4 _ColorFactor;
	XMFLOAT4 _AmbientClor;
	float _MetallicFactor;
	float _RoughnessFactor;
	float _IBLFactor;
	float offsetX = 0;
	float offsetY = 0;
	float scaleX = 1;
	float scaleY = 1;

};

struct MaterialLerpParameter
{
	//disney bsdf lerp parameter
	float eta = 1.5;
	float strans = 0.0;
	float dt = 0.0;
	float sheenWeight = 0.0;
	float sTint = 0.0;
	float flat = 0.0;
	float anisotropic = 0.0;
	float specTint = 0.0;
	float cc = 0.f;
	float glossTint = 0.0;
};

class DxMaterials {
public:
	
	//constant
	MaterialParameter parameter;
	MaterialLerpParameter lerp;
	bool empty;
	MaterialType type;

	//texture
	ID3D11ShaderResourceView** albedoSRV;
	ID3D11SamplerState** albedoSampleState;
	int albedoID;

	ID3D11ShaderResourceView** normalSRV;
	ID3D11SamplerState** normalSampleState;
	int normalID;

	ID3D11ShaderResourceView** mraSRV;
	ID3D11SamplerState** mraSampleState;
	int mraID;


	DxMaterials() {
		parameter._ColorFactor  = XMFLOAT4(1.f,1.f,1.f,1.f);
		parameter._MetallicFactor = 1.f;
		parameter._RoughnessFactor = 1.f;
		parameter._IBLFactor = 1.f;
		parameter._AmbientClor = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
		type = Opaque;
		empty = true;

		albedoSRV = NULL;
		albedoSampleState = NULL;
		albedoID = -1;

		normalSRV = NULL;
		normalSampleState = NULL;
		normalID = -1;

		mraSRV = NULL;
		mraSampleState = NULL;
		mraID = -1;
	}
	

	DxMaterials &operator=(const DxMaterials &mtl) {
		parameter._ColorFactor = mtl.parameter._ColorFactor;
		parameter._MetallicFactor = mtl.parameter._MetallicFactor;
		parameter._RoughnessFactor = mtl.parameter._RoughnessFactor;
		parameter._IBLFactor = mtl.parameter._IBLFactor;
		parameter._AmbientClor = mtl.parameter._AmbientClor;
		type = mtl.type;

		albedoSRV = mtl.albedoSRV;
		albedoSampleState = mtl.albedoSampleState;
		albedoID = mtl.albedoID;
		normalSRV = mtl.normalSRV;
		normalSampleState = mtl.normalSampleState;
		normalID = mtl.normalID;
		mraSRV = mtl.mraSRV;
		mraSampleState = mtl.mraSampleState;
		mraID = mtl.mraID;


		return *this;
	}


};