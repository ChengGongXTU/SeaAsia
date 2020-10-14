#include "primitivemanager.h"


void PrimitiveManager::StartUp()
{
	objId = -1;
	layout[0] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,   0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	layout[1] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,   16, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	layout[2] = { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  28, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	layout[3] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,  40, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	layout[4] = { "TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  48, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	numElements = ARRAYSIZE(layout);
	vertexlayout = NULL;

	ZeroMemory(&bd, sizeof(bd));
	ZeroMemory(&data, sizeof(data));
	vertexBuffer = NULL;
	indexBuffer = NULL;
	vertexlayout = NULL;

	topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ppLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	ppLayout[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	ppNumElements = ARRAYSIZE(ppLayout);
	ppVertexlayout = NULL;
	ppVertexBuffer = NULL;
	ppIndexBuffer = NULL;

}

void PrimitiveManager::ShutUp()
{	
	objId = -1;
	delete layout;
	numElements = 0;
	vertexlayout = NULL;

	ZeroMemory(&bd, sizeof(bd));
	ZeroMemory(&data, sizeof(data));
	vertexBuffer = NULL;
	indexBuffer = NULL;

	topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

bool PrimitiveManager::LoadUnity(DxDevice &dev,ObjManager &objMng,Unity & unity)
{	
	if (vertexBuffer != NULL && vertexBuffer != nullptr) vertexBuffer->Release(), vertexBuffer = NULL;
	if (indexBuffer != NULL && indexBuffer != nullptr) indexBuffer->Release(), indexBuffer = NULL;

	if (objId == -1) {

		ZeroMemory(&bd, sizeof(bd));
		ZeroMemory(&data, sizeof(data));

		objId = unity.objId;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(vertexData)*(objMng.DxObjMem[unity.objId]->vertexNum);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		data.pSysMem = objMng.DxObjMem[unity.objId]->vData;
		HRESULT hr = dev.device->CreateBuffer(&bd, &data, &vertexBuffer);

		if (FAILED(hr))	return false;

		bd.ByteWidth = sizeof(WORD)*(objMng.DxObjMem[unity.objId]->faceNum) * 3;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		data.pSysMem = objMng.DxObjMem[unity.objId]->indices;
		hr = dev.device->CreateBuffer(&bd, &data, &indexBuffer);

		if (FAILED(hr))	return false;

		return true;

	}
	else if(objId >= 0 && objId <objMng.endObjId) 
	{
		
		ZeroMemory(&bd, sizeof(bd));
		ZeroMemory(&data, sizeof(data));

		objId = unity.objId;

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(vertexData)*(objMng.DxObjMem[unity.objId]->vertexNum);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		data.pSysMem = objMng.DxObjMem[unity.objId]->vData;
		HRESULT hr = dev.device->CreateBuffer(&bd, &data, &vertexBuffer);

		if (FAILED(hr))	return false;

		bd.ByteWidth = sizeof(WORD)*(objMng.DxObjMem[unity.objId]->faceNum) * 3;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		data.pSysMem = objMng.DxObjMem[unity.objId]->indices;
		hr = dev.device->CreateBuffer(&bd, &data, &indexBuffer);

		if (FAILED(hr))	return false;

		return true;
	}
	
	return false;
	
}

bool PrimitiveManager::InputVertexBuffer(DxDevice & dev,Unity &unity, ShaderManager &shaderMng)
{	
	if (vertexlayout != NULL)	vertexlayout->Release();
	HRESULT hr = dev.device->CreateInputLayout(layout, numElements, shaderMng.vsBlob[shaderMng.endVSId-1]->GetBufferPointer(),
		shaderMng.vsBlob[shaderMng.endVSId-1]->GetBufferSize(), &vertexlayout);

	if (FAILED(hr)) return false;

	dev.context->IASetInputLayout(vertexlayout);

	UINT stride = sizeof(vertexData);
	UINT offset = 0;
	dev.context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	dev.context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	dev.context->IASetPrimitiveTopology(topologyType);
	
	return true;
}

bool PrimitiveManager::InputVertexBufferGeometryShading(DxDevice & dev, Unity &unity, ShaderManager &shaderMng)
{
	if (vertexlayout != NULL)	vertexlayout->Release();
	if (shaderMng.vsBlob[0] == NULL) return false;
	HRESULT hr = dev.device->CreateInputLayout(layout, numElements, shaderMng.vsBlob[0]->GetBufferPointer(),
		shaderMng.vsBlob[0]->GetBufferSize(), &vertexlayout);

	if (FAILED(hr)) return false;

	dev.context->IASetInputLayout(vertexlayout);

	UINT stride = sizeof(vertexData);
	UINT offset = 0;
	dev.context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	dev.context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	dev.context->IASetPrimitiveTopology(topologyType);

	return true;
}

bool PrimitiveManager::LoadPPVertex(DxDevice &dev, ObjManager &objMng)
{	
	if (ppVertexBuffer != NULL && ppVertexBuffer != nullptr) ppVertexBuffer->Release(),ppVertexBuffer = NULL;
	if (ppIndexBuffer != NULL && ppIndexBuffer != nullptr) ppIndexBuffer->Release(), ppIndexBuffer = NULL;

	ScreenQuadVertex screenVertexQuad[4];
	screenVertexQuad[0].Pos = XMFLOAT3(-1.0f, 1.0f, 0.5f);
	screenVertexQuad[0].Tex = XMFLOAT2(0.0f, 0.0f);
	screenVertexQuad[1].Pos = XMFLOAT3(1.0f, 1.0f, 0.5f);
	screenVertexQuad[1].Tex = XMFLOAT2(1.0f, 0.0f);
	screenVertexQuad[2].Pos = XMFLOAT3(-1.0f, -1.0f, 0.5f);
	screenVertexQuad[2].Tex = XMFLOAT2(0.0f, 1.0f);
	screenVertexQuad[3].Pos = XMFLOAT3(1.0f, -1.0f, 0.5f);
	screenVertexQuad[3].Tex = XMFLOAT2(1.0f, 1.0f);

	D3D11_BUFFER_DESC vertexBufferDescription =
	{
		4 * sizeof(ScreenQuadVertex),
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_VERTEX_BUFFER,
		0,
		0
	};

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = screenVertexQuad;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	HRESULT hr = dev.device->CreateBuffer(&vertexBufferDescription,&initData,&ppVertexBuffer);

	if (FAILED(hr))	return false;

	WORD index[6];
	index[0] = 0;
	index[1] = 1;
	index[2] = 2;
	index[3] = 2;
	index[4] = 1;
	index[5] = 3;

	vertexBufferDescription.ByteWidth = sizeof(WORD)*6;
	vertexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
	initData.pSysMem = index;
	hr = dev.device->CreateBuffer(&vertexBufferDescription, &initData, &ppIndexBuffer);

	if (FAILED(hr))	return false;

	return true;
}

bool PrimitiveManager::InputVertexBufferLightShading(DxDevice & dev, ShaderManager &shaderMng)
{	

	if (ppVertexlayout != NULL && ppVertexlayout != nullptr)	ppVertexlayout->Release(), ppVertexlayout =NULL;

	if (shaderMng.vsBlob[1] == NULL) return false;
	HRESULT hr = dev.device->CreateInputLayout(ppLayout, ppNumElements, shaderMng.vsBlob[1]->GetBufferPointer(),
		shaderMng.vsBlob[1]->GetBufferSize(), &ppVertexlayout);

	if (FAILED(hr)) return false;

	dev.context->IASetInputLayout(ppVertexlayout);

	UINT stride = sizeof(ScreenQuadVertex);
	UINT offset = 0;
	dev.context->IASetVertexBuffers(0, 1, &ppVertexBuffer, &stride, &offset);
	dev.context->IASetIndexBuffer(ppIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	dev.context->IASetPrimitiveTopology(topologyType);

	return true;
}