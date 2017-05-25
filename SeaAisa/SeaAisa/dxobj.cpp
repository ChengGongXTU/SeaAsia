#include "dxobj.h"

//void DxTriangleMesh::LoadOBJ(wstring fileName)
//{	
//	ifstream fl(fileName);
//	string line;
//	stringstream ss;
//	string word;
//	vector<float> vp;
//	vp.reserve(50000);
//	vector<float> vn;
//	vn.reserve(50000);
//	vector<int> vni;
//	vni.reserve(50000);
//	vector<int> vtri;
//	vtri.reserve(50000);
//	vector<float> vuv;
//	vuv.reserve(50000);
//	vector<int> vuvi;
//	vuvi.reserve(50000);
//	vector<int> mtli;
//	mtli.reserve(50000);
//	vector<int> beginfaceineachmtl;
//	beginfaceineachmtl.reserve(1000);
//
//	int currMtlId = 0;
//	int faceNumb = 0;
//
//	int pNum = 0, triNum = 0, nNum = 0, vuNum = 0;
//	int j, k, l;
//	float a, b, c;
//	string as, bs, cs;
//
//	//if (!fl)	return false;
//
//	while (getline(fl, line)) {
//		ss.clear();
//		ss.str(line);
//		ss >> word;
//
//		// load vertex position
//		if ((int)word[0] == 118 && (int)word[1] == NULL) {
//			pNum++;
//			ss >> a >> b >> c;
//			vp.push_back(a);
//			vp.push_back(b);
//			vp.push_back(c);
//			continue;
//		}
//
//		//load vertex normal
//		if ((int)word[0] == 118 && (int)word[1] == 110) {
//			nNum++;
//			ss >> a >> b >> c;
//			vn.push_back(a);
//			vn.push_back(b);
//			vn.push_back(c);
//			continue;
//		}
//
//		//load vertex texture
//		if ((int)word[0] == 118 && (int)word[1] == 116) {
//			vuNum++;
//			a = b = c = 0.f;
//			ss >> a >> b >> c;
//			vuv.push_back(a);
//			vuv.push_back(b);
//			vuv.push_back(c);
//			continue;
//		}
//
//		//load vertex, normal and texture indices of each face
//		if((int)word[0] == 117 && (int)word[1] == 115) {
//			ss >> as;
//		//	int id = 0;
//		//	for (int i = 1; as[i] != NULL; i++) {
//		//		id = id * 10 + (int)as[i] - 48;
//		//	}
//		//	currMtlId = id;
//		//	beginfaceineachmtl.push_back(triNum);
//		//	continue;
//
//			currMtlId++;
//			beginfaceineachmtl.push_back(triNum);
//
//		}
//			
//
//		if ((int)word[0] == 102 && (int)word[1] == NULL) {
//			triNum++;
//			faceNumb++;
//			mtli.push_back(currMtlId);
//			ss >> as >> bs >> cs;
//			FindFace(as, j, k, l);
//			vtri.push_back(j);
//			vuvi.push_back(k);
//			vni.push_back(l);
//
//			FindFace(bs, j, k, l);
//			vtri.push_back(j);
//			vuvi.push_back(k);
//			vni.push_back(l);
//
//			FindFace(cs, j, k, l);
//			vtri.push_back(j);
//			vuvi.push_back(k);
//			vni.push_back(l);
//			continue;
//		}
//	}
//
//	materialNum = currMtlId;
//	faceNum = faceNumb;
//
//	beginFaceInEachMtl = new int[materialNum];
//	for (int i = 0; i < materialNum; i++) {
//		beginFaceInEachMtl[i] = beginfaceineachmtl[i];
//	}
//
//	FaceNumInEachMtl = new int[materialNum];
//	for (int i = 0; i < materialNum; i++) {
//		if (i + 1 == materialNum) {
//			FaceNumInEachMtl[i] = triNum - beginFaceInEachMtl[i];
//		}
//		else {
//			FaceNumInEachMtl[i] = beginFaceInEachMtl[i + 1] - beginFaceInEachMtl[i];
//		}
//	}
//
//	faceMaterialIndices = new int[triNum];
//	for (int i = 0; i < triNum; i++) {
//		faceMaterialIndices[i] = mtli[i];
//	}
//
//	if (vuNum > pNum) {
//		vertexNum = vuNum;
//		faceNum = triNum;
//		vData = new vertexData[vuNum];
//		memset(vData, 0, sizeof(vertexData)*vuNum);
//
//		indices = new WORD[triNum * 3];
//		memset(indices, 0, sizeof(WORD)*triNum * 3);
//
//
//		for (int i = 0; i < 3*triNum; i++)
//		{	
//			int vuInd = vuvi[i];
//			indices[i] = vuInd +1;
//			vData[vuInd].Tex = XMFLOAT2(vuv[3*vuInd], vuv[3 * vuInd+1]);
//
//			int vpInd = vtri[i];
//			vData[vuInd].Pos = XMFLOAT3(vp[3 * vpInd], vp[3 * vpInd + 1], vp[3 * vpInd + 2]);
//
//			int vnInd = vni[i] ;
//			vData[vuInd].Normal = XMFLOAT3(vn[3 * vnInd], vn[3 * vnInd + 1], vn[3 * vnInd + 2]);
//		}
//	}
//	
//	if (pNum >= vuNum) {
//		vertexNum = pNum;
//		vData = new vertexData[pNum];
//		memset(vData, 0, sizeof(vertexData)*pNum);
//
//		indices = new WORD[triNum * 3];
//		memset(indices, 0, sizeof(WORD)*triNum * 3);
//
//		for (int i = 0; i < 3*triNum; i++)
//		{	
//			if (i == 361)
//			{
//				int kkk = 0;
//			}
//			int vpInd = vtri[i];
//
//			indices[i] = vpInd;
//
//			vData[vpInd].Pos = XMFLOAT3(vp[3 * vpInd], vp[3 * vpInd + 1], vp[3 * vpInd + 2]);
//
//			if (vuNum == 0) {
//				/*int vuInd = vuvi[i];*/
//				vData[vpInd].Tex = XMFLOAT2(0.f, 0.f);
//			}
//			else {
//				int vuInd = vuvi[i];
//				vData[vpInd].Tex = XMFLOAT2(vuv[3 * vuInd], vuv[3 * vuInd + 1]);
//			}
//
//
//			int vnInd = vni[i];
//			vData[vpInd].Normal = XMFLOAT3(vn[3 * vnInd], vn[3 * vnInd + 1], vn[3 * vnInd + 2]);
//		}
//	}
//
//
//	
//
//	//Point
//	vp.clear();
//	vector<float>().swap(vp);
//	vn.clear();
//	vector<float>().swap(vn);
//	vni.clear();
//	vector<int>().swap(vni);
//	vtri.clear();
//	vector<int>().swap(vtri);
//	vuv.clear();
//	vector<float>().swap(vuv);
//	vuvi.clear();
//	vector<int>().swap(vuvi);
//	mtli.clear();
//	vector<int>().swap(mtli);
//	beginfaceineachmtl.clear();
//	vector<int>().swap(beginfaceineachmtl);
//
//	fl.clear();
//	fl.close();
//	ss.clear();
//
//}

bool FindFace(string &str, int &j, int &k, int &l) {

	int charNum = str.length();
	int begin = 0;
	int end = 0;
	string a;
	string b;
	string c;

	for (int i = 0; i < charNum; i++) {
		if ((int)str[i] == 47) {
			begin = i;
			break;
		}
	}

	for (int i = begin + 1; i < charNum; i++) {
		if ((int)str[i] == 47) {
			end = i;
			break;
		}
	}

	if (begin == 0 && end == 0) {
		return false;
	}

	a = str.substr(0, begin - 0);

	if ((end - begin) != 1) {
		b = str.substr(begin + 1, end - begin - 1);
		k = atoi(b.c_str()) - 1;
	}
	else
		k = 0;

	c = str.substr(end + 1, charNum - end - 1);
	j = atoi(a.c_str()) - 1;
	l = atoi(c.c_str()) - 1;

	return true;
}

void DxTriangleMesh::LoadOBJ(wstring fileName)
{
	ifstream fl(fileName);
	string line;
	stringstream ss;
	string word;
	vector<float> vp;
	vp.reserve(50000);
	vector<float> vn;
	vn.reserve(50000);
	vector<int> vni;
	vni.reserve(50000);
	vector<int> vtri;
	vtri.reserve(50000);
	vector<float> vuv;
	vuv.reserve(50000);
	vector<int> vuvi;
	vuvi.reserve(50000);
	vector<int> mtli;
	mtli.reserve(50000);
	vector<int> beginfaceineachmtl;
	beginfaceineachmtl.reserve(1000);

	int currMtlId = 0;
	int faceNumb = 0;

	int pNum = 0, triNum = 0, nNum = 0, vuNum = 0;
	int j, k, l;
	float a, b, c;
	string as, bs, cs;

	//if (!fl)	return false;

	while (getline(fl, line)) {
		ss.clear();
		ss.str(line);
		ss >> word;

		// load vertex position
		if ((int)word[0] == 118 && (int)word[1] == NULL) {
			pNum++;
			ss >> a >> b >> c;
			vp.push_back(a);
			vp.push_back(b);
			vp.push_back(c);
			continue;
		}

		//load vertex normal
		if ((int)word[0] == 118 && (int)word[1] == 110) {
			nNum++;
			ss >> a >> b >> c;
			vn.push_back(a);
			vn.push_back(b);
			vn.push_back(c);
			continue;
		}

		//load vertex texture
		if ((int)word[0] == 118 && (int)word[1] == 116) {
			vuNum++;
			a = b = c = 0.f;
			ss >> a >> b >> c;
			vuv.push_back(a);
			vuv.push_back(b);
			vuv.push_back(c);
			continue;
		}

		//load vertex, normal and texture indices of each face
		if ((int)word[0] == 117 && (int)word[1] == 115) {
			ss >> as;
			//	int id = 0;
			//	for (int i = 1; as[i] != NULL; i++) {
			//		id = id * 10 + (int)as[i] - 48;
			//	}
			//	currMtlId = id;
			//	beginfaceineachmtl.push_back(triNum);
			//	continue;

			currMtlId++;
			beginfaceineachmtl.push_back(triNum);

		}


		if ((int)word[0] == 102 && (int)word[1] == NULL) {
			triNum++;
			faceNumb++;
			mtli.push_back(currMtlId-1);
			ss >> as >> bs >> cs;
			FindFace(as, j, k, l);
			vtri.push_back(j);
			vuvi.push_back(k);
			vni.push_back(l);

			FindFace(cs, j, k, l);
			vtri.push_back(j);
			vuvi.push_back(k);
			vni.push_back(l);

			FindFace(bs, j, k, l);
			vtri.push_back(j);
			vuvi.push_back(k);
			vni.push_back(l);


			continue;
		}
	}

	materialNum = currMtlId;
	faceNum = faceNumb;

	beginFaceInEachMtl = new int[materialNum];
	for (int i = 0; i < materialNum; i++) {
		beginFaceInEachMtl[i] = beginfaceineachmtl[i];
	}

	FaceNumInEachMtl = new int[materialNum];
	for (int i = 0; i < materialNum; i++) {
		if (i + 1 == materialNum) {
			FaceNumInEachMtl[i] = triNum - beginFaceInEachMtl[i];
		}
		else {
			FaceNumInEachMtl[i] = beginFaceInEachMtl[i + 1] - beginFaceInEachMtl[i];
		}
	}

	faceMaterialIndices = new int[triNum];
	for (int i = 0; i < triNum; i++) {
		faceMaterialIndices[i] = mtli[i];
	}

	//vertex > normal and texture
	if (pNum >= vuNum && pNum >= nNum) 
	{
		vertexNum = pNum;
		vData = new vertexData[pNum];

		memset(vData, 0, sizeof(vertexData)*pNum);

		indices = new WORD[triNum * 3];
		memset(indices, 0, sizeof(WORD)*triNum * 3);

		for (int i = 0; i < 3 * triNum; i++)
		{
			if (i == 361)
			{
				int kkk = 0;
			}
			int vpInd = vtri[i];

			indices[i] = vpInd;

			vData[vpInd].Pos = XMFLOAT3(vp[3 * vpInd], vp[3 * vpInd + 1], vp[3 * vpInd + 2]);

			if (vuNum == 0) {
				/*int vuInd = vuvi[i];*/
				vData[vpInd].Tex = XMFLOAT2(0.f, 0.f);
			}
			else {
				int vuInd = vuvi[i];
				vData[vpInd].Tex = XMFLOAT2(vuv[3 * vuInd], vuv[3 * vuInd + 1]);
			}


			int vnInd = vni[i];
			vData[vpInd].Normal = XMFLOAT3(vn[3 * vnInd], vn[3 * vnInd + 1], vn[3 * vnInd + 2]);
		}
	}



	else if (vuNum > pNum && vuNum>nNum ) {
		vertexNum = vuNum;
		faceNum = triNum;
		vData = new vertexData[vuNum];
		memset(vData, 0, sizeof(vertexData)*vuNum);

		indices = new WORD[triNum * 3];
		memset(indices, 0, sizeof(WORD)*triNum * 3);


		for (int i = 0; i < 3 * triNum; i++)
		{
			int vuInd = vuvi[i];
			indices[i] = vuInd;
			vData[vuInd].Tex = XMFLOAT2(vuv[3 * vuInd], vuv[3 * vuInd + 1]);

			int vpInd = vtri[i];
			vData[vuInd].Pos = XMFLOAT3(vp[3 * vpInd], vp[3 * vpInd + 1], vp[3 * vpInd + 2]);

			int vnInd = vni[i];
			vData[vuInd].Normal = XMFLOAT3(vn[3 * vnInd], vn[3 * vnInd + 1], vn[3 * vnInd + 2]);
		}
	}

	else if (nNum > pNum && nNum > vuNum)
	{
		vertexNum = nNum;
		faceNum = triNum;
		vData = new vertexData[nNum];
		memset(vData, 0, sizeof(vertexData)*nNum);

		indices = new WORD[triNum * 3];
		memset(indices, 0, sizeof(WORD)*triNum * 3);


		for (int i = 0; i < 3 * triNum; i++)
		{
			int vnInd = vni[i];
			indices[i] = vnInd;

			if (vuNum == 0) {
				/*int vuInd = vuvi[i];*/
				vData[vnInd].Tex = XMFLOAT2(0.f, 0.f);
			}
			else
			{
				int vuInd = vuvi[i];
				vData[vnInd].Tex = XMFLOAT2(vuv[3 * vuInd], vuv[3 * vuInd + 1]);
			}

			int vpInd = vtri[i];
			vData[vnInd].Pos = XMFLOAT3(vp[3 * vpInd], vp[3 * vpInd + 1], vp[3 * vpInd + 2]);

			vData[vnInd].Normal = XMFLOAT3(vn[3 * vnInd], vn[3 * vnInd + 1], vn[3 * vnInd + 2]);
		}
	}

	//Point
	vp.clear();
	vector<float>().swap(vp);
	vn.clear();
	vector<float>().swap(vn);
	vni.clear();
	vector<int>().swap(vni);
	vtri.clear();
	vector<int>().swap(vtri);
	vuv.clear();
	vector<float>().swap(vuv);
	vuvi.clear();
	vector<int>().swap(vuvi);
	mtli.clear();
	vector<int>().swap(mtli);
	beginfaceineachmtl.clear();
	vector<int>().swap(beginfaceineachmtl);

	fl.clear();
	fl.close();
	ss.clear();
}

void DxTriangleMesh::SetBoundBox()
{	
	if (vertexNum <= 0) return;
	bbox.minPoint = Point(vData[0].Pos.x, vData[0].Pos.y, vData[0].Pos.z);
	bbox.maxPoint = Point(vData[0].Pos.x, vData[0].Pos.y, vData[0].Pos.z);

	for (int i = 0; i < vertexNum; i++)
	{
		XMFLOAT3 &p = vData[i].Pos;

		bbox.minPoint.x = min(bbox.minPoint.x, p.x);
		bbox.minPoint.y = min(bbox.minPoint.y, p.y);
		bbox.minPoint.z = min(bbox.minPoint.z, p.z);
		bbox.maxPoint.x = max(bbox.maxPoint.x, p.x);
		bbox.maxPoint.y = max(bbox.maxPoint.y, p.y);
		bbox.maxPoint.z = max(bbox.maxPoint.z, p.z);
	}
}
