#pragma once
#include"SeeAisa.h"
#include"dxscene.h"
#include"dxobj.h"
#include"objmanager.h"
#include"dxdevice.h"
#include"shadermanager.h"

class PrimitiveManager {
public:
	int objId;
	D3D11_INPUT_ELEMENT_DESC layout[3];
	UINT numElements;
	ID3D11InputLayout* vertexlayout;

	D3D11_BUFFER_DESC bd;
	D3D11_SUBRESOURCE_DATA data;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	D3D_PRIMITIVE_TOPOLOGY topologyType;


	PrimitiveManager(){}
	~PrimitiveManager(){}

	void StartUp();
	void ShutUp();
	bool LoadUnity(DxDevice &dev,ObjManager &objMng,Unity &unity);
	bool InputVertexBuffer(DxDevice &dev, Unity &unity,ShaderManager &shaderMng);
	

};