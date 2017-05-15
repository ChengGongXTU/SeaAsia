#include"pathtrace.h"
void setObj(TriangleMesh &tMesh2, RefType type, Ls &ls, Vector &v) {
	tMesh2.reftype = type;
	tMesh2.uv = new float[tMesh2.triNum * 3];
	for (int i = 0; i < tMesh2.triNum; i++) {
		tMesh2.uv[3 * i] = ls.r;
		tMesh2.uv[3 * i + 1] = ls.g;
		tMesh2.uv[3 * i + 2] = ls.b;
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
