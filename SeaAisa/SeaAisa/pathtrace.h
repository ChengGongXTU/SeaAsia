#include"Radiance.h"

bool PathTrace(int nx, int ny, int subNxy,Camera &camera, Triangle *tri,int triNum,int depth,int sampleNum) {

	//transform triangle to cameara coordinate
	for (int i = 0; i < triNum; i++) {
		tri[i].va.p = camera.worldToEye(tri[i].va.p);
		tri[i].vb.p = camera.worldToEye(tri[i].vb.p);
		tri[i].vc.p = camera.worldToEye(tri[i].vc.p);
	}

	//Create a memory for store color
	Ls* c = new Ls[nx*ny];
	float inv = 1.f / sampleNum;
	float dx = (camera.oRight - camera.oLeft) / nx;
	float dy = (camera.oTop - camera.oBottom) / ny;
	float subdx = dx / subNxy;
	float subdy = dy / subNxy;
	float subInv = 1.f / (subNxy*2);

	//compute color at each pixel
	#pragma omp parallel for 
	for (int y = 0; y < ny; y++)
		for (int x = 0; x < nx; x++) {

			int pNum = (h - y - 1)*nx + x;
			Ls subLs;

			for (int sy = 0; sy < subNxy;sy++)
				for (int sx = 0; sx < subNxy; sx++) {

					//create a ray for the pixel
					float ex = camera.oLeft + dx*x + subdx*(sx+ dis(generator));
					float ey = camera.oBottom + dy*y + subdy*(sy+ dis(generator));
					float ez = camera.oNear;
					Point e = Point(ex, ey, ez);
					Vector d = Normalize(e - Point(0.f, 0.f, 0.f));
					Ray r = Ray(e, d, 0, INFINITY, INFINITY);
					r.depth = 0;

					//-------test--------
					
					//sample the color of this pixel  sampleNum times
						for (int k = 0; k < sampleNum; k++)
							subLs = subLs + Radiance(r, depth, tri, triNum, sampleNum)*inv;
				}
			
			c[pNum] = c[pNum] + subLs*subInv;
		}

	FILE* fp = NULL;
	errno_t err = fopen_s(&fp,"RayTraceImage-5dep-8sample.ppm", "w");

	if (err) {
		delete c; return FALSE;
	}

	fprintf(fp, "P3\n%d %d\n%d\n", w, h, 255);

	for (int i = 0; i<w*h; i++)
		fprintf(fp, "%d %d %d ", (int)(min(c[i].r,1.f)*255), (int)(min(c[i].g, 1.f) * 255), (int)(min(c[i].b, 1.f) * 255));

	delete c;

	return true;

}

bool DirectAndIndirectLightPathTrace(int nx, int ny, int subNxy, Camera &camera, Triangle *tri, int triNum, int depth, int sampleNum) {

	//transform triangle to cameara coordinate
	for (int i = 0; i < triNum; i++) {
		tri[i].va.p = camera.worldToEye(tri[i].va.p);
		tri[i].vb.p = camera.worldToEye(tri[i].vb.p);
		tri[i].vc.p = camera.worldToEye(tri[i].vc.p);
	}

	//Create a memory for store color
	Ls* c = new Ls[nx*ny];
	float inv = 1.f / sampleNum;
	float dx = (camera.oRight - camera.oLeft) / nx;
	float dy = (camera.oTop - camera.oBottom) / ny;
	float subdx = dx / subNxy;
	float subdy = dy / subNxy;
	float subInv = 1.f / (subNxy * 2);

	//compute color at each pixel
#pragma omp parallel for num_threads(3)
	for (int y = 0; y < ny; y++)
		for (int x = 0; x < nx; x++) {

			int pNum = (h - y - 1)*nx + x;
			Ls subLs;

			for (int sy = 0; sy < subNxy; sy++)
				for (int sx = 0; sx < subNxy; sx++) {
					//create a ray for the pixel
					float ex = camera.oLeft + dx*x + subdx*(sx + dis(generator));
					float ey = camera.oBottom + dy*y + subdy*(sy + dis(generator));
					float ez = camera.oNear;
					Point e = Point(ex, ey, ez);
					Vector d = Normalize(e - Point(0.f, 0.f, 0.f));
					Ray r = Ray(e, d, 0, INFINITY, INFINITY);
					r.depth = 0;

					//sample the color of this pixel  sampleNum times
					for (int k = 0; k < sampleNum; k++)
						subLs = subLs + OneDirectLight(r, tri, triNum)*inv;
				}

			c[pNum] = c[pNum] + subLs*subInv;
		}

	FILE* fp = NULL;
	errno_t err = fopen_s(&fp, "RayTraceImage-5dep-8sample-directlight2.ppm", "w");

	if (err) {
		delete c; return FALSE;
	}

	fprintf(fp, "P3\n%d %d\n%d\n", w, h, 255);

	for (int i = 0; i<w*h; i++)
		fprintf(fp, "%d %d %d ", (int)(min(c[i].r, 1.f) * 255), (int)(min(c[i].g, 1.f) * 255), (int)(min(c[i].b, 1.f) * 255));

	delete c;

	return true;

}

bool PathTrace(int nx, int ny, int subNxy, Camera &camera, TriangleMesh* mesh,int meshNum,int depth, int sampleNum) {

	//transform triangle to cameara coordinate
	for (int i = 0; i < meshNum; i++) {
		for (int j = 0; j < mesh[i].verNum;j++)
		{
			mesh[i].p[j] = camera.worldToEye(mesh[i].p[j]);
		}
	}

	//Create a memory for store color
	Ls* c = new Ls[nx*ny];
	float inv = 1.f / sampleNum;
	float dx = (camera.oRight - camera.oLeft) / nx;
	float dy = (camera.oTop - camera.oBottom) / ny;
	float subdx = dx / subNxy;
	float subdy = dy / subNxy;
	float subInv = 1.f / (subNxy * 2);

	//compute color at each pixel
#pragma omp parallel for 
	for (int y = 0; y < ny; y++)
		for (int x = 0; x < nx; x++) {

			int pNum = (h - y - 1)*nx + x;
			Ls subLs;

			for (int sy = 0; sy < subNxy; sy++)
				for (int sx = 0; sx < subNxy; sx++) {

					//create a ray for the pixel
					float ex = camera.oLeft + dx*x + subdx*(sx + dis(generator));
					float ey = camera.oBottom + dy*y + subdy*(sy + dis(generator));
					float ez = camera.oNear;
					Point e = Point(ex, ey, ez);
					Vector d = Normalize(e - Point(0.f, 0.f, 0.f));
					Ray r = Ray(e, d, 0, INFINITY, INFINITY);
					r.depth = 0;

					//-------test--------

					//sample the color of this pixel  sampleNum times
					for (int k = 0; k < sampleNum; k++)
						subLs = subLs + OneDirectLight(r, mesh, meshNum)*inv;
				}

			c[pNum] = c[pNum] + subLs*subInv;
		}

	FILE* fp = NULL;
	errno_t err = fopen_s(&fp, "RayTraceImage-5dep-8sample.ppm", "w");

	if (err) {
		delete c; return FALSE;
	}

	fprintf(fp, "P3\n%d %d\n%d\n", w, h, 255);

	for (int i = 0; i<w*h; i++)
		fprintf(fp, "%d %d %d ", (int)(min(c[i].r, 1.f) * 255), (int)(min(c[i].g, 1.f) * 255), (int)(min(c[i].b, 1.f) * 255));

	delete c;

	return true;

}

void setObj(TriangleMesh &tMesh2, RefType type, Vector &v) {
	tMesh2.reftype = type;
	tMesh2.uv = new float[tMesh2.triNum * 3];
	//for (int i = 0; i < tMesh2.uvNum; i++) {
	//	tMesh2.uv[3 * i] = ls.r;
	//	tMesh2.uv[3 * i + 1] = ls.g;
	//	tMesh2.uv[3 * i + 2] = ls.b;
	//}

	for (int i = 0; i < tMesh2.verNum; i++)
	{
		tMesh2.p[i].z = -tMesh2.p[i].z;
		tMesh2.p[i] = tMesh2.p[i] + v;
	}

	for (int i = 0; i < tMesh2.nNum; i++) {
		tMesh2.n[i].z = -tMesh2.n[i].z;
	}
}

void setObj(TriangleMesh &tMesh2, RefType type, Ls &ls, Vector &v) {
	tMesh2.reftype = type;
	for (int i = 0; i < tMesh2.uvNum; i++) {
		tMesh2.kd[0] = ls.r;
		tMesh2.kd[1] = ls.g;
		tMesh2.kd[2] = ls.b;
	}

	for (int i = 0; i < tMesh2.verNum; i++)
	{
		tMesh2.p[i].z = -tMesh2.p[i].z;
		tMesh2.p[i] = tMesh2.p[i] + v;
	}

	for (int i = 0; i < tMesh2.nNum; i++) {
		tMesh2.n[i].z = -tMesh2.n[i].z;
	}
}