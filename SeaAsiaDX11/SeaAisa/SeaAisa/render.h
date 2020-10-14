#include"Contours.h"

class DIBColor {
public:

	unsigned int  rgb[3];
	float alpha;

	DIBColor() { rgb[0] = rgb[1] = rgb[2] = 0,alpha = 1.f; }
};

Vertex VertexTransform(const Vertex &v,const Camera &camera) {
	Vertex newV;
	newV = v;
	newV.p = camera.worldToEye(v.p);
	newV.p = camera.eyeToPer(newV.p);
	newV.p = camera.proToView(newV.p);
	return newV;
}

Triangle TriangleTransform(const Triangle &tri,const Camera &camera) {
	Triangle newTri = tri;
	newTri.va = VertexTransform(tri.va,camera);
	newTri.vb = VertexTransform(tri.vb, camera);
	newTri.vc = VertexTransform(tri.vc, camera);
	newTri.n = Cross((newTri.va.p - newTri.vb.p), (newTri.va.p - newTri.vc.p));
	newTri.n = Normalize(newTri.n);
	return newTri;
}


void BresenhamAlgorithm(HDC hdc, const Triangle &tri,const PointLight &pl,const Point &eye) {

	Box b = Box(tri.va.p, tri.vb.p);
	Box c = Union(b, tri.vc.p);
	Vertex vt;

	int x0, y0, x1, y1;
	x0 = floor(c.minPoint.x);
	y0 = floor(c.minPoint.y);
	x1 = ceil(c.maxPoint.x);
	y1 = ceil(c.maxPoint.y);

	for (int i = x0; i < x1; i++)
		for (int j = y0; j < y1; j++) {
			float a = MidPointDistance(i, j, tri.vb.p, tri.vc.p) / MidPointDistance(tri.va.p.x, tri.va.p.y, tri.vb.p, tri.vc.p);
			float b = MidPointDistance(i, j, tri.vc.p, tri.va.p) / MidPointDistance(tri.vb.p.x, tri.vb.p.y, tri.vc.p, tri.va.p);
			float c = MidPointDistance(i, j, tri.va.p, tri.vb.p) / MidPointDistance(tri.vc.p.x, tri.vc.p.y, tri.va.p, tri.vb.p);

			if (a > 0 && b > 0 && c > 0) {
				Point p = a*tri.va.p + b*tri.vb.p + c*tri.vc.p;
					
				if (p.z > zbuffer[i*w + j] && p.z<1.f&&p.z>-1.f) {
					zbuffer[i*w + j] = p.z;					
					float rp = a*tri.va.r + b*tri.vb.r + c*tri.vc.r;
					float gp = a*tri.va.g + b*tri.vb.g + c*tri.vc.g;
					float bp = a*tri.va.b + b*tri.vb.b + c*tri.vc.b;
					float p1 = a*tri.va.pr + b*tri.vb.pr + c*tri.vc.pr;
					float p2 = a*tri.va.pg + b*tri.vb.pg + c*tri.vc.pg;
					float p3 = a*tri.va.pb + b*tri.vb.pb + c*tri.vc.pb;

					vt = Vertex(p.x, p.y, p.z, rp, gp, bp,p1,p2,p3);
					vt.n = tri.n;
					if (Dot(vt.n, vt.p-eye) > 0) vt.n = -vt.n;

					LightCompute(&vt, pl, eye);

					SetPixel(hdc, i, h-j, RGB(vt.r,vt.g,vt.b));
				}					
			}				
		}

}

void RenderPipeline(Triangle &t, Camera &camera, PointLight &pl) {

	Triangle tri = TriangleTransform(t, camera);

	Box b = Box(tri.va.p, tri.vb.p);
	Box c = Union(b, tri.vc.p);
	Vertex vt;

	if((c.minPoint.x<0&&c.maxPoint.x<0)||(c.maxPoint.y<0&&c.minPoint.y<0)){}
	if ((c.minPoint.x>w && c.maxPoint.x>w) || (c.maxPoint.y>h && c.minPoint.y>h)){}


	int x0, y0, x1, y1;
	x0 = floor(c.minPoint.x);
	x0 = max(0, x0);
	y0 = floor(c.minPoint.y);
	y0 = max(0, y0);
	x1 = ceil(c.maxPoint.x);
	x1 = min(w, x1);
	y1 = ceil(c.maxPoint.y);
	y1 = min(h, y1);

	for (int i = x0; i < x1; i++)
		for (int j = y0; j < y1; j++) {
			
			float a = MidPointDistance(i, j, tri.vb.p, tri.vc.p) / MidPointDistance(tri.va.p.x, tri.va.p.y, tri.vb.p, tri.vc.p);
			float b = MidPointDistance(i, j, tri.vc.p, tri.va.p) / MidPointDistance(tri.vb.p.x, tri.vb.p.y, tri.vc.p, tri.va.p);
			float c = MidPointDistance(i, j, tri.va.p, tri.vb.p) / MidPointDistance(tri.vc.p.x, tri.vc.p.y, tri.va.p, tri.vb.p);

			if (a > 0 && b > 0 && c > 0) {
				Point p = a*t.va.p + b*t.vb.p + c*t.vc.p;
				float z = a*tri.va.p.z + b*tri.vb.p.z + c*tri.vc.p.z;
				if (z > zbuffer[i*w + j] && z<1.f&&z>-1.f) {
					zbuffer[i*w + j] = z;
					float rp = a*tri.va.r + b*tri.vb.r + c*tri.vc.r;
					float gp = a*tri.va.g + b*tri.vb.g + c*tri.vc.g;
					float bp = a*tri.va.b + b*tri.vb.b + c*tri.vc.b;
					float p1 = a*tri.va.pr + b*tri.vb.pr + c*tri.vc.pr;
					float p2 = a*tri.va.pg + b*tri.vb.pg + c*tri.vc.pg;
					float p3 = a*tri.va.pb + b*tri.vb.pb + c*tri.vc.pb;

					vt = Vertex(p.x, p.y, p.z, rp, gp, bp, p1, p2, p3);
					vt.n = t.n;

					if (Dot(Normalize(vt.n), Normalize(p - camera.eye)) < 0) vt.n = -vt.n;
					LightCompute(&vt, pl, camera.eye);

					vt.r = min(vt.r * 255, 255);
					vt.g = min(vt.g * 255, 255);
					vt.b = min(vt.b * 255, 255);

					SetPixel(hdc, i, h - j, RGB(vt.r, vt.g, vt.b));
				}
			}
		}
}

void RenderObj(TriangleMesh &mesh, Camera &camera, PointLight &pl) {

	for (int k = 0; k < mesh.triNum; k++) {

		//transform it vertex position
		Triangle t = Triangle(Vertex(mesh.p[mesh.vi[3 * k]]), Vertex(mesh.p[mesh.vi[3 * k + 1]]), Vertex(mesh.p[mesh.vi[3 * k + 2]]));
		t.n = mesh.Nor(k);
		t.va.r = t.vb.r = t.vc.r = mesh.kd[0];
		t.va.g = t.vb.g = t.vc.g = mesh.kd[1];
		t.va.b = t.vb.b = t.vc.b = mesh.kd[2];
		t.va.pr = t.vb.pr = t.vc.pr = 0;
		t.va.pg = t.vb.pg = t.vc.pg = 0;
		t.va.pb = t.vb.pb = t.vc.pb = 0;
		t.va.n = mesh.n[mesh.ni[3 * k]];
		t.vb.n = mesh.n[mesh.ni[3 *k+1]];
		t.vc.n = mesh.n[mesh.ni[3 *k+2]];
		t.va.textu = mesh.uv[mesh.uvi[3 * k] * 3 ];
		t.va.textv = mesh.uv[mesh.uvi[3 * k] * 3+1];
		t.va.textw = mesh.uv[mesh.uvi[3 * k] * 3+2];
		t.vb.textu = mesh.uv[mesh.uvi[3 * k+1] * 3];
		t.vb.textv = mesh.uv[mesh.uvi[3 * k+1] * 3 + 1];
		t.vb.textw = mesh.uv[mesh.uvi[3 * k+1] * 3 + 2];
		t.vc.textu = mesh.uv[mesh.uvi[3 * k + 2] * 3];
		t.vc.textv = mesh.uv[mesh.uvi[3 * k + 2] * 3 + 1];
		t.vc.textw = mesh.uv[mesh.uvi[3 * k + 2] * 3 + 2];

		Triangle tri = TriangleTransform(t, camera);

		Box b = Box(tri.va.p, tri.vb.p);
		Box c = Union(b, tri.vc.p);
		Vertex vt;

		//if ((c.minPoint.x<0 && c.maxPoint.x<0) || (c.maxPoint.y < 0 && c.minPoint.y < 0)) { continue; }
		//if ((c.minPoint.x > w && c.maxPoint.x > w) || (c.maxPoint.y>h && c.minPoint.y>h)) { continue; }


		int x0, y0, x1, y1;
		x0 = floor(c.minPoint.x);
		x0 = max(0, x0);
		y0 = floor(c.minPoint.y);
		y0 = max(0, y0);
		x1 = ceil(c.maxPoint.x);
		x1 = min(w, x1);
		y1 = ceil(c.maxPoint.y);
		y1 = min(h, y1);

		for (int i = x0; i < x1; i++)
			for (int j = y0; j < y1; j++) {

				float a = MidPointDistance(i, j, tri.vb.p, tri.vc.p) / MidPointDistance(tri.va.p.x, tri.va.p.y, tri.vb.p, tri.vc.p);
				float b = MidPointDistance(i, j, tri.vc.p, tri.va.p) / MidPointDistance(tri.vb.p.x, tri.vb.p.y, tri.vc.p, tri.va.p);
				float c = MidPointDistance(i, j, tri.va.p, tri.vb.p) / MidPointDistance(tri.vc.p.x, tri.vc.p.y, tri.va.p, tri.vb.p);

				if (a > 0 && b > 0 && c > 0) {
					vt.p = a*t.va.p + b*t.vb.p + c*t.vc.p;
					float z = a*tri.va.p.z + b*tri.vb.p.z + c*tri.vc.p.z;
					if (z > zbuffer[j*w + i] && z<1.f&&z>-1.f) {
						zbuffer[j*w + i] = z;
						if (mesh.flag == 0) {
							vt.r = a*t.va.r + b*t.vb.r + c*t.vc.r;
							vt.g = a*t.va.g + b*t.vb.g + c*t.vc.g;
							vt.b = a*t.va.b + b*t.vb.b + c*t.vc.b;
							vt.pr = a*t.va.pr + b*t.vb.pr + c*t.vc.pr;
							vt.pg = a*t.va.pg + b*t.vb.pg + c*t.vc.pg;
							vt.pb = a*t.va.pb + b*t.vb.pb + c*t.vc.pb;
							vt.n = Normalize(a*t.va.n + b*t.vb.n + c*t.vc.n);
							//if (Dot(Normalize(vt.n), Normalize(p - camera.eye)) < 0) vt.n = -vt.n;
							LightCompute(&vt, pl, camera.eye);

							vt.r = (int)min(vt.r * 255, 255);
							vt.g = (int)min(vt.g * 255, 255);
							vt.b = (int)min(vt.b * 255, 255);
							int num = 4 * w*j + 4 * i;

							pbuffer[num] = (BYTE)vt.b;
							pbuffer[num + 1] = (BYTE)vt.g;
							pbuffer[num + 2] = (BYTE)vt.r;
							pbuffer[num + 3] = (BYTE)0;
						}
						else if (mesh.flag == 1) {
							vt.textu = a*t.va.textu + b*t.vb.textu + c*t.vc.textu;
							vt.textv = a*t.va.textv + b*t.vb.textv + c*t.vc.textv;
							vt.textw = a*t.va.textw + b*t.vb.textw + c*t.vc.textw;
							vt.pr = a*t.va.pr + b*t.vb.pr + c*t.vc.pr;
							vt.pg = a*t.va.pg + b*t.vb.pg + c*t.vc.pg;
							vt.pb = a*t.va.pb + b*t.vb.pb + c*t.vc.pb;
							vt.n = Normalize(a*t.va.n + b*t.vb.n + c*t.vc.n);
							textureCompute(mesh, &vt, pl, camera.eye);
							vt.r = (int)min(vt.r * 255, 255);
							vt.g = (int)min(vt.g * 255, 255);
							vt.b = (int)min(vt.b * 255, 255);
							int num = 4 * w*j + 4 * i;

							pbuffer[num] = (BYTE)vt.b;
							pbuffer[num + 1] = (BYTE)vt.g;
							pbuffer[num + 2] = (BYTE)vt.r;
							pbuffer[num + 3] = (BYTE)0;
						}
						else if (mesh.flag == 2) {
							vt.r = a*t.va.r + b*t.vb.r + c*t.vc.r;
							vt.g = a*t.va.g + b*t.vb.g + c*t.vc.g;
							vt.b = a*t.va.b + b*t.vb.b + c*t.vc.b;
							vt.pr = a*t.va.pr + b*t.vb.pr + c*t.vc.pr;
							vt.pg = a*t.va.pg + b*t.vb.pg + c*t.vc.pg;
							vt.pb = a*t.va.pb + b*t.vb.pb + c*t.vc.pb;
							vt.n = Normalize(a*t.va.n + b*t.vb.n + c*t.vc.n);
							CartoonCompute(mesh, &vt, pl, camera.eye);
							vt.r = (int)min(vt.r * 255, 255);
							vt.g = (int)min(vt.g * 255, 255);
							vt.b = (int)min(vt.b * 255, 255);
							int num = 4 * w*j + 4 * i;

							pbuffer[num] = (BYTE)vt.b;
							pbuffer[num + 1] = (BYTE)vt.g;
							pbuffer[num + 2] = (BYTE)vt.r;
							pbuffer[num + 3] = (BYTE)0;
						}
					}
				}
			}

	}

}



void RenderPipeline(TriangleMesh *mesh,int meshNum,Camera &camera, PointLight &pl) {
	for (int i = 0; i < meshNum; i++)
		RenderObj(mesh[i], camera, pl);
}

void screenupdate() {
	for (int i = 0; i < 4 * w*h; i++)
		pBits[i] = pbuffer[i];
	hdc = BeginPaint(hwnd, &ps);
	GetObject(hBitmap, sizeof(BITMAP), &bitmap);
	hdcmem = CreateCompatibleDC(hdc);
	SelectObject(hdcmem, hBitmap);

	BitBlt(hdc, 0, 0, w, h, hdcmem, 0, 0, SRCCOPY);
	DeleteDC(hdcmem);
	EndPaint(hwnd, &ps);
}



