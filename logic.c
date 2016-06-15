
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
#include "logic.h"
#include "texture.h"
#include "scene.h"
#include "types.h"
#include "vector.h"
#include "quaternions.h"

/* Globale Variablen */
/** Licht an/aus */
InitType G_Light = ON;
/** Hilfe an/aus */
InitType G_Help  = OFF;
/** Textur an/aus */
InitType G_Texture = ON;
/** Mausbewegung zoom/move/none */
MouseInterpretType G_Mouse;
/** Kameraposition */
CGVector3D G_CameraPosition = {CAMERA_X,CAMERA_Y,CAMERA_Z};
/** Position der Maus */
Movement G_MouseMove = {0.0,0.0,0.0};
/** Startpunkt der Mausbewegung */
CGVector3D G_LastMouseCenter = {0.0,0.0,0.0};
/** Anzahl der Quadrate für den Boden */
int G_QuadCount = 5;
/** Welche Textur soll angezeigt werden */
int G_TextureCount = 5;
/** Partikel hinzufügen? */
int G_UpDownKeys[2] = {0,0};
/** FPS */
int G_FPS = 0;
/** FPS counter */
double G_Counter = 0.0-EPS;

int G_MouseCameraStatus = 0;

double G_CameraDirections[3] = {CAMERA_DIR_X,CAMERA_DIR_Y,CAMERA_DIR_Z};

/** Globales HMD-Quaternion */
Quaternion G_Quaternion;

/** Rotiert der Wuerfel ? */
static int g_rotationTypes = 0;


/** Position der Maus */
Movement G_QuaternionMove = {0.0,0.0,0.0};
/** Startpunkt der Mausbewegung */
CGVector3D G_LastQuaternionCenter = {0.0,0.0,0.0};

/** Das sich zu drehende Objekt */
Vertex * G_Object;
/** Ursprungskoordinaten des Objekts */
Vertex * G_StartObject;

/** Anzahl der Punkte */
int G_ObjectSize;
/** Die Indizes von Object */
int G_IndicesCount = 0;
/** Indices */
GLuint * G_Indices;
/** Mouse Action */
MouseType G_MouseType;

/** Rotationswinkel des Wuerfels. */
static float g_rotationAngleX = 0.0f;
static float g_rotationAngleY = 0.0f;
static float g_rotationAngleZ = 0.0f;

GLfloat G_up[3] = {0.0,0.0,0.0};
GLfloat G_center[3] = {0.0,0.0,0.0};
GLfloat G_eye[3] = {0.0,0.0,0.0};

/* ------- GETTER / SETTER ------- */

double getCenter (int pos)
{
	return G_center[pos];
}

double getUp (int pos)
{
	return G_up[pos];
}

/**==============================*/

double * rotateVectorVectorVector3d (Vector3d axis, Vector3d vector, double angle)
{
	Quaternion q = malloc (sizeof (*q));
	Quaternion p = malloc (sizeof (*p));
	Quaternion res = malloc (sizeof (*res));
	double * resV = malloc (3 * sizeof (double));
	
	p->s = 0.0;
	p->v[0] = vector[0];
	p->v[1] = vector[1];
	p->v[2] = vector[2];
	
	q->s = cos (angle * PI / 180 / 2);
	q->v[0] = sin (axis[0] / 180 * PI / 2);
	q->v[1] = sin (axis[1] / 180 * PI / 2);
	q->v[2] = sin (axis[2] / 180 * PI / 2);
	
	/*  ---  q p q*  ---  */
	res = multQuaterionQuaterion (q, p);
	
	res = multQuaterionQuaterion (res, inverseQuaternion (q));
	
	resV[0] = res->v[1];
	resV[1] = res->v[2];
	resV[2] = res->v[3];
	
	return resV;
}

/**==============================*/

/**
 * Berechnet aktuellen Rotationswinkel des Wuerfels.
 * @param interval Dauer der Bewegung in Sekunden.
 */
void
calcRotation (double interval)
{
	float ypr[3] = {0.0,0.0,0.0};
    float q[4] = {0.0,0.0,0.0,0.0};
    
	getYPR(ypr);
	
	

    getQuaternion (q);
    
    /*printf("A: %f, B: %f, C: %f D: %f\n", q[0], q[1], q[2], q[3]);*/
    
    
    
    G_Quaternion->s     = q[0];
    G_Quaternion->v[0]  = q[1];
    G_Quaternion->v[1]  = q[2];
    G_Quaternion->v[2]  = q[3];
    
    /*
    setQuaternionMovement ();
    */
    
    /*printf("Yaw: %f, Pitch: %f, Roll: %f\n", ypr[0], ypr[1], ypr[2]);*/
    
    /* printf("A: %f, B: %f, C: %f D: %f\n", G_Quaternion->s, G_Quaternion->v[0], G_Quaternion->v[1], G_Quaternion->v[2]);*/
    
    g_rotationAngleY = ypr[0];
    g_rotationAngleX = ypr[1];
    printf  ("ypr x : %f\n", ypr[1]);
    g_rotationAngleZ = ypr[2];
    
      
}

double getCameraDirection(int axis)
{
    return G_CameraDirections[axis];
}

int getMouseCameraStatus (void) 
{
    return G_MouseCameraStatus;
}

void toggleMouseCameraStatus (void)
{
    G_MouseCameraStatus = !G_MouseCameraStatus;
}

int isCameraMode (void)
{
	return G_MouseType == camera;
}

/**
 * Schaltet um, ob mit einer Mouse-Bewegung entweder die Kamera
 * bewegt werden soll, oder das Objekt gedreht (Quaternionen!).
 */
void toggleMouseAction (void)
{
	G_MouseType = (G_MouseType == camera) ? quaternion : camera;
	printf("Mouse type set to: ");
	printf ("%s\n", (G_MouseType == quaternion) ? "Quaternion" : "Camera");
}

int getFPS(void)
{
	return G_FPS;
}

int getObjectIndicesCount(void)
{
	return G_IndicesCount;
}

GLuint * getObjectIndices(void)
{
	return G_Indices;
}

/**
 * Liefert den aktuellen Rotationsstatus des Wuerfels.
 * @return aktueller Rotationsstatus des Wuerfels.
 */
int
getRotationTypes (void)
{
  return g_rotationTypes;
}

/**
 * Setzt den Rotationsstatus des Wuerfels.
 * @param rotationStatus Rotationsstatus des Wuerfels.
 */
void
setRotationTypes (int rotationStatus)
{
    g_rotationTypes = rotationStatus;
    if (!g_rotationTypes)
        printf ("Rotation wird durch Quaternionen berechnet!\n");
    else
        printf ("Rotation wird durch Eulerwinkel berechnet!\n");
}


/**
 * Setzt, ob ein Up/Down-Key gedrückt ist.
 * key == 0 = down
 * key == 1 = up
 * value = gesetzt/nicht gesetzt.
 */
void setKey (int key, int value)
{
    if (key < 2)
        G_UpDownKeys[key] = value;
}

/**
 * Get-Function für den Status des Lichts
 * @return Status des Lichts
 */
InitType getLightStatus() {
    return G_Light;
}

/**
 * Set-Function für den Status des Lichts
 * @param Status des Lichts
 */
void setLightStatus(InitType state) {
    G_Light = state;
}

void toggleLight (void)
{
	G_Light = !G_Light;
}

/**
 * Get-Function für den Status der Hilfe
 * @return Status der Hilfe
 */
InitType getHelpStatus() {
    return G_Help;
}

/**
 * Set-Function für den Status der Hilfe
 * @param Status der Hilfe
 */
void setHelpStatus(InitType state) {
    G_Help = state;
}

/**
 * Get-Function für den Status der Texturen
 * @return Status der Texturen
 */
InitType getTextureStatus() {
    return G_Texture;
}

/**
 * Set-Function für den Status der Textur
 * @param Status der Textur
 */
void setTextureStatus(InitType state) {
    G_Texture = state;
}

/**
 * Set-Function für den Status der Maus
 * @param Status der Maus
 */
void setMouseEvent(MouseInterpretType state,int x, int y) {
    G_MouseMove[0] = 0.0;
    G_MouseMove[2] = 0.0;
    G_LastMouseCenter[0] = x;
    G_LastMouseCenter[2] = y;
    
    
    G_Mouse = state;
}

/**
 * Get-Function für den Status der Maus
 * @return Status der Maus
 */
MouseInterpretType getMouseEvent() {
    return G_Mouse;
}

/**
 * Gibt die Kamerakoordinate zurück.
 */
double getCameraPosition (int axis)
{
    if (axis >= 0 && axis < 3)
        return G_CameraPosition[axis];
        
    return 0.0;
}

/**
 * Getter für die aktuelle Textur
 */
int getTextureCount() {
    return G_TextureCount;
}

/**
 * Setzt die Textur weiter 
 */
void nextTexture() {
    if (G_TextureCount < TEX_COUNT)
        ++G_TextureCount;
    else
        G_TextureCount = 0;
}

/**
 * Erhöht die Größe der Unterteilungen um den Faktor 2
 */
void incQuadCount() {
    if (G_QuadCount < 9)
        ++G_QuadCount;
}

/**
 * Verkleinert die Größe der Unterteilungen um den Divisor 2
 */
void decQuadCount() {
    if (G_QuadCount > 2)
        --G_QuadCount;
}

/**
 * Rekursionstiefe für die Bodenfläche
 */
int getQuadCount() {
    return G_QuadCount;
}

/**
 * Bewegt die Kamera auf einem Kugelradius um den Szenenmittelpunkt.
 */
void setCameraMovement(int x,int y)
{
	CGVector3D tmp;
	double factor, radius = vectorLength3D(G_CameraPosition);
	
	tmp[0] = G_CameraPosition[0];
	tmp[1] = G_CameraPosition[1];
	tmp[2] = G_CameraPosition[2];
	
	G_MouseMove[0] = x-G_LastMouseCenter[0];
	G_MouseMove[2] = y-G_LastMouseCenter[2];
	
	G_LastMouseCenter[0] = x;
	G_LastMouseCenter[2] = y;
	
	/* Bewegung um Y-Achse: */
	G_CameraPosition[0] = cos(-G_MouseMove[0]*PI/180.0/CAMERA_MOVEMENT_SPEED)*tmp[0] + sin(-G_MouseMove[0]*PI/180.0/CAMERA_MOVEMENT_SPEED)*tmp[2];
	G_CameraPosition[2] = -sin(-G_MouseMove[0]*PI/180.0/CAMERA_MOVEMENT_SPEED)*tmp[0] + cos(-G_MouseMove[0]*PI/180.0/CAMERA_MOVEMENT_SPEED)*tmp[2];
	
	/* Bewegung oben/unten */
	G_CameraPosition[1] += G_MouseMove[2]/(CAMERA_MOVEMENT_SPEED/2)*(vectorLength3D(G_CameraPosition)/100.0);
	factor = 1.0 / (vectorLength3D(G_CameraPosition) / radius);
	multiplyVectorScalar (G_CameraPosition, factor, &G_CameraPosition);
		
}

/**
 * Bewegt alle Punkte des Objektes um einen kleinen Winkel und nutzt dabei Quaternionen.
 */
void setQuaternionMovement (void)
{
	int i;
    
    initObject ();
	
	for (i=0;i<G_ObjectSize; i++)
	{
		/* Quaternion des Punktes */
		Quaternion p = malloc(sizeof(*p));
		Quaternion qtmp = malloc(sizeof(*qtmp));
		Quaternion res = malloc(sizeof(*res));
		
		p->s = 0.0;
		p->v[0] = G_Object[i][0];
		p->v[1] = G_Object[i][1];
		p->v[2] = G_Object[i][2];
		
		qtmp->s = G_Quaternion->s;
		qtmp->v[0] = G_Quaternion->v[0];
		qtmp->v[1] = G_Quaternion->v[1];
		qtmp->v[2] = G_Quaternion->v[2];
		
		/*  ---  q p q*  ---  */
		res = multQuaterionQuaterion (qtmp, p);
		
		res = multQuaterionQuaterion (res, inverseQuaternion (qtmp));
		
		/* Ergebnis zurück schreiben */
		G_Object[i][0] = res->v[0];
		G_Object[i][1] = res->v[1];
		G_Object[i][2] = res->v[2];
		
		free(res);
		free(p);
		free(qtmp);
	}

}

/**
 * Berechnet die neue Kamerarichtung mithilfe des Quaternions aus der HMD.
 */
void updateCameraPosition2 (void)
{
    /* Quaternion des Punktes */
    Quaternion p = malloc(sizeof(*p));
    Quaternion qtmp = malloc(sizeof(*qtmp));
    Quaternion res = malloc(sizeof(*res));
 
    /* Zurücksetzen der Werte!!! */
    G_CameraDirections[0] = CAMERA_DIR_X;
    G_CameraDirections[1] = CAMERA_DIR_Y;
    G_CameraDirections[2] = CAMERA_DIR_Z;
    
    p->s = 0.0;
    p->v[0] = G_CameraDirections[0];
    p->v[1] = G_CameraDirections[1];
    p->v[2] = G_CameraDirections[2];
    
    qtmp->s = G_Quaternion->s;
    qtmp->v[0] = G_Quaternion->v[0];
    qtmp->v[1] = G_Quaternion->v[1];
    qtmp->v[2] = G_Quaternion->v[2];
    
    /*  ---  q p q*  ---  */
    res = multQuaterionQuaterion (qtmp, p);
    
    res = multQuaterionQuaterion (res, inverseQuaternion (qtmp));
    
    /* Ergebnis zurück schreiben */
    G_CameraDirections[0] = res->v[0];
    G_CameraDirections[1] = res->v[1];
    G_CameraDirections[2] = res->v[2];
    
    free(res);
    free(p);
    free(qtmp);
}

/**
 * Erstellt aus den Drehungen Yaw, Pitch, Roll ein (mehrere) Quaternionen
 * und wendet diese passend auf den Up- und Center-Vector an.
 */
void cameraCalcYprToQuaternion (void)
{
    Quaternion qr = malloc (sizeof(*qr));
    Quaternion qp = malloc (sizeof(*qp));
    Quaternion qy = malloc (sizeof(*qy));
    Quaternion qres = malloc (sizeof(*qres));
    Quaternion p = malloc (sizeof (*p));
    Vector3d upTmp = {0.0,0.0,0.0};
    Vector3d centerTmp = {0.0,0.0,0.0};
    Vector3d axis = {0.0,0.0,0.0};
    int i;
    
    /* Up- und Centervector zwischenspeichern */
    for (i=0;i<3;i++) upTmp[i] = G_up[i];
    for (i=0;i<3;i++) centerTmp[i] = G_center[i];
        
    printf ("logic up: %f, %f, %f\n", G_up[0],G_up[1],G_up[2]);
        
    /* Up-Vector */
        /* Axis für qr (roll) */
        for (i=0;i<3;i++) axis[i] = upTmp[i];
        /* qr erstellen */
        qr->s = cos ((g_rotationAngleZ/2.0) * PI / 180.0);
        printf ("qr->s = %f\n", qr->s);
        for (i=0;i<3;i++) qr->v[i] = sin ((g_rotationAngleZ/2.0) * PI / 180.0) * axis[i];
        
        /* Axis für qp (pitch) */
        crossProduct3D (axis, upTmp, centerTmp);
        
        printVector (axis);
        printf("g_rotationAngle... %f\n", g_rotationAngleX);
        /* qp erstellen */
        qp->s = cos ((g_rotationAngleX/2.0) * PI / 180.0);
        printf ("pq->s : %f\n", cos ((g_rotationAngleX/2.0) * PI / 180.0));
        for (i=0;i<3;i++) qp->v[i] = sin ((g_rotationAngleX/2.0) * PI / 180.0) * axis[i];
        
        printVector (qp->v);
        
        qr = normQuaternion (qr);
        qp = normQuaternion (qp);
        
        /* qr * qp */
        qres = multQuaterionQuaterion (qr, qp);
        
        /* p erstellen */
        p->s = 0.0;
        for (i=0;i<3;i++) p->v[i] = upTmp[i];
        
        qres = normQuaternion (qres);
        p = normQuaternion (p);
        
        /* q * p * q* */
        qres = multQuaterionQuaterion (multQuaterionQuaterion (qres, p), inverseQuaternion (qres));
        qres = normQuaternion (qres);
        /* up sichern */
        for (i=0;i<3;i++) G_up[i] = qres->v[i];
        
        printf ("logic up: %f, %f, %f\n", G_up[0],G_up[1],G_up[2]);
        /*exit(1);*/
        
    /* Center-Vector */
        /* Axis für qp (pitch) */
        crossProduct3D (axis, upTmp, centerTmp);
        /* qp erstellen */
        qp->s = cos ((g_rotationAngleX/2.0) * PI / 180.0);
        for (i=0;i<3;i++) qp->v[i] = sin ((g_rotationAngleX/2.0) * PI / 180.0) * axis[i];
        
        /* qy erstellen */
        qy->s = cos ((g_rotationAngleY/2.0) * PI / 180.0);
        for (i=0;i<3;i++) qy->v[i] = sin ((g_rotationAngleY/2.0) * PI / 180.0) * upTmp[i];
        
        qy = normQuaternion (qr);
        qp = normQuaternion (qp);
        
        /* qp * qy */
        qres = multQuaterionQuaterion (qp, qy);
        
        qres = normQuaternion (qres);
        
        /* p erstellen */
        p->s = 0.0;
        for (i=0;i<3;i++) p->v[i] = centerTmp[i];
        
        qres = normQuaternion (qres);
        p = normQuaternion (p);
        
        /* q * p * p* */
        qres = multQuaterionQuaterion (multQuaterionQuaterion (qres, p), inverseQuaternion (qres));
        
        qres = normQuaternion (qres);
        
        /* center sichern */
        for (i=0;i<3;i++) G_center[i] = qres->v[i];
        
    
}

/**
 * Berechnet die neue Kamerarichtung mithilfe des Quaternions aus der HMD.
 */
void updateCameraPosition (void)
{
    if (1) {
        int i;
        Vector3d axis={0.0,0.0,0.0};
        Vector3d vector={0.0,0.0,0.0};
        double angle=0.0;
        
        G_center[0]=0.0;
        G_center[1]=0.0;
        G_center[2] = -1.0;

        G_up[0] = 0.0;
        G_up[1] = 1.0;
        G_up[2] = 0.0;
        
        
             /* Yaw */
            for (i=0;i<3;i++) axis[i]=G_up[i];
            for (i=0;i<3;i++) vector[i]=G_center[i];
            angle = -g_rotationAngleY;
            rotateVectorVector3d (&vector, axis, angle);
            for (i=0;i<3;i++) G_center[i]=vector[i];
            
            /* Pitch */
            crossProduct3D (axis, G_up, G_center);
            angle = g_rotationAngleX;
            
            for (i=0;i<3;i++) vector[i]=G_up[i];
            rotateVectorVector3d(&vector, axis, angle);
            for (i=0;i<3;i++) G_up[i]=vector[i];
            
            for (i=0;i<3;i++) vector[i]=G_center[i];
            rotateVectorVector3d(&vector, axis, angle);
            for (i=0;i<3;i++) G_center[i]=vector[i];
            
            /* Roll */
            for (i=0;i<3;i++) axis[i] = G_center[i]-G_eye[i];
            for (i=0;i<3;i++) vector[i] = G_up[i];
            angle = -g_rotationAngleZ;
            rotateVectorVector3d(&vector, axis, angle);
            for (i=0;i<3;i++) G_up[i]=vector[i];
    } else {
        
        G_center[0] = 0.0;
        G_center[1] = 0.0;
        G_center[2] = -1.0;

        G_up[0] = 0.0;
        G_up[1] = 1.0;
        G_up[2] = 0.0;
        
        cameraCalcYprToQuaternion ();
        
    }
        
	
    
   
	
}



/**
 * Verlängert/verkürzt den Vektor zur Kamera.
 */
void setCameraZoom(int x,int y)
{
	double factor = 1.0 + (CAMERA_ZOOM_SPEED / 1000.0) * ((G_MouseMove[2] < 0.0) ? -1.0 : 1.0);
	
	G_MouseMove[0] = x-G_LastMouseCenter[0];
	G_MouseMove[2] = y-G_LastMouseCenter[2];
	
	G_LastMouseCenter[0] = x;
	G_LastMouseCenter[2] = y;
	
	G_CameraPosition[0] *= factor;
	G_CameraPosition[1] *= factor;
	G_CameraPosition[2] *= factor;
}

/* ------- BERECHNUNGEN ------- */



/**
 * Berechnet alle Funktionen, die vom interval abhängig sind
 * @param interval - Zeit
 */
void calcTimeRelatedStuff (double interval)
{
    

    G_Counter += interval;
    
    if (G_Counter >= 1.0)
		G_Counter = 0.0 -EPS;
    
    if (G_Counter <= 0.0)
		G_FPS = (int) 1.0/interval;
        
    /*setQuaternionMovement (interval);*/
}

/**
 * Liefert den aktuellen Rotationswinkel des Wuerfels.
 * @return aktueller Rotationswinkel des Wuerfels.
 */
float
getRotationX (void)
{
  return g_rotationAngleX;
}

float
getRotationY (void)
{
  return g_rotationAngleY;
}

float
getRotationZ (void)
{
  return g_rotationAngleZ;
}

/**
 * Berechnet anhand der Rekursionstiefe die Anzahl der Quadrate auf dem Boden
 * @param p1,p2,p3,p4 - Die Ecken des Quadrats
 * @param rek - Rekursionstiefe
 * @param color - die Farbe der Quadrate
 * 
 */
void drawRecursiveQuad(CGPoint3f p1, CGPoint3f p2, CGPoint3f p3, CGPoint3f p4, int rek, int color)
{
    CGPoint3f middle0 = {0.0,0.0,0.0},
              middle1 = {0.0,0.0,0.0},
              middle2 = {0.0,0.0,0.0},
              middle3 = {0.0,0.0,0.0},
              center = {0.0,0.0,0.0};

    center[0] = (p1[0] + p3[0]) / 2;
    center[2] = (p1[2] + p3[2]) / 2;

    middle0[0] = (p1[0] + p2[0]) / 2;
    middle0[2] = (p1[2] + p2[2]) / 2;

    middle1[0] = (p2[0] + p3[0]) / 2;
    middle1[2] = (p2[2] + p3[2]) / 2;

    middle2[0] = (p3[0] + p4[0]) / 2;
    middle2[2] = (p3[2] + p4[2]) / 2;


    middle3[0] = (p4[0] + p1[0]) / 2;
    middle3[2] = (p4[2] + p1[2]) / 2;

    if (rek > 0)
    {
      drawRecursiveQuad(center,middle0,p2,middle1, rek-1, color);
      drawRecursiveQuad(center,middle1,p3,middle2, rek-1, !color);
      drawRecursiveQuad(center,middle2,p4,middle3, rek-1, color);
      drawRecursiveQuad(center,middle3,p1,middle0, rek-1, !color);


    } else {
        drawVertex(p1,p2,p3,p4, color);
    }
}

void callibrate (void)
{
	G_up[0] = 0.0;
	G_up[1] = 1.0;
	G_up[2] = 0.0;
	
	G_center[0] = 0.0;
	G_center[1] = 0.0;
	G_center[2] = -1.0;
	
	G_eye[0] = 0.0;
	G_eye[1] = 0.0;
	G_eye[2] = 0.0;
}

/* ------- INIT ------- */

/**
 * Initialisiert die Kamera.
 */
void initCameraPosition ()
{
    G_CameraPosition[0] = CAMERA_X;
    G_CameraPosition[1] = CAMERA_Y;
    G_CameraPosition[2] = CAMERA_Z;
}

/**
 * Objekt
 */
void initObject (void)
{
	G_ObjectSize = 3;
	
	G_Object = malloc (G_ObjectSize * sizeof (Vertex));
	
	G_IndicesCount = 3;
	
	G_Indices = malloc (G_IndicesCount * sizeof(GLuint));
	
	G_Object[0][0] = 0.0;
	G_Object[0][1] = 0.0;
	G_Object[0][2] = 10.0;
	
	G_Object[1][0] = 10.0;
	G_Object[1][1] = 0.0;
	G_Object[1][2] = 0.0;
	
	G_Object[2][0] = 0.0;
	G_Object[2][1] = 0.0;
	G_Object[2][2] = -10.0;
	
	
	G_Indices[0] = 0;
	G_Indices[1] = 1;
	G_Indices[2] = 2;
	
	

	glVertexPointer(3,           
                GL_DOUBLE,           
                0,
                &(G_Object[0][0]));
}

/**
 * Hier findet die komplette Initialisierung des kompletten SPIEeles statt.
 * Inklusive der Datenhaltung und des SPIEelfeldes.
 */
void initGame ()
{   
    initCameraPosition();
    
    initObject ();
    
    G_Quaternion = malloc (sizeof (*G_Quaternion));
    
    callibrate ();
        
    
}

