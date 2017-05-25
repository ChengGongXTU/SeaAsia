#pragma once
#include"SeeAisa.h"
#include"dxscene.h"
#include"dxobj.h"
#include"basicmanager.h"
#include"shadermanager.h"
#include"lightmanager.h"
#include"primitivemanager.h"
#include"cameramanager.h"

class LowLevelRendermanager {
public:
	ShaderManager shaderManager;
	PrimitiveManager primitiveManager;
	LightManager lightManager;
	CameraManager cameraManager;
	

	LowLevelRendermanager(){}
	~LowLevelRendermanager(){}

	bool StartUp();
	void ShutUp();

	bool LoadUnityFromObjFile(wstring objName, wstring mtlName, wstring textureName,DxScene &scene,
		BasicManager &basicMng,ObjectType type);
	bool SetScene(DxDevice &dev,DxScene &scene);
	bool DrawUnity(BasicManager &basicMng, Unity &unity);
	bool DrawSceneUnity(BasicManager & basicMng, int SceneId, int cameraId, int dlightId);
	void RenderScene(BasicManager &basicMng, WindowsDevice &wnDev, int SceneId);
	void SetViewPort(BasicManager &basicMng, float x, float y, float w, float h, float mind, float maxd);
	void ResizeRenderpipeline(BasicManager &basicMng,WindowsDevice &wnDev);
	void ReverseUnityNormal(BasicManager &basicMng, Unity &unity);
	void ReverseUnityNormalZaxis(BasicManager &basicMng, Unity &unity);
	void RenderMaterialChange(BasicManager &basicMng, int materialID, MaterialParameter &para);
};