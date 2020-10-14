#pragma once
#include"SeeAisa.h"
#include "geometry.h"
struct Matrix4x4 {
#ifdef _WIN32

	Matrix4x4() {
		m = XMFLOAT4X4(1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f);
	}

	//赋值
	Matrix4x4(float mat[4][4]);
	Matrix4x4(float t00, float t01, float t02, float t03,
		float t10, float t11, float t12, float t13,
		float t20, float t21, float t22, float t23,
		float t30, float t31, float t32, float t33);
	Matrix4x4(XMFLOAT4X4 xmf);
	Matrix4x4(XMMATRIX xm);

	//访问矩阵元素
	float operator()(int x, int y)const {
		return m(x, y);
	}

	//矩阵m1右乘m2
	static Matrix4x4 Mul(const Matrix4x4 &m1, const Matrix4x4 &m2) {
		Matrix4x4 r;
		XMMATRIX xm1 = DirectX::XMLoadFloat4x4(&m1.m);
		XMMATRIX xm2 = DirectX::XMLoadFloat4x4(&m2.m);
		XMMATRIX xm3 = DirectX::XMMatrixMultiply(xm1, xm2);
		DirectX::XMStoreFloat4x4(&r.m, xm3);
		return r;
	}

	//矩阵转置
	friend Matrix4x4 Transpose(const Matrix4x4 &m);

	//矩阵求逆
	friend Matrix4x4 Inverse(const Matrix4x4 &);


	//两个矩阵是否相同
	bool operator==(const Matrix4x4 &m2) const {
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				if (m(i, j) != m2.m(i, j)) return false;
		return true;
	}

	//两个矩阵是否不同
	bool operator!=(const Matrix4x4 &m2) const {
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				if (m(i, j) != m2.m(i, j)) return true;
		return false;
	}

	XMFLOAT4X4 m;
#else
	Matrix4x4() {
		m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.f;
		m[0][1] = m[0][2] = m[0][3] = 0.f;
		m[1][0] = m[1][2] = m[1][3] = 0.f;
		m[2][0] = m[2][1] = m[2][3] = 0.f;
		m[3][0] = m[3][1] = m[3][2] = 0.f;
	}

	//赋值
	Matrix4x4(float mat[4][4]);
	Matrix4x4(float t00, float t01, float t02, float t03,
		float t10, float t11, float t12, float t13,
		float t20, float t21, float t22, float t23,
		float t30, float t31, float t32, float t33);

	//矩阵m1右乘m2
	static Matrix4x4 Mul(const Matrix4x4 &m1, const Matrix4x4 &m2) {
		Matrix4x4 r;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				r.m[i][j] = m1.m[i][0] * m2.m[0][j] +
				m1.m[i][1] * m2.m[1][j] +
				m1.m[i][2] * m2.m[2][j] +
				m1.m[i][3] * m2.m[3][j];
		return r;
	}

	//矩阵转置
	friend Matrix4x4 Transpose(const Matrix4x4 &m);

	//矩阵求逆
	friend Matrix4x4 Inverse(const Matrix4x4 &);


	//两个矩阵是否相同
	bool operator==(const Matrix4x4 &m2) const {
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				if (m[i][j] != m2.m[i][j]) return false;
		return true;
	}

	//两个矩阵是否不同
	bool operator!=(const Matrix4x4 &m2) const {
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				if (m[i][j] != m2.m[i][j]) return true;
		return false;
	}

	//matrix's float element
	float m[4][4];
#endif
};





class Transform {

public:
	//---------初始化：Matrix4x4结构中已赋默认值-----------
	Transform() {}

	//赋值
	Transform(const float mat[4][4]) {
		m = Matrix4x4(mat[0][0], mat[0][1], mat[0][2], mat[0][3],
			mat[1][0], mat[1][1], mat[1][2], mat[1][3],
			mat[2][0], mat[2][1], mat[2][2], mat[2][3],
			mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
		mInv = Inverse(m);
	}

	//赋值
	Transform(const Matrix4x4 &mat)
		:m(mat), mInv(Inverse(mat)) {
	}

	//给矩阵和逆矩阵赋值
	Transform(const Matrix4x4 &mat, const Matrix4x4 &minv)
		:m(mat), mInv(minv) {
	}

	//矩阵和逆矩阵相互交换
	friend Transform Inverse(const Transform &t) {
		return Transform(t.mInv, t.m);
	}

	//平移变换T和它的逆变换（平移向量delta）
	friend Transform Translate(const Vector &delta);

	//比例变换S
	friend Transform Scale(float x, float y, float z);

	//旋转变换X,Y,Z轴
	friend Transform RotateX(float angle);
	friend Transform RotateY(float angle);
	friend Transform RotateZ(float angle);

	//
	friend Transform MatrixIdentity();

	//任意旋转轴的旋转变换R
	friend Transform Rotate(float angle, const Vector &axis);

	//look-at变换(camera变换项)
	friend Transform LookAt(const Point &pos, const Point &look, const Vector &up);

	friend Transform MatrixPerspectiveFov(float  aspect, float ratio, float cNear, float cFar);

	//点坐标变换，得到新点
	inline Point operator()(const Point &pt) const;

	//点坐标变换，结果返回给ptrans指针
	inline void operator()(const Point &pt, Point *ptrans)const;

	//向量变换（向量的齐次权重w=0，可不考虑wp）
	inline Vector operator()(const Vector &v) const;

	//向量变换，结果返回给vtrans指针
	inline void operator()(const Vector &v, Vector *vtrans)const;

	//法线转换矩阵，需要列向量左乘转置的逆转换矩阵
	inline Normal operator()(const Normal &n)const;

	//矩阵右乘t2，用于多重转换的矩阵推导
	inline Transform operator*(const Transform &t2)const;

public:

	//transform private data
	Matrix4x4 m, mInv;

	//四元数
	friend struct Quaternion;
};


#ifdef _WIN32
inline Point Transform::operator()(const Point &pt)const {
	XMFLOAT4 xmfPoint = XMFLOAT4(pt.x, pt.y, pt.z, 1.f);
	XMVECTOR xmvPoint = XMLoadFloat4(&xmfPoint);
	XMMATRIX xm = XMLoadFloat4x4(&m.m);
	xmvPoint = XMVector4Transform(xmvPoint, xm);
	XMStoreFloat4(&xmfPoint, xmvPoint);
	if (xmfPoint.w == 1.f) return Point(xmfPoint.x, xmfPoint.y, xmfPoint.z);
	else return Point(xmfPoint.x, xmfPoint.y, xmfPoint.z) / xmfPoint.w;
}


inline void Transform::operator()(const Point &pt, Point *ptrans)const {
	XMFLOAT4 xmfPoint = XMFLOAT4(pt.x, pt.y, pt.z, 1.f);
	XMVECTOR xmvPoint = XMLoadFloat4(&xmfPoint);
	XMMATRIX xm = XMLoadFloat4x4(&m.m);
	xmvPoint = XMVector4Transform(xmvPoint, xm);
	XMStoreFloat4(&xmfPoint, xmvPoint);
	ptrans->x = xmfPoint.x;
	ptrans->y = xmfPoint.y;
	ptrans->z = xmfPoint.z;
	if (xmfPoint.w != 1.) *ptrans /= xmfPoint.w;
}


inline Vector Transform::operator()(const Vector &v)const {
	XMFLOAT4 xmfPoint = XMFLOAT4(v.x, v.y, v.z, 0.f);
	XMVECTOR xmvPoint = XMLoadFloat4(&xmfPoint);
	XMMATRIX xm = XMLoadFloat4x4(&m.m);
	xmvPoint = XMVector4Transform(xmvPoint, xm);
	XMStoreFloat4(&xmfPoint, xmvPoint);
	return Vector(xmfPoint.x, xmfPoint.y, xmfPoint.z);

}


inline void Transform::operator()(const Vector &v, Vector *vtrans)const {
	XMFLOAT4 xmfPoint = XMFLOAT4(v.x, v.y, v.z, 0.f);
	XMVECTOR xmvPoint = XMLoadFloat4(&xmfPoint);
	XMMATRIX xm = XMLoadFloat4x4(&m.m);
	xmvPoint = XMVector4Transform(xmvPoint, xm);
	XMStoreFloat4(&xmfPoint, xmvPoint);
	vtrans->x = xmfPoint.x;
	vtrans->y = xmfPoint.y;
	vtrans->z = xmfPoint.z;
}


inline Normal Transform::operator()(const Normal &n)const {
	XMFLOAT4 xmfPoint = XMFLOAT4(n.x, n.y, n.z, 0.f);
	XMVECTOR xmvPoint = XMLoadFloat4(&xmfPoint);
	XMMATRIX xm = XMLoadFloat4x4(&mInv.m);
	xmvPoint = XMVector4Transform(xmvPoint, xm);
	XMStoreFloat4(&xmfPoint, xmvPoint);
	return Normal(xmfPoint.x, xmfPoint.y, xmfPoint.z);
}


inline Transform Transform::operator*(const Transform &t2)const {
	Matrix4x4 m1 = Matrix4x4::Mul(m, t2.m);
	Matrix4x4 m2 = Matrix4x4::Mul(t2.mInv, mInv);
	return Transform(m1, m2);
}
#else
inline Point Transform::operator()(const Point &pt)const {
	float x = pt.x, y = pt.y, z = pt.z;
	float xp = m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z + m.m[0][3];
	float yp = m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z + m.m[1][3];
	float zp = m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z + m.m[2][3];
	float wp = m.m[3][0] * x + m.m[3][1] * y + m.m[3][2] * z + m.m[3][3];
	if (wp == 1.) return Point(xp, yp, zp);
	else return Point(xp, yp, zp) / wp;
}


inline void Transform::operator()(const Point &pt, Point *ptrans)const {
	float x = pt.x, y = pt.y, z = pt.z;
	ptrans->x = m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z + m.m[0][3];
	ptrans->y = m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z + m.m[1][3];
	ptrans->z = m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z + m.m[2][3];
	float w = m.m[3][0] * x + m.m[3][1] * y + m.m[3][2] * z + m.m[3][3];
	if (w != 1.) *ptrans /= w;
}


inline Vector Transform::operator()(const Vector &v)const {
	float x = v.x, y = v.y, z = v.z;
	return Vector(m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z,
		m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z,
		m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z);
}


inline void Transform::operator()(const Vector &v, Vector *vtrans)const {
	float x = v.x, y = v.y, z = v.z;
	vtrans->x = m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z;
	vtrans->y = m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z;
	vtrans->z = m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z;
}


inline Normal Transform::operator()(const Normal &n)const {
	float x = n.x, y = n.y, z = n.z;
	return Normal(mInv.m[0][0] * x + mInv.m[1][0] * y + mInv.m[2][0] * z,
		mInv.m[0][1] * x + mInv.m[1][1] * y + mInv.m[2][1] * z,
		mInv.m[0][2] * x + mInv.m[1][2] * y + mInv.m[2][2] * z);
}


inline Transform Transform::operator*(const Transform &t2)const {
	Matrix4x4 m1 = Matrix4x4::Mul(m, t2.m);
	Matrix4x4 m2 = Matrix4x4::Mul(t2.mInv, mInv);
	return Transform(m1, m2);
}
#endif


