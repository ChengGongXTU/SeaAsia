#include "SkyBox.h"

// come from web:https://www.braynzarsoft.net/viewtutorial/q16390-20-cube-mapping-skybox
//https://www.cnblogs.com/zhangbaochong/p/5573970.html
void SkyBox::CreateSphereMesh(float radius, int latitude,int longitude,  DxDevice & dxDev)
{	

	vertexNum = ((latitude - 2) * longitude) + 2;
	faceNum = ((latitude - 3)*(longitude) * 2) + (longitude * 2);

	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	skyBoxMeshVerices = new SkyBoxMeshVertex[vertexNum];

	XMVECTOR currVertPos = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	skyBoxMeshVerices[0].pos.x = 0.0f;
	skyBoxMeshVerices[0].pos.y = 0.0f;
	skyBoxMeshVerices[0].pos.z = 1.0f;

	for (DWORD i = 0; i < latitude - 2; ++i)
	{
		spherePitch = (i + 1) * (3.14 / (latitude - 1));
		Rotationx = XMMatrixRotationX(spherePitch);
		for (DWORD j = 0; j < longitude; ++j)
		{
			sphereYaw = j * (6.28 / (longitude));
			Rotationy = XMMatrixRotationZ(sphereYaw);
			currVertPos = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx * Rotationy));
			currVertPos = XMVector3Normalize(currVertPos);
			skyBoxMeshVerices[i*longitude + j + 1].pos.x = XMVectorGetX(currVertPos);
			skyBoxMeshVerices[i*longitude + j + 1].pos.y = XMVectorGetY(currVertPos);
			skyBoxMeshVerices[i*longitude + j + 1].pos.z = XMVectorGetZ(currVertPos);
		}
	}

	skyBoxMeshVerices[vertexNum - 1].pos.x = 0.0f;
	skyBoxMeshVerices[vertexNum - 1].pos.y = 0.0f;
	skyBoxMeshVerices[vertexNum - 1].pos.z = -1.0f;

	/*
	int vertexNumPerRow = sliceCount + 1;
	int rowCount = stackCount - 1;
	vertexNum = vertexNumPerRow * rowCount + 2;
	faceNum = (rowCount - 1) * sliceCount * 2 + sliceCount * 2;
	skyBoxMeshVerices = new SkyBoxMeshVertex[vertexNum];
	for (int i = 1; i <= rowCount; ++i)
	{
		float phy = XM_PI * i / stackCount;
		float tmpRadius = radius * sin(phy);
		for (int j = 0; j < vertexNumPerRow; ++j)
		{
			float theta = XM_2PI * j / sliceCount;
			UINT index = (i - 1)*vertexNumPerRow + j;
			float x = tmpRadius * cos(theta);
			float y = radius * cos(phy);
			float z = tmpRadius * sin(theta);
			skyBoxMeshVerices[index].pos = XMFLOAT3(x, y, z);
			skyBoxMeshVerices->uv = XMFLOAT2(j*1.f / sliceCount, i*1.f / sliceCount);
			XMStoreFloat3(&skyBoxMeshVerices->normal, XMVector3Normalize(XMVectorSet(x, y, z, 0)));
		}
	}
	skyBoxMeshVerices[vertexNumPerRow * rowCount].pos = XMFLOAT3(0.f, radius, 0.f);
	skyBoxMeshVerices[vertexNumPerRow * rowCount].uv = XMFLOAT2(0.f, 0.f);
	skyBoxMeshVerices[vertexNumPerRow * rowCount].normal = XMFLOAT3(0.f, 1.f, 0.f);
	skyBoxMeshVerices[vertexNumPerRow * rowCount+1].pos = XMFLOAT3(0.f, -radius, 0.f);
	skyBoxMeshVerices[vertexNumPerRow * rowCount+1].uv = XMFLOAT2(0.f, 1.f);
	skyBoxMeshVerices[vertexNumPerRow * rowCount+1].normal = XMFLOAT3(0.f, -1.f, 0.f);
	*/

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(SkyBoxMeshVertex) * vertexNum;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = &skyBoxMeshVerices[0];

	HRESULT hr = dxDev.device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBuffer);


	indices = new DWORD[faceNum * 3];
	
	int k = 0;
	for (DWORD l = 0; l < longitude - 1; ++l)
	{
		indices[k] = 0;
		indices[k + 1] = l + 1;
		indices[k + 2] = l + 2;
		k += 3;
	}

	indices[k] = 0;
	indices[k + 1] = longitude;
	indices[k + 2] = 1;
	k += 3;

	for (DWORD i = 0; i < latitude - 3; ++i)
	{
		for (DWORD j = 0; j < longitude - 1; ++j)
		{
			indices[k] = i * longitude + j + 1;
			indices[k + 1] = i * longitude + j + 2;
			indices[k + 2] = (i + 1)*longitude + j + 1;

			indices[k + 3] = (i + 1)*longitude + j + 1;
			indices[k + 4] = i * longitude + j + 2;
			indices[k + 5] = (i + 1)*longitude + j + 2;

			k += 6; // next quad
		}

		indices[k] = (i*longitude) + longitude;
		indices[k + 1] = (i*longitude) + 1;
		indices[k + 2] = ((i + 1)*longitude) + longitude;

		indices[k + 3] = ((i + 1)*longitude) + longitude;
		indices[k + 4] = (i*longitude) + 1;
		indices[k + 5] = ((i + 1)*longitude) + 1;

		k += 6;
	}

	for (DWORD l = 0; l < longitude - 1; ++l)
	{
		indices[k] = vertexNum - 1;
		indices[k + 1] = (vertexNum - 1) - (l + 1);
		indices[k + 2] = (vertexNum - 1) - (l + 2);
		k += 3;
	}

	indices[k] = vertexNum - 1;
	indices[k + 1] = (vertexNum - 1) - longitude;
	indices[k + 2] = vertexNum - 2;


	/*
	indices = new UINT[faceNum * 3];	
	UINT tmp = 0;
	int start1 = 0;
	int start2 = vertexNum - vertexNumPerRow - 2;
	int top = vertexNumPerRow * rowCount;
	int bottom = vertexNumPerRow * rowCount + 1;
	for (int i = 0; i < sliceCount; ++i)
	{
		indices[tmp] = top;
		indices[tmp + 1] = start1 + i + 1;
		indices[tmp + 2] = start1 + i;
		tmp += 3;
	}
	
	for (int i = 0; i < sliceCount; ++i)
	{
		indices[tmp] = bottom;
		indices[tmp + 1] = start2 + i;
		indices[tmp + 2] = start2 + i + 1;
		tmp += 3;
	}
	
	for (int i = 0; i < rowCount - 1; ++i)
	{
		for (int j = 0; j < sliceCount; ++j)
		{
			indices[tmp] = i * vertexNumPerRow + j;
			indices[tmp + 1] = (i + 1) * vertexNumPerRow + j + 1;
			indices[tmp + 2] = (i + 1) * vertexNumPerRow + j;
			indices[tmp + 3] = i * vertexNumPerRow + j;
			indices[tmp + 4] = i * vertexNumPerRow + j + 1;
			indices[tmp + 5] = (i + 1) * vertexNumPerRow + j + 1;
			tmp += 6;
		}
	}
	*/

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * faceNum * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = &indices[0];
	hr = dxDev.device->CreateBuffer(&indexBufferDesc, &iinitData, &indexBuffer);
}

void SkyBox::Startup(BasicManager & basicMng, ShaderManager & shaderMng)
{
	CreateSphereMesh(1,20, 20, basicMng.dxDevice);
	
	vertexLayout = new D3D11_INPUT_ELEMENT_DESC[1];
	vertexLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	//vertexLayout[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	//vertexLayout[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0};
	layoutNum = 1;

	if (basicMng.textureManager.cubemapSRVs[0] != NULL)
	{
		skyboxCubemapSRV = basicMng.textureManager.cubemapSRVs[0];
		skyBoxCubemapSampleState = basicMng.textureManager.cubemapSampleStatePointer[0];
	}

	if (basicMng.textureManager.texViewPointer[3] != NULL)
	{	
		texIndex = 3;
		skyBoxTexViewPointer = basicMng.textureManager.texViewPointer[3];
		skyBoxTexSampleStatePointer = basicMng.textureManager.sampleStatePointer[3];
	}

	vertexShaderIndex = 2;
	pixelShaderIndex = 2;

	HRESULT hr = basicMng.dxDevice.device->CreateInputLayout(vertexLayout, layoutNum, shaderMng.vsBlob[vertexShaderIndex]->GetBufferPointer(),
		shaderMng.vsBlob[vertexShaderIndex]->GetBufferSize(), &skyBoxInputLayout);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(XMMATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	hr = basicMng.dxDevice.device->CreateBuffer(&bd, NULL, &wvpBuffer);

	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;
	cmdesc.FrontCounterClockwise = false;
	cmdesc.CullMode = D3D11_CULL_NONE;
	hr = basicMng.dxDevice.device->CreateRasterizerState(&cmdesc, &RSCullNone);
	cmdesc.CullMode = D3D11_CULL_BACK;
	hr = basicMng.dxDevice.device->CreateRasterizerState(&cmdesc, &RSCullBack);

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	basicMng.dxDevice.device->CreateDepthStencilState(&dssDesc, &DSLessEqual);

}

void SkyBox::ShutUp()
{
	delete skyBoxMeshVerices;
	delete indices;
	delete vertexLayout;

	indexBuffer->Release();
	indexBuffer = NULL;
	vertexBuffer->Release();
	vertexBuffer = NULL;

	skyboxCubemapSRV->Release();
	skyboxCubemapSRV = NULL;
	skyBoxCubemapSampleState->Release();
	skyBoxCubemapSampleState = NULL;

	skyBoxTexViewPointer->Release();
	skyBoxTexViewPointer = NULL;
	skyBoxTexSampleStatePointer->Release();
	skyBoxTexSampleStatePointer = NULL;

	skyBoxInputLayout->Release();
	skyBoxInputLayout = NULL;
	wvpBuffer->Release();
	wvpBuffer = NULL;

	DSLessEqual->Release();
	DSLessEqual = NULL;
	RSCullNone->Release();
	RSCullNone = NULL;
}

void SkyBox::RenderSkyBox(ShaderManager & shaderMng, DxDevice & dxdev, DxCamera & camera)
{		
	
	dxdev.context->VSSetShader(shaderMng.VS[vertexShaderIndex], 0, 0);
	dxdev.context->PSSetShader(shaderMng.PS[pixelShaderIndex], 0, 0);
	dxdev.context->IASetInputLayout(skyBoxInputLayout);
	UINT stride = sizeof(SkyBoxMeshVertex);
	UINT offset = 0;
	dxdev.context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	dxdev.context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);


	///////////////**************new**************////////////////////
	//Reset sphereWorld
	sphereWorld = XMMatrixIdentity();

	//Define sphereWorld's world space matrix
	Scale = XMMatrixScaling(500.0f, 500.0f, 500.0f);
	//Make sure the sphere is always centered around camera
	Translation = XMMatrixTranslation(camera.at.x, camera.at.y, camera.at.z);

	//Set sphereWorld's world space using the transformations
	sphereWorld = Scale * Translation;

	XMFLOAT4X4 m1 = camera.mView.m;
	XMMATRIX camView = XMMATRIX(m1._11, m1._12, m1._13, m1._14,
		m1._21, m1._22, m1._23, m1._24,
		m1._31, m1._32, m1._33, m1._34,
		m1._41, m1._42, m1._43, m1._44);

	XMFLOAT4X4 m2 = camera.mProjection.m;
	XMMATRIX camProjection = XMMATRIX(m2._11, m2._12, m2._13, m2._14,
		m2._21, m2._22, m2._23, m2._24,
		m2._31, m2._32, m2._33, m2._34,
		m2._41, m2._42, m2._43, m2._44);

	XMMATRIX WVP = sphereWorld * camView * camProjection;
	WVP = XMMatrixTranspose(WVP);
	dxdev.context->UpdateSubresource(wvpBuffer, 0, NULL, &WVP, 0, 0);
	dxdev.context->VSSetConstantBuffers(0, 1, &wvpBuffer);
	dxdev.context->PSSetShaderResources(0, 1, &skyboxCubemapSRV);
	dxdev.context->PSSetSamplers(0, 1, &skyBoxCubemapSampleState);

	//test hdr
	dxdev.context->PSSetShaderResources(1, 1, &skyBoxTexViewPointer);
	dxdev.context->PSSetSamplers(1, 1, &skyBoxTexSampleStatePointer);


	dxdev.context->OMSetRenderTargets(1, &dxdev.rtv[5], dxdev.dsv);
	dxdev.context->OMSetDepthStencilState(DSLessEqual, 0);
	dxdev.context->RSSetState(RSCullNone);
	dxdev.context->DrawIndexed(faceNum * 3, 0, 0);
	dxdev.context->RSSetState(RSCullBack);
	dxdev.context->OMSetDepthStencilState(NULL, 0);


}

void SkyBox::ChooseTexture(TextureManager& texMng, int textureIndex)
{
	if (textureIndex >= texMng.endTextureId)
		return;

	texIndex = textureIndex;
	skyBoxTexViewPointer = texMng.texViewPointer[textureIndex];
	skyBoxTexSampleStatePointer = texMng.sampleStatePointer[textureIndex];
}