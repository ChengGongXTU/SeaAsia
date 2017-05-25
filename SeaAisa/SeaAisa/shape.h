#pragma once
#include"SeeAisa.h"
#include"dxobj.h"
#include"dxmaterials.h"
#include"dxscene.h"
#include"basicmanager.h"
#include"scenenmanager.h"
#include"objmanager.h"
#include"materialsmanager.h"
#include"geometry.h"
#include"transform.h"
#include"texture.h"


#include"BoundBox.h"
#include"ray.h"
#include"information.h"


class Shape
{
public:
	BoundBox bbox;
	virtual void SetBoundBox() = 0;
	virtual bool Intersection(Ray &ray, float *tHit, float *tHitError,Info &infro)= 0;
	virtual void LoadDxUnity(Unity &unity, BasicManager &basicMng) = 0;
	virtual void ViewTransform(Transform &View) = 0;
};

enum RefType
{
	Dif, Spe, Mix,Ani,Lum   //Diffuse , Specular, Mix Diffuse and Specular,Luminaire;
};

class Triangle:public Shape {
public:
	
	int faceID;
	int vi[3];
	Point p1, p2, p3;

	Triangle(){}
	~Triangle(){}

	void SetBoundBox();
	bool Intersection(Ray &ray, float *tHit, float *tHitError, Info &infro);
	void LoadDxUnity(Unity &unity, BasicManager &basicMng) {};
	void ViewTransform(Transform &View) {};
};

class RayTriangleMesh:public Shape {
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

	//vertex normal's indices
	int* ni;

	//pointer to an array of vertex's position
	Point *p;

	//pointer to an array of triangles' normal
	Normal *n;

	//pointer to texture per vertex
	Point* uv;

	int mtlNum;

	//material
	DxMaterials *mtl;

	//materila Index for each face;
	int* mtli;

	//texture buffer
	
	Texture* texture;

	//BoundBox

	//initialization
	RayTriangleMesh() {
		triNum = verNum = nNum = uvNum = 0.f;
		vi = NULL;
		p = NULL;
		n = NULL;
		uv = NULL;
		uvi = NULL;
		mtl = NULL;
		mtli = NULL;
		texture = NULL;

	}

	void SetBoundBox();
	bool Intersection(Ray &ray, float *tHit, float *tHitError,Info &infro);
	void LoadDxUnity(Unity &unity, BasicManager &basicMng);
	void ViewTransform(Transform &View);

	friend bool FindFace(string &str, int &j, int &k, int &l);
	friend class Triangle;

};


class RaySphere: public Shape 
{
	public:
	//position in world coordinate
	Point p;

	//radius
	float radius;

	DxMaterials mtl;

	Texture* texture;

	//color
	void SetBoundBox() {};
	bool Intersection(Ray &ray, float *tHit, float *tHitError, Info &infro);
	void LoadDxUnity(Unity &unity, BasicManager &basicMng);
	void ViewTransform(Transform &View);

	//mesh
	RayTriangleMesh mesh;

	//initialization
	RaySphere() { p = Point(0, 0, 0); radius = 0; }
	~RaySphere(){ }

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



