#include "BoundBox.h"

BoundBox Union(const BoundBox & b, const Point & p)
{
	BoundBox ret = b;
	ret.minPoint.x = min(b.minPoint.x, p.x);
	ret.minPoint.y = min(b.minPoint.y, p.y);
	ret.minPoint.z = min(b.minPoint.z, p.z);
	ret.maxPoint.x = max(b.maxPoint.x, p.x);
	ret.maxPoint.y = max(b.maxPoint.y, p.y);
	ret.maxPoint.z = max(b.maxPoint.z, p.z);
	return ret;
}

BoundBox Union(const BoundBox & b, const BoundBox & b2)
{	
	BoundBox ret;
	ret.minPoint.x = min(b.minPoint.x, b2.minPoint.x);
	ret.minPoint.y = min(b.minPoint.y, b2.minPoint.y);
	ret.minPoint.z = min(b.minPoint.z, b2.minPoint.z);
	ret.maxPoint.x = max(b.maxPoint.x, b2.maxPoint.x);
	ret.maxPoint.y = max(b.maxPoint.y, b2.maxPoint.y);
	ret.maxPoint.z = max(b.maxPoint.z, b2.maxPoint.z);
	return ret;
}

bool BoundBox::intersection(const Ray & ray)
{	
	
	float t = 0.f;
	Point intersect;

	float invx = 1.f / ray.d.x;
	float invy = 1.f / ray.d.y;
	float invz = 1.f / ray.d.z;
	
	//first ,x-axis face intersection testing
	if (ray.d.x != 0.f)
	{
		if (ray.d.x > 0.f)
		{
			float t = (minPoint.x - ray.o.x) *invx;
		}
		else
		{
			float t = (maxPoint.x - ray.o.x) *invx;
		}

		if (t != 0.f)
		{
			intersect = ray.o + ray.d*t;
		}

		if ((intersect.y > minPoint.y&&intersect.y<maxPoint.y)
			&&(intersect.z>minPoint.z&&intersect.z < maxPoint.z))
		{
			return true;
		}
	}

	else if (ray.d.y != 0.f)
	{
		if (ray.d.y > 0.f)
		{
			float t = (minPoint.y - ray.o.y) *invy;
		}
		else
		{
			float t = (maxPoint.y - ray.o.y) *invy;
		}

		if (t != 0.f)
		{
			intersect = ray.o + ray.d*t;
		}

		if ((intersect.x > minPoint.x&&intersect.x<maxPoint.x)
			&& (intersect.z>minPoint.z&&intersect.z < maxPoint.z))
		{
			return true;
		}
	}

	else if (ray.d.z != 0.f)
	{
		if (ray.d.z > 0.f)
		{
			float t = (minPoint.z - ray.o.z) *invz;
		}
		else
		{
			float t = (maxPoint.z - ray.o.z) *invz;
		}

		if (t != 0.f)
		{
			intersect = ray.o + ray.d*t;
		}

		if ((intersect.x > minPoint.x&&intersect.x<maxPoint.x)
			&& (intersect.y>minPoint.y&&intersect.y < maxPoint.y))
		{
			return true;
		}
	}

	else return false;
}
