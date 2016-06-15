/*
 * hmd.h
 *
 *  Author: Mervyn McCreight, Maurice Tollmien
 *  zuletzt geändert am: 4.4.2013
 */

/* Joystickbehandlungsfunktionen */
#include "joystick.h"

#define HMD_PI 3.141592654
#define JOY_DEV "/dev/input/js1"

/**
 * Liefert das Quaternion vom HMD normalisiert zurück.
 */
int getQuaternion(float q[4]);

/**
 * Initialisiert/Überprüft die Verbindung mit dem HMD.
 * @return 1 = okay; 0 = fehler
 */
int initializeHMD();

/**
 * Liefert die Roll- Pitch- und Yaw-Winkel
 * @param ypr YawPitchRoll
 */
int getYPR(float ypr[3]);

/**
 * Beendet die Verbindung zum HMD.
 */
int closeHMD();

/**
 * Liest die Events von dem HMD aus der Treiber-Queue.
 * Sollte kontinuierlich ausgeführt werden.
 */
void handleHMDEvent();
