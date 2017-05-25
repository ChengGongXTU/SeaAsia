#pragma once

#include"SeeAisa.h"
#include"geometry.h"
#include"ray.h"

class BoundBox
{
public:
	Point minPoint,maxPoint;

	BoundBox() { minPoint = Point(0, 0, 0); maxPoint = Point(0,0,0); }
	~BoundBox(){}

	bool intersection(const Ray &ray);
	
};

BoundBox Union(const BoundBox &b, const BoundBox &b2);
BoundBox Union(const BoundBox &b, const Point &p);