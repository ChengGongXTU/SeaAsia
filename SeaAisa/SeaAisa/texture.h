#pragma once

#include"SeeAisa.h"

class Texture{

public:	
	//texture buffer
	BYTE *tb;

	int bmpW;
	int bmpH;
	int pBitCount;
	int lineByte;

	Texture() {
		bmpW = bmpH = pBitCount = lineByte = 0;
		tb = NULL;
	}

	friend bool readTexture(char*bmpName, Texture &tx);
	friend bool LoadTexture(Texture &tx, char*bmpName);
	friend bool Set1DTexture(Texture &tx, float* colorTable, int tableNum);
	friend float CartoonShade(float &cos, Texture* tx);
};
