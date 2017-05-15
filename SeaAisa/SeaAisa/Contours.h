#include"light.h"

//midPoint-Line-Draw function for any slop
void DrawLine(float x3, float y3, float x4, float y4, int width) {

	if (x3 > x4) {
		swap(x3, x4);
		swap(y3, y4);
	}
	int x0, y0, x1, y1;

	if (x3 == x4) {
		x0 = round(x3);
		x1 = round(x4);
	}
	else {
		x0 = floor(x3);
		x1 = ceil(x4);
	}

	if (y3 < y4) {
		y0 = floor(y3);
		y1 = ceil(y4);
	}
	else if (y3 == y4) {
		y0 = round(y3);
		y1 = round(y4);
	}
	else {
		y0 = ceil(y3);
		y1 = floor(y4);
	}


	if (x0 == x1) {
		for (int x = x0, y = y0; y <= y1; y++) {
			for (int i = 0; i < width; i++) {
				int num = 4 * w*y + 4 * (x - width / 2 + i);
				pbuffer[num] = (BYTE)0;
				pbuffer[num + 1] = (BYTE)0;
				pbuffer[num + 2] = (BYTE)0;
				pbuffer[num + 3] = (BYTE)0;
			}
		}
	}

	else if (y0 == y1) {
		for (int x = x0, y = y0; x <= x1; x++) {
			for (int j = 0; j < width; j++) {
				int num = 4 * w*(y - width / 2 + j) + 4 * x;
				pbuffer[num] = (BYTE)0;
				pbuffer[num + 1] = (BYTE)0;
				pbuffer[num + 2] = (BYTE)0;
				pbuffer[num + 3] = (BYTE)0;
			}

		}
	}

	else {

		float m = (float)(y1 - y0) / (float)(x1 - x0);

		// m belong to (0,1)
		if (m > 0 && m <= 1) {
			int y = y0;
			float d = MidPointDistance(x0 + 1, y0 + 0.5, Point(x0, y0, 0), Point(x1, y1, 0));

			for (int x = x0; x < x1; x++) {

				for (int j = 0; j < width; j++) {
					int num = 4 * w*(y - width / 2 + j) + 4 * x;
					pbuffer[num] = (BYTE)0;
					pbuffer[num + 1] = (BYTE)0;
					pbuffer[num + 2] = (BYTE)0;
					pbuffer[num + 3] = (BYTE)0;
				}

				if (d < 0) {
					y++;
					d = d + (x1 - x0) + (y0 - y1);
				}
				else
					d = d + (y0 - y1);
			}
		}

		//m belong to (1,++)
		if (m > 1) {
			int x = x0;
			float d = MidPointDistance(x0 + 0.5, y0 + 1, Point(x0, y0, 0), Point(x1, y1, 0));

			for (int y = y0; y <y1; y++) {
				for (int i = 0; i < width; i++) {
					int num = 4 * w*y + 4 * (x - width / 2 + i);
					pbuffer[num] = (BYTE)0;
					pbuffer[num + 1] = (BYTE)0;
					pbuffer[num + 2] = (BYTE)0;
					pbuffer[num + 3] = (BYTE)0;
				}

				if (d > 0) {
					x++;
					d = d + (x1 - x0) + (y0 - y1);
				}
				else
					d = d + (x1 - x0);
			}
		}

		if (m < 0 && m >= -1) {
			int y = y0;
			float d = MidPointDistance(x0 + 1, y0 - 0.5, Point(x0, y0, 0), Point(x1, y1, 0));

			for (int x = x0; x <x1; x++) {
				for (int j = 0; j < width; j++) {
					int num = 4 * w*(y - width / 2 + j) + 4 * x;
					pbuffer[num] = (BYTE)0;
					pbuffer[num + 1] = (BYTE)0;
					pbuffer[num + 2] = (BYTE)0;
					pbuffer[num + 3] = (BYTE)0;
				}

				if (d >0) {
					y--;
					d = d + (x0 - x1) + (y0 - y1);
				}
				else
					d = d + (y0 - y1);
			}
		}

		if (m < -1) {
			int x = x0;
			float d = MidPointDistance(x0 + 0.5, y0 - 1, Point(x0, y0, 0), Point(x1, y1, 0));

			for (int y = y0; y >y1; y--) {
				for (int i = 0; i < width; i++) {
					int num = 4 * w*y + 4 * (x - width / 2 + i);
					pbuffer[num] = (BYTE)0;
					pbuffer[num + 1] = (BYTE)0;
					pbuffer[num + 2] = (BYTE)0;
					pbuffer[num + 3] = (BYTE)0;
				}

				if (d <= 0) {
					x++;
					d = d + (x0 - x1) + (y0 - y1);
				}
				else
					d = d + (x0 - x1);
			}

		}
	}

}

// detect the share egde of two triangles. 
bool FindShareEdge(TriangleMesh &mesh, int triId1, int triId2, int &p1, int &p2) {
	int &p11 = mesh.vi[triId1 * 3];
	int &p12 = mesh.vi[triId1 * 3 + 1];
	int &p13 = mesh.vi[triId1 * 3 + 2];
	int &p21 = mesh.vi[triId2 * 3];
	int &p22 = mesh.vi[triId2 * 3 + 1];
	int &p23 = mesh.vi[triId2 * 3 + 2];

	int shareNum = 0;

	if (p11 == p21 || p11 == p22 || p11 == p23) {
		p1 = p11;
		if (p12 == p21 || p12 == p22 || p12 == p23) {
			p2 = p12;
			return true;
		}
		else if (p13 == p21 || p13 == p22 || p13 == p23) {
			p2 = p13;
			return true;
		}
		else
			return false;
	}

	else if (p12 == p21 || p12 == p22 || p12 == p23) {
		p1 = p12;
		if (p13 == p21 || p13 == p22 || p13 == p23) {
			p2 = p13;
			return true;
		}
		else
			return false;
	}

	else
		return false;

}


// find the front and back triangles, and record their ID in diferent list
void FrontFaceDetect(TriangleMesh &mesh, Camera &camera, int *&front, int &frontNumber, int *&back, int &backNumber) {
	vector<int> frontface;
	vector<int> backface;

	int frontNum = 0;
	int backNum = 0;

	for (int i = 0; i < mesh.triNum; i++) {
		//Normal triN = Normalize(mesh.n[mesh.ni[i * 3]] + mesh.n[mesh.ni[i * 3 + 1]] + mesh.n[mesh.ni[i * 3 + 2]]);
		Vector triN = Normalize(Cross(mesh.p[mesh.vi[3 * i + 1]] - mesh.p[mesh.vi[3 * i]], mesh.p[mesh.vi[3 * i + 2]] - mesh.p[mesh.vi[3 * i]]));
		Vector eyeV = Normalize(camera.eye - mesh.p[mesh.vi[3 * i]]);
		float flag = Dot(triN, eyeV);

		if (flag > 0) {
			frontNum++;
			frontface.push_back(i);
		}
		else
		{
			backNum++;
			backface.push_back(i);
		}
	}


	front = new int[frontNum];
	frontNumber = frontNum;
	for (int i = 0; i < frontNum; i++)
	{
		front[i] = frontface[i];
	}
	frontface.clear();

	back = new int[backNum];
	backNumber = backNum;
	for (int i = 0; i < backNum; i++)
	{
		back[i] = backface[i];
	}
	backface.clear();
}


//note! not complete!
void RandomSilhouetteDetect(TriangleMesh &mesh, Camera &camera, int *front, int &frontNumber, int *back, int &backNumber) {

	vector<int> SilP;
	int SilPN = 0;
	int findEgde = 0;

	//find first egde
	while (findEgde != 1) {
		float random = dis(generator);
		int frontId = front[(int)(random*(frontNumber - 1))];

		for (int i = 0; i < backNumber; i++) {
			int pStart, pEnd;
			int backId = back[i];
			if (FindShareEdge(mesh, frontId, backId, pStart, pEnd)) {
				SilPN++;
				SilP.push_back(pStart);
				SilP.push_back(pEnd);
				findEgde = 1;
			}
			else
				continue;
		}
	}

	//find nearest 
}


// search silhouette edges between frontace and backface,and put edges into edgelist.
void BruteForceSilhouetteDetect(TriangleMesh &mesh, Camera &camera, int *&front, int &frontNumber, int *&back, int &backNumber, int *&edgeList, int &edgeNum) {

	vector<int> SilP;
	int SilPN = 0;

	//find share egde
	for (int j = 0; j < frontNumber; j++) {
		int frontId = front[j];

		for (int i = 0; i < backNumber; i++) {
			int pStart, pEnd;
			int backId = back[i];
			if (FindShareEdge(mesh, frontId, backId, pStart, pEnd)) {
				SilPN++;
				SilP.push_back(pStart);
				SilP.push_back(pEnd);
			}
			else
				continue;
		}
	}

	edgeNum = SilPN;
	edgeList = new int[SilPN * 2];
	for (int k = 0; k < SilPN; k++) {
		edgeList[2 * k] = SilP[2 * k];
		edgeList[2 * k + 1] = SilP[2 * k + 1];
	}
	SilP.clear();

}

//draw edges of list;
void DrawSilhouette(TriangleMesh &mesh, Camera &camera, int *edgeList, int &edgeNum, BYTE* pbuffer, int width) {

	//tranform vertex position
	Point *pt = new Point[mesh.verNum];
	for (int i = 0; i < mesh.verNum; i++)
	{
		Point p = mesh.p[i];
		p = camera.worldToEye(p);
		p = camera.eyeToPer(p);
		p = camera.proToView(p);
		pt[i] = p;
	}

	for (int i = 0; i < edgeNum; i++) {
		float x0 = pt[edgeList[2 * i]].x;
		float y0 = pt[edgeList[2 * i]].y;
		float x1 = pt[edgeList[2 * i + 1]].x;
		float y1 = pt[edgeList[2 * i + 1]].y;

		DrawLine(x0, y0, x1, y1, width);
	}

}


//Fast High-Quality Line visibility testing and draw by <Cole and Finkelstein 2009>
void ProjectAndClipSegmentTable(int *&edgeList, int &edgeNum, Camera &camera, TriangleMesh &mesh, int k, float *&segmentTable) {

	//input: 6-channel framebuffer packed with 3D coordinate of the endpoint per segment(p,q)
	//there we instead of endpoints' ID "edgeList" and "mesh"'s points.

	//output: 9-channel buffer, includes two endpoints' coordinates and the number of visibility sample l;
	segmentTable = new float[7 * edgeNum ];

	//1st, project each endpoint (vertex in 'mesh').
	Point *pArr = new Point[mesh.verNum];
	for (int i = 0; i < mesh.verNum; i++)
	{
		Point p = mesh.p[i];
		p = camera.worldToEye(p);
		p = camera.eyeToPer(p);
		pArr[i] = p;
	}

	// clip each segment in edgeList
	float l;										//samples
	for (int i = 0; i < edgeNum; i++) {

		// find endpoint's position
		int end1 = edgeList[2 * i];
		int end2 = edgeList[2 * i+1];
		Point endPoint1 = pArr[end1];
		Point endPoint2 = pArr[end2];

		//check endpoint and clip segment
		int flag = LineCull(endPoint1, endPoint2);
		
		//project point into screen coordinate
		endPoint1 = camera.proToView(endPoint1);
		endPoint2 = camera.proToView(endPoint2);

		// compute visibility samples
		if (flag == 2) {
			l = 0;
			segmentTable[i * 7 ]   = endPoint1.x;
			segmentTable[i * 7 +1] = endPoint1.y;
			segmentTable[i * 7 +2] = endPoint1.z;
			segmentTable[i * 7 +3] = endPoint2.x;
			segmentTable[i * 7 +4] = endPoint2.y;
			segmentTable[i * 7 +5] = endPoint2.z;
			segmentTable[i * 7 +6] = l;
		}
		else {
			float length = (endPoint2 - endPoint1).Length();
			l = length / k;

			if (l < 1) {
				l = 0;
				segmentTable[i * 7] = endPoint1.x;
				segmentTable[i * 7 + 1] = endPoint1.y;
				segmentTable[i * 7 + 2] = endPoint1.z;
				segmentTable[i * 7 + 3] = endPoint2.x;
				segmentTable[i * 7 + 4] = endPoint2.y;
				segmentTable[i * 7 + 5] = endPoint2.z;
				segmentTable[i * 7 + 6] = l;
			}
			else {
				segmentTable[i * 7] = endPoint1.x;
				segmentTable[i * 7 + 1] = endPoint1.y;
				segmentTable[i * 7 + 2] = endPoint1.z;
				segmentTable[i * 7 + 3] = endPoint2.x;
				segmentTable[i * 7 + 4] = endPoint2.y;
				segmentTable[i * 7 + 5] = endPoint2.z;
				segmentTable[i * 7 + 6] = l;
			}
			
		}
	}
}

void SegmentAtlasCreation(int &edgeNum, float *&segmentTable,float *&segmentAtla, int &samples) {

	//compute the sum of each edge samples
	int sampleNum = 0;
	for (int i = 0; i < edgeNum; i++) {
		sampleNum =sampleNum + (int)segmentTable[i * 7 + 6];
	}
	segmentAtla = new float[sampleNum * 3];
	samples = sampleNum;

	int num = 0;
	for (int i = 0; i < edgeNum; i++) {

		if (segmentTable[i * 7 + 6] == 0.f)
			continue;

		for (int j = 0; j < (int)segmentTable[i * 7 + 6]; j++) {
			
			float t = j / (segmentTable[i * 7 + 6]-1);

			segmentAtla[num] = segmentTable[i * 7] + (segmentTable[i * 7 + 3] - segmentTable[i * 7])*t;
			num++;
			segmentAtla[num] = segmentTable[i * 7+1] + (segmentTable[i * 7 + 4] - segmentTable[i * 7+1])*t;
			num++;
			segmentAtla[num] = segmentTable[i * 7+2] + (segmentTable[i * 7 + 5] - segmentTable[i * 7+2])*t;
			num++;
		}
	}
}

void VisibilityTestAndDrawLine(BYTE *&pbuffer,float *&zbuffer,float *&segmentAtla, int &samples,float* &segmentTable, int &edgeNum) {

	for (int i = 0; i < samples; i++) {

				int j = i + 1;
				Point p1 = Point(segmentAtla[i * 3], segmentAtla[i * 3 + 1], segmentAtla[i * 3 + 2]);
				//Point p2 = Point(segmentAtla[j * 3], segmentAtla[j * 3 + 1], segmentAtla[j * 3 + 2]);

				float depth1 = zbuffer[w*(int)floor(p1.y) + (int)floor(p1.x)];
				float depth2 = zbuffer[w*(int)ceil(p1.y) + (int)ceil(p1.x)];
				float depth3 = zbuffer[w*(int)floor(p1.y) + (int)ceil(p1.x)];
				float depth4 = zbuffer[w*(int)ceil(p1.y) + (int)floor(p1.x)];
				//float depth3 = zbuffer[w*(int)floor(p2.y) + (int)floor(p2.x)];
				//float depth4 = zbuffer[w*(int)ceil(p2.y) + (int)ceil(p2.x)];

				//if ((p1.z>=depth1 || p1.z>=depth2) && (p2.z>=depth3 || p2.z>=depth4)) {
					//DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, 4);
				if (p1.z >= depth1 || p1.z >= depth2 || p1.z >= depth3|| p1.z >= depth4) {
					for (int a = 0; a < 3; a++)
						for (int b = 0; b < 3; b++) {
						int num = 4 * w*((int)p1.y-1+a) + 4 * ((int)p1.x-1+b);
						pbuffer[num] = (BYTE)25*abs(b-1);
						pbuffer[num + 1] = (BYTE)25 * abs(b - 1);
						pbuffer[num + 2] = (BYTE)25 * abs(b - 1);
						pbuffer[num + 3] = (BYTE)0;
						}		
				}
				else
					continue;
			}

}

