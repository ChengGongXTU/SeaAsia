#pragma once
#include"SeeAisa.h"
#include"basicmanager.h"
#include"lowlevelrendermanager.h"
#include "PathTracerManager.h"

class PreIntergratManager : PathTracerManager
{
public:
	int envWidth = 1600;
	int envHeight = 800;
	int envMipmap = 5;
	int sqrtNumSamples = 100;


	bool Setup(BasicManager &basicMng, LowLevelRendermanager &renderMng, int mipmapIndex);
	void Bake(BasicManager &basicMng, LowLevelRendermanager &renderMng);
	void UpdateTexture(BasicManager &basicMng, LowLevelRendermanager &renderMng, RTbuffer buffer, int mipmapIndex);
};
