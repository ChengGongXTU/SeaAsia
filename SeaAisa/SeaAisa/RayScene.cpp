#include "RayScene.h"

void RayUnity::CreateDxUnity(Unity & dxUnity)
{
	if (dxUnity.type == TriangleType)
		shape = new RayTriangleMesh[1];
	if (dxUnity.type == SphereType)
		shape = new RaySphere[1];
}
