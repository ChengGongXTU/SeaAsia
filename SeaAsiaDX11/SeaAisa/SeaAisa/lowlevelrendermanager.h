#pragma once
#include"SeeAisa.h"
#include"dxscene.h"
#include"dxobj.h"
#include"basicmanager.h"
#include"shadermanager.h"
#include"lightmanager.h"
#include"primitivemanager.h"
#include"cameramanager.h"
#include"SkyBox.h"
#include"PostProcessManager.h"
#include "ShadowManager.h"

class LowLevelRendermanager {
public:
	ShaderManager shaderManager;
	PrimitiveManager primitiveManager;
	LightManager lightManager;
	CameraManager cameraManager;
	SkyBox skybox;
	PostProcessManager ppMng;
	ShadowManager shadowMng;

	LowLevelRendermanager(){}
	~LowLevelRendermanager(){}

	bool StartUp( BasicManager &basicMng);
	void ShutUp();

	bool LoadUnityFromObjFile(wstring objName, wstring mtlName, wstring textureName,DxScene &scene,
		BasicManager &basicMng,ObjectType type);
	bool LoadUnityFromFBXFile(const char* fbxName, DxScene &scene,BasicManager &basicMng);
	bool SetScene(DxDevice &dev,DxScene &scene);
	bool DrawUnity(BasicManager &basicMng, Unity &unity);
	bool DrawSceneUnity(BasicManager & basicMng, int SceneId, int cameraId, int dlightId);
	bool DeferredDrawGeometry(BasicManager & basicMng, Unity & unity);
	bool DeferredDrawSceneGeometry(BasicManager & basicMng, int SceneId, int cameraId, int dlightId);
	bool DeferredDrawSceneLighting(BasicManager & basicMng, int SceneId, int cameraId, int dlightId);
	void RenderScene(BasicManager &basicMng, WindowsDevice &wnDev, int SceneId);
	void SetViewPort(BasicManager &basicMng, float x, float y, float w, float h, float mind, float maxd);
	void SetDefaultViewPort(BasicManager &basicMng);
	void ResizeRenderpipeline(BasicManager &basicMng,WindowsDevice &wnDev);
	void ReverseUnityNormal(BasicManager &basicMng, Unity &unity);
	void ReverseUnityNormalZaxis(BasicManager &basicMng, Unity &unity);
	void RenderMaterialChange(BasicManager &basicMng, int materialID, MaterialParameter &para,int type);
	void LoadFBXMesh(FbxNode *pNode, DxScene &scene, BasicManager &basicMng, Unity* unity);
	void LoadFBXLight(FbxNode * pNode, DxScene & scene, BasicManager & basicMng, Unity * unity);
	void LoadFbxNode(FbxNode* pNode, Unity* pParentUnity, int childIndex, DxScene &scene, BasicManager &basicMng);
	void DeferredRenderScene(BasicManager &basicMng, WindowsDevice &wnDev, int SceneId);
	
};