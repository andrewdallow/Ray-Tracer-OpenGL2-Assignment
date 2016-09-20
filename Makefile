# File:   Makefile
# Author: A. T. Dallow
# Date:   01 Apr 2015
# Descr:  Makefile for Raytracer
CC=g++
# Hey!, I am comment number 2. I want to say that CFLAGS will be the
# options I'll pass to the compiler.
CFLAGS=-Wall -Werror -lGLU -lGL -lglut

all: RayTracer

RayTracer: RayTracer.o Vector.o Sphere.o Plane.o Cylinder.o Cone.o Color.o Object.o
	$(CC) $(CFLAGS) RayTracer.o Vector.o Sphere.o Plane.o Cylinder.o Cone.o Color.o Object.o -o RayTracer

RayTracer.o: RayTracer.cpp Vector.h Sphere.h Plane.h Cylinder.h Cone.h Color.h Object.h
	$(CC) -c $(CFLAGS) RayTracer.cpp

Vector.o: Vector.cpp Vector.h
	$(CC) -c $(CFLAGS) Vector.cpp

Sphere.o: Sphere.cpp Sphere.h
	$(CC) -c $(CFLAGS) Sphere.cpp
	
Plane.o: Plane.cpp Plane.h Vector.h
	$(CC) -c $(CFLAGS) Plane.cpp
	
Cylinder.o: Cylinder.cpp Cylinder.h
	$(CC) -c $(CFLAGS) Cylinder.cpp

Cone.o: Cone.cpp Cone.h
	$(CC) -c $(CFLAGS) Cone.cpp	
	
Color.o: Color.cpp Color.h
	$(CC) -c $(CFLAGS) Color.cpp	
	
Object.o: Object.cpp Object.h
	$(CC) -c $(CFLAGS) Object.cpp

clean:
	rm *o

