// ========================================================================
// COSC 363  Computer Graphics  Lab07
// A simple ray tracer
// ========================================================================

#include <iostream>
#include <cmath>
#include <vector>
#include "Vector.h"
#include "Sphere.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Cone.h"
#include "Color.h"
#include "Object.h"
#include <GL/glut.h>
using namespace std;

const float WIDTH = 20;  
const float HEIGHT = 20;
const float EDIST = 40;
const int PPU = 30;     //Total 600x600 pixels
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;

vector<Object*> sceneObjects;

Vector light = Vector(-10.0, 20.0, -5.0);
Color backgroundCol = Color::GRAY;

//A useful struct
struct PointBundle   
{
	Vector point;
	int index;
	float dist;
};

/*
* This function compares the given ray with all objects in the scene
* and computes the closest point  of intersection.
*/
PointBundle closestPt(Vector pos, Vector dir)
{
    Vector  point(0, 0, 0);
	float min = 10000.0;

	PointBundle out = {point, -1, 0.0};

    for(unsigned int i = 0;  i < sceneObjects.size();  i++)
	{
        float t = sceneObjects[i]->intersect(pos, dir);
		if(t > 0)        //Intersects the object
		{
			point = pos + dir*t;
			if(t < min)
			{
				out.point = point;
				out.index = i;
				out.dist = t;
				min = t;
			}
		}
	}

	return out;
}


Vector refraction(Vector lightVector, Vector n, float n1, float n2)
{
	float indexRatio = n1 / n2;
	float lDotn = lightVector.dot(n);

	float cosThetaT = sqrt(1 - (indexRatio*indexRatio) * (1 - lDotn*lDotn));
	Vector refractionVector = (lightVector * indexRatio) - n * (indexRatio * lDotn + cosThetaT);
	refractionVector.normalise();

	return refractionVector;

}
/*
* Computes the colour value obtained by tracing a ray.
* If reflections and refractions are to be included, then secondary rays will 
* have to be traced from the point, by converting this method to a recursive
* procedure.
*/

Color trace(Vector pos, Vector dir, int step)
{
    PointBundle q = closestPt(pos, dir);

    if(q.index == -1) return backgroundCol;        //no intersection
	Color colorSum;
    
	Vector n = sceneObjects[q.index]->normal(q.point);
	Color col = sceneObjects[q.index]->getColor(); //Object's colour

	Vector v(-dir.x, -dir.y, -dir.z); //View Vector

	Vector l = light - q.point;
	l.normalise();
	float lDotn = l.dot(n);

	Vector lightVector = light - q.point;
	float lightDist = lightVector.length();
	lightVector.normalise();

	PointBundle s = closestPt(q.point, lightVector);

	if (s.index > -1 && s.dist < lightDist) // Background
	{
		colorSum = col.phongLight(backgroundCol, 0.0, 0.0);
	}
	else
	{
		//Shaded areas
		if (lDotn <= 0)
		{
			colorSum = col.phongLight(backgroundCol, 0.0, 0.0);

		}
		//Colours and shadows
		else
		{
			Vector r = ((n * 2) * lDotn) - l;
			r.normalise();
			
			float rDotv = r.dot(v);
			float spec;
			if (rDotv < 0)
			{
				spec = 0.0;
			}
			else
			{
				spec = pow(rDotv, 10);
			}

			colorSum = col.phongLight(backgroundCol, lDotn, spec);

		}


	}
	
	//Checker Floor
	if (q.index == 3 )
	{
		if (remainder(q.point.z, 6) >= 0 && remainder(q.point.z, 6) < 3)
		{
			if (remainder(q.point.x, 6) >= 0 && remainder(q.point.x, 6) < 3)
			{
				colorSum.combineColor(Color(0.5, 1, 1), 0.5);
			}
			else
			{
				colorSum.combineColor(Color(0, 0, 0.4), 0.5);
			}
		}
		else
		{
			if (remainder(q.point.x, 6) >= 0 && remainder(q.point.x, 6) < 3)
			{
				
				colorSum.combineColor(Color(0, 0, 0.4), 0.5);
			}
			else
			{
				colorSum.combineColor(Color(0.5, 1, 1), 0.5);
			}
		}
	}
	//textured sphere
	if (q.index == 1)
	{
		float colorR = (1 + sin(q.point.x * 3)) / 2;
		float colorG = (1 + cos(q.point.x * 3)) / 2;

		colorSum.combineColor(Color(colorR, colorG, 0), 0.6);
	}

	//Generate Reflections
	if ((q.index == 0 || q.index == 3) && step < MAX_STEPS)
	{
		float nDotv = n.dot(v);
		Vector refelectionVector = ((n * 2) * nDotv) - v;
		refelectionVector.normalise();

		float reflCoeff = 0.4;

		Color refelectionCol = trace(q.point, refelectionVector, step + 1);
		colorSum.combineColor(refelectionCol, reflCoeff);
	}

	//Generate Refractions
	if (q.index == 2 && step < MAX_STEPS)
	{
		float n1 = -0.01;
		float n2 = 0.115;
		float refrCoeff = 0.4;

		Vector refractionVector1 = refraction(v, n, n1, n2);
		PointBundle refra = closestPt(q.point, refractionVector1);
		Vector norm2 = sceneObjects[q.index]->normal(refra.point);
		Vector refractionVector2 = refraction(refractionVector1, norm2 * -1, n2, n1);
		

		Color refractionCol = trace(refra.point, refractionVector2, step);
		colorSum.combineColor(refractionCol, refrCoeff);
	}

	return colorSum;

}



//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each pixel as quads.
//---------------------------------------------------------------------------------------
void display()
{
	int widthInPixels = (int)(WIDTH * PPU);
	int heightInPixels = (int)(HEIGHT * PPU);
	float pixelSize = 1.0/PPU;
	float halfPixelSize = pixelSize/2.0;
	float x1, y1, xc, yc;
	Vector eye(0., 0., 0.);

	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_QUADS);  //Each pixel is a quad.

	for(int i = 0; i < widthInPixels; i++)	//Scan every "pixel"
	{
		x1 = XMIN + i*pixelSize;
		xc = x1 + halfPixelSize;
		for(int j = 0; j < heightInPixels; j++)
		{
			y1 = YMIN + j*pixelSize;
			yc = y1 + halfPixelSize;

		    Vector dir(xc, yc, -EDIST);	//direction of the primary ray

		    dir.normalise();			//Normalise this direction

		    Color col = trace (eye, dir, 1); //Trace the primary ray and get the colour value
			glColor3f(col.r, col.g, col.b);
			glVertex2f(x1, y1);				//Draw each pixel with its color value
			glVertex2f(x1 + pixelSize, y1);
			glVertex2f(x1 + pixelSize, y1 + pixelSize);
			glVertex2f(x1, y1 + pixelSize);
        }
    }

    glEnd();
    glFlush();
}



void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0, 0, 0, 1);

	
	Sphere *sphere2 = new Sphere(Vector(0, 1, -60), 8.0, Color::GREEN);
	sceneObjects.push_back(sphere2);
	Sphere *sphere1 = new Sphere(Vector(-10, -7, -60), 3.0, Color::BLUE);
	sceneObjects.push_back(sphere1);
	Sphere *sphere3 = new Sphere(Vector(4, -4.5, -42), 2.0, Color::GRAY);
	sceneObjects.push_back(sphere3);
	//Floor
	Plane *plane = new Plane(
		Vector(-34, -10, -35), 
		Vector(34, -10, -35),
		Vector(34, -10, -120), 
		Vector(-34, -10, -120), 
		Color(0.2, 0.2, 0.2));
	sceneObjects.push_back(plane);
	//Back Wall
	Plane *wall1 = new Plane(
		Vector(-34, -10, -120), 
		Vector(34, -10, -120), 
		Vector(34, 30, -120), 
		Vector(-34, 30, -120),
		Color(0.6, 0.6, 0));
	sceneObjects.push_back(wall1);
	//Right Wall
	Plane *wall2 = new Plane(
		Vector(34, -10, -120),
		Vector(34, -10, -35),
		Vector(34, 30, -35),
		Vector(34, 30, -120),
		Color(0.8, 0.1, 0));
	sceneObjects.push_back(wall2);
	//Left Wall
	Plane *wall3 = new Plane(
		Vector(-34, -10, -120),
		Vector(-34, 30, -120),
		Vector(-34, 30, -35),
		Vector(-34, -10, -35),
		Color(0.8, 0.1, 0));
	sceneObjects.push_back(wall3);


	//Cube
	float size = 4;
	float theta = 45;
	Vector axis = Vector(0, 1, 0);
	Vector trans = Vector(5, -10, -55);
	Color cube_color = Color(0.98, 0.49, 0.14);

	//bottom
	Plane *cube1 = new Plane(
		Vector(0, 0, 0).rotation(theta, axis) + trans,
		Vector(size, 0, 0).rotation(theta, axis) + trans,
		Vector(size, 0, size).rotation(theta, axis) + trans,
		Vector(0, 0, size).rotation(theta, axis) + trans,
		cube_color);
	sceneObjects.push_back(cube1);
	//back
	Plane *cube2 = new Plane(
		Vector(0, 0, size).rotation(theta, axis) + trans,
		Vector(size, 0, size).rotation(theta, axis) + trans,
		Vector(size, size, size).rotation(theta, axis) + trans,
		Vector(0, size , size).rotation(theta, axis) + trans,
		cube_color);
	sceneObjects.push_back(cube2);
	//right side
	Plane *cube3 = new Plane(
		Vector(size, 0, 0).rotation(theta, axis) + trans,
		Vector(size, size, 0).rotation(theta, axis) + trans,
		Vector(size, size, size).rotation(theta, axis) + trans,
		Vector(size, 0, size).rotation(theta, axis) + trans,
		cube_color);
	sceneObjects.push_back(cube3);
	//top
	Plane *cube4 = new Plane(
		Vector(0, size, 0).rotation(theta, axis) + trans,
		Vector(0, size, size).rotation(theta, axis) + trans,
		Vector(size, size, size).rotation(theta, axis) + trans,
		Vector(size, size, 0).rotation(theta, axis) + trans,
		cube_color);
	sceneObjects.push_back(cube4);
	//left side
	Plane *cube5 = new Plane(
		Vector(0, 0, 0).rotation(theta, axis) + trans,
		Vector(0, 0, size).rotation(theta, axis) + trans,
		Vector(0, size, size).rotation(theta, axis) + trans,
		Vector(0, size , 0).rotation(theta, axis) + trans,
		cube_color);
	sceneObjects.push_back(cube5);
	//front
	Plane *cube6 = new Plane(
		Vector(0, 0, 0).rotation(theta, axis) + trans,
		Vector(0, size, 0).rotation(theta, axis) + trans,
		Vector(size, size, 0).rotation(theta, axis) + trans,
		Vector(size, 0, 0).rotation(theta, axis) + trans,
		cube_color);
	sceneObjects.push_back(cube6);

	Sphere *sphere4 = new Sphere(Vector(-7, -8, -55), 2.0, Color::YELLOW);
	sceneObjects.push_back(sphere4);

	Sphere *sphere5 = new Sphere(Vector(-5, -9, -50), 1.0, Color::GRAY);
	sceneObjects.push_back(sphere5);

	Cone *cone = new Cone(Vector(0, -7, -60), 2.0, 3, Color::YELLOW);
	sceneObjects.push_back(cone);

	Cylinder *cylinder2 = new Cylinder(Vector(-15, -9.9999, -70), 2.0, 30, Color(0.98, 0.49, 0.14));
	sceneObjects.push_back(cylinder2);
	Cylinder *cylinder3 = new Cylinder(Vector(-15, -9.9999, -70), 5.0, 2, Color(0.98, 0.49, 0.14));
	sceneObjects.push_back(cylinder3);

	Cylinder *cylinder4 = new Cylinder(Vector(15, -9.9999, -70), 2.0, 30, Color(0.98, 0.49, 0.14));
	sceneObjects.push_back(cylinder4);
	Cylinder *cylinder5 = new Cylinder(Vector(15, -9.9999, -70), 5.0, 2, Color(0.98, 0.49, 0.14));
	sceneObjects.push_back(cylinder5);

	

}


int main(int argc, char *argv[]) 
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracing");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
