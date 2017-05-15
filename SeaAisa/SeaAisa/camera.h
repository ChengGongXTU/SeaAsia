#pragma once
#include "SeeAisa.h"
#include"geometry.h"
#include"transform.h"

class Camera {
public:

	// eye position
	Point eye;
	//gaze position
	Point gaze;
	//up vector
	Vector upView;

	// transform from world to eye ccordinate
	Transform worldToEye;

	//Orthographic view volume
	float oLeft, oRight, oTop, oBottom, oNear, oFar;

	//Transform form eye to orthographic projection coordinate
	Transform eyeToOrth;

	//transform from eye to perspective projection coordinate
	Transform eyeToPer;

	//pixel's number of viewport
	float nx, ny;

	//tranform from projection to viewport coordinate
	Transform proToView;

	//transform form world to viewport coordinate
	Transform orthView;
	Transform perView;

	//compute the world to eye transform
	friend Transform WorldToEye(const Point &eye,const Point &gaze,const Vector &upVeiew);

	//compute eye to orthographic projection transform
	friend Transform EyeToOrth(const float &l,const float &r, const float &b,const float &t,
		const float &n,const float &f);

	//compute eye to perspective transform;
	friend Transform EyeToPer(const float &l,const float &r,const float &b,const float &t,
		const float &n,const float &f);

	//compute  projection canonical to viewport 
	friend Transform ProToView(const int &nx,const int &ny);

	//orthographic transform
	Transform OrthView() const{
		Transform t;
		t = proToView*eyeToOrth*worldToEye;
		return t;
	}

	//perspective transform
	Transform PerView() const {
		Transform t;
		t = proToView*eyeToPer*worldToEye;
		return t;
	}


	//initialization
	Camera() { oLeft = oRight = oBottom = oTop = oNear = oFar = nx = ny = 0.f; }

	Camera(const Point e, const Point g, const Vector up, const float l, const float r, const float b, const float t,
		const float n, const float f, const int &x, const int &y) {
		eye = e;
		gaze = g;
		upView = up;
		oLeft = l;
		oRight = r;
		oBottom = b;
		oTop = t;
		oNear = n;
		oFar = f;
		nx = x;
		ny = y;
		worldToEye = WorldToEye(e, g, up);
		eyeToOrth = EyeToOrth(l, r, b, t, n, f);
		eyeToPer = EyeToPer(l, r, b, t, n, f);
		proToView = ProToView(x, y);
		orthView = OrthView();
		perView = PerView();
	}

};
