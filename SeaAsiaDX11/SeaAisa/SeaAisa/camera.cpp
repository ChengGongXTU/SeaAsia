#include"camera.h"
Transform WorldToEye(const Point &eye, const Point &gaze, const Vector &upVeiew) {
	Transform t;
	t = LookAt(eye, gaze, upVeiew);
	return t;
}

Transform EyeToOrth(const float &l, const float &r, const float &b, const float &t,
	const float &n, const float &f) {
	Matrix4x4 m;
	m.m[0][0] = 2.f / (r - l);
	m.m[1][1] = 2.f / (t - b);
	m.m[2][2] = -2.f / (f - n);

	m.m[0][3] = -(r + l) / (r - l);
	m.m[1][3] = -(t + b) / (t - b);
	m.m[2][3] = (f + n) / (f - n);

	m.m[0][1] = m.m[0][2] = 0.f;
	m.m[1][0] = m.m[1][2] = 0.f;
	m.m[2][0] = m.m[2][1] = 0.f;
	m.m[3][0] = m.m[3][1] = m.m[3][2] = 0.f;
	m.m[3][3] = 1.0f;

	return Transform(m);
}

Transform EyeToPer(const float &l, const float &r, const float &b, const float &t,
	const float &n, const float &f) {
	Matrix4x4 m;

	m.m[0][0] = 2 * (n) / (r - l);
	m.m[1][1] = 2 * (n) / (t - b);
	m.m[2][2] = (f + n) / (n - f);

	m.m[0][3] = 0.f;
	m.m[1][3] = 0.f;
	m.m[2][3] = 2 * (f)*(n) / (f - n);

	m.m[0][1] = 0.f;
	m.m[0][2] = (l + r) / (l - r);

	m.m[1][0] = 0.f;
	m.m[1][2] = (b + t) / (b - t);
	m.m[2][0] = m.m[2][1] = 0.f;
	m.m[3][0] = m.m[3][1] = 0.f;
	m.m[3][2] = 1.f;
	m.m[3][3] = 0.f;

	return Transform(m);
}


Transform ProToView(const int &nx, const int &ny) {
	Matrix4x4 m;

	m.m[0][0] = nx / 2.f;
	m.m[1][1] = ny / 2.f;
	m.m[2][2] = 1.f;

	m.m[0][3] = (nx - 1) / 2.f;
	m.m[1][3] = (ny - 1) / 2.f;
	m.m[2][3] = 0.f;

	m.m[0][1] = 0.f;
	m.m[0][2] = 0.f;

	m.m[1][0] = 0.f;
	m.m[1][2] = 0.f;
	m.m[2][0] = m.m[2][1] = 0.f;
	m.m[3][0] = m.m[3][1] = 0.f;
	m.m[3][2] = 0.f;
	m.m[3][3] = 1.f;

	return Transform(m);
}

int ComputeOutcode(Point &p) {

	int code = 0;
	//for x-axis: left or right

	if (p.x < -1)	code = code + 1;
	else if (p.x>1)	code = code + 2;

	if (p.y < -1)	code = code + 4;
	else if (p.y>1)	code = code + 8;

	if (p.z > 1)	code = code + 16;
	else if (p.z < -1) code = code + 32;

	return code;
}

int LineCull(Point &p1, Point &p2) {

	int code1 = ComputeOutcode(p1);
	int code2 = ComputeOutcode(p2);

	Point pn1;
	Point pn2;

	int flag = 0;

	if ((code1 | code2) == 0) return 0;
	if ((code1 & code2) != 0)	return 2;

	// check x= -1  face
	if ((code1 & 1) ^ (code2 & 1)) {
		float t = (-1 - p1.x) / (p2.x - p1.x);
		Point newp = p1 + (p2 - p1)*t;
		if ((newp.y >= -1 && newp.y <= 1) && (newp.z >= -1 && newp.z <= 1)) {
			if (flag == 0) {
				pn1 = newp;
				flag = 1;
			}
			else if (flag == 1) {
				pn2 = newp;
				flag = 2;
			}
		}

	}

	//x = 1 face
	if ((code1 & 2) ^ (code2 & 2)) {
		float t = (1 - p1.x) / (p2.x - p1.x);
		Point newp = p1 + (p2 - p1)*t;
		if ((newp.y >= -1 && newp.y <= 1) && (newp.z >= -1 && newp.z <= 1)) {
			if (flag == 0) {
				pn1 = newp;
				flag = 1;
			}
			else if (flag == 1) {
				pn2 = newp;
				flag = 2;
			}
		}

	}

	//y = -1 face
	if ((code1 & 4) ^ (code2 & 4)) {
		float t = (-1 - p1.y) / (p2.y - p1.y);
		Point newp = p1 + (p2 - p1)*t;
		if ((newp.x >= -1 && newp.x <= 1) && (newp.z >= -1 && newp.z <= 1)) {
			if (flag == 0) {
				pn1 = newp;
				flag = 1;
			}
			else if (flag == 1) {
				pn2 = newp;
				flag = 2;
			}
		}
	}

	//y = 1 face
	if ((code1 & 8) ^ (code2 & 8)) {
		float t = (1 - p1.y) / (p2.y - p1.y);
		Point newp = p1 + (p2 - p1)*t;
		if ((newp.x >= -1 && newp.x <= 1) && (newp.z >= -1 && newp.z <= 1)) {
			if (flag == 0) {
				pn1 = newp;
				flag = 1;
			}
			else if (flag == 1) {
				pn2 = newp;
				flag = 2;
			}
		}
	}

	//z = 1 face
	if ((code1 & 16) ^ (code2 & 16)) {
		float t = (1 - p1.z) / (p2.z - p1.z);
		Point newp = p1 + (p2 - p1)*t;
		if ((newp.x >= -1 && newp.x <= 1) && (newp.y >= -1 && newp.y <= 1)) {
			if (flag == 0) {
				pn1 = newp;
				flag = 1;
			}
			else if (flag == 1) {
				pn2 = newp;
				flag = 2;
			}

		}
	}

	//z = -1 face
	if ((code1 & 32) ^ (code2 & 32)) {
		float t = (-1 - p1.z) / (p2.z - p1.z);
		Point newp = p1 + (p2 - p1)*t;
		if ((newp.x >= -1 && newp.x <= 1) && (newp.y >= -1 && newp.y <= 1)) {
			if (flag == 0) {
				pn1 = newp;
				flag = 1;
			}
			else if (flag == 1) {
				pn2 = newp;
				flag = 2;
			}

		}
	}

	if (flag == 1) {
		if (code1 == 0)
			p2 = pn1;
		else if (code2 == 0)
			p1 = pn1;
	}

	else if (flag == 2) {
		p1 = pn1;
		p2 = pn2;
	}

	return 1;

}