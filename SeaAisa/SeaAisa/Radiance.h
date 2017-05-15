#include"d3dApp.h"

struct Ls{
	float r, g, b;

	Ls() { r = g = b = 0.f; }

	Ls(float x, float y, float z) { r = x, g = y, b = z; }

	Ls operator+(const Ls ls) const {
		return Ls(r + ls.r, g + ls.g, b + ls.b);
	}

	Ls operator*(const Ls ls)const {
		return Ls(r * ls.r, g * ls.g, b * ls.b);
	}

	Ls operator*(const float t) const {
		return Ls(r*t, g*t, b*t);
	}
};


//search nearest ray-intersected triangle, update Ray's t and triId;
bool IntersectionSearch(Ray &r,Triangle* tri, int triNum) {

	float mint = INFINITY;
	int minId = 0;
	int sect = 0;

	for (int i = 0; i < triNum; i++) {
		if (intersection(r, tri[i])) {
			sect = 1;
			if (r.t < mint) {
				mint = r.t;
				minId = i;
			}
		}			
	}

	r.t = mint;
	r.objId = minId;

	if (sect == 0) return FALSE;
	else return TRUE;
}


bool IntersectionSearch(Ray &r, TriangleMesh* mesh, int meshNum) {

	float mint = INFINITY;
	int minObjId = 0;
	int minMeshId = 0;
	int sect = 0;

	for (int k = 0; k < meshNum; k++) {
		for (int i = 0; i < mesh[k].triNum; i++) {
			Triangle tri = Triangle(Vertex(Point(mesh[k].p[mesh[k].vi[i * 3]])),
				Vertex(Point(mesh[k].p[mesh[k].vi[i * 3 + 1]])),
				Vertex(Point(mesh[k].p[mesh[k].vi[i * 3 + 2]])));
			if (intersection(r, tri)) {
				sect = 1;
				if (r.t < mint) {
					mint = r.t;
					minObjId = i;
					minMeshId = k;
				}
			}
		}
	}

	r.t = mint;
	r.objId = minObjId;
	r.meshId = minMeshId;

	if (sect == 0) return FALSE;
	else return TRUE;
}


Ls InterpolateRadiance(Ray &r, int depth, Triangle* tri, int triNum, int sampleNum) {

	//find ray-intersetcion triangle's id and ray's t;
	if (!IntersectionSearch(r, tri, triNum))
	{
		//if not intersect, return background
		return Ls(backcolor[0], backcolor[1], backcolor[2]);
	}
		//if intersected, ray trace time increases. 
	else r.depth++;

	//build this triangle's reference
	Triangle &tri1 = tri[r.objId];

	//intersection 's position
	Vertex v = Vertex(r.o + r.t*r.d);

	//intersection 's normal
	if(Dot(tri1.n,-r.d)>0)
		v.n = tri1.n;
	else
		v.n = -tri1.n;

	//intersection's barycemtric coordinate
	Point bary = Tri3DBarycentric(v.p, tri1);
	float a = bary.x;
	float b = bary.y;
	float c = bary.z;

	//intersection's radiance parameter
	//diffuse reflection

	v.p = a*tri1.va.p + b*tri1.vb.p + c*tri1.vc.p;

	v.r = a*tri1.va.r + b*tri1.vb.r + c*tri1.vc.r;
	v.g = a*tri1.va.g + b*tri1.vb.g + c*tri1.vc.g;
	v.b = a*tri1.va.b + b*tri1.vb.b + c*tri1.vc.b;

	//phong reflection
	v.pr = a*tri1.va.pr + b*tri1.vb.pr + c*tri1.vc.pr;
	v.pg = a*tri1.va.pg + b*tri1.vb.pg + c*tri1.vc.pg;
	v.pb = a*tri1.va.pb + b*tri1.vb.pb + c*tri1.vc.pb;

	//emission
	v.er = a*tri1.va.er + b*tri1.vb.er + c*tri1.vc.er;
	v.eg = a*tri1.va.eg + b*tri1.vb.eg + c*tri1.vc.eg;
	v.eb = a*tri1.va.eb + b*tri1.vb.eb + c*tri1.vc.eb;
	
	//material's type
	v.refType = tri1.va.refType;
	float p = max(v.r, max(v.g, v.b));

	//if ray trace time beyound depth, use Russian roulette to instead of "only returning zero". 
	if (r.depth > depth) {
		
		if (dis(generator) < p) {
			v.r = v.r*(1.f / p);
			v.g = v.g*(1.f / p);
			v.r = v.b*(1.f / p);
		}
		else return Ls(v.r, v.g, v.b);
	}


	//if hit a luminaire, stop reflecting

	if ((v.refType == Dif) || (v.refType == Lum)) {

		//compute random numbers
		double r1 = 2 * M_PI*dis(generator);
		double r2 = dis(generator);
		float r2a = sqrtf(r2);
		float r2b = sqrtf(1 - r2);

		//compute coodinate at intersection
		Vector w = Normalize(Vector(v.n.x, v.n.y, v.n.z));
		Vector u = Normalize(tri1.vb.p - tri1.va.p);
		Vector v1 = Cross(w, u);

		//compute random direction
		Vector d = cos(r1)*r2a*u + sin(r1)*r2a*v1 + r2b*w;

		//if (Dot(d, r.d) > 0) d = -d;

		Ray rt = Ray(v.p, d, 0, INFINITY, INFINITY);
		rt.t0 = 0.001f;
		rt.t = 0.f;
		rt.depth = r.depth;
		rt.objId = r.objId;

		return Ls(v.er, v.eg, v.eb) + Ls(v.r, v.g, v.b)*InterpolateRadiance(rt, depth, tri, triNum, sampleNum);
	}
}


Ls Radiance(Ray &r, int depth, Triangle* tri, int triNum, int sampleNum) {

	float L = 100.f;
	Point c = tri[2].va.p + (tri[2].vc.p - tri[2].va.p)*0.5;

	//find ray-intersetcion triangle's id and ray's t;
	if (!IntersectionSearch(r, tri, triNum))
	{
		//if not intersect, return background
		return Ls(backcolor[0], backcolor[1], backcolor[2]);
	}

	else if (r.objId == 2 || r.objId == 3) {
		if (((r.o + r.t*r.d) - c).Length() < L)
			return Ls(12.f, 12.f, 12.f);
	}

	//if intersected, ray trace time increases. 
	else r.depth++;


	//build this triangle's reference
	Triangle &tri1 = tri[r.objId];

	//intersection 's position
	Point p0 = Point(r.o + r.t*r.d);
	Vector n0 = Normalize(Cross(tri1.vb.p - tri1.va.p, tri1.vc.p - tri1.va.p));

	//intersection 's normal
	if (Dot(n0, -r.d) < 0)
		n0 = -n0;

	float p = max(tri1.va.r, max(tri1.va.g, tri1.va.b));

	Ls R = Ls(tri1.va.r, tri1.va.g, tri1.va.b);

	//if ray trace time beyound depth, use Russian roulette to instead of "only returning zero". 
	if (r.depth > depth) {

		if (dis(generator) < p) {
			R.r = tri1.va.r*(1.f / p);
			R.g = tri1.va.g*(1.f / p);
			R.b = tri1.va.b*(1.f / p);
		}
		else return Ls(tri1.va.er, tri1.va.eg, tri1.va.eb);
	}

	//if hit a luminaire, stop reflecting

	if (tri1.va.refType == Dif) {

		//compute random numbers
		double r1 = 2 * M_PI*dis(generator);
		double r2 = dis(generator);
		float r2a = sqrtf(r2);
		float r2b = sqrtf(1 - r2);

		//compute coodinate at intersection
		Vector w = Normalize(Vector(n0.x, n0.y, n0.z));
		Vector u = Normalize(tri1.vb.p - tri1.va.p);
		Vector v1 = Cross(w, u);

		//compute random direction
		Vector d = cos(r1)*r2a*u + sin(r1)*r2a*v1 + r2b*w;

		//if (Dot(d, r.d) > 0) d = -d;

		Ray rt = Ray(p0, d, 0.1, INFINITY, INFINITY);
		rt.depth = r.depth;

		return  R*Radiance(rt, depth, tri, triNum, sampleNum);

	}

	else if(tri1.va.refType == Spe){
		//compute random numbers
		double r1 = 2 * M_PI*dis(generator);
		double r2 = dis(generator);
		float r2a = sqrtf(r2);
		float r2b = sqrtf(1 - r2);

		//compute coodinate at intersection
		Vector w = Normalize(Vector(n0.x, n0.y, n0.z));
		Vector u = Normalize(tri1.vb.p - tri1.va.p);
		Vector v1 = Cross(w, u);

		//compute random direction
		Vector d = cos(r1)*r2a*u + sin(r1)*r2a*v1 + r2b*w;

		//if (Dot(d, r.d) > 0) d = -d;

		Ray rt = Ray(p0, d, 0.1, INFINITY, INFINITY);
		rt.depth = r.depth;

		return  Ls(0.999,0.999,0.999)*Radiance(rt, depth, tri, triNum, sampleNum);
	}

	//else if (tri1.va.refType == Ani) {

	//	float nu = 400;
	//	float nv = 400;

	//	//random numbers rang[0,1]
	//	double r1 = dis(generator);
	//	double r2 = dis(generator);
	//	float r2a = sqrtf(r2);
	//	float r2b = sqrtf(1 - r2);

	//	//spherical angles
	//	double phi = atan(sqrtf((nu + 1) / (nv + 1))*tan(M_PI*r1 / 2.f));
	//	double cosphi = cos(phi);
	//	double sinphi = sin(phi);

	//	double costheta = pow((1 - r2), 1.f / (nu*cosphi*cosphi + nv*sinphi*sinphi + 1));
	//	double theta = acos(costheta);
	//	double sintheta = asin(theta);
	//	
	//	Vector w = Normalize(Vector(n0.x, n0.y, n0.z));
	//	Vector u = Normalize(tri1.vb.p - tri1.va.p);
	//	Vector v1 = Cross(w, u);

	//	//random direction
	//	Vector w0 = cos(2*M_PI*r1)*r2a*u + sin(2 * M_PI*r1)*r2a*v1 + r2b*w;
	//	Vector wi = Normalize(r.d);
	//	Vector h = sintheta*cosphi*u + sintheta*sinphi*v1 + costheta*w;

	//	float ph = sqrtf((nu + 2.f)*(nv + 2.f))*pow(Dot(h, n0), nu*cosphi*cosphi + nv*sinphi*sinphi) / 2 * M_PI;
	//	
	//	// glossy specular term
	//	Ls Fkh = Ls(tri1.va.pr, tri1.va.pg, tri1.va.pb) + Ls(1 - tri1.va.pr, 1 - tri1.va.pg, 1 - tri1.va.pb)*pow(1 - costheta, 5);
	//	float sinv = 1.f / (4 * M_PI*Dot(h, wi)*max(Dot(n0, w0), Dot(n0, wi)));
	//	Ls fs = Fkh*ph *sinv;

	//	//diffuse term

	//	float dc = (1 - pow(1 - Dot(n0, wi) / 2.f, 5))*(1 - pow(1 - Dot(n0, w0) / 2.f, 5));
	//	float dinv = 28 / 23 * M_PI;
	//	Ls fd = Ls(tri1.va.r, tri1.va.g, tri1.va.b)*dinv*Ls(1 - tri1.va.pr, 1 - tri1.va.pg, 1 - tri1.va.pb)*dc;

	//	// path ray
	//	Ray rt = Ray(p0, w0, 0, INFINITY, INFINITY);
	//	rt.t0 = 0.1f;
	//	rt.t = 0.f;
	//	rt.depth = r.depth;
	//	rt.objId = r.objId;

	//	return Ls(tri1.va.er, tri1.va.eg, tri1.va.eb)+(fs + fd)*Radiance(rt, depth, tri, triNum, sampleNum);
	
	//}
}

Ls OneDirectLight(Ray &r, Triangle* tri, int triNum) {
	
	float R = 100.f;
	Point c = tri[2].va.p+(tri[2].vc.p-tri[2].va.p)*0.5;

	//find ray-intersetcion triangle's id and ray's t;
	if (!IntersectionSearch(r, tri, triNum))
	{
		//if not intersect, return background
		return Ls(backcolor[0], backcolor[1], backcolor[2]);
	}

	else if (r.objId == 2 || r.objId == 3) {
		if (((r.o + r.t*r.d) - c).Length() < R)
			return Ls(1.f, 1.f, 1.f);
	}

	//intersected object
	Triangle &tri1 = tri[r.objId];

	if (tri1.va.refType == Lum)	return Ls(tri1.va.er, tri1.va.eg, tri1.va.eb);

	//intersection position
	Point p0 = Point(r.o + r.t*r.d);
	
	//triangle's normal
	if (Dot(tri1.n, r.d) > 0) tri1.n = -tri1.n;
	Normal n0 = tri1.n;

	// find a light source
	if (1) {
		if (1) {

			Vector w = Normalize(c - p0);
			Vector u = Normalize(Cross(w, n0));
			Vector v = Cross(w, u);

			//compute random numbers
			double r1 = dis(generator);
			double r2 = dis(generator);

			//random direction
			float cosmax = sqrtf(1 - R*R / ((p0-c).Length()*(p0 - c).Length()));
			float cosalpha = 1 - r1 + r1*cosmax;
			float sinalpha = sqrtf(1 - cosalpha*cosalpha);

			float phi = 2 * M_PI*r2;
			float cosphi = cos(phi);
			float sinphi = sin(phi);

			Vector ki = u*cosphi*sinalpha + v*sinphi*sinalpha + w*cosalpha;
			ki = Normalize(ki);

			Ray rt = Ray(p0, ki, 0.1, INFINITY, INFINITY);

			//intersection check
			if (IntersectionSearch(rt, tri, triNum) && (rt.objId == 2 || rt.objId == 3)) {
				if (tri1.va.refType == Dif)
				{
					if (Dot(tri[rt.objId].n, rt.d) > 0)
						tri[rt.objId].n = -tri[rt.objId].n;
					double omega = 2 * M_PI*(1 - cosmax);
					float inv = 1.f / M_PI;
					return Ls(tri1.va.r, tri1.va.g, tri1.va.b)*Ls(18.f, 18.f, 18.f)*Dot(ki, n0)*omega*inv;
				}

				else if (tri1.va.refType == Spe) {
					if (Dot(tri[rt.objId].n, rt.d) > 0)
						tri[rt.objId].n = -tri[rt.objId].n;
					double omega = 2 * M_PI*(1 - cosmax);
					float inv = 1.f / M_PI;
					return Ls(0.999, 0.999, 0.999)*Ls(18.f, 18.f, 18.f)*Dot(ki, n0)*omega*inv;
				}
			}

			else return Ls(tri1.va.er, tri1.va.eg, tri1.va.eb);
		}
	}
}

Ls IndirectLight(Ray &r, int depth, Triangle* tri, int triNum, int sampleNum) {

	//find ray-intersetcion triangle's id and ray's t;
	if (!IntersectionSearch(r, tri, triNum))
	{
		//if not intersect, return background
		return Ls(backcolor[0], backcolor[1], backcolor[2]);
	}

	//if intersected, ray trace time increases. 
	else r.depth++;

	//build this triangle's reference
	Triangle &tri1 = tri[r.objId];

	//intersection 's position
	Point p0 = Point(r.o + r.t*r.d);
	Vector n0 = Normalize(Cross(tri1.vb.p - tri1.va.p, tri1.vc.p - tri1.va.p));

	//intersection 's normal
	if (Dot(n0, -r.d) < 0)
		n0 = -n0;

	float p = max(tri1.va.r, max(tri1.va.g, tri1.va.b));

	Ls R = Ls(tri1.va.r, tri1.va.g, tri1.va.b);

	//if ray trace time beyound depth, use Russian roulette to instead of "only returning zero". 
	if (r.depth > depth) {

		if (dis(generator) < p) {
			R.r = tri1.va.r*(1.f / p);
			R.g = tri1.vb.g*(1.f / p);
			R.b = tri1.vc.b*(1.f / p);
		}
		else return Ls(tri1.va.er, tri1.va.eg, tri1.va.eb);
	}

	//if hit a luminaire, stop reflecting

	if ((tri1.va.refType == Dif) || (tri1.va.refType == Lum)) {

		//compute random numbers
		double r1 = 2 * M_PI*dis(generator);
		double r2 = dis(generator);
		float r2a = sqrtf(r2);
		float r2b = sqrtf(1 - r2);

		//compute coodinate at intersection
		Vector w = Normalize(Vector(n0.x, n0.y, n0.z));
		Vector u = Normalize(tri1.vb.p - tri1.va.p);
		Vector v1 = Cross(w, u);

		//compute random direction
		Vector d = cos(r1)*r2a*u + sin(r1)*r2a*v1 + r2b*w;

		//if (Dot(d, r.d) > 0) d = -d;

		Ray rt = Ray(p0, d, 0, INFINITY, INFINITY);
		rt.t0 = 0.1f;
		rt.t = 0.f;
		rt.depth = r.depth;
		rt.objId = r.objId;

		return R*Radiance(rt, depth, tri, triNum, sampleNum);

	}
}

Ls Radiance(Ray &r, int depth, TriangleMesh* mesh, int meshNum, int sampleNum) {

	float L = 100.f;
	Point c = mesh[0].p[4] + (mesh[0].p[6] - mesh[0].p[4])*0.5;

	//find ray-intersetcion triangle's id and ray's t;
	if (!IntersectionSearch(r, mesh, meshNum))
	{
		//if not intersect, return background
		return Ls(backcolor[0], backcolor[1], backcolor[2]);
	}

	else if ((r.meshId == 0)&&(r.objId == 4 || r.objId == 5)) {
		if (((r.o + r.t*r.d) - c).Length() < L)
			return Ls(12.f, 12.f, 12.f);
	}

	//if intersected, ray trace time increases. 
	else r.depth++;


	//build this triangle's reference

	//intersection 's position
	Point p0 = Point(r.o + r.t*r.d);

	//intersection 's normal

	float p = max(mesh[r.meshId].uv[r.objId*3], max(mesh[r.meshId].uv[r.objId * 3+1], mesh[r.meshId].uv[r.objId * 3+2]));


	Ls R = Ls(mesh[r.meshId].uv[r.objId * 3], mesh[r.meshId].uv[r.objId * 3+1], mesh[r.meshId].uv[r.objId * 3+2]);

	//if ray trace time beyound depth, use Russian roulette to instead of "only returning zero". 
	if (r.depth > depth) {

		if (dis(generator) < p) {
			R.r = mesh[r.meshId].uv[r.objId * 3] *(1.f / p);
			R.g = mesh[r.meshId].uv[r.objId * 3+1] *(1.f / p);
			R.b = mesh[r.meshId].uv[r.objId * 3+2] *(1.f / p);
		}
		else return Ls(0.f, 0.f, 0.f);
	}

	//if hit a luminaire, stop reflecting

	if (mesh[r.meshId].reftype == Dif) {

		//compute random numbers
		double r1 = 2 * M_PI*dis(generator);
		double r2 = dis(generator);
		float r2a = sqrtf(r2);
		float r2b = sqrtf(1 - r2);

		//compute coodinate at intersection
		Vector w = Vector(mesh[r.meshId].n[r.objId].x, mesh[r.meshId].n[r.objId].y, mesh[r.meshId].n[r.objId].z);
		if (Dot(r.d, w) > 0) w = -w;
		Vector u = Normalize(mesh[r.meshId].p[mesh[r.meshId].vi[r.objId*3+1]] - mesh[r.meshId].p[mesh[r.meshId].vi[r.objId * 3]]);
		Vector v1 = Cross(w, u);

		//compute random direction
		Vector d = cos(r1)*r2a*u + sin(r1)*r2a*v1 + r2b*w;

		//if (Dot(d, r.d) > 0) d = -d;

		Ray rt = Ray(p0, d, 0.1, INFINITY, INFINITY);
		rt.depth = r.depth;

		return  R*Radiance(rt, depth, mesh,meshNum, sampleNum);

	}

	else if (mesh[r.meshId].reftype == Spe) {
		//compute random numbers
		double r1 = 2 * M_PI*dis(generator);
		double r2 = dis(generator);
		float r2a = sqrtf(r2);
		float r2b = sqrtf(1 - r2);

		//compute coodinate at intersection
		Vector w = Vector(mesh[r.meshId].n[r.objId].x, mesh[r.meshId].n[r.objId].y, mesh[r.meshId].n[r.objId].z);
		Vector u = Normalize(mesh[r.meshId].p[mesh[r.meshId].vi[r.objId * 3 + 1]] - mesh[r.meshId].p[mesh[r.meshId].vi[r.objId * 3]]);
		Vector v1 = Cross(w, u);

		//compute random direction
		Vector d = cos(r1)*r2a*u + sin(r1)*r2a*v1 + r2b*w;

		//if (Dot(d, r.d) > 0) d = -d;

		Ray rt = Ray(p0, d, 0.1, INFINITY, INFINITY);
		rt.depth = r.depth;

		return  Ls(0.999,0.999,0.999)*Radiance(rt, depth, mesh, meshNum, sampleNum);
	}
}

Ls OneDirectLight(Ray &r, TriangleMesh* mesh, int meshNum) {

	float L = 100.f;
	Point c = mesh[0].p[4] + (mesh[0].p[6] - mesh[0].p[4])*0.5;

	//find ray-intersetcion triangle's id and ray's t;
	if (!IntersectionSearch(r, mesh, meshNum))
	{
		//if not intersect, return background
		return Ls(backcolor[0], backcolor[1], backcolor[2]);
	}

	else if ((r.meshId == 0) && (r.objId == 2 || r.objId == 3)) {
		if (((r.o + r.t*r.d) - c).Length() < L)
			return Ls(1.f, 1.f, 1.f);
	}


	//intersection position
	Point p0 = Point(r.o + r.t*r.d);
	Vector n0 = mesh[r.meshId].Nor(r.objId);
	if (Dot(n0, r.d) > 0)	n0 = -n0;

	// find a light source
	if (1) {
		if (1) {

			Vector w = Normalize(c - p0);
			Vector u = Normalize(Cross(w, n0));
			Vector v = Cross(w, u);

			//compute random numbers
			double r1 = dis(generator);
			double r2 = dis(generator);

			//random direction
			float cosmax = sqrtf(1 - L*L / ((p0 - c).Length()*(p0 - c).Length()));
			float cosalpha = 1 - r1 + r1*cosmax;
			float sinalpha = sqrtf(1 - cosalpha*cosalpha);

			float phi = 2 * M_PI*r2;
			float cosphi = cos(phi);
			float sinphi = sin(phi);

			Vector ki = u*cosphi*sinalpha + v*sinphi*sinalpha + w*cosalpha;
			ki = Normalize(ki);

			Ray rt = Ray(p0, ki, 0.1, INFINITY, INFINITY);

			//intersection check
			if (IntersectionSearch(rt, mesh, meshNum) && (rt.objId == 2 || rt.objId == 3)) {
				if (mesh[rt.meshId].reftype == Dif)
				{	
					Vector n1 = mesh[rt.meshId].Nor(rt.objId);
					if (Dot(n1, rt.d) > 0)
						n1 = -n1;
					double omega = 2 * M_PI*(1 - cosmax);
					float inv = 1.f / M_PI;
					float cos1 = Dot(ki, n0);
					Ls R = Ls(mesh[r.meshId].uv[r.objId * 3], mesh[r.meshId].uv[r.objId * 3 + 1], mesh[r.meshId].uv[r.objId * 3 + 2]);
					return R*Ls(12.f, 12.f, 12.f)*cos1*omega*inv;
				}

				else if (mesh[rt.meshId].reftype == Spe) {
					Vector n1 = mesh[rt.meshId].Nor(rt.objId);
					if (Dot(n1, rt.d) > 0)
						n1 = -n1;
					double omega = 2 * M_PI*(1 - cosmax);
					float inv = 1.f / M_PI;
					return Ls(0.999, 0.999, 0.999)*Ls(4.f, 4.f, 4.f)*Dot(ki,n0)*omega*inv;
				}
			}

			else return Ls(0.f,0.f,0.f);
		}
	}
}