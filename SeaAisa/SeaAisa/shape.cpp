#include"shape.h"

Box Union(const Box &b, const Point &p) {
	Box ret = b;
	ret.minPoint.x = min(b.minPoint.x, p.x);
	ret.minPoint.y = min(b.minPoint.y, p.y);
	ret.minPoint.z = min(b.minPoint.z, p.z);
	ret.maxPoint.x = max(b.maxPoint.x, p.x);
	ret.maxPoint.y = max(b.maxPoint.y, p.y);
	ret.maxPoint.z = max(b.maxPoint.z, p.z);
	return ret;
}

Box Union(const Box &b, const Box &b2) {
	Box ret;
	ret.minPoint.x = min(b.minPoint.x, b2.minPoint.x);
	ret.minPoint.y = min(b.minPoint.y, b2.minPoint.y);
	ret.minPoint.z = min(b.minPoint.z, b2.minPoint.z);
	ret.maxPoint.x = max(b.maxPoint.x, b2.maxPoint.x);
	ret.maxPoint.y = max(b.maxPoint.y, b2.maxPoint.y);
	ret.maxPoint.z = max(b.maxPoint.z, b2.maxPoint.z);
	return ret;
}

void CutBox(const Camera &camera, Box *ret) {
	if (ret->minPoint.x < 0) ret->minPoint.x = 0;
	if (ret->minPoint.y < 0) ret->minPoint.y = 0;
	if (ret->minPoint.z < 0) ret->minPoint.y = 0;
}

void LoadObj(TriangleMesh &mesh, string fileName) {

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

	int pNum = 0, triNum = 0, nNum = 0, vuNum = 0;
	int j, k, l;
	float a, b, c;
	string as, bs, cs;

	//if (!fl)	return false;

	while (getline(fl, line)) {
		ss.clear();
		ss.str(line);
		ss >> word;
		if ((int)word[0] == 118 && (int)word[1] == NULL) {
			pNum++;
			ss >> a >> b >> c;
			vp.push_back(a);
			vp.push_back(b);
			vp.push_back(c);
			continue;
		}

		if ((int)word[0] == 118 && (int)word[1] == 110) {
			nNum++;
			ss >> a >> b >> c;
			vn.push_back(a);
			vn.push_back(b);
			vn.push_back(c);
			continue;
		}

		if ((int)word[0] == 118 && (int)word[1] == 116) {
			vuNum++;
			a = b = c = 0.f;
			ss >> a >> b >> c;
			vuv.push_back(a);
			vuv.push_back(b);
			vuv.push_back(c);
			continue;
		}

		if ((int)word[0] == 102 && (int)word[1] == NULL) {
			triNum++;
			ss >> as >> bs >> cs;
			FindFace(as, j, k, l);
			vtri.push_back(j);
			vuvi.push_back(k);
			vni.push_back(l);

			FindFace(bs, j, k, l);
			vtri.push_back(j);
			vuvi.push_back(k);
			vni.push_back(l);

			FindFace(cs, j, k, l);
			vtri.push_back(j);
			vuvi.push_back(k);
			vni.push_back(l);
			continue;
		}
	}


	//if (pNum == 0 && triNum == 0)	return false;

	mesh.p = new Point[pNum];
	mesh.n = new Normal[nNum];
	mesh.uv = new float[3 * vuNum];

	mesh.vi = new int[3 * triNum];
	mesh.uvi = new int[3 * triNum];
	mesh.ni = new int[3 * triNum];

	mesh.verNum = pNum;
	mesh.triNum = triNum;
	mesh.nNum = nNum;
	mesh.uvNum = vuNum;

	//Point
	for (int i = 0; i < pNum; i++) {
		mesh.p[i].x = vp[i * 3];
		mesh.p[i].y = vp[i * 3 + 1];
		mesh.p[i].z = vp[i * 3 + 2];
	}
	vp.clear();
	vector<float>().swap(vp);

	//Normal
	for (int i = 0; i < nNum; i++) {
		mesh.n[i].x = vn[i * 3];
		mesh.n[i].y = vn[i * 3 + 1];
		mesh.n[i].z = vn[i * 3 + 2];
	}
	vn.clear();
	vector<float>().swap(vn);

	//normal's indices
	for (int i = 0; i < triNum; i++) {
		mesh.ni[i * 3] = vni[i * 3];
		mesh.ni[i * 3 + 1] = vni[i * 3 + 1];
		mesh.ni[i * 3 + 2] = vni[i * 3 + 2];
	}
	vni.clear();
	vector<int>().swap(vni);

	//triangle's veretx indices
	for (int i = 0; i < triNum; i++) {
		mesh.vi[i * 3] = vtri[i * 3];
		mesh.vi[i * 3 + 1] = vtri[i * 3 + 1];
		mesh.vi[i * 3 + 2] = vtri[i * 3 + 2];
	}
	vtri.clear();
	vector<int>().swap(vtri);

	//texture's position
	for (int i = 0; i < vuNum; i++) {
		mesh.uv[i * 3] = vuv[i * 3];
		mesh.uv[i * 3 + 1] = vuv[i * 3 + 1];
		mesh.uv[i * 3 + 2] = vuv[i * 3 + 2];
	}
	vuv.clear();
	vector<float>().swap(vuv);

	//triangle's vertex texture indices
	for (int i = 0; i < triNum; i++) {
		mesh.uvi[i * 3] = vuvi[i * 3];
		mesh.uvi[i * 3 + 1] = vuvi[i * 3 + 1];
		mesh.uvi[i * 3 + 2] = vuvi[i * 3 + 2];
	}
	vuvi.clear();
	vector<int>().swap(vuvi);


	fl.clear();
	fl.close();
	ss.clear();

}


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

// vertex position line-interpolation
void VertexLineInterp(Vertex *v1, Vertex *v2, float t, Vertex *vt) {
	vt->p.x = (1 - t)*v2->p.x + t*v2->p.x;
	vt->p.y = (1 - t)*v2->p.y + t*v2->p.y;
	vt->p.z = (1 - t)*v2->p.z + t*v2->p.z;
}


//barycentric coordinates in triangle;
float MidPointDistance(int x, int y, const Point &p0, const Point &p1) {
	float f = (p0.y - p1.y)*x + (p1.x - p0.x)*y + p0.x*p1.y - p1.x*p0.y;
	return f;
}

float MidPointDistance(float x, float y, const Point &p0, const Point &p1) {
	double f = (p0.y - p1.y)*x + (p1.x - p0.x)*y + p0.x*p1.y - p1.x*p0.y;
	return f;
}


Point Tri3DBarycentric(const Point &p, const Triangle &tri) {
	Vector na = Cross((tri.vc.p - tri.vb.p), (p - tri.vb.p));
	Vector nb = Cross((tri.va.p - tri.vc.p), (p - tri.vc.p));
	Vector nc = Cross((tri.vb.p - tri.va.p), (p - tri.va.p));

	Vector n = Cross((tri.vb.p - tri.va.p), (tri.vc.p - tri.va.p));

	float a = Dot(n, na) / n.LengthSquared();
	float b = Dot(n, nb) / n.LengthSquared();
	float c = Dot(n, nc) / n.LengthSquared();

	return Point(a, b, c);
}