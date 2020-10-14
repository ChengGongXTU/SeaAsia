#pragma once
#include"SeeAisa.h"
#include"basicmanager.h"
#include"lowlevelrendermanager.h"
#include "PathTracerManager.h"

enum NgptDenoiseType
{
	None,
	GraAndELoss,
	ELoss,
	GraAndL1Loss,
	L1Loss
};

class GradientPathTracer : PathTracerManager
{
public:
	int rrBeginDepth = 4;
	int sqrtNumSamples = 10;
	float sceneEpsilon = 1.e-3f;
	int camWidth = 1280;
	int camHeight = 720;
	int loopCount = 12;

	NgptDenoiseType denoiseType = None;

	bool Setup(BasicManager &basicMng, LowLevelRendermanager &renderMng);
	void Bake(BasicManager &basicMng, LowLevelRendermanager &renderMng);
};