/*
 * hmd.h
 * 
 *  Author: Mervyn McCreight
 *  zuletzt geändert am: 10.4.2013
 */

/* - System Header einbinden */ 
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/* - eigene Header einbinden */
#include "hmd.h"

#define NORM_PITCH 1.188

/**
 * Zurückrechnung der Quaternionendaten vom HMD.
 * Gegeben als 16-Bit int, 2er Komplement.
 * @param x umzurechnender Wert
 * 
 * @return zurückgewandelter Wert.
 */
static int fromTwos(unsigned short x)
{
    int y = 0;
    x = x & 0xfff0; /* die letzten 4 stellen abschneiden */
   
    if (x > 32768) {
        x = ~x;
        y = x;
       
        if (y != 32767) ++y;
        y *= -1;
        return y;
    } else {
        return x;
    }
}
 

/**
 * Rechnet ein Einheits-Quaternion in eine Rotationsmatrix um.
 * @param q das Quaternion
 * @param m die Rotationsmatrix
 * 
 */ 
static void quaternionToMatrix2( float q[4], float m[3][3] )
{
   m[0][0] = q[3] * q[3] + q[0] * q[0] - q[1] * q[1] - q[2] * q[2];
   m[0][1] = 2 * (q[0] * q[1] + q[2] * q[3]);
   m[0][2] = 2 * (q[0] * q[2] - q[1] * q[3]);

   m[1][0] = 2 * (q[0] * q[1] - q[2] * q[3]);
   m[1][1] = q[3] * q[3] - q[0] * q[0] + q[1] * q[1] - q[2] * q[2];
   m[1][2] = 2 * (q[1] * q[2] + q[0] * q[3]);

   m[2][0] = 2 * (q[0] * q[2] + q[1] * q[3]);
   m[2][1] = 2 * (q[1] * q[2] - q[0] * q[3]);
   m[2][2] = q[3] * q[3] - q[0] * q[0] - q[1] * q[1] + q[2] * q[2];
}

/**
 * Rechnet ein Einheits-Quaternion in eine Rotationsmatrix um.
 * @param q das Quaternion
 * @param m die Rotationsmatrix
 * 
 */ 
 static void quaternionToMatrix (float q[4], float m[3][3])
 {
	 /* http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/ */
	 /**
	  * w = 0; x = 1; y = 2; z = 3.
	  */
	 
	 m[0][0] = 1 - 2 * q[2] * q[2] - 2 * q[3] * q[3];
	 m[0][1] = 2 * q[1] * q[2] - 2 * q[3] * q[0];
	 m[0][2] = 2 * q[1] * q[3] + 2 * q[2] * q[0];
	 
	 m[1][0] = 2 * q[1] * q[2] + 2 * q[3] * q[0];
	 m[1][1] = 1 - 2 * q[1] * q[1] - 2 * q[3] * q[3];
	 m[1][2] = 2 * q[2] * q[3] - 2 * q[1] * q[0];
	 
	 m[2][0] = 2 * q[1] * q[3] - 2 * q[2] * q[0];
	 m[2][1] = 2 * q[2] * q[3] + 2 * q[1] * q[0];
	 m[2][2] = 1 - 2 * q[1] * q[1] - 2 * q[2] * q[2];
 }

static float norm(float i) {
    float k = (1 + 1)/(NORM_PITCH+NORM_PITCH);
    
    return (i + NORM_PITCH) * k - 1;
    
}

/**
 * Errechnet basierend auf eine Rotationsmatrix
 * Roll-, Pitch- und Yawwinkel.
 * @param m Rotationsmatrix
 * @param ypr YawPitchRoll
 */
static void matrixToYPR( float m[3][3], float ypr[3] )
{
   ypr[0] = atan2( m[0][1], m[0][0] );    
   
   
   ypr[1] = asin( norm(m[0][2]) );  
   
    
   printf("m: %f\n", m[0][2]);           
   ypr[2] = -atan2( m[1][2], m[2][2] );   
}
 
/**
 * Berechnet die Länge eines Quaternions.
 * @param q das Quaternion
 * @return die Länge
 */  
static float quaternionLength (float q[4]) {
	return sqrt(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
}

/**
 * Normalisiert ein Quaternion.
 * @param q das Quaternion
 */
static void normalizeQuaternion (float q[4]) {
	q[0] = q[0] / quaternionLength (q);
	q[1] = q[1] / quaternionLength (q);
	q[2] = q[2] / quaternionLength (q);
	q[3] = q[3] / quaternionLength (q);
}

/**
 * Liefert das Quaternion vom HMD normalisiert zurück.
 */
int getQuaternion(float q[4]) {
	short a,b,c,d;
	
	/* im Fehlerfall gib 0 zurück. */
	if (!(getAxisValue(3, &a)
		&& getAxisValue(4, &b)
		&& getAxisValue(0, &c)
		&& getAxisValue(1, &d))) {
			return 0;
	}
	
	q[0] = fromTwos(a+32768) / 16384.0;
    q[1] = fromTwos(b+32768) / 16384.0;
    q[2] = fromTwos(c+32768) / 16384.0;
    q[3] = fromTwos(d+32768) / 16384.0;
    normalizeQuaternion(q);
    
    return 1;
    /*fprintf(stdout, "q[0]: %i, q[1]: %i,q[2]: %i,q[3]: %i\n", q[0],q[1],q[2],q[3] );*/
}

/**
 * Initialisiert/Überprüft die Verbindung mit dem HMD.
 * @return 1 = okay; 0 = fehler
 */
int initializeHMD() {
	if  (!startDeviceConnection(JOY_DEV)) {
		return 0;
	}
	
	if (!setCalibrationCoefficients(24617, 40917, 21844, 21844, 1, 255)) {
		return 0;
	}
	
	return 1; 
}


/**
 * Liefert die Roll- Pitch- und Yaw-Winkel
 * @param ypr YawPitchRoll
 */
int getYPR(float ypr[3]) {
	float Q[4];
	float R[3][3];
	
	if (!getQuaternion(Q)) {
		return 0;
	}
	quaternionToMatrix(Q, R);
	matrixToYPR(R, ypr);
	
	ypr[0] = ypr[0] / HMD_PI * 180;
	ypr[1] = ypr[1] / HMD_PI * 180;
	ypr[2] = ypr[2] / HMD_PI * 180;
	
	return 1;
}

/**
 * Beendet die Verbindung zum HMD.
 */
int closeHMD() {
	return endDeviceConnection();
}

/**
 * Liest die Events von dem HMD aus der Treiber-Queue.
 * Sollte kontinuierlich ausgeführt werden.
 */
void handleHMDEvent() {
	handleJoystickEvents();
}

/*
 * MODUL-TEST
int main(void) {
	
	float ypr[3];
	initializeHMD();
	
	while (1) {
		if (!getYPR(ypr)) {
			printf("idiot!\n");
			exit(1);
		}
		printf("yaw: %f, pitch: %f, roll: %f\n", ypr[0], ypr[1], ypr[2]);
	}
}
*/

