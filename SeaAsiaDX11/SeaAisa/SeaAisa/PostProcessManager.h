#pragma once
#include"SeeAisa.h"
#include"dxobj.h"
#include"dxdevice.h"
#include"shadermanager.h"
#include"basicmanager.h"

class PostProcessManager
{
public:

	UINT rtIndex;
	UINT shaderIndex;

	ScreenQuadVertex screenVertexQuad[4];

	D3D11_BUFFER_DESC vertexBufferDescription;
	D3D11_SUBRESOURCE_DATA initData;
	WORD index[6];

	D3D_PRIMITIVE_TOPOLOGY topologyType;
	D3D11_INPUT_ELEMENT_DESC ppLayout[2];
	UINT ppNumElements;
	ID3D11InputLayout* ppVertexlayout;
	ID3D11Buffer* ppVertexBuffer;
	ID3D11Buffer* ppIndexBuffer;

	ID3D11Buffer* tonemapping_paramater_buffer;

	float adapted_lumn;

	void Startup(DxDevice &dev, ShaderManager &shaderMng);
	void Render(BasicManager & basicMng, ShaderManager & shaderMng);


};
