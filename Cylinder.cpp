/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cylinder class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cylinder.h"
#include <math.h>

/**
* Cylinder's intersection method.  The input is a ray (pos, dir).
*/
float Cylinder::intersect(Vector pos, Vector dir)
{
	float a = dir.x * dir.x + dir.z * dir.z;
	float b = 2 * (dir.x * (pos.x - center.x) + dir.z * (pos.z - center.z));
	float c = (pos.x - center.x) * (pos.x - center.x) 
		+ (pos.z - center.z) * (pos.z - center.z) - radius * radius;
	float delta = b*b - 4 * a * c;

	float t1 = (-b + sqrt(delta)) / (2 * a);
	float t2 = (-b - sqrt(delta)) / (2 * a);

	if (t1>t2) { float tmp = t1; t1 = t2; t2 = tmp; }

	float y1 = (pos.y + t1 * dir.y) - center.y;
	float y2 = (pos.y + t2 * dir.y) - center.y;

	float ymin = 0;

	

	if (y1 < ymin)
	{
		if (y2 < ymin)
		{
			return -1;
		}
		else 
		{
			//hit the cap
			float th = t1 + (t2 - t1) * ((y1 + height) / (y1 - y2));
			if (th <= 0) return -1;
			
			return th;
		}
	}
	else if (y1 >= ymin && y1 <= height)
	{
		//hit side
		if (t1 <= 0)
		{
			return -1;
		}
		else
		{
			return t1;
		}
	}
	else if (y1 > height)
	{
		if (y2 > height)
		{
			return -1;
		}
		else
		{
			//hit cap
			float th = t1 + (t2 - t1) * ((y1 - ymin) / (y1 - y2));
			if (th <= 0) return -1;
			return th;
		}
	}

	return -1;
	
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the Cylinder.
*/
Vector Cylinder::normal(Vector p)
{
	Vector n = p - center;
	n.y = 0.0;
	n.normalise();
	return n;
}
