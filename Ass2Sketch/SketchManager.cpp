// SketchManager.cpp: implementation of the CSketchManager class.
// Copyright: (c) Burkhard Wuensche
//////////////////////////////////////////////////////////////////////

#include "SketchManager.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>


#define POINT_SIZE_STROKE 5.0
#define POINT_SIZE_SIMPLIFIED 8.0
#define ERROR_THRESHOLD_SIMPLIFICATION 0.01

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSketchManager::CSketchManager()
{
}

CSketchManager::~CSketchManager()
{
}


void CSketchManager::sketchInit(GLuint button, int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	sketch_button = button;
	numPoints = 0;
	rawSketch.clear();
	simplifiedSketch.clear();

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);
}



void CSketchManager::sketchMouse(int button, int state, int x, int y)
{
	if(button == sketch_button){
		if (state == GLUT_DOWN && numPoints<maxPoints){
			sketch_tracking = GL_TRUE;
			sketch_lasttime = glutGet(GLUT_ELAPSED_TIME);
			sketch_lastposition[0]=x; sketch_lastposition[1]=y;

			GLdouble p[3];
			gluUnProject((GLdouble) x, (GLdouble) (windowHeight-y), 0.611111, modelview, projection, viewport, &p[0], &p[1], &p[2]);
			rawSketch.push_back(new CVec3df((float) p[0], (float) p[1], (float) p[2]));
		}
		else{ // state == GLUT_UP || numPoints>=maxPoints
			sketch_tracking = GL_FALSE;
			simplify(simplifiedSketch, rawSketch.begin(), (rawSketch.end()-1), ERROR_THRESHOLD_SIMPLIFICATION);
			glutPostRedisplay();
		}
	}
}

void CSketchManager::sketchKeyboard(int key)
{
}

void CSketchManager::sketchMotion(int x, int y)
{
	if ((sketch_tracking == GL_FALSE) || ((sketch_lastposition[0]==x) && (sketch_lastposition[1]==y)) || (numPoints>=maxPoints)) return;
	// else tracking enabled and current pixel different from previous pixel
	sketch_lasttime = glutGet(GLUT_ELAPSED_TIME);
	sketch_lastposition[0]=x; sketch_lastposition[1]=y;

	GLdouble p[3];
	gluUnProject((GLdouble) x, (GLdouble) (windowHeight-y), 0.611111, modelview, projection, viewport, &p[0], &p[1], &p[2]);
//	rawSketch[numPoints++]= new CVec3df((float) p[0], (float) p[1], (float) p[2]);
	rawSketch.push_back(new CVec3df((float) p[0], (float) p[1], (float) p[2]));

	// draw current sketch
	glutPostRedisplay();
}

void CSketchManager::draw()
{

cout << "\n raw num points= " << rawSketch.size();
	glPointSize(POINT_SIZE_STROKE);
	glColor3f(0,0,0);
	glBegin(GL_POINTS);
	for (vector<CVec3df*>::iterator it = rawSketch.begin() ; it != rawSketch.end(); ++it)
		glVertex3fv((*it)->getArray());
	glEnd();

cout << "\n simplified num points= " << simplifiedSketch.size();
	glPointSize(POINT_SIZE_SIMPLIFIED);
	glColor3f(1,0,0);
	glBegin(GL_POINTS);
	for (vector<CVec3df*>::iterator it = simplifiedSketch.begin() ; it != simplifiedSketch.end(); ++it)
		glVertex3fv((*it)->getArray());
	glEnd();
}

void CSketchManager::simplify(vector<CVec3df*>& result, vector<CVec3df*>::const_iterator start, vector<CVec3df*>::const_iterator end, float threshold)
{
	_DouglasPeuker(result, start, end, threshold);
	result.push_back(*end);
}

void CSketchManager::_DouglasPeuker(vector<CVec3df*>& result, vector<CVec3df*>::const_iterator start, vector<CVec3df*>::const_iterator end, float threshold)
{
	if (end - start <= 1) { // Can't subdivide any more
		result.push_back(*start);
		return;
	}

	// Find line parameters
	CVec3df lineStart = **start;
	CVec3df lineEnd = **end;

	// Find point with greatest error
	vector<CVec3df*>::const_iterator mid = start + 1;
	float maxError = 0;
	for (vector<CVec3df*>::const_iterator i = start + 1; i != end; i++)
	{
		float dist = (**i).distanceToLine(lineStart, lineEnd);
		float error = dist * dist; // Squared distance
		if (error > maxError)
		{
			maxError = error;
			mid = i;
		}
	}

	if (maxError >= threshold)
	{
		_DouglasPeuker(result, start, mid, threshold);
		vector<CVec3df*> rhs;
		_DouglasPeuker(rhs, mid, end, threshold);
		result.insert(result.end(), rhs.begin(), rhs.end());
		return;
	}

	// All points within threshold, just return single line segment
	result.push_back(*start);

	return;
}

void CSketchManager::getControlPoints(CVec3df**& points, int& numPoints){
	int n = simplifiedSketch.size();
	cout << "\n num old Points = " << n;
	if (n==0) return;

	int i,j; // index of new point list

	// copy points in array of CVec3df* (better for computing parametric curve)
	CVec3df** oldPoints=(CVec3df**) malloc(n*sizeof(CVec3df*));
	i=0;
	for (vector<CVec3df*>::iterator it = simplifiedSketch.begin() ; it != simplifiedSketch.end(); ++it){
		cout << "\n iterator value " << **it;
		oldPoints[i++]=*it;
	}

	// remove points on positive x-axis & set first point on x-axis
	int nTemp=n+4;
	i=0; // i is index of old point list
	j=0; // j is index of new point list
	CVec3df p, pOld;
	CVec3df** newPoints=(CVec3df**) malloc(nTemp*sizeof(CVec3df*));
	p=*oldPoints[i];
	if(p[X]>0){
		// remove all point on positive x axis and set last point onto positive x-axis
		do {
			newPoints[j]=new CVec3df(0.0, p[Y], p[Z]);
			p=*oldPoints[++i];
		} while (p[X]>0);
		j++;
	}
	else {
		// first point on x-axis or has positive x-value - move point onto x-axis
		newPoints[j++]=new CVec3df(0.0, p[Y], p[Z]);
		newPoints[j++]=oldPoints[i++];
	}
	// at this case i is index of first point with non-positive x-value
	// copy all remaining points into new array
	for(;i<n;)
		newPoints[j++]=oldPoints[i++];
	// make sure last point lies on x-axis
	p=*newPoints[--j];
	if(p[X]<=0){
		// last point on negative x-axis, insert new point on x-axis
		newPoints[++j]=new CVec3df(0.0, p[Y], p[Z]);
	}
	else{
		// decrement index until p[j] is on negative x-axis and p[j+1] on positive x-axis
		do {
			pOld=p;
			p=*newPoints[--j];
		} while (p[X]>0);
		newPoints[++j]=new CVec3df(0.0, pOld[Y], pOld[Z]);
	}
	j++;

	// copy all points into new array,
	// add new bogus point at start and end in order to get a start and end tangent parallel to x-axis
	numPoints=j+4;
	points=(CVec3df**) malloc(numPoints*sizeof(CVec3df*));
	CVec3df p1, p2, p3, p4, diff;
	p2=*newPoints[0];
	p4=*newPoints[1];
	diff = 0.1*(p4-p2);
	p1.setVector(-diff[X], p2[Y], p2[Z]);
	p3.setVector(diff[X], p2[Y], p2[Z]);
	points[0]=new CVec3df(p1);
	points[1]=new CVec3df(p2);
	points[2]=new CVec3df(p3);
	for(i=1;i<(j-1);i++) points[i+2]=newPoints[i];
	p2=*newPoints[j-2];
	p4=*newPoints[j-1];
	diff = 0.1*(p4-p2);
	p1.setVector(-diff[X], p4[Y], p4[Z]);
	p3.setVector(diff[X], p4[Y], p4[Z]);
	points[j+1]=new CVec3df(p1);
	points[j+2]=new CVec3df(p4);
	points[j+3]=new CVec3df(p3);

}
