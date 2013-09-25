// CatmullRom.h: interface for the CCatmullRom class.
// Copyright: (c) Burkhard Wuensche
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CATMULLROM_INCLUDED_)
#define AFX_CATMULLROM_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>
#include <gl/gl.h>
#include "Geometry.h"

#define NUM_CURVE_POINTS 100
#define NUM_CIRCUMFERENTIAL_POINTS 32

class CCatmullRom
{
public:
	CCatmullRom();
	virtual ~CCatmullRom();
	void computeCurveAndSurface(int& numPoints, CVec3df**& points);
	void displayControlPoints();
	void displayCurve();
	void displaySurface();
private:
	void _computeCurvePoint(int segment, float t, CVec3df& p);
	void _computeCurveTangent(int segment, float t, CVec3df& v);
	CVec3df curvePoints[NUM_CURVE_POINTS];	// points for drawing Catmull-Rom spline
	CVec3df curveTangents[NUM_CURVE_POINTS];	// tangents of Catmull-Rom spline for computing surface normals of surface of revolution
	CVec3df surfacePoints[NUM_CURVE_POINTS][NUM_CIRCUMFERENTIAL_POINTS];	// points for drawing surface of revolution
	CVec3df surfaceNormals[NUM_CURVE_POINTS][NUM_CIRCUMFERENTIAL_POINTS];	// normals for drawing surface of revolution

	CVec3df** ctrlPoints;	// control points for Catmull-Rom spline, first and last point are bogus points
	int n;					// number of control points

	GLfloat mat_specular[4];
	GLfloat mat_ambient_and_diffuse[4];
	GLfloat mat_shininess[1];
};

#endif // !defined(AFX_CATMULLROM_INCLUDED_)
