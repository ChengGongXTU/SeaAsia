#pragma once
#include"SeeAisa.h"
//#include"shape.h"
//#include"ray.h"
#include"BoundBox.h"

struct vertexData {
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 Tex;
};


class DxObj {

public:

	int objId;

	BoundBox bbox;

	//vertex number
	int vertexNum;
	int faceNum;

	// vertex attribute
	vertexData* vData;
	
	// vertex index
	WORD* indices;

	int* faceMaterialIndices;
	int materialNum;
	int* beginFaceInEachMtl;
	int* FaceNumInEachMtl;

	//// D3D texture resource view
	//ID3D11ShaderResourceView* texView;

	////D3dD sample state
	//D3D11_SAMPLER_DESC* sampDescPointer;
	//ID3D11SamplerState* sampleState;

	virtual void Draw() = 0;

	virtual void LoadX(string fileName) = 0;
	virtual void LoadOBJ(wstring fileName) = 0;
	/*virtual void LoadOBJTotal(wstring fileName) = 0;*/

	virtual void exchange() = 0;
	virtual void SetBoundBox() = 0;
	/*virtual bool Intersection(const Ray &ray, float *tHit, float tHitError) = 0;*/
};

class DxTriangleMesh :public DxObj {

public:
	void LoadX(string fileName) {};
	void LoadOBJ(wstring fileName);
	/*void LoadOBJTotal(wstring fileName);*/
	void exchange() {};
	void Draw() {};
	void SetBoundBox();
	bool CanIntersection() { return true; };
	/*bool Intersection(const Ray &ray, float *tHit, float tHitError) { return true; };*/
};

class Sphere :public DxObj
{
public:
	void LoadX(string fileName) {};
	void LoadOBJ(wstring fileName) {};
	void exchange() {};
	void Draw() {};
	void SetBoundBox() {};
	/*bool Intersection(const Ray &ray, float *tHit, float tHitError) {};*/

};