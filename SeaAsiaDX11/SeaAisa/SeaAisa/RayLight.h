#pragma once
#include"SeeAisa.h"
#include"ray.h"
#include"geometry.h"
#include"information.h"
#include"RGB.h"
#include"shape.h"
#include"RayScene.h"
#include"sample.h"

class RayLight
{
public:

	float color[3];

	virtual float GetDistance(Info &info) = 0;
	virtual bool ShadowText(Ray &ray, Info &info);
	virtual Vector GetDirection(Info &info) = 0;
	virtual ColorRGB GetColor(Info &info) = 0;
	virtual float G(Info &info) = 0;
	virtual float Pdf(Info &info) = 0;
};

class RayPointLight : public RayLight
{
public:
	Point location;
	float t0, t1;

	RayPointLight() { t0 = 0; t1 = INFINITY; color[0] = color[1] = color[2] = 1.f; }
	~RayPointLight(){}

	float GetDistance(Info &info);
	float G(Info &info) { return 1.f; };
	float Pdf(Info &info) { return 1.f; };
	Vector GetDirection(Info &info);
	ColorRGB GetColor(Info &info);
};

class RayDirLight : public RayLight
{
public:
	Ray dir;

	RayDirLight() { color[0] = color[1] = color[2] = 1.f; };
	~RayDirLight() {};

	float GetDistance(Info &info);
	float G(Info &info) { return 1.f; };
	float Pdf(Info &info) { return 1.f; };
	Vector GetDirection(Info &info);
	ColorRGB GetColor(Info &info);
};

class AreaLight :public RayLight
{
public:
	Shape* obj;

	RayMaterial* mtl;

	AreaLight() { obj = NULL; mtl = NULL; color[0] = color[1] = color[2] = 1.f;}

	Vector GetDirection(Info &info);

	Vector wi;
	Point p;
	Normal n;

	float GetDistance(Info &info);
	ColorRGB GetColor(Info &info);
	//bool ShadowText(Ray &ray,Info &info);
	float G(Info &info);
	float Pdf(Info &info);
};
