#include"device.h"

class Ray {
public:
	//origin
	Point o;
	
	//direction
	Vector d;

	//max, min and variable t
	float t0, t1;
	float t;

	//the intersection's depth
	int depth;

	//intersect-objection number
	int objId;
	int meshId;

	//initialization
	Ray() {
		t0 = 0.f; t1 = INFINITY; t = 0.f; depth = 0; objId = 0;
	}

	Ray(const Point &p, const Vector &v, float tmin, float tmax, float tx) {
		o = p; d = Normalize(v); t0 = tmin; t1 = tmax; t = tx;
		depth = 0; objId = 0;
	}

	//get a point from t
	Point operator()(float t) {
		return (o + d*t);
	}
};


// check the triangle-ray intersection and update ray's t;
bool intersection(Ray &r,Triangle &tri) {
	Vector e1;
	e1= tri.vb.p - tri.va.p;
	Vector e2;
	e2= tri.vc.p - tri.va.p;
	Vector s1 = Cross(r.d, e2);
	float inv =  Dot(s1, e1);

	if (inv == 0) return FALSE;

	//barycentric coordinate
	Vector s = r.o - tri.va.p;
	float b1 = Dot(s1,s) / inv;

	Vector s2 = Cross(s, e1);
	float b2 = Dot(s2, r.d) / inv;

	if (b1 < 0.f || b2<0.f || (b1 + b2)>1.f) return FALSE;

	float tx = Dot(s2, e2) / inv;

	if (tx < r.t0 || tx>r.t1)
		return FALSE;
	else
		r.t = tx;
		return TRUE;
}

bool intersection(Ray &r, Triangle* tri, int i) {
	Vector e1 = tri[i].vb.p - tri[i].va.p;
	Vector e2 = tri[i].vc.p - tri[i].va.p;
	Vector s1 = Cross(r.d, e2);
	float inv = Dot(s1, e1);

	if (inv == 0) return FALSE;

	//barycentric coordinate
	Vector s = r.o - tri[i].va.p;
	float b1 = Dot(s1, s) / inv;

	Vector s2 = Cross(s, e1);
	float b2 = Dot(s2, r.d) / inv;

	if (b1 < 0.f || b2<0.f || (b1 + b2)>1.f) return FALSE;

	float tx = Dot(s2, e2) / inv;

	if (tx < r.t0 || tx>r.t1)
		return FALSE;
	else
		r.t = tx;
	return TRUE;
}


