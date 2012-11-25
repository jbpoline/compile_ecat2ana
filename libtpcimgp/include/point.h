/*********************************************************************
* point.h (c) 2003 by Turku PET Centre
* Riku Klén
* Header for a three dimensional point.
*
***********************************************************************/

#ifndef _POINT_H
#define _POINT_H

typedef struct point{
	float x;	// x coordinate.
	float y;	// y coordinate.
	float z;	// z coordinate.
} point;

extern int pRound(float);
extern float getDistance(point, point);
extern float getAngle(point, point);

#endif
