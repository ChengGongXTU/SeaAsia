#pragma once

#include"SeeAisa.h"
#include"geometry.h"
#include"dxmaterials.h"

class Info
{
public:


	int faceID;
	Point p;
	Vector tangent;
	Normal n;
	Point uv;
	int vi[3];

	float b1, b2, b3;

	DxMaterials mtl;

	int traceTime;

	Info() { faceID = 0; traceTime = 0; }

	void Init();
};