#pragma once

#include"SeeAisa.h"
#include"RGB.h"
#include"RayScene.h"
#include"RayLight.h"
#include"dxmaterials.h"
#include"information.h"
#include"ray.h"

class RayTraceAlgorithm
{
public:
	RayScene* scenePTR;

	Info info;

	virtual ColorRGB compute(Ray &ray) = 0;
};

class GlobalIllumination: public RayTraceAlgorithm
{
public:
	ColorRGB compute(Ray &ray);
};
