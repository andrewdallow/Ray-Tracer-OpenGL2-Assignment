/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cone class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cone.h"
#include <math.h>

/**
* Cone's intersection method.  The input is a ray (pos, dir).
*/
float Cone::intersect(Vector pos, Vector dir)
{
	float a = dir.x * dir.x + dir.z * dir.z - dir.y * dir.y;
	float b = 2 * ((pos.x - center.x) * dir.x + 
		(pos.z - center.z) * dir.z - (pos.y - center.y) * dir.y);
	float c = (pos.x - center.x) * (pos.x - center.x) + 
		(pos.z - center.z) * (pos.z - center.z) - 
		(pos.y - center.y) * (pos.y - center.y);

	float delta = b*b - 4 * a * c;

	float t1 = (-b + sqrt(delta)) / (2 * a);
	float t2 = (-b - sqrt(delta)) / (2 * a);

	if (t1>t2) { float tmp = t1; t1 = t2; t2 = tmp; }

	float y1 = (pos.y + t1 * dir.y) - center.y;
	float y2 = (pos.y + t2 * dir.y) - center.y;

	float ymin = -height;



	if (y1 >= ymin && y1 <= 0)
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
	else if (y1 > 0)
	{
		if (y2 > 0)
		{
			return -1;
		}
		else
		{
			return t2;
		}
	}

	return -1;

}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the Cone.
*/
Vector Cone::normal(Vector p)
{
	Vector n = (p - center) * height / radius;
	n.y = (radius / height);
	n.normalise();
	return n;
}
