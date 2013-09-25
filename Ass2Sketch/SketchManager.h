// SketchManager.h: interface for the CSketchManager class.
// Copyright: (c) Burkhard Wuensche
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKETCHMANAGER_INCLUDED_)
#define AFX_SKETCHMANAGER_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>
#include <gl/gl.h>
#include "Geometry.h"
#include <vector>
using namespace std;

class CSketchManager
{
public:
	CSketchManager();
	virtual ~CSketchManager();
	void sketchInit(GLuint button, int width, int height);
	void sketchMouse(int button, int state, int x, int y);
	void sketchKeyboard(int key);
	void sketchMotion(int x, int y);
	void draw();
	void simplify(vector<CVec3df*>& simplifiedSketch, vector<CVec3df*>::const_iterator start, vector<CVec3df*>::const_iterator end, float threshold);
	void getControlPoints(CVec3df**& points, int& numPoints);
private:
	void _DouglasPeuker(vector<CVec3df*>& simplifiedSketch, vector<CVec3df*>::const_iterator start, vector<CVec3df*>::const_iterator end, float threshold);
	// Sketch defined as 3D points on z=0 plane
	static const int maxPoints=100000;
	vector<CVec3df*> rawSketch;
	vector<CVec3df*> simplifiedSketch;
	int numPoints;
	// Variables for computing 3D screen coordinates from window coordinates
	GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
	int windowWidth;
	int windowHeight;
	// Variables for processing sketch input
	GLint     sketch_button;
	GLboolean sketch_tracking;
	GLuint    sketch_lasttime;		// currently not used, but useful to detect how fast and how far apart strokes were drawn
	GLint	  sketch_lastposition[2];
};

#endif // !defined(AFX_SKETCHMANAGER_INCLUDED_)
