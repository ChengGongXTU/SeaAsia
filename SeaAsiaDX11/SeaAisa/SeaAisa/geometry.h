
#pragma once
#include"SeeAisa.h"
class Vector {
public:

	 //�趨������ʼֵ
	Vector() { x = y = z = 0.f; }

	//��xx,yy,zz��ֵ�������ڵ�xyz
	Vector(float xx, float yy, float zz)
		: x(xx), y(yy), z(zz) {         
	}

	//��һ��������ֵ������һ��
	Vector(const Vector &v) {
		x = v.x, y = v.y, z = v.z;
	}

	//����=����������������ֵ
	Vector &operator=(const Vector &v) {
		x = v.x, y = v.y, z = v.z;
		return *this;
	}

	//����+��������ʹ2�������ܹ���ӣ�����һ���¶���
	Vector operator+(const Vector &v) const {  
		return Vector(x + v.x, y + v.y, z + v.z);
	}

	//����+=�����������ض���������ǰ���õ�Vector
	Vector& operator+=(const Vector &v) {       
		x += v.x; y += v.y; z += v.z;
		return *this;                    
	}

	//����-������
	Vector operator-(const Vector &v) const {  
		return Vector(x - v.x, y - v.y, z - v.z);
	}


	//����-=������
	Vector& operator-=(const Vector &v) {       
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}

	//���ء�*��������
	Vector operator*(float f) const {         
		return Vector(f*x, f*y, f*z);
	}

	//����*=������
	Vector &operator*=(float f) {            
		x *= f; y *= f; z *= f;
		return *this;
	}

	//���ء�/������
	Vector operator/(float f)const {                 
		float inv = 1.f / f;
		return Vector(x*inv, y*inv, z*inv);
	}

	//����/=������
	Vector &operator/=(float f) {
		float inv = 1.f / f;
		x *= inv; y *= inv; z *= inv;
		return *this;
	}
	//����һ���෴�����-��
	Vector operator-() const {                       
		return Vector(-x, -y, -z);
	}

	//�����±�
	float operator[](int i) const {                   
		return (&x)[i];
	}

	//�����±�
	float &operator[](int i) {
		return (&x)[i];
	}

	//�������ĳ��ȵ�ƽ��
	float LengthSquared() const { return x*x + y*y + z*z; }   

	//�������ĳ���
	float Length() const { return sqrtf(LengthSquared()); }


	//Vector's component
	float x, y, z;
};



class Point {
public:
	
	//��ʼ��
	Point() { x = y = z = 0.f; }

	//ֱ�Ӹ�ֵ
	Point(float xx, float yy, float zz)
		:x(xx), y(yy), z(zz) {
	}

	//��һ���㸳ֵ���õ�
	Point(const Point &p) {
		x = p.x, y = p.y, z = p.z;
	}

	//����=�����������õ㸳ֵ
	Point &operator=(const Point &p) {
		x = p.x, y = p.y, z = p.z;
		return *this;
	}

	//��+��=�µ�
	Point operator+(const Point &p) const {
		return Point(x + p.x, y + p.y, z + p.z);
	}

	//��+����=�µ�
	Point operator+(const Vector &v) const {                
		return Point(x + v.x, y + v.y, z + v.z);
	}

	//����+=����������Vector�ı�õ������
	Point &operator+=(const Vector &v) {
		x += v.x; y += v.y; z += v.z;
		return *this;
	}

	//��-��=������
	Vector operator-(const Point &p) const {              
		return Vector(x - p.x, y - p.y, z - p.z);
	}

	//��-����=�µ�
	Point operator-(const Vector &v) const {              
		return Point(x - v.x, y - v.y, z - v.z);
	}

	//����-=����������vector�ı�õ�����
	Point &operator-=(const Vector &v) {
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}

	//���ء�*�����������õ��µ�����
	Point operator*(float f) const {         
		return Point(f*x, f*y, f*z);
	}

	//����*=��������������f�ı�õ�����
	Point &operator*=(float f) {            
		x *= f; y *= f; z *= f;
		return *this;
	}

	//����/���������õ��µ�����
	Point operator/(float f)const {         
		float inv = 1.f / f;
		return Point(x*inv, y*inv, z*inv);
	}

	//������f�ı�õ�����
	Point &operator/=(float f) {
		float inv = 1.f / f;
		x *= inv, y *= inv, z *= inv;
		return *this;
	}

	//�趨�±�
	float operator[](int i) const {        
		return (&x)[i];
	}

	//�趨�±�
	float &operator[](int i) {
		return (&x)[i];
	}

	//�ж����������Ƿ����
	bool operator==(const Point &p) const {
		return (x == p.x) && (y == p.y) && (z == p.z);
	}

	//�ж����������Ƿ����
	bool operator!=(const Point &p)const {
		return x != p.x || y != p.y || z != p.z;
	}

	//point's position
	float x, y, z;
};


class Normal {
public:
	
	Normal() { x = y = z = 0.f; }
	Normal(float xx, float yy, float zz)
		: x(xx), y(yy), z(zz) {
	}

	Normal(const Normal &n) {
		x = n.x, y = n.y, z = n.z;
	}

	Normal &operator=(const Normal &n) {
		x = n.x, y = n.y, z = n.z;
		return *this;
	}

	Normal &operator=(const Vector &n) {
		x = n.x, y = n.y, z = n.z;
		return *this;
	}

	Normal operator+(const Normal &v) const {  //----------����+������---------------
		return Normal(x + v.x, y + v.y, z + v.z);
	}

	Normal &operator+=(const Normal &v) {       //-----------����+=�����������ض�����-------------
		x += v.x; y += v.y; z += v.z;
		return *this;
	}

	Normal operator-(const Normal &n)const {        //-----------����-������-----------------
		return Normal(x - n.x, y - n.y, z - n.z);
	}

	Normal &operator-=(const Normal &n) {
		x -= n.x, y -= n.y, z -= n.z;
		return *this;
	}

	Normal operator-() const {                        //----------����һ���෴�����-��------------
		return Normal(-x, -y, -z);
	}

	float operator[](int i) const {					//---------�����±�--------
		return (&x)[i];
	}

	float &operator[](int i) {
		return (&x)[i];
	}

	Normal operator*(float f) const {         //----------���ء�*��������------------
		return Normal(f*x, f*y, f*z);
	}

	Normal &operator*=(float f) {            //----------����*=������---------------
		x *= f; y *= f; z *= f;
		return *this;
	}

	Normal operator/(float f) const {                 //-----���ء�/������----------
		float inv = 1.f / f;
		return Normal(x*inv, y*inv, z*inv);
	}

	Normal &operator/=(float f) {
		float inv = 1.f / f;
		x *= f; y *= f; z *= f;
		return *this;
	}


	//������߳��ȵ�ƽ��
	float LengthSquared() const { return x*x + y*y + z*z; }    

	//������߳���
	float Length() const { return sqrtf(LengthSquared()); }


	//normal��s component
	float x, y, z;

};


//�������ص�v*f����������f*v����
inline Vector operator*(float f, const Vector &v) {            
	return v*f;
}

//������ˣ����������нǵ�����ֵ
inline float Dot(const Vector &v1, const Vector &v2) {         
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

//������˾���ֵ
inline float AbsDot(const Vector &v1, const Vector &v2) {      
	return fabsf(Dot(v1, v2));
}

//������ˣ��õ���2�������ഹֱ��1����������Ҳ���ı��ε����
inline Vector Cross(const Vector &v1, const Vector &v2) {      
	return Vector((v1.y*v2.z) - (v1.z*v2.y),                   
		(v1.z*v2.x) - (v1.x*v2.z),
		(v1.x*v2.y) - (v1.y*v2.x));
}

//�����뷨�ߵĲ��
inline Vector Cross(const Vector &v1, const Normal &v2) {      
	float v1x = v1.x, v1y = v1.y, v1z = v1.z;
	float v2x = v2.x, v2y = v2.y, v2z = v2.z;
	return Vector((v1y * v2z) - (v1z * v2y),
		(v1z * v2x) - (v1x * v2z),
		(v1x * v2y) - (v1y * v2x));
}

//�����������Ĳ��
inline Vector Cross(const Normal &v1, const Vector &v2) {
	float v1x = v1.x, v1y = v1.y, v1z = v1.z;
	float v2x = v2.x, v2y = v2.y, v2z = v2.z;
	return Vector((v1y * v2z) - (v1z * v2y),
		(v1z * v2x) - (v1x * v2z),
		(v1x * v2y) - (v1y * v2x));
}

//���㵥λ����
inline Vector Normalize(const Vector &v) { return v / v.Length(); }    

//������v1�������ֲ�����ϵ��v1,v2,v3��
inline void CoordinateSystem(const Vector &v1, Vector *v2, Vector *v3) {
	if (fabsf(v1.x) > fabsf(v1.y)) {
		float invLen = 1.f / sqrtf(v1.x*v1.x + v1.z*v1.z);
		*v2 = Vector(-v1.z*invLen, 0.f, v1.x*invLen);
	}
	else {
		float invLen = 1.f / sqrtf(v1.y*v1.y + v1.z*v1.z);
		*v2 = Vector(0.f, v1.z*invLen, -v1.y*invLen);
	}
	*v3 = Cross(v1, *v2);
}

//��������
inline float Distance(const Point &p1, const Point &p2) {
	return (p1 - p2).Length();
}

//���������ƽ��
inline float DistanceSquared(const Point &p1, const Point &p2) {
	return (p1 - p2).LengthSquared();
}

//��p*f������ȥ����f*v����
inline Point operator*(float f, const Point &p) {
	return p*f;
}


//�õ�λ����
inline Normal Normalize(const Normal &n) { return n / n.Length(); }    

//�ó���f������С����
inline Normal operator*(float f, const Normal &n) {         
	return Normal(f*n.x, f*n.y, f*n.z);
}

//���ߵ�ˣ����������߼нǵ�����ֵ
inline float Dot(const Normal &n1, const Normal &n2) {         
	return n1.x*n2.x + n1.y*n2.y + n1.z*n2.z;
}

//���ߵ�˾���ֵ
inline float AbsDot(const Normal &n1, const Normal &n2) {      
	return fabsf(Dot(n1, n2));
}

//�������������
inline float Dot(const Normal &n, const Vector &v) {         
	return n.x*v.x + n.y*v.y + n.z*v.z;
}

//�����뷨�ߵ��
inline float Dot(const Vector &v1, const Normal &n2) {
	return v1.x * n2.x + v1.y * n2.y + v1.z * n2.z;
}

//��˵ľ���ֵ
inline float AbsDot(const Normal &n, const Vector &v) {      
	return fabsf(Dot(n, v));
}

//��˵ľ���ֵ
inline float AbsDot(const Vector &v1, const Normal &n2) {
	return fabsf(v1.x * n2.x + v1.y * n2.y + v1.z * n2.z);
}


//����������������ת��ͬһ����
inline Normal Faceforword(const Normal &n, const Vector &v) {      
	return(Dot(n, v) < 0.f) ? -n : n;
}

//����ת����һ���ߵ�ͬ����
inline Normal Faceforward(const Normal &n, const Normal &n2) {    
	return (Dot(n, n2) < 0.f) ? -n : n;
}


//����ת����һ������ͬ����
inline Vector Faceforward(const Vector &v, const Vector &v2) {      
	return (Dot(v, v2) < 0.f) ? -v : v;
}


//����ת����һ���ߵ�ͬ����
inline Vector Faceforward(const Vector &v, const Normal &n2) {      
	return (Dot(v, n2) < 0.f) ? -v : v;
}


