#include "ShadowManager.h"

bool ShadowManager::Init(ID3D11Device* device, DxDevice* dxDev,ShaderManager &shaderMng)
{	
	int shadowShaderIndex = 4;
	width = 1024;
	height = 1024;

	HRESULT result;
	vertexShader = &shaderMng.VS[shadowShaderIndex];
	pixelShader = &shaderMng.PS[shadowShaderIndex];
	vsBlob = &shaderMng.vsBlob[shadowShaderIndex];
	psBlob = &shaderMng.psBlob[shadowShaderIndex];

	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc2;


	// Create a wrap texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create a clamp texture sampler state description.
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &sampleStateClamp);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the light dynamic constant buffer that is in the vertex shader.
	lightBufferDesc2.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc2.ByteWidth = sizeof(LightBufferType2);
	lightBufferDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc2.MiscFlags = 0;
	lightBufferDesc2.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc2, NULL, &lightBuffer2);
	if (FAILED(result))
	{
		return false;
	}

	// ---------------------------------depth texture RT-----------------------------------------
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	device->CreateTexture2D(&textureDesc, NULL, &rt);

	//dxDev->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&rt));
	device->CreateRenderTargetView(rt, 0, &rtv);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	shaderResourceViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	device->CreateShaderResourceView(rt, &shaderResourceViewDesc, &rtSV);


	D3D11_TEXTURE2D_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.Width = width;
	dsDesc.Height = height;
	dsDesc.MipLevels = 1;
	dsDesc.ArraySize = 1;
	dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;				// the format of element in array
	dsDesc.SampleDesc.Count = 1;								// MSAA
	dsDesc.SampleDesc.Quality = 0;
	dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;				// this texture is bound as depth and stencil
	dsDesc.Usage = D3D11_USAGE_DEFAULT;							// determine if CPU and GPU can read or write the resource 
	dsDesc.CPUAccessFlags = 0;									// how the CPU access the resource, set with "Usage" method
	dsDesc.MiscFlags = 0;

	if (mDepthStencilBuffer != NULL) mDepthStencilBuffer->Release(), mDepthStencilBuffer = NULL;
	HRESULT hr = device->CreateTexture2D(&dsDesc, NULL, &mDepthStencilBuffer);				// get point to depth buffer

	D3D11_DEPTH_STENCIL_VIEW_DESC dsDSV;
	ZeroMemory(&dsDSV, sizeof(dsDSV));
	dsDSV.Format = dsDesc.Format;
	dsDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsDSV.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(mDepthStencilBuffer, &dsDSV, &dsv);

	ZeroMemory(&depthDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthDesc.DepthEnable = true;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthDesc.StencilEnable = true;
	depthDesc.StencilReadMask = 0xFF;
	depthDesc.StencilWriteMask = 0xFF;

	depthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = device->CreateDepthStencilState(&depthDesc, &depthState);

	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = float(width);
	vp.Height = float(height);
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.0f;

	////-----------------------------------screen RT-----------------------------------------
	//D3D11_TEXTURE2D_DESC textureDesc2;
	//ZeroMemory(&textureDesc2, sizeof(textureDesc2));
	//textureDesc2.Width = (int)dxDev->vp.Width;
	//textureDesc2.Height = (int)dxDev->vp.Height;
	//textureDesc2.MipLevels = 1;
	//textureDesc2.ArraySize = 1;
	//textureDesc2.SampleDesc.Count = 1;
	//textureDesc2.Usage = D3D11_USAGE_DEFAULT;
	//textureDesc2.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	//textureDesc2.CPUAccessFlags = 0;
	//textureDesc2.MiscFlags = 0;

	//textureDesc2.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//device->CreateTexture2D(&textureDesc2, NULL, &screenRt);

	////dxDev->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&rt));
	//device->CreateRenderTargetView(screenRt, 0, &screenRtv);

	//D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc2;

	//shaderResourceViewDesc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//shaderResourceViewDesc2.Texture2D.MostDetailedMip = 0;
	//shaderResourceViewDesc2.Texture2D.MipLevels = 1;

	//shaderResourceViewDesc2.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//device->CreateShaderResourceView(screenRt, &shaderResourceViewDesc2, &screenRtSV);


	//D3D11_TEXTURE2D_DESC dsDesc2;
	//ZeroMemory(&dsDesc2, sizeof(dsDesc2));
	//dsDesc2.Width = (int)dxDev->vp.Width;
	//dsDesc2.Height = (int)dxDev->vp.Height;
	//dsDesc2.MipLevels = 1;
	//dsDesc2.ArraySize = 1;
	//dsDesc2.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;				// the format of element in array
	//dsDesc2.SampleDesc.Count = 1;								// MSAA
	//dsDesc2.SampleDesc.Quality = 0;
	//dsDesc2.BindFlags = D3D11_BIND_DEPTH_STENCIL;				// this texture is bound as depth and stencil
	//dsDesc2.Usage = D3D11_USAGE_DEFAULT;							// determine if CPU and GPU can read or write the resource 
	//dsDesc2.CPUAccessFlags = 0;									// how the CPU access the resource, set with "Usage" method
	//dsDesc2.MiscFlags = 0;

	//if (mscreenDepthStencilBuffer != NULL) mscreenDepthStencilBuffer->Release(), mscreenDepthStencilBuffer = NULL;
	//HRESULT hr = device->CreateTexture2D(&dsDesc2, NULL, &mscreenDepthStencilBuffer);				// get point to depth buffer

	//D3D11_DEPTH_STENCIL_VIEW_DESC dsDSV2;
	//ZeroMemory(&dsDSV2, sizeof(dsDSV2));
	//dsDSV2.Format = dsDesc.Format;
	//dsDSV2.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//dsDSV2.Texture2D.MipSlice = 0;
	//hr = device->CreateDepthStencilView(mscreenDepthStencilBuffer, &dsDSV2, &screenDsv);

	//ZeroMemory(&screenDepthDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	//screenDepthDesc.DepthEnable = true;
	//screenDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//screenDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;

	//screenDepthDesc.StencilEnable = true;
	//screenDepthDesc.StencilReadMask = 0xFF;
	//screenDepthDesc.StencilWriteMask = 0xFF;

	//screenDepthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//screenDepthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	//screenDepthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//screenDepthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//screenDepthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//screenDepthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	//screenDepthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//screenDepthDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//hr = device->CreateDepthStencilState(&screenDepthDesc, &screenDepthState);

	return true;
}

bool ShadowManager::SetShaderParameter(
	ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix,
	ID3D11ShaderResourceView* depthMapTexture, XMVECTOR lightPosition)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* dataPtr;
	LightBufferType2* dataPtr3;


	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	lightViewMatrix = XMMatrixTranspose(lightViewMatrix);
	lightProjectionMatrix = XMMatrixTranspose(lightProjectionMatrix);

	transposeLightMatrices.world = worldMatrix;
	transposeLightMatrices.lightView = lightViewMatrix;
	transposeLightMatrices.lightProjection = lightProjectionMatrix;

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.  
	dataPtr->world = worldMatrix;
	dataPtr->lightView = lightViewMatrix;
	dataPtr->lightProjection = lightProjectionMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &depthMapTexture);

	// Lock the second light constant buffer so it can be written to.
	result = deviceContext->Map(lightBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr3 = (LightBufferType2*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr3->lightPosition = lightPosition;
	//dataPtr3->padding = 0.0f;

	// Unlock the constant buffer.
	deviceContext->Unmap(lightBuffer2, 0);

	// Set the position of the light constant buffer in the vertex shader.
	bufferNumber = 1;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &lightBuffer2);

	return true;
}
bool ShadowManager::DrawShadow(ID3D11DeviceContext* context, 
	BasicManager & basicMng, Unity & unity,
	PrimitiveManager& primitiveMng)
{	
	//load and create vertex buffer
	if (!primitiveMng.LoadUnity(basicMng.dxDevice, basicMng.objManager, unity))
		return false;

	//input vertex buffer
	if (primitiveMng.vertexlayout != NULL)	primitiveMng.vertexlayout->Release();
	if ((*vsBlob) == NULL) return false;

	HRESULT hr = basicMng.dxDevice.device->CreateInputLayout(
		primitiveMng.layout, primitiveMng.numElements, (*vsBlob)->GetBufferPointer(),
		(*vsBlob)->GetBufferSize(), &primitiveMng.vertexlayout);

	context->IASetInputLayout(primitiveMng.vertexlayout);

	UINT stride = sizeof(vertexData);
	UINT offset = 0;
	basicMng.dxDevice.context->IASetVertexBuffers(0, 1, &primitiveMng.vertexBuffer, &stride, &offset);
	basicMng.dxDevice.context->IASetIndexBuffer(primitiveMng.indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	basicMng.dxDevice.context->IASetPrimitiveTopology(primitiveMng.topologyType);

	//set shader
	context->VSSetShader(*vertexShader, NULL, 0);
	context->PSSetShader(*pixelShader, NULL, 0);

	context->PSSetSamplers(0, 1, &sampleStateClamp);

	//draw unity
	for (int i = 0; i < unity.materialNum; i++) {
		basicMng.dxDevice.context->DrawIndexed(basicMng.objManager.DxObjMem[unity.objId]->FaceNumInEachMtl[i] * 3,
			basicMng.objManager.DxObjMem[unity.objId]->beginFaceInEachMtl[i] * 3, 0);
	}
	return true;
}

bool ShadowManager::DrawSceneShadow(
	BasicManager & basicMng, 
	int SceneId, int cameraId, int dlightId, 
	PrimitiveManager& primitiveMng,
	LightManager& lightMng)
{	

	float color[4] = { 0.0f,0.0f,0.0f,1.0f };
	basicMng.dxDevice.context->ClearRenderTargetView(rtv, color);
	basicMng.dxDevice.context->ClearDepthStencilView(
		dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	basicMng.dxDevice.context->OMSetRenderTargets(1, &rtv, dsv);
	basicMng.dxDevice.context->OMSetDepthStencilState(depthState, 1);
;
	basicMng.dxDevice.context->RSSetViewports(1, &vp);

	int unityNum = basicMng.sceneManager.sceneList[SceneId].endUnityId;
	DxScene& scene = basicMng.sceneManager.sceneList[SceneId];
	
	//light buffer
	DxDirLight& light = lightMng.dxDirLights[dlightId];
	Vector forward = Vector(light.Dir.x, light.Dir.y, light.Dir.z);
	Vector up = Vector(light.up.x, light.up.y, light.up.z);
	float size = (scene.bounds.maxPoint - scene.bounds.minPoint).Length() * 0.5;	
	Point lightPos = (scene.bounds.maxPoint + scene.bounds.minPoint) * 0.5 - forward * size;
	//Point lightPos = (scene.bounds.maxPoint + scene.bounds.minPoint) * 0.5 - Vector(0, 1, 0) * size;
	XMFLOAT4X4 lvm = LookAt(lightPos, lightPos + forward, up).m.m;
	//XMFLOAT4X4 lvm = LookAt(lightPos, lightPos + Vector(0, -1, 0), Vector(0, 0, 1)).m.m;
	XMMATRIX lightViewMatrix = XMLoadFloat4x4(&lvm);
	XMMATRIX lightProjectMatrix = XMMatrixOrthographicLH(2 * size, 2 * size, 0, 2 * size);
	XMVECTOR lightPosition = XMVectorSet(lightPos.x, lightPos.y, lightPos.z, 0.0);

	if (unityNum <= 0)	return false;

	for (int i = 0; i < unityNum; i++)
	{
		if (scene.unityList[i].objId == -1)	continue;

		//camera buffer
		XMMATRIX worldMatrix = XMLoadFloat4x4(&scene.unityList[i].wolrdTransform.m.m);

		//set buffer
		if (!SetShaderParameter(basicMng.dxDevice.context, worldMatrix,
			lightViewMatrix,lightProjectMatrix, NULL,lightPosition))
			
			return false;

		if (!DrawShadow(basicMng.dxDevice.context,basicMng, scene.unityList[i] , primitiveMng)) return false;

	}

	return true;
}