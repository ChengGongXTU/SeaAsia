#pragma once
#include"SeeAisa.h"



struct DxPointLight {
public:
	XMFLOAT4 Pos;
	XMFLOAT4 Color;
	float intensity;
	float range;

};

struct DxDirLight {
public:
	XMFLOAT4 Dir;
	XMFLOAT4 Color;
	XMFLOAT4 up;
	XMFLOAT4 right;
	float intensity;
};

struct DxSpotLight {
public:
	XMFLOAT4 Pos;
	XMFLOAT4 Color;
	XMFLOAT4 Dir;
	float intensity;
	float range;
	float angle_u;
	float angle_p;
};
