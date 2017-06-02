#include "materialsmanager.h"

void MaterialsManager::CreateDxMemory(int mtlNum)
{
	endMtlId = 0;
	currentMtlId = 0;
	dxMaterial = new DxMaterials[mtlNum];
	for (int i = 0; i < mtlNum; i++)
	{
		dxMaterial[i].mtlType = matte;
		dxMaterial[i].parameter.Ka = XMFLOAT4(0, 0, 0, 1);
		dxMaterial[i].parameter.Kd = XMFLOAT4(0, 0, 0, 1);
		dxMaterial[i].parameter.Ks = XMFLOAT4(0, 0, 0, 1);
		dxMaterial[i].parameter.Ke = XMFLOAT4(0, 0, 0, 1);
		dxMaterial[i].parameter.alpha = 1.f;
		dxMaterial[i].parameter.illum = 0.f;
		dxMaterial[i].parameter.Ni = 0.f;
		dxMaterial[i].parameter.Ns = 0;
	}
}

void MaterialsManager::StartUp()
{
	CreateDxMemory(500);
}

void MaterialsManager::ShutUp()
{
	delete dxMaterial;
	endMtlId = 0;
	currentMtlId = 0;
}

void MaterialsManager::LoadMtlFile(wstring fileName)
{
	ifstream fl(fileName);
	string line ;
	stringstream ss ;
	string word ;

	int currMtlId = endMtlId;
	int j =0, k =0, l = 0;
	float a = 0.f, b = 0.f, c = 0.f;
	string as , bs , cs ;
	
	//if (!fl)	return false;
	while (getline(fl, line)) {
		ss.clear();
		ss.str(line);
		ss >> word;


		//load material id
		if ((int)word[0] == 110 && (int)word[1] == 101) {
			mtlNumber++;
			endMtlId++;
			continue;
		}

		//load Ka
		if ((int)word[0] == 75 && (int)word[1] == 97) {
			a = b = c = 0.f;
			ss >> a >> b >> c;
			dxMaterial[endMtlId-1].parameter.Ka = XMFLOAT4(a, b, c,1);
			continue;
		}

		//load Kd
		if ((int)word[0] == 75 && (int)word[1] == 100) {
			a = b = c = 0.f;
			ss >> a >> b >> c;
			dxMaterial[endMtlId - 1].parameter.Kd = XMFLOAT4(a, b, c,1);
			continue;
		}

		//load Ks
		if ((int)word[0] == 75 && (int)word[1] == 115) {
			a = b = c = 0.f;
			ss >> a >> b >> c;
			dxMaterial[endMtlId - 1].parameter.Ks = XMFLOAT4(a, b, c,1);
			continue;
		}


		//load Ke
		if ((int)word[0] == 75 && (int)word[1] == 101) {
			a = b = c = 0.f;
			ss >> a >> b >> c;
			dxMaterial[endMtlId - 1].parameter.Ke = XMFLOAT4(a, b, c,1);
			continue;
		}

		//load Ns
		if ((int)word[0] == 78 && (int)word[1] == 115) {
			j = 0;
			ss >> j;
			dxMaterial[endMtlId - 1].parameter.Ns = j;
			continue;
		}

		//load alpha
		if ((int)word[0] == 100 ) {
			a = 0.f;
			ss >> a;
			dxMaterial[endMtlId - 1].parameter.alpha = a;
			continue;
		}

		if ((int)word[0] == 105) {
			a = 0.f;
			ss >> a;
			dxMaterial[endMtlId - 1].parameter.illum = a;
			continue;
		}

	}

}
