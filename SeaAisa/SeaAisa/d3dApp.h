#include"render.h"

struct SimpleVertex
{
	XMFLOAT3 Pos;  // Position
	XMFLOAT3 Normal; // Normal
};

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

bool D3DApp() {

	// compile vertex shader
	ID3D10Blob* vshader = NULL;
	ID3D10Blob* verror;
	
	HRESULT hr = D3DX11CompileFromFile(L"Tutorial02.fx", 0, 0, "VS", "vs_4_0", D3DCOMPILE_DEBUG, 0, 0, &vshader, &verror, 0);
	if (FAILED(hr)) {
		MessageBox(NULL, L"VertexShader compiling is failed",L"ERROR", MB_OK);
		return hr;
	}

	// create vertex shader
	hr = mainDev.device->CreateVertexShader(vshader->GetBufferPointer(), vshader->GetBufferSize(), 0, &mainDev.vertexShaer);


	// compile pixel shader
	ID3D10Blob* pshader=NULL;
	ID3D10Blob* perror;

	hr = D3DX11CompileFromFile(L"Tutorial02.fx", 0, 0, "PS", "ps_4_0", D3DCOMPILE_DEBUG, 0, 0, &pshader, &perror, 0);
	if (FAILED(hr)) {
		MessageBox(NULL, L"PixelShader compiling is failed", L"ERROR", MB_OK);
		return hr;
	}

	//create pixel shader
	hr = mainDev.device->CreatePixelShader(pshader->GetBufferPointer(), pshader->GetBufferSize(), 0, &mainDev.pixelShader);

	//Input layout Configuration
	D3D11_INPUT_ELEMENT_DESC inputDs[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numInputDs = ARRAYSIZE(inputDs);

	//create input object
	hr = mainDev.device->CreateInputLayout(inputDs, numInputDs, vshader->GetBufferPointer(),
											vshader->GetBufferSize(), &mainDev.inputLayouyt);
	mainDev.context->IASetInputLayout(mainDev.inputLayouyt);

	//set primitove topology
	mainDev.context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//create and set vertex buffer
	SimpleVertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
	};

	D3D11_BUFFER_DESC vbDs;
	ZeroMemory(&vbDs, sizeof(vbDs));
	vbDs.Usage = D3D11_USAGE_DEFAULT;
	vbDs.ByteWidth = sizeof(SimpleVertex) * 24;
	vbDs.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDs.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sData;
	ZeroMemory(&sData, sizeof(sData));
	sData.pSysMem = vertices;
	mainDev.device->CreateBuffer(&vbDs, &sData,&mainDev.vertexBuffer);

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	mainDev.context->IASetVertexBuffers(0, 1, &mainDev.vertexBuffer, &stride, &offset);

	//create and set index buffer
	WORD indices[] =
	{
		3,1,0,
		2,1,3,

		6,4,5,
		7,4,6,

		11,9,8,
		10,9,11,

		14,12,13,
		15,12,14,

		19,17,16,
		18,17,19,

		22,20,21,
		23,20,22
	};

	vbDs.ByteWidth = sizeof(WORD) * 36;
	vbDs.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vbDs.CPUAccessFlags = 0;

	sData.pSysMem = indices;
	hr = mainDev.device->CreateBuffer(&vbDs, &sData, &mainDev.indexBuffer);

	mainDev.context->IASetIndexBuffer(mainDev.indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	//create and set constant buffer ( note! it is empty )
	vbDs.Usage = D3D11_USAGE_DEFAULT;
	vbDs.ByteWidth = sizeof(ConstantBuffer);
	vbDs.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vbDs.CPUAccessFlags = 0;

	hr = mainDev.device->CreateBuffer(&vbDs, NULL, &mainDev.constantBuffer);

	mainDev.gWorld = XMMatrixIdentity();
	XMVECTOR eye = XMVectorSet(0.0f, 4.0f, -10.0f, 0.0f);
	XMVECTOR at = XMVectorSet(0.0, 1.0, 0.0, 0.0);
	XMVECTOR up = XMVectorSet(0.0, 1.0, 0.0, 0.0);
	mainDev.gView = XMMatrixLookAtLH(eye, at, up);
	mainDev.gProjection = XMMatrixPerspectiveFovLH(M_PI / 4.f, w / (float)h, 0.01f, 100.f);

	return true;

}

void Render() {

	static float t = 0.0f;

	static DWORD dwTimeStart = 0;
	DWORD dwTimeCur = GetTickCount();
	if (dwTimeStart == 0)
		dwTimeStart = dwTimeCur;
	t = (dwTimeCur - dwTimeStart) / 1000.0f;

	float color[4] = { 0.0f,0.0f,0.0f,1.0f };
	mainDev.context->ClearRenderTargetView(mainDev.rtv, color);
	mainDev.context->ClearDepthStencilView(mainDev.dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//first cube transform
	mainDev.gWorld = XMMatrixRotationY(t);

	ConstantBuffer cb1;
	cb1.mWorld = XMMatrixTranspose(mainDev.gWorld);
	cb1.mView = XMMatrixTranspose(mainDev.gView);
	cb1.mProjection = XMMatrixTranspose(mainDev.gProjection);

	mainDev.context->UpdateSubresource(mainDev.constantBuffer, 0, NULL, &cb1, 0, 0);

	mainDev.context->VSSetShader(mainDev.vertexShaer, NULL, 0);
	mainDev.context->VSSetConstantBuffers(0, 1, &mainDev.constantBuffer);
	mainDev.context->PSSetShader(mainDev.pixelShader, NULL, 0);
	mainDev.context->DrawIndexed(36, 0, 0);

	//second cube transform	
	XMMATRIX mSpin = XMMatrixRotationZ(-t);
	XMMATRIX mOrbit = XMMatrixRotationY(-t*2.0f);
	XMMATRIX mTranslate = XMMatrixTranslation(-4.0, 0.0, 0.0);
	XMMATRIX mScale = XMMatrixScaling(0.3, 0.3, 0.3);
	mainDev.gWorld = mScale*mSpin*mTranslate*mOrbit;

	ConstantBuffer cb2;
	cb2.mWorld = XMMatrixTranspose(mainDev.gWorld);
	cb2.mView = XMMatrixTranspose(mainDev.gView);
	cb2.mProjection = XMMatrixTranspose(mainDev.gProjection);

	mainDev.context->UpdateSubresource(mainDev.constantBuffer, 0, NULL, &cb2, 0, 0);
	mainDev.context->DrawIndexed(36, 0, 0);

	mainDev.swapChain->Present(0, 0);
}