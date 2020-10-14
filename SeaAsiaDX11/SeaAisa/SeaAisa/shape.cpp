#include"shape.h"




//barycentric coordinates in triangle;
float MidPointDistance(int x, int y, const Point &p0, const Point &p1) {
	float f = (p0.y - p1.y)*x + (p1.x - p0.x)*y + p0.x*p1.y - p1.x*p0.y;
	return f;
}

//float MidPointDistance(float x, float y, const Point &p0, const Point &p1) {
//	double f = (p0.y - p1.y)*x + (p1.x - p0.x)*y + p0.x*p1.y - p1.x*p0.y;
//	return f;
//}



void RayTriangleMesh::SetBoundBox()
{
	if (verNum <= 0) return;
	bbox.minPoint = p[0];
	bbox.maxPoint = p[0];

	for (int i = 0; i < verNum; i++)
	{

		bbox.minPoint.x = min(bbox.minPoint.x, p[i].x);
		bbox.minPoint.y = min(bbox.minPoint.y, p[i].y);
		bbox.minPoint.z = min(bbox.minPoint.z, p[i].z);
		bbox.maxPoint.x = max(bbox.maxPoint.x, p[i].x);
		bbox.maxPoint.y = max(bbox.maxPoint.y, p[i].y);
		bbox.maxPoint.z = max(bbox.maxPoint.z, p[i].z);
	}
}

bool RayTriangleMesh::Intersection(Ray & ray, float * tHit, float *tHitError, Info &infro)
{	

	float t = 0.f;
	float terror = 0.f;
	Info info2;

	// if ray  not hit boundbox 
	if (!bbox.intersection(ray))	return false;

	for (int i = 0; i < triNum; i++)
	{	
		if (Dot(-ray.d, Cross(p[vi[3 * i + 1]]- p[vi[3 * i]] ,p[vi[3 * i + 2]] - p[vi[3 * i + 1]] )) < 0)
			continue;

		Triangle tri;
		tri.p1 = p[vi[3 * i]]; tri.p2 = p[vi[3 * i + 1]]; tri.p3 = p[vi[3 * i + 2]];
		tri.faceID = i;

		if (!tri.Intersection(ray, &ray.t, &ray.tError, info2))
		{
			continue;
		}
	}

	if (ray.t != 0.f && info2.faceID != -1)
	{
		//{
		//	Point& p1 = p[vi[3 * infro.faceID]]; Point& p2 = p[vi[3 * infro.faceID + 1]]; Point& p3 = p[vi[3 * infro.faceID + 2]];
		//	Vector e1 = p2 - p1;
		//	Vector e2 = p3 - p1;
		//	Vector s1 = Cross(ray.d, e2);
		//	float divisor = Dot(s1, e1);
		//	float invDivisor = 1.f / divisor;

		//	//b1
		//	Vector d = ray.o - p1;
		//	float b1 = Dot(d, s1) * invDivisor;

		//	//b2
		//	Vector s2 = Cross(d, e1);
		//	float b2 = Dot(ray.d, s2) * invDivisor;

		//	infro.p = ray.o + ray.t*ray.d;
		//	infro.b1 = b1;
		//	infro.b2 = b2;
		//	infro.b3 = 1 - b1 - b2;
		//}


		int faceID = info2.faceID;
		infro.faceID = info2.faceID;
		//infro.p = infro.b1*p[vi[faceID * 3]] + infro.b2*p[vi[faceID * 3 + 1]] + infro.b3*p[vi[faceID * 3 + 2]];
		infro.p =ray.o+ray.t*ray.d ;
		infro.n = Normalize(Cross(p[vi[3 * faceID + 1]] - p[vi[3 * faceID]], p[vi[3 * faceID + 2]] - p[vi[3 * faceID + 1]]));
		infro.uv = infro.b1*uv[uvi[faceID * 3]] + infro.b2*uv[ni[faceID * 3 + 1]] + infro.b3*uv[uvi[faceID * 3 + 2]];
		infro.tangent = Normalize(p[vi[faceID * 3 + 1]] - p[vi[faceID * 3]]);

		infro.mtl = mtl[mtli[faceID]];


		return true;
	}

	else
	{
		return false;
	}
}
		
	

void RayTriangleMesh::LoadDxUnity(Unity & unity, BasicManager &basicMng)
{	

	if (unity.objId < 0)
	{
		triNum = 0;
		return;
	}

	DxObj* obj = basicMng.objManager.DxObjMem[unity.objId];

	triNum = obj->faceNum;
	verNum = obj->vertexNum;
	nNum = obj->vertexNum;
	uvNum = obj->vertexNum;

	p = new Point[verNum];
	n = new Normal[nNum];
	uv = new Point[uvNum];

	for (int i = 0; i < verNum;i++)
	{	
		p[i] = Point(obj->vData[i].Pos.x,
			obj->vData[i].Pos.y,
			obj->vData[i].Pos.z);
		n[i] = Normal(obj->vData[i].Normal.x,
			obj->vData[i].Normal.y,
			obj->vData[i].Normal.z);
		uv[i] = Point(obj->vData[i].Tex.x,
			obj->vData[i].Tex.y,
			0.f);
	}

	vi = new int[triNum * 3];
	ni = new int[triNum * 3];
	uvi = new int[triNum * 3];
	for (int i = 0; i < 3*triNum; i++)
	{
		vi[i] = obj->indices[i];
		ni[i] = obj->indices[i];
		uvi[i] = obj->indices[i];
	}

	int beginMTLID = unity.MaterialsIdIndex[0];
	mtlNum = unity.materialNum;
	mtl = new RayMaterial*[mtlNum];
	for (int i = 0; i < mtlNum; i++)
	{
		mtl[i] = NULL;
	}

	for (int i = 0; i < mtlNum; i++)
	{	
		Creatmaterial(&mtl[i], basicMng.materialsManager.dxMaterial[beginMTLID + i]);
		mtl[i]->LoadMaterial(basicMng.materialsManager.dxMaterial[beginMTLID + i]);
	}

	mtli = new int[triNum];

	for(int i = 0; i < triNum; i++)
	{
		mtli[i] = obj->faceMaterialIndices[i];
	}
	
	SetBoundBox();
}

void RayTriangleMesh::ViewTransform(Transform & View)
{	
	if (triNum < 0)	return;
	for (int i = 0; i < verNum; i++)
	{
		p[i] = View(p[i]);
		n[i] = View(n[i]);
	}
}

void RayTriangleMesh::Creatmaterial(RayMaterial** ptr, DxMaterials& dxmtl)
{
	if (*ptr != NULL)
	{
		delete *ptr;
		*ptr = NULL;
	}

	if (true)
		*ptr = new Matte[1];
	if (true)
		*ptr = new Phong[1];
	if (true)
		*ptr = new Emissive[1];
}

void Triangle::SetBoundBox()
{
	bbox.minPoint = p1;
	bbox.maxPoint = p1;
	bbox = Union(bbox, p2);
	bbox = Union(bbox, p3);
}

bool Triangle::Intersection(Ray & ray, float * tHit, float *tHitError, Info &infro)
{
	// compute s1
	Vector e1 = p2 - p1;
	Vector e2 = p3 - p1;
	Vector s1 = Cross(ray.d, e2);
	float divisor = Dot(s1, e1);
	if (divisor == 0.)
		return false;
	float invDivisor = 1.f / divisor;

	//compute three barycentric coordinate
	//b1
	Vector d = ray.o - p1;
	float b1 = Dot(d, s1) * invDivisor;
	if (b1 < 0.f || b1 > 1.f)
		return false;
	//b2
	Vector s2 = Cross(d, e1);
	float b2 = Dot(ray.d, s2) * invDivisor;
	if (b2 < 0.f || b1 + b2 > 1.f)
		return false;

	//compute t for intersection point
	float t = Dot(e2, s2) * invDivisor;
	if (t < ray.t0 || t > ray.t1)
		return false;

	//output t and t-error
	if (*tHit > t)
	{
		infro.faceID = faceID;
		*tHit = t;
		return true;
	}
	else
	{
		return false;
	}
}

float RaySphere::Pdf(Info & info)
{
	return 1.f;
}

Point RaySphere::GetSample(const Point & p2)
{	
	Vector w = GetNormal(p2);
	Vector v = Vector(w.x*0.0034, w.y*1.0, w.z*0.0071);
	v = Normalize(v);
	Vector u = Cross(w, v);

	Point sampleP = samplePTR->GetHmieSphereSample();
	return sampleP;
}

Vector RaySphere::GetNormal(const Point & p2)
{
	return Normalize(Vector(p2-p));
}

bool RaySphere::Intersection(Ray & ray, float * tHit, float * tHitError, Info & infro)
{	

	float t0 = 0.f; float t1 = 0.f;

	Vector R2SO = p - ray.o;
	float R2SF = Dot(R2SO, ray.d);

	if (R2SF < 0)	return false;

	float d = sqrt(Dot(R2SO, R2SO) - R2SF*R2SF);

	if (d > radius)	return false;

	float dis = sqrt(radius*radius - d*d);

	if (dis <= 0.001)
	{
		t0 = t1 = R2SF;
	}
	else
	{
		t0 = R2SF - dis;
		t1 = R2SF + dis;
	}

	if (t0 < ray.t)
	{
		infro.faceID = 0;
		ray.t = t0;
		infro.p = ray.o + ray.d*ray.t ;
		infro.n = Normalize(infro.p - p);
		infro.mtl = mtl;
		infro.tangent = Normalize(Cross(infro.n, -ray.d));
		*tHit = t0;
		//*tHitError = *tHit*0.001;
 		return true;
	}
	else
	{
		return false;
	}
		

}

void RaySphere::LoadDxUnity(Unity & unity, BasicManager & basicMng)
{
	if (unity.objId < 0)
	{
		radius = 0.f;
		return;
	}

	DxObj* obj = basicMng.objManager.DxObjMem[unity.objId];

	float inv = 1.f / obj->vertexNum;

	for (int i = 0; i < obj->vertexNum; i++)
	{
		p = p + Point(obj->vData[i].Pos.x,
			obj->vData[i].Pos.y,
			obj->vData[i].Pos.z)*inv;
	}

	radius = (Point(obj->vData[0].Pos.x, obj->vData[0].Pos.y, obj->vData[0].Pos.z) - p).Length();
	mtl  =NULL;
	Creatmaterial(&mtl, basicMng.materialsManager.dxMaterial[unity.MaterialsIdIndex[0]]);
	mtl->LoadMaterial(basicMng.materialsManager.dxMaterial[unity.MaterialsIdIndex[0]]);
}

void RaySphere::ViewTransform(Transform & View)
{
	p = View(p);
}

void RaySphere::Creatmaterial(RayMaterial** ptr, DxMaterials & mtl)
{
		if (*ptr != NULL)
		{
			delete *ptr;
			*ptr = NULL;
		}

		if (true)
			*ptr = new Matte[1];
		if (true)
			*ptr = new Phong[1];
		if (true)
			*ptr = new Emissive[1];
}

void RayRectangle::Creatmaterial(RayMaterial** ptr, DxMaterials & dxmtl)
{
	if (*ptr != NULL)
	{
		delete *ptr;
		*ptr = NULL;
	}

	if (true)
		*ptr = new Matte[1];
	if (true)
		*ptr = new Phong[1];
	if (true)
		*ptr = new Emissive[1];
}

bool RayRectangle::Intersection(Ray & ray, float * tHit, float * tHitError, Info & infro)
{
	float t = Dot((o - ray.o), n) / Dot(ray.d , n);

	if (t <= *tHitError)
		return (false);

	Point p = ray.o + t * ray.d;
	Vector d = p - o;

	float ddota = Dot(d ,a);

	if (ddota < 0.0 || ddota > a_len_squared)
		return (false);

	float ddotb = Dot(d , b);

	if (ddotb < 0.0 || ddotb > b_len_squared)
		return (false);

	if (t < ray.t)
	{
		ray.t = t;
		infro.n = n;
		infro.p = p;
		infro.mtl = mtl;
		infro.faceID = 0;
		return true;
	}

	return false;

	//Vector e1 = a;
	//Vector e2 = b;
	//Vector s1 = Cross(ray.d, e2);
	//float divisor = Dot(s1, e1);
	//if (divisor == 0.)
	//	return false;
	//float invDivisor = 1.f / divisor;

	////compute three barycentric coordinate
	////b1
	//Vector d = ray.o - o;
	//float b1 = Dot(d, s1) * invDivisor;
	//Vector s2 = Cross(d, e1);
	//float b2 = Dot(ray.d, s2) * invDivisor;
	////compute t for intersection point
	//float t = Dot(e2, s2) * invDivisor;

	//if (t <= *tHitError)
	//	return (false);

	//Point pv = ray.o + t * ray.d;
	//Vector dv = pv - o;

	//float ddota = Dot(dv ,a)/a.Length();

	//if (ddota < 0.0 || ddota > a_len_squared)
	//	return (false);

	//float ddotb = Dot(dv , b)/b.Length();

	//if (ddotb < 0.0 || ddotb > b_len_squared)
	//	return (false);

	//if (t < ray.t)
	//{
	//	ray.t = t;
	//	infro.n = n;
	//	infro.p = pv;
	//	infro.mtl = mtl;
	//	infro.faceID = 0;
	//	return true;
	//}

	//return false;

	
}

void RayRectangle::LoadDxUnity(Unity & unity, BasicManager & basicMng)
{
	if (unity.objId < 0)
	{
		a_len_squared = 0.f; b_len_squared = 0.f;
		return;
	}

	DxObj* obj = basicMng.objManager.DxObjMem[unity.objId];

		o = Point(obj->vData[0].Pos.x,
			obj->vData[0].Pos.y,
			obj->vData[0].Pos.z);

		Vector a1 = Point(obj->vData[1].Pos.x,
			obj->vData[1].Pos.y,
			obj->vData[1].Pos.z)
			- Point(obj->vData[0].Pos.x,
				obj->vData[0].Pos.y,
				obj->vData[0].Pos.z);
		Vector b1 = Point(obj->vData[2].Pos.x,
			obj->vData[2].Pos.y,
			obj->vData[2].Pos.z)
			- Point(obj->vData[0].Pos.x,
				obj->vData[0].Pos.y,
				obj->vData[0].Pos.z);
		Vector c1 = Point(obj->vData[3].Pos.x,
			obj->vData[3].Pos.y,
			obj->vData[3].Pos.z)
			- Point(obj->vData[0].Pos.x,
				obj->vData[0].Pos.y,
				obj->vData[0].Pos.z);

		if (a1.Length() > b1.Length() && a1.Length() > c1.Length())
		{
			a = b1; b = c1;
		}
		else if (b1.Length() > a1.Length() && b1.Length() > c1.Length())
		{
			a = a1; b = c1;
		}
		else if (c1.Length() > a1.Length() && c1.Length() > b1.Length())
		{
			a = a1; b = b1;
		}

		a_len_squared = a.LengthSquared();
		b_len_squared = b.LengthSquared();

		n = Vector(obj->vData[0].Normal.x, obj->vData[0].Normal.y, obj->vData[0].Normal.z)
			+ Vector(obj->vData[1].Normal.x, obj->vData[1].Normal.y, obj->vData[1].Normal.z)
			+ Vector(obj->vData[2].Normal.x, obj->vData[2].Normal.y, obj->vData[2].Normal.z);
		n = Normalize(n);

		uv = Point(obj->vData[2].Tex.x,
			obj->vData[2].Tex.y,
			0.f);


	Creatmaterial(&mtl, basicMng.materialsManager.dxMaterial[unity.MaterialsIdIndex[0]]);
	mtl->LoadMaterial(basicMng.materialsManager.dxMaterial[unity.MaterialsIdIndex[0]]);

}

void RayRectangle::ViewTransform(Transform & View)
{

		o = View(o);
		n = View(n);
		a = View(a);
		b = View(b);
}

float RayRectangle::Pdf(Info& info)
{
	return (1.f / (a.Length()*b.Length()));
};

Point RayRectangle::GetSample(const Point &p)
{
	Point sampleP  = samplePTR->GetSquareSample(); 

	Point sp = o + a*sampleP.x + b*sampleP.y;
	return sp;
}