#pragma once
#include"SeeAisa.h"
#include"ray.h"
#include"geometry.h"

class RayLight
{
public:

	float color[3];

	virtual Vector GetDirection(const Point &p) = 0;
};

class RayPointLight : public RayLight
{
public:
	Point location;
	float t0, t1;

	RayPointLight() { t0 = 0; t1 = INFINITY; }
	~RayPointLight(){}

	Vector GetDirection(const Point &p);
};

class RayDirLight : public RayLight
{
public:
	Ray dir;

	RayDirLight() {};
	~RayDirLight() {};

	Vector GetDirection(const Point &p);
};
