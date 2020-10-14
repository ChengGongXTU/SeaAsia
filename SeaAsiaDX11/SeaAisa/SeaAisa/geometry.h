
#pragma once
#include"SeeAisa.h"
class Vector {
public:

	 //设定向量初始值
	Vector() { x = y = z = 0.f; }

	//将xx,yy,zz赋值给函数内的xyz
	Vector(float xx, float yy, float zz)
		: x(xx), y(yy), z(zz) {         
	}

	//将一个向量的值赋给另一个
	Vector(const Vector &v) {
		x = v.x, y = v.y, z = v.z;
	}

	//重载=操作符，给向量赋值
	Vector &operator=(const Vector &v) {
		x = v.x, y = v.y, z = v.z;
		return *this;
	}

	//重载+操作符，使2个向量能够相加，返回一个新对象
	Vector operator+(const Vector &v) const {  
		return Vector(x + v.x, y + v.y, z + v.z);
	}

	//重载+=操作符，返回对象本身，即当前调用的Vector
	Vector& operator+=(const Vector &v) {       
		x += v.x; y += v.y; z += v.z;
		return *this;                    
	}

	//重载-操作符
	Vector operator-(const Vector &v) const {  
		return Vector(x - v.x, y - v.y, z - v.z);
	}


	//重载-=操作符
	Vector& operator-=(const Vector &v) {       
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}

	//重载“*”操作符
	Vector operator*(float f) const {         
		return Vector(f*x, f*y, f*z);
	}

	//重载*=操作符
	Vector &operator*=(float f) {            
		x *= f; y *= f; z *= f;
		return *this;
	}

	//重载“/”除法
	Vector operator/(float f)const {                 
		float inv = 1.f / f;
		return Vector(x*inv, y*inv, z*inv);
	}

	//重载/=操作符
	Vector &operator/=(float f) {
		float inv = 1.f / f;
		x *= inv; y *= inv; z *= inv;
		return *this;
	}
	//重载一个相反算符‘-’
	Vector operator-() const {                       
		return Vector(-x, -y, -z);
	}

	//建立下标
	float operator[](int i) const {                   
		return (&x)[i];
	}

	//建立下标
	float &operator[](int i) {
		return (&x)[i];
	}

	//求向量的长度的平方
	float LengthSquared() const { return x*x + y*y + z*z; }   

	//求向量的长度
	float Length() const { return sqrtf(LengthSquared()); }


	//Vector's component
	float x, y, z;
};



class Point {
public:
	
	//初始化
	Point() { x = y = z = 0.f; }

	//直接赋值
	Point(float xx, float yy, float zz)
		:x(xx), y(yy), z(zz) {
	}

	//用一个点赋值给该点
	Point(const Point &p) {
		x = p.x, y = p.y, z = p.z;
	}

	//重载=操作符来给该点赋值
	Point &operator=(const Point &p) {
		x = p.x, y = p.y, z = p.z;
		return *this;
	}

	//点+点=新点
	Point operator+(const Point &p) const {
		return Point(x + p.x, y + p.y, z + p.z);
	}

	//点+向量=新点
	Point operator+(const Vector &v) const {                
		return Point(x + v.x, y + v.y, z + v.z);
	}

	//重载+=操作符，用Vector改变该点的坐标
	Point &operator+=(const Vector &v) {
		x += v.x; y += v.y; z += v.z;
		return *this;
	}

	//点-点=新向量
	Vector operator-(const Point &p) const {              
		return Vector(x - p.x, y - p.y, z - p.z);
	}

	//点-向量=新点
	Point operator-(const Vector &v) const {              
		return Point(x - v.x, y - v.y, z - v.z);
	}

	//重载-=操作符，用vector改变该点坐标
	Point &operator-=(const Vector &v) {
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}

	//重载“*”操作符，得到新点坐标
	Point operator*(float f) const {         
		return Point(f*x, f*y, f*z);
	}

	//重载*=操作符，按比例f改变该点坐标
	Point &operator*=(float f) {            
		x *= f; y *= f; z *= f;
		return *this;
	}

	//重载/操作符，得到新点坐标
	Point operator/(float f)const {         
		float inv = 1.f / f;
		return Point(x*inv, y*inv, z*inv);
	}

	//按比例f改变该点坐标
	Point &operator/=(float f) {
		float inv = 1.f / f;
		x *= inv, y *= inv, z *= inv;
		return *this;
	}

	//设定下标
	float operator[](int i) const {        
		return (&x)[i];
	}

	//设定下标
	float &operator[](int i) {
		return (&x)[i];
	}

	//判断两点坐标是否相等
	bool operator==(const Point &p) const {
		return (x == p.x) && (y == p.y) && (z == p.z);
	}

	//判断两点坐标是否不相等
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

	Normal operator+(const Normal &v) const {  //----------重载+操作符---------------
		return Normal(x + v.x, y + v.y, z + v.z);
	}

	Normal &operator+=(const Normal &v) {       //-----------重载+=操作符，返回对象本身-------------
		x += v.x; y += v.y; z += v.z;
		return *this;
	}

	Normal operator-(const Normal &n)const {        //-----------重载-操作符-----------------
		return Normal(x - n.x, y - n.y, z - n.z);
	}

	Normal &operator-=(const Normal &n) {
		x -= n.x, y -= n.y, z -= n.z;
		return *this;
	}

	Normal operator-() const {                        //----------重载一个相反算符‘-’------------
		return Normal(-x, -y, -z);
	}

	float operator[](int i) const {					//---------建立下标--------
		return (&x)[i];
	}

	float &operator[](int i) {
		return (&x)[i];
	}

	Normal operator*(float f) const {         //----------重载“*”操作符------------
		return Normal(f*x, f*y, f*z);
	}

	Normal &operator*=(float f) {            //----------重载*=操作符---------------
		x *= f; y *= f; z *= f;
		return *this;
	}

	Normal operator/(float f) const {                 //-----重载“/”除法----------
		float inv = 1.f / f;
		return Normal(x*inv, y*inv, z*inv);
	}

	Normal &operator/=(float f) {
		float inv = 1.f / f;
		x *= f; y *= f; z *= f;
		return *this;
	}


	//求出法线长度的平方
	float LengthSquared() const { return x*x + y*y + z*z; }    

	//求出法线长度
	float Length() const { return sqrtf(LengthSquared()); }


	//normal’s component
	float x, y, z;

};


//用已重载的v*f操作，重载f*v操作
inline Vector operator*(float f, const Vector &v) {            
	return v*f;
}

//向量点乘，即两向量夹角的余弦值
inline float Dot(const Vector &v1, const Vector &v2) {         
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

//向量点乘绝对值
inline float AbsDot(const Vector &v1, const Vector &v2) {      
	return fabsf(Dot(v1, v2));
}

//向量叉乘，得到与2个向量相垂直的1个新向量，也即四边形的面积
inline Vector Cross(const Vector &v1, const Vector &v2) {      
	return Vector((v1.y*v2.z) - (v1.z*v2.y),                   
		(v1.z*v2.x) - (v1.x*v2.z),
		(v1.x*v2.y) - (v1.y*v2.x));
}

//向量与法线的叉乘
inline Vector Cross(const Vector &v1, const Normal &v2) {      
	float v1x = v1.x, v1y = v1.y, v1z = v1.z;
	float v2x = v2.x, v2y = v2.y, v2z = v2.z;
	return Vector((v1y * v2z) - (v1z * v2y),
		(v1z * v2x) - (v1x * v2z),
		(v1x * v2y) - (v1y * v2x));
}

//法线与向量的叉乘
inline Vector Cross(const Normal &v1, const Vector &v2) {
	float v1x = v1.x, v1y = v1.y, v1z = v1.z;
	float v2x = v2.x, v2y = v2.y, v2z = v2.z;
	return Vector((v1y * v2z) - (v1z * v2y),
		(v1z * v2x) - (v1x * v2z),
		(v1x * v2y) - (v1y * v2x));
}

//计算单位向量
inline Vector Normalize(const Vector &v) { return v / v.Length(); }    

//用向量v1，建立局部坐标系（v1,v2,v3）
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

//两点间距离
inline float Distance(const Point &p1, const Point &p2) {
	return (p1 - p2).Length();
}

//两点间距离的平方
inline float DistanceSquared(const Point &p1, const Point &p2) {
	return (p1 - p2).LengthSquared();
}

//用p*f操作，去重载f*v操作
inline Point operator*(float f, const Point &p) {
	return p*f;
}


//得单位法线
inline Normal Normalize(const Normal &n) { return n / n.Length(); }    

//用常数f扩大缩小法线
inline Normal operator*(float f, const Normal &n) {         
	return Normal(f*n.x, f*n.y, f*n.z);
}

//法线点乘，得两个法线夹角的余弦值
inline float Dot(const Normal &n1, const Normal &n2) {         
	return n1.x*n2.x + n1.y*n2.y + n1.z*n2.z;
}

//法线点乘绝对值
inline float AbsDot(const Normal &n1, const Normal &n2) {      
	return fabsf(Dot(n1, n2));
}

//法线与向量点乘
inline float Dot(const Normal &n, const Vector &v) {         
	return n.x*v.x + n.y*v.y + n.z*v.z;
}

//向量与法线点乘
inline float Dot(const Vector &v1, const Normal &n2) {
	return v1.x * n2.x + v1.y * n2.y + v1.z * n2.z;
}

//点乘的绝对值
inline float AbsDot(const Normal &n, const Vector &v) {      
	return fabsf(Dot(n, v));
}

//点乘的绝对值
inline float AbsDot(const Vector &v1, const Normal &n2) {
	return fabsf(v1.x * n2.x + v1.y * n2.y + v1.z * n2.z);
}


//将法线与所给向量转到同一半球
inline Normal Faceforword(const Normal &n, const Vector &v) {      
	return(Dot(n, v) < 0.f) ? -n : n;
}

//法线转到另一法线的同半球
inline Normal Faceforward(const Normal &n, const Normal &n2) {    
	return (Dot(n, n2) < 0.f) ? -n : n;
}


//向量转到另一向量的同半球
inline Vector Faceforward(const Vector &v, const Vector &v2) {      
	return (Dot(v, v2) < 0.f) ? -v : v;
}


//向量转到另一法线的同半球
inline Vector Faceforward(const Vector &v, const Normal &n2) {      
	return (Dot(v, n2) < 0.f) ? -v : v;
}


