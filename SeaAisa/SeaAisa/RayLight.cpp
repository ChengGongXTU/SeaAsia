#include "RayLight.h"

Vector RayPointLight::GetDirection(const Point &p)
{	
	Vector v = Normalize(location - p);

	return v;
}

Vector RayDirLight::GetDirection(const Point & p)
{
	Vector v = Normalize(dir.d);

	return v;
}
