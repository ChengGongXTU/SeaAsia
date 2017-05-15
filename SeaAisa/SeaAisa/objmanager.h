#pragma once
#include"SeeAisa.h"
#include"dxobj.h"


class ObjManager {
public:

	int ObjNumber;

	// mmemory for DxObj's pointers
	DxObj** DxObjMem;

	int endObjId;
	int currentObjId;

	ObjManager(){}

	~ObjManager(){}

	void CreateDxMemory(int ObjNum);

	void StartUp();

	void ShutUp();

	bool LoadTriangelMeshObj(wstring fileName);

	friend class SceneManager;
};
