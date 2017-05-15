#include"dxscene.h"

void DxScene::setUp(int uniNum, int camNum, int plNum, int dlNum) {
	
	endUnityId = 0;
	currentUnityId = -1;
	maxUnityNum = uniNum;

	endCameraId = 0;
	currentCameraId = -1;
	maxCameraNum = camNum;

	endPlId = 0;
	currentPlId = -1;
	maxPlNum = plNum;

	endDlId = 0;
	currentDlId = -1;
	maxDlNum = dlNum;

	unityList = new Unity[uniNum];
	for (int i = 0; i < uniNum; i++)
	{
		unityList[i].objId = -1;
		unityList[i].materialNum = 0;
		unityList[i].samplerStateId = -1;
		unityList[i].textureId = -1;
		unityList[i].MaterialsIdIndex = NULL;
		unityList[i].Pos = XMFLOAT3(0.f, 0.f, 0.f);
		unityList[i].transform = XMMatrixIdentity();
	}
	cameraList = new DxCamera[camNum];
	for (int i = 0; i < maxCameraNum; i++)
	{
		cameraList[i].eye = XMVectorSet(0, 0, 0,0.f);
		cameraList[i].at = XMVectorSet(0, 0, 0, 0.f);
		cameraList[i].up = XMVectorSet(0, 0, 0, 0.f);
		cameraList[i].aspect = 0.f;
		cameraList[i].cNear = 0.f;
		cameraList[i].cFar = 0.f;
		cameraList[i].cWidth = 1.f;
		cameraList[i].cHeight = 1.f;
		cameraList[i].mView = XMMatrixIdentity();
		cameraList[i].mProjection = XMMatrixIdentity();

	}

	plList = new DxPointLight[plNum];
	for (int i = 0; i < maxPlNum; i++)
	{
		plList[i].Pos = XMFLOAT4(0.f, 0.f, 0.f,1.f);
		plList[i].Color = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	}
	dlList = new DxDirLight[dlNum];
	for (int i = 0; i < maxPlNum; i++)
	{
		dlList[i].Dir = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		dlList[i].Color = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	}
}



//bool unity::LoadObjFile(wstring obj, wstring mtl)
//{
//	
//}
