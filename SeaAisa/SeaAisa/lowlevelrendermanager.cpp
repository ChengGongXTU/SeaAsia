#include "lowlevelrendermanager.h"

bool LowLevelRendermanager::StartUp()
{
	shaderManager.StartUp();
	primitiveManager.StartUp();
	lightManager.StartUp();
	cameraManager.StartUp();

	if (primitiveManager.objId != -1)	return false;

	return true;
}

void LowLevelRendermanager::ShutUp()
{
	shaderManager.ShutUp();
	primitiveManager.ShutUp();
	lightManager.ShutUp();
	cameraManager.ShutUp();
}

bool LowLevelRendermanager::SetScene(DxDevice &dev,DxScene & scene)
{
	if(!cameraManager.LoadCamera(dev, scene.cameraList[0]))	return false;
	if (!lightManager.SetDirLight(dev, scene.dlList[0]))	return false;

	return true;

}

bool LowLevelRendermanager::DrawUnity(BasicManager & basicMng, Unity & unity)
{	

	if (!primitiveManager.LoadUnity(basicMng.dxDevice, basicMng.objManager, unity))	return false;

	if (!primitiveManager.InputVertexBuffer(basicMng.dxDevice, unity, shaderManager))	return false;

	if (unity.textureId != -1)
	{
		basicMng.dxDevice.context->PSSetShaderResources(0, 1, &basicMng.textureManager.texViewPointer[unity.textureId]);
		basicMng.dxDevice.context->PSSetSamplers(0, 1, &basicMng.textureManager.sampleStatePointer[unity.samplerStateId]);
	}



	ID3D11Buffer* materialConstant = NULL;
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MaterialParameter);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HRESULT hr = basicMng.dxDevice.device->CreateBuffer(&bd, NULL, &materialConstant);
	if (FAILED(hr))	return false;

	for (int i = 0; i < unity.materialNum; i++) {

		// update material
		basicMng.dxDevice.context->UpdateSubresource(materialConstant, 0, 0,
			&basicMng.materialsManager.dxMaterial[unity.MaterialsIdIndex[i]].parameter, 0, 0);

		//input material to constant buffer slot 4th
		basicMng.dxDevice.context->PSSetConstantBuffers(4, 1, &materialConstant);

		//draw mesh which belong to this material
		basicMng.dxDevice.context->DrawIndexed(basicMng.objManager.DxObjMem[unity.objId]->FaceNumInEachMtl[i] * 3,
			basicMng.objManager.DxObjMem[unity.objId]->beginFaceInEachMtl[i]*3, 0);
	}

	//basicMng.dxDevice.context->UpdateSubresource(materialConstant, 0, 0,
	//	&basicMng.materialsManager.dxMaterial[unity.MaterialsIdIndex[0]].parameter, 0, 0);
	//basicMng.dxDevice.context->PSSetConstantBuffers(4, 1, &materialConstant);

	//basicMng.dxDevice.context->DrawIndexed(basicMng.objManager.DxObjMem[unity.objId]->faceNum*3, 0, 0);
	
	return true;
}

bool LowLevelRendermanager::DrawSceneUnity(BasicManager & basicMng, int SceneId,int cameraId, int dlightId)
{	
	int unityNum = basicMng.sceneManager.sceneList[SceneId].endUnityId;
	DxScene& scene = basicMng.sceneManager.sceneList[SceneId];

	if (unityNum <= 0)	return false;
	
	for (int i = 0; i < unityNum; i++)
	{
		if (scene.unityList[i].objId == -1)	continue;

		if (!DrawUnity(basicMng, scene.unityList[i])) return false;

	}

	return true;
}

void LowLevelRendermanager::RenderScene(BasicManager & basicMng, WindowsDevice &wnDev,int SceneId)
{	
	DxScene& scene = basicMng.sceneManager.sceneList[SceneId];

	/*ResizeRenderpipeline(basicMng, wnDev);*/

	float color[4] = { 0.5f,0.5f,0.5f,1.0f };
	basicMng.dxDevice.context->ClearRenderTargetView(basicMng.dxDevice.rtv, color);
	basicMng.dxDevice.context->ClearDepthStencilView(basicMng.dxDevice.dsv, D3D11_CLEAR_DEPTH| D3D11_CLEAR_STENCIL, 1.0f,0);
	
	shaderManager.InputVertexShader(basicMng.dxDevice);

	shaderManager.InputPixelShader(basicMng.dxDevice);
	
	if (cameraManager.viewTransformBuffer != NULL)
	{
		cameraManager.LoadCamera(basicMng.dxDevice, scene.cameraList[scene.currentCameraId]);
		cameraManager.InputCamera(basicMng.dxDevice);
	}

	if (lightManager.DirLightBuffer != NULL)
	{	
		lightManager.SetDirLight(basicMng.dxDevice, scene.dlList[scene.currentDlId]);
		DrawSceneUnity(basicMng, SceneId,0, scene.currentDlId);		
	}

}

void LowLevelRendermanager::SetViewPort(BasicManager & basicMng, float x, float y, float w, float h, float mind, float maxd)
{
	basicMng.dxDevice.vp.TopLeftX = x;
	basicMng.dxDevice.vp.TopLeftY = y;
	basicMng.dxDevice.vp.Width = w;
	basicMng.dxDevice.vp.Height = h;
	basicMng.dxDevice.vp.MinDepth = mind;
	basicMng.dxDevice.vp.MaxDepth = maxd;
	basicMng.dxDevice.context->RSSetViewports(1, &basicMng.dxDevice.vp);
}

void LowLevelRendermanager::ResizeRenderpipeline(BasicManager &basicMng, WindowsDevice &wnDev)
{
	RECT rect;
	GetClientRect(wnDev.hwnd, &rect);
	float w = (float)(rect.right - rect.left);
	float h = (float)(rect.bottom - rect.top);

	if (w == NULL)	return;

	if (wnDev.w != w || wnDev.h != h)
	{
		wnDev.w = w;
		wnDev.h = h;
		DxDevice &dxdev = basicMng.dxDevice;		

		// create depth/stencil buffer and view
		// set the parameter for buffer
		D3D11_TEXTURE2D_DESC dsDesc;
		ZeroMemory(&dsDesc, sizeof(dsDesc));
		dsDesc.Width = w;
		dsDesc.Height = h;
		dsDesc.MipLevels = 1;
		dsDesc.ArraySize = 1;
		dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;				// the format of element in array
		dsDesc.SampleDesc.Count = 1;								// MSAA
		dsDesc.SampleDesc.Quality = 0;
		dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;				// this texture is bound as depth and stencil
		dsDesc.Usage = D3D11_USAGE_DEFAULT;							// determine if CPU and GPU can read or write the resource 
		dsDesc.CPUAccessFlags = 0;									// how the CPU access the resource, set with "Usage" method
		dsDesc.MiscFlags = 0;										//Identifies other, less common options for resources.
																	// create buffer and view
		ID3D11Texture2D* mDepthStencilBuffer = 0;
		HRESULT hr = dxdev.device->CreateTexture2D(&dsDesc, NULL, &mDepthStencilBuffer);				// get point to depth buffer

		ID3D11DepthStencilState* depthState = NULL;
		D3D11_DEPTH_STENCIL_DESC depthDesc;
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

		hr = dxdev.device->CreateDepthStencilState(&depthDesc, &depthState);
		dxdev.context->OMSetDepthStencilState(depthState, 1);

		D3D11_DEPTH_STENCIL_VIEW_DESC dsDSV;
		ZeroMemory(&dsDSV, sizeof(dsDSV));
		dsDSV.Format = dsDesc.Format;
		dsDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsDSV.Texture2D.MipSlice = 0;
		hr = dxdev.device->CreateDepthStencilView(mDepthStencilBuffer, &dsDSV, &dxdev.dsv);  //get a view for depth buffer resource
																				 // bind RTV and DSV to Output Merge Stage																						// bind render target and depth/stencil view to output merger stage
		dxdev.context->OMSetRenderTargets(1, &dxdev.rtv, dxdev.dsv);
		mDepthStencilBuffer->Release();

		

		//set the viewport
		//D3D11_VIEWPORT vp;
		dxdev.vp.TopLeftX = 0.0f;
		dxdev.vp.TopLeftY = 0.0f;
		dxdev.vp.Width = float(w);
		dxdev.vp.Height = float(h);
		dxdev.vp.MinDepth = 0.f;
		dxdev.vp.MaxDepth = 1.0f;
		dxdev.context->RSSetViewports(1, &dxdev.vp);
	}
}

void LowLevelRendermanager::ReverseUnityNormal(BasicManager & basicMng, Unity & unity)
{	
	DxObj* obj = basicMng.objManager.DxObjMem[unity.objId];
	int normalNum = obj->vertexNum;

	for (int i = 0; i < normalNum; i++)
	{
		obj->vData[i].Normal.x = -obj->vData[i].Normal.x;
		obj->vData[i].Normal.y = -obj->vData[i].Normal.y;
		obj->vData[i].Normal.z = -obj->vData[i].Normal.z;
	}

	ZeroMemory(&primitiveManager.bd, sizeof(primitiveManager.bd));
	ZeroMemory(&primitiveManager.data, sizeof(primitiveManager.data));

	primitiveManager.bd.Usage = D3D11_USAGE_DEFAULT;
	primitiveManager.bd.ByteWidth = sizeof(vertexData)*(basicMng.objManager.DxObjMem[unity.objId]->vertexNum);
	primitiveManager.bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	primitiveManager.bd.CPUAccessFlags = 0;
	primitiveManager.data.pSysMem = basicMng.objManager.DxObjMem[unity.objId]->vData;
	HRESULT hr =basicMng.dxDevice.device->CreateBuffer(&primitiveManager.bd, &primitiveManager.data, &primitiveManager.vertexBuffer);
}

void LowLevelRendermanager::ReverseUnityNormalZaxis(BasicManager & basicMng, Unity & unity)
{
	DxObj* obj = basicMng.objManager.DxObjMem[unity.objId];
	int normalNum = obj->vertexNum;

	for (int i = 0; i < normalNum; i++)
	{
		obj->vData[i].Normal.z = -obj->vData[i].Normal.z;
	}

	ZeroMemory(&primitiveManager.bd, sizeof(primitiveManager.bd));
	ZeroMemory(&primitiveManager.data, sizeof(primitiveManager.data));

	primitiveManager.bd.Usage = D3D11_USAGE_DEFAULT;
	primitiveManager.bd.ByteWidth = sizeof(vertexData)*(basicMng.objManager.DxObjMem[unity.objId]->vertexNum);
	primitiveManager.bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	primitiveManager.bd.CPUAccessFlags = 0;
	primitiveManager.data.pSysMem = basicMng.objManager.DxObjMem[unity.objId]->vData;
	HRESULT hr = basicMng.dxDevice.device->CreateBuffer(&primitiveManager.bd, &primitiveManager.data, &primitiveManager.vertexBuffer);
}

void LowLevelRendermanager::RenderMaterialChange(BasicManager & basicMng, int materialID, MaterialParameter & para)
{	
	MaterialParameter& currentPara = basicMng.materialsManager.dxMaterial[materialID].parameter;
	currentPara.Ka = para.Ka;
	currentPara.Kd = para.Kd;
	currentPara.Ks = para.Ks;
	currentPara.Ke = para.Ke;
	currentPara.alpha = para.alpha;
	currentPara.illum = para.illum;
	currentPara.Ns = para.Ns;
	currentPara.Ni = para.Ni;

}


bool LowLevelRendermanager::LoadUnityFromObjFile(wstring objName, wstring mtlName, wstring textureName, DxScene &scene, BasicManager &basicMng)
{
	//load mesh 
	if (!basicMng.objManager.LoadTriangelMeshObj(objName)) return false;
	scene.unityList[scene.endUnityId].objId = basicMng.objManager.endObjId - 1;
	for (int i = 0; i < basicMng.objManager.DxObjMem[scene.unityList[scene.endUnityId].objId]->vertexNum; i++)
	{
		basicMng.objManager.DxObjMem[scene.unityList[scene.endUnityId].objId]->vData[i].Pos.z = -basicMng.objManager.DxObjMem[scene.unityList[scene.endUnityId].objId]->vData[i].Pos.z;
		basicMng.objManager.DxObjMem[scene.unityList[scene.endUnityId].objId]->vData[i].Normal.z = -basicMng.objManager.DxObjMem[scene.unityList[scene.endUnityId].objId]->vData[i].Normal.z;
	}


	//load mmaterials
	int current1MtlId = basicMng.materialsManager.endMtlId;
	basicMng.materialsManager.LoadMtlFile(mtlName);
	scene.unityList[scene.endUnityId].materialNum = basicMng.materialsManager.endMtlId - current1MtlId;
	if (scene.unityList[scene.endUnityId].materialNum == 0) return false;
	scene.unityList[scene.endUnityId].MaterialsIdIndex = new int[scene.unityList[scene.endUnityId].materialNum];
	for (int i = 0; i < scene.unityList[scene.endUnityId].materialNum; i++) {
		scene.unityList[scene.endUnityId].MaterialsIdIndex[i] = current1MtlId + i;
	}

	//load texture, no set samplerstate!
	if (!basicMng.textureManager.DxLoadTexture(textureName, basicMng.dxDevice))	return false;
	if (textureName[0] != NULL)
	{
		scene.unityList[scene.endUnityId].textureId = basicMng.textureManager.endTextureId - 1;
		scene.unityList[scene.endUnityId].samplerStateId = basicMng.textureManager.endSamplerStateId;
		basicMng.textureManager.endSamplerStateId++;
	}
	
	scene.currentUnityId = scene.endUnityId;
	scene.endUnityId++;
}
