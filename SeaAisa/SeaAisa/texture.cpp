#include"texture.h"

bool readTexture(char*bmpName, Texture &tx)
{
	FILE *fp;
	bool asd = fopen_s(&fp, bmpName, "rb");

	if (fp == 0)
		return 0;

	fseek(fp, sizeof(BITMAPFILEHEADER), 0);


	BITMAPINFOHEADER head;

	fread(&head, sizeof(BITMAPINFOHEADER), 1, fp);

	tx.bmpW = head.biWidth;

	tx.bmpH = head.biHeight;

	tx.pBitCount = head.biBitCount;

	tx.lineByte = (tx.bmpW * tx.pBitCount / 8 + 3) / 4 * 4;


	tx.tb = new BYTE[tx.lineByte * tx.bmpH];

	fread(tx.tb, 1, tx.lineByte * tx.bmpH, fp);

	fclose(fp);

	return 1;
}


bool LoadTexture(Texture &tx, char*bmpName) {
	//if(!readTexture(bmpName, tx.bmpW, tx.bmpH, tx.pBitCount, tx.lineByte, &(tx.tb))) return true;
	if (!readTexture(bmpName, tx)) return true;
	else return false;
}

bool Set1DTexture(Texture &tx, float* colorTable, int tableNum) {
	tx.bmpW = tableNum;
	tx.tb = new BYTE[tableNum * 2];
	for (int i = 0; i < 2 * tableNum; i++) {
		tx.tb[i] = int(colorTable[i] * 255);
	}
	return true;
}

float CartoonShade(float &cos, Texture* tx) {
	float x = 0.f;
	for (int i = 0; i < tx->bmpW; i++) {

		if (cos >= ((int)tx->tb[2 * i]) / 255.f)
			x = ((int)tx->tb[2 * i + 1]) / 255.f;

	}
	return x;
}