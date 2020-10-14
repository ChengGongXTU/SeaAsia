#include"objmanager.h"

void ObjManager::CreateDxMemory(int ObjNum) {
	DxObjMem = new DxObj*[ObjNum];
}

void ObjManager::StartUp() {
	ObjNumber = 0;
	endObjId = 0;
	currentObjId = 0;
	CreateDxMemory(10000);
}

void ObjManager::ShutUp() {
	delete DxObjMem;
}

bool ObjManager::LoadTriangelMeshObj(wstring fileName)
{	
	
	
	DxObjMem[endObjId] = new DxTriangleMesh;
	DxObjMem[endObjId]->vertexNum = 0;
	DxObjMem[endObjId]->faceNum = 0;
	DxObjMem[endObjId]->vData = NULL;
	DxObjMem[endObjId]->indices = NULL;

	DxObjMem[endObjId]->LoadOBJ(fileName);
	DxObjMem[endObjId]->objId = endObjId;

	if (DxObjMem[endObjId]->vertexNum == 0) {
		delete DxObjMem[endObjId];
		return false;
	}

	endObjId++;
	ObjNumber++;
	return true;	
}
