#pragma once
#include"SeeAisa.h"
#include"geometry.h"
#include"texture.h"




enum RefType
{
	Dif, Spe, Mix,Ani,Lum   //Diffuse , Specular, Mix Diffuse and Specular,Luminaire;
};

class Vertex {
public:

	Vertex(){ p.x = p.y = p.z = 0.f; 
	r = g = b = 0.f; alpha = 1.f; 
	er = eg = eb = 0.f;
	pr = pg = pb = 0.f;
	textu = textv = textw = 0.f;
	refType = Dif;
	}

	Vertex(float xx, float yy, float zz, float rr, float gg, float bb,float phong,RefType rt) {
		p.x = xx, p.y = yy, p.z = zz;
		r = rr, g = gg, b = bb;
		er = eg = eb = 0.f;
		pr = pg = pb = phong;
		textu = textv = 0.f;
		refType = rt;
	}

	Vertex(float xx, float yy, float zz, float rr, float gg, float bb, float phong,float e, RefType rt) {
		p.x = xx, p.y = yy, p.z = zz;
		r = rr, g = gg, b = bb;
		er = eg = eb = e;
		pr = pg = pb = phong;
		textu = textv = 0.f;
		refType = rt;
	}

	Vertex(float xx, float yy, float zz, float rr, float gg, float bb, float p1,float p2,float p3) {
		p.x = xx, p.y = yy, p.z = zz;
		r = rr, g = gg, b = bb;
		pr = p1; pg = p2; pb = p3;
		er = eg = eb = 0.f;
		textu = textv = 0.f;

	}

	Vertex(const Point &p1) {
		p.x = p1.x;
		p.y = p1.y;
		p.z = p1.z;
	}
	//position
	Point p;

	//reflection type
	RefType refType;

	//diffuse reflection (color)
	float r, g, b, alpha;

	//emission (light source)
	float er, eg, eb;

	//Phong reflection (mirror reflection)
	float pr, pg, pb;

	//texture coordinate
	float textu, textv,textw;

	//normal
	Normal n;



	Vertex &operator=(const Vertex &v){
		p = v.p; 
		r = v.r; g = v.g; b = v.b; alpha = v.alpha;
		er = v.er; eg = v.eg; eb = v.eb;
		pr = v.pr; pg = v.pg; pb = v.pb;
		textu = v.textu; textv = v.textv;
		n = v.n;
		refType = v.refType;
		return *this;
	}

	friend float MidPointDistance(float x, float y, const Point &p0, const Point &p1);
	friend float MidPointDistance(int x, int y, const Point &p0, const Point &p1);
	friend void VertexLineInterp(Vertex *v1, Vertex *v2, float t, Vertex *vt);
	
};

class Triangle {
public:
	//vertex of triangle
	Vertex va, vb, vc;

	//center of gravity of triangle shape
	Point center;

	//normal of triangle
	Normal n;

	//initialization
	Triangle(){}

	//Triangle = another Triangle
	Triangle &operator=(const Triangle &tri) {
		va = tri.va; vb = tri.vb; vc = tri.vc;
		center = tri.center;
		n = tri.n;
		return *this;
	}
	
	Triangle(Vertex &v1, Vertex &v2, Vertex &v3) {
		va = v1;
		vb = v2;
		vc = v3;

		center = (v1.p + v2.p + v3.p)*0.3333;

		n.x = Cross(v2.p-v1.p, v3.p-v2.p).x;
		n.y = Cross(v2.p - v1.p, v3.p - v2.p).y;
		n.z = Cross(v2.p - v1.p, v3.p - v2.p).z;

		n = Normalize(n);
	}

	friend Point Tri3DBarycentric(const Point &p, const Triangle &tri);
	friend class TriangleMesh;

};


class TriangleMesh {
public:

	//number of triangles
	int triNum;

	//number of vertex
	int verNum;

	//normal number
	int nNum;

	//texture vertex number
	int uvNum;

	// pointer to ith triangle's vertex indices
	int *vi;

	//vertex's texture indices
	int* uvi;

	//pointer to an array of vertex's position
	Point *p;

	//pointer to an array of triangles' normal
	Normal *n;

	//vertex normal's indices
	int* ni;

	//pointer to texture per vertex
	float *uv;

	//Diffuse
	float kd[3];

	//Ambient
	float ka[3];

	//texture buffer
	
	Texture* texture;

	//texture flag
	int flag;
	RefType reftype;

	//initialization
	TriangleMesh() {
		triNum = verNum = nNum = uvNum = 0.f;
		vi = NULL;
		p = NULL;
		n = NULL;
		uv = NULL;
		uvi = NULL;
		kd[0] = kd[1] = kd[2] = 0.f;
		ka[0] = ka[1] = ka[2] = 0.0f;
		texture = NULL;
		flag = 0;

	}

	//suface normal
	Vector Nor(int objId) {
		Normal n0 = n[ni[objId * 3]] + n[ni[objId * 3 + 1]] + n[ni[objId * 3 + 2]];
		Vector v0 = Normalize(Vector(n0.x, n0.y, n0.z));
		return v0;
	}

	friend void LoadObj(TriangleMesh &mesh, string fileName);
	friend bool FindFace(string &str, int &j, int &k, int &l);

	friend class Triangle;

};


class Sphere {
public:
	//position in world coordinate
	Point p;

	//radius
	float radius;

	//color
	float r, g, b, alpha;

	//mesh
	TriangleMesh mesh;

	//initialization
	Sphere() { r = 0.f; r = g = b = 0.f; alpha = 1.f; }
};




//bool BindTexture(Texture*tx, TriangleMesh &mesh) {
//	mesh.texture = tx;
//	mesh.flag = 1;
//	if (mesh.texture == NULL) return false;
//	else return true;
//}
//
//
//float Get2DTexture(TriangleMesh &mesh,float textu,float textv ,int k) {
//	int i = int(textu*(mesh.texture->bmpW-1));
//	int j = int(textv*(mesh.texture->bmpH-1));
//	if (i < 0)	i = mesh.texture->bmpW + i;
//	if (j < 0)	j = mesh.texture->bmpH + j;
//	if (i > mesh.texture->bmpW)	i = i % mesh.texture->bmpW;
//	if (j > mesh.texture->bmpH)	j = j % mesh.texture->bmpH;
//	auto color = mesh.texture->tb[j*mesh.texture->lineByte + i*mesh.texture->pBitCount / 8 + k];
//	return ((unsigned int)color) / 255.f;
//}
//void SetCartoonStyle(int i,TriangleMesh &mesh){
//	if (i == 1) mesh.flag = 2;
//}

class Box {
public:
	Point minPoint, maxPoint;

	Box() {}

	Box(const Point &p1, const Point &p2) {
		minPoint = Point(min(p1.x, p2.x), min(p1.y, p2.y), min(p1.z, p2.z));
		maxPoint = Point(max(p1.x, p2.x), max(p1.y, p2.y), max(p1.z, p2.z));
	}

	friend Box Union(const Box &b, const Point &p);
	friend Box Union(const Box &b, const Box &b2);
	friend void CutBox(const Camera &camera, Box *ret);

};

