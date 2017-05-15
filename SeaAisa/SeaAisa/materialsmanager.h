#pragma once
#include"SeeAisa.h"
#include"dxmaterials.h"




class MaterialsManager {
public:

	int mtlNumber;

	DxMaterials* dxMaterial;

	int endMtlId;
	int currentMtlId;


	MaterialsManager() {}

	~MaterialsManager(){}

	void CreateDxMemory(int mtlNum);

	void StartUp();

	void ShutUp();

	void LoadMtlFile(wstring fileName);
};
