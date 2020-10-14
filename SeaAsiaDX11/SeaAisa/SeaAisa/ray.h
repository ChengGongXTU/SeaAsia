#pragma once

#include"geometry.h"

class Ray {
public:
	//origin
	Point o;
	
	//direction
	Vector d;

	//max, min and variable t
	float t0, t1;
	float t;
	float tError;

	//the intersection's depth
	int depth;

	//intersect-unity ID;
	int unityID;
	

	//initialization
	Ray() {
		t0 = 0.f; t1 = INFINITY; t = 0.f; depth = 0;unityID= 0;
	}

	Ray(const Point &p, const Vector &v, float tmin, float tmax, float tx) {
		o = p; d = Normalize(v); t0 = tmin; t1 = tmax; t = tx;
		depth = 0; unityID = 0;
	}

	//get a point from t
	Point operator()(float t) {
		return (o + d*t);
	}
};




