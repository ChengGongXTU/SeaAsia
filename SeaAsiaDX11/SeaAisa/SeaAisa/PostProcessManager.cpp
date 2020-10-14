#include "PostProcessManager.h"

void PostProcessManager::Startup(DxDevice & dev, ShaderManager &shaderMng)
{	

	topologyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ppLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	ppLayout[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	ppNumElements = ARRAYSIZE(ppLayout);
	ppVertexlayout = NULL;
	ppVertexBuffer = NULL;
	ppIndexBuffer = NULL;

	rtIndex = 0;
	shaderIndex = 3;

	screenVertexQuad[0].Pos = XMFLOAT3(-1.0f, 1.0f, 0.5f);
	screenVertexQuad[0].Tex = XMFLOAT2(0.0f, 0.0f);
	screenVertexQuad[1].Pos = XMFLOAT3(1.0f, 1.0f, 0.5f);
	screenVertexQuad[1].Tex = XMFLOAT2(1.0f, 0.0f);
	screenVertexQuad[2].Pos = XMFLOAT3(-1.0f, -1.0f, 0.5f);
	screenVertexQuad[2].Tex = XMFLOAT2(0.0f, 1.0f);
	screenVertexQuad[3].Pos = XMFLOAT3(1.0f, -1.0f, 0.5f);
	screenVertexQuad[3].Tex = XMFLOAT2(1.0f, 1.0f);

	vertexBufferDescription =
	{
		4 * sizeof(ScreenQuadVertex),
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_VERTEX_BUFFER,
		0,
		0
	};

	initData.pSysMem = screenVertexQuad;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	HRESULT hr = dev.device->CreateBuffer(&vertexBufferDescription, &initData, &ppVertexBuffer);

	index[0] = 0;
	index[1] = 1;
	index[2] = 2;
	index[3] = 2;
	index[4] = 1;
	index[5] = 3;

	vertexBufferDescription.ByteWidth = sizeof(WORD) * 6;
	vertexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
	initData.pSysMem = index;
	hr = dev.device->CreateBuffer(&vertexBufferDescription, &initData, &ppIndexBuffer);

	hr = dev.device->CreateInputLayout(ppLayout, ppNumElements, shaderMng.vsBlob[shaderIndex]->GetBufferPointer(),
		shaderMng.vsBlob[shaderIndex]->GetBufferSize(), &ppVertexlayout);

	tonemapping_paramater_buffer = NULL;
	D3D11_BUFFER_DESC constantBd;
	ZeroMemory(&constantBd, sizeof(constantBd));
	constantBd.Usage = D3D11_USAGE_DEFAULT;
	constantBd.ByteWidth = sizeof(XMFLOAT4);
	constantBd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBd.CPUAccessFlags = 0;
	hr = dev.device->CreateBuffer(&constantBd, NULL, &tonemapping_paramater_buffer);

	adapted_lumn = 1.0;

}

void PostProcessManager::Render(BasicManager & basicMng, ShaderManager & shaderMng)
{	
	basicMng.dxDevice.context->VSSetShader(shaderMng.VS[shaderIndex], 0, 0);
	basicMng.dxDevice.context->PSSetShader(shaderMng.PS[shaderIndex], 0, 0);
	basicMng.dxDevice.context->OMSetRenderTargets(1, &basicMng.dxDevice.rtv[0], basicMng.dxDevice.dsv);

	basicMng.dxDevice.context->IASetInputLayout(ppVertexlayout);

	UINT stride = sizeof(ScreenQuadVertex);
	UINT offset = 0;
	basicMng.dxDevice.context->IASetVertexBuffers(0, 1, &ppVertexBuffer, &stride, &offset);
	basicMng.dxDevice.context->IASetIndexBuffer(ppIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	basicMng.dxDevice.context->IASetPrimitiveTopology(topologyType);

	basicMng.dxDevice.context->PSSetShaderResources(0, 1, &basicMng.dxDevice.rtsrv[5]);
	basicMng.dxDevice.context->PSSetSamplers(0, 1, &basicMng.dxDevice.rtSampler[5]);

	basicMng.dxDevice.context->UpdateSubresource(tonemapping_paramater_buffer, 0, NULL, &adapted_lumn, 0, 0);
	basicMng.dxDevice.context->PSSetConstantBuffers(0, 1, &tonemapping_paramater_buffer);

	basicMng.dxDevice.context->DrawIndexed(6, 0, 0);
	basicMng.dxDevice.context->OMSetDepthStencilState(NULL, 0);
}
