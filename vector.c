/**
 * @file
 * Hier ist die Datenhaltung und Programmlogik
 *
 * @author Tilman Nedele, Maurice Tollmien
 */

/* ---- System Header einbinden ---- */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <stringOutput.h>
#include <assert.h>

#ifdef MACOSX
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/* ---- Eigene Header einbinden ---- */
#include "types.h"


/**
 * Printet einen Vector aus.
 */
void printVector (CGVector3D a)
{
	int i;
	printf("\nprintVector:\n");
	for (i=0;i<3;i++)
		printf("%.1f\n", a[i]);
	printf("\n");
}

/**
 * Konvertierungsfunktion,
 * wandelt drei Koordinaten in den dazugehörigen Vektor um
 * @param x
 * @param y
 * @param z
 */
void toVector3D(CGVector3D vector, GLfloat x, GLfloat y, GLfloat z)
{
  vector[0] = x;
  vector[1] = y;
  vector[2] = z;
}

/**
 * Berechnet die Länge es Vektors
 * @param v
 *@return float
 */
float vectorLength3D(CGVector3D vector)
{
  return sqrt((vector[0]*vector[0])+
              (vector[1]*vector[1])+
              (vector[2]*vector[2]));
}

/**
 * Normiert eine Vektor
 * @param v der zu normierende Vektor
 * @return der normierte Vektor
 */
void normVector3D(CGVector3D vector)
{
  float l = vectorLength3D(vector);
  if (l != .0f)
    toVector3D(vector, vector[0]/l, vector[1]/l, vector[2]/l);
}

/**
 * Berechnet das Kreuzprodukt zweier Vektoren
 * @param
 * @param
 * @return
 */
void crossProduct3D(CGVector3D product, CGVector3D a, CGVector3D b)
{
  toVector3D(product, (a[1]*b[2] - a[2]*b[1]),
                      (a[2]*b[0] - a[0]*b[2]),
                      (a[0]*b[1] - a[1]*b[0]));
}

/**
 * Multipliziert zwei Vektoren miteinander (Skalarprodukt)
 */
double multiplyVectorVector (CGVector3D a, CGVector3D b)
{
	int i;
	double res = 0.0;
	for (i=0;i<3;i++)
		res += a[i]*b[i];
	return res;
}

/**
 * Multipliziert einen Vektor mit einem Skalar.
 */
void multiplyVectorScalar (CGVector3D a, double s, CGVector3D * res)
{
	int i;
	for (i=0;i<3;i++)
		(*res)[i] = a[i]*s;
}

/**
 * Zieht b von a ab, also: a-b
 */
void subtractVectorVector (CGVector3D a, CGVector3D b, CGVector3D * res)
{
	int i;
	for (i=0;i<3;i++)
		(*res)[i] = a[i] - b[i];
}

/**
 * Teilt den Vector a durch s.
 */
void divideVectorScalar (CGVector3D a, double s, CGVector3D * res)
{
	int i;
	for (i=0;i<3;i++)
		(*res)[i] = a[i] / s;
}

/**
 * Addiert a und b und schreibt das Ergebnis in res.
 */
void addVectorVector (CGVector3D a, CGVector3D b, CGVector3D * res)
{
	int i;
	for (i=0;i<3;i++)
		(*res)[i] = a[i] + b[i];
}

/**
 * Wandelt eine Zahl (Grad) in Radiant.
 * deg muss im Wertebereich 0..359 liegen!
 */
double degToRad (double deg)
{
	return deg*PI/180.0;
}

/**
 * Wandelt eine Zahl (Radiant) in Grad um.
 * deg muss im Wertebereich 0..PI liegen!
 */
double radToDeg (double rad)
{
	return rad*180.0/PI;
}

/**
 * Berechnet den Winkel zwischen zwei Vektoren und gibt das Ergebnis in 
 * ° zurück (nicht radiant!).
 */
double angleVectorVector (CGVector3D a, CGVector3D b)
{
	return radToDeg (acos (multiplyVectorVector (a, b) / (vectorLength3D(a)*vectorLength3D(b))));
}

void printVector3d(Vector3d v) {
	int i = 0;
	
	for (i=0; i<3; ++i) {
		printf("%i: %f; ",i,v[i]);
	}
	
	printf("\n");
}

double vectorLength3d(Vector3d v) {
	return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

void normalizeVector3d(Vector3d * v) {
	double norm;
	
	norm = vectorLength3d((*v));
	
	(*v)[0] = (*v)[0] / norm;
	(*v)[1] = (*v)[1] / norm;
	(*v)[2] = (*v)[2] / norm;
}

void crossProduct3d(Vector3d v1, Vector3d v2, Vector3d * res) {
	(*res)[0] = v1[1] * v2[2] - v1[2] * v2[1];
	(*res)[1] = v1[2] * v2[0] - v1[0] * v2[2];
	(*res)[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

double scalarProduct3d(Vector3d v1, Vector3d v2) {
	return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

void scalarMult3d(Vector3d v1, double s, Vector3d * res) {	
	(*res)[0] = v1[0] * s;
	(*res)[1] = v1[1] * s;
	(*res)[2] = v1[2] * s;
}

void rotateVectorVector3d(Vector3d * x, Vector3d n, double angle) {
	Vector3d res;
	
	Vector3d p1;
	Vector3d nxcross;
	Vector3d p2;
	Vector3d p22;
	Vector3d p3;
	
	/* n*(n*x) */
	scalarMult3d(n, scalarProduct3d(n, (*x)), &p1);
	
	/*printf("p1: ");
	printVector3d(p1);*/
	
	
	/* cos(alpha)*(nxx)xn */
	crossProduct3d(n, (*x), &nxcross);
	
	/*printf("nxcross: ");
	printVector3d(nxcross);*/
	
	scalarMult3d(nxcross, cos(angle * PI / 180), &p2);
	
	/*printf("p2pre: ");
	printVector3d(p2);*/
	
	crossProduct3d(p2, n, &p22);
	
	/*printf("p22: ");
	printVector3d(p22);*/
	
	/* */
	scalarMult3d(nxcross, sin(angle * PI / 180), &p3);
	
	
	
	
	res[0] = p1[0] + p22[0] + p3[0];
	res[1] = p1[1] + p22[1] + p3[1];
	res[2] = p1[2] + p22[2] + p3[2];
	
	(*x)[0] = res[0];
	(*x)[1] = res[1];
	(*x)[2] = res[2];
}













