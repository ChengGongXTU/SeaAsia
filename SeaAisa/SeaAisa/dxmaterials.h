#pragma once

#include"SeeAisa.h"

enum MaterialType
{
	mtlDiffuse
};

struct MaterialParameter {
	XMFLOAT4 Ka;
	XMFLOAT4 Kd;
	XMFLOAT4 Ks;
	XMFLOAT4 Ke;
	float alpha;
	long int illum;
	long int Ns;
	float Ni;
};

class DxMaterials {
public:
	MaterialParameter parameter;




	MaterialType mtlType;

	DxMaterials() {
		parameter.Ka = parameter.Kd = parameter.Ks = parameter.Ke = XMFLOAT4(0.f,0.f,0.f,1.f);
		parameter.alpha = 1.f;
		parameter.Ni = 0.f;
		parameter.Ns = 0;
		mtlType = mtlDiffuse;
	}
	
};