/*********************************************************************
* point.c (c) 2003 by Turku PET Centre
* Riku Klén
* A three dimensional point.
*
***********************************************************************/

#include <math.h>
#include "point.h"

/********************************************************************* 
* int round(float)
* Inputs:	float, any number
* Outputs:	
* Returns:	
* Description:	Round float to int.
***********************************************************************/

int pRound(float number){
	if(number - floor(number) < 0.5){
		return floor(number);
	}
	return ceil(number);
}

/********************************************************************* 
* float getDistance(point, point)
* Inputs:	point, any point
*		point, any point
* Outputs:	
* Returns:	Distance between two input points
* Description:	Calculates distance between two input points.
***********************************************************************/

float getDistance(point begin, point end){
	float dx, dy, dz;
	dx = begin.x - end.x;	// Difference in x cooridnates.
	dy = begin.y - end.y;	// Difference in y cooridnates.
	dz = begin.z - end.z;	// Difference in z cooridnates.
	return sqrt(dx*dx + dy*dy + dz*dz);
}

/********************************************************************* 
* float getAngle(point, point)
* Inputs:	point, any point (first)
*		point, any point (center)
* Outputs:	
* Returns:	Angle first - center - x in degrees (0-360) and
*		-360.0 if first point is equal to center point
* Description:	Calculates xy-pojection of angle FCX in degrees,
*		where F=first point, C=center point and X=point with
*		higher x coordinate (y and z coordinate remain the same).
*		This is used to calculate polar angle with two
*		dimensional points (z=constant).
***********************************************************************/

float getAngle(point begin, point center){
#ifndef PI  
	float PI = 3.141592654;
#endif
	float dx, dy;
	dx = begin.x - center.x;	// Difference in x cooridnates.
	dy = begin.y - center.y;	// Difference in y cooridnates.

// Check trivial cases.

	if(dx == 0.0){
		if(dy == 0.0) return -360.0;
		if(dy > 0.0) return 90.0;
		else return 270.0;
	}
	if(dy == 0.0){
		if(dx > 0.0) return 0.0;
		else return 180.0;
	}

// If it was not a trivial case, then calculate angle in...

// ...the second and the third quarter.

	if(dx<0.0) return 180.0 + atan(dy/dx)*180.0/PI;

// ...the first quarter.

	if(dy>0.0) return atan(dy/dx)*180.0/PI;

// ...the fourth quarter.

	return 360.0 + atan(dy/dx)*180.0/PI;
}
