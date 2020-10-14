#pragma once

#include"SeeAisa.h"
#include"geometry.h"
#include"ray.h"
#include"RGB.h"

class Info
{
public:

	Ray ray;
	int faceID;
	Point p;
	Vector tangent;
	Normal n;
	Point uv;
	int vi[3];
	RayScene* scene;

	RayMaterial* mtl;

	float b1, b2, b3;

	int traceTime;
	ColorRGB backColor;

	Info() { faceID = -1; traceTime = 0; scene = NULL; mtl = NULL; backColor = ColorRGB(0.5f, 0.5f, 0.5f); }

	void Init();
};