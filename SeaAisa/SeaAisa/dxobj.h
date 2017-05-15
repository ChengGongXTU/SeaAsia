#pragma once
#include"SeeAisa.h"
#include"shape.h"

struct vertexData {
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 Tex;
};


class DxObj {

public:

	int objId;

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
};

class DxTriangleMesh :public DxObj {

public:
	void LoadX(string fileName) {};
	void LoadOBJ(wstring fileName);
	/*void LoadOBJTotal(wstring fileName);*/
	void exchange() {};
	void Draw() {};
};