// CatmullRom.cpp: implementation of the CCatmullRom class.
// Copyright: (c) Burkhard Wuensche
// Raoul D'Cunha UPI:rdcu001, ID:1845734
//////////////////////////////////////////////////////////////////////

#include "CatmullRom.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#define _USE_MATH_DEFINES
#include <math.h>


#define POINT_SIZE_CTRLPOINT 10.0
#define WIDTH_CURVE 3.0

#define ORDER 4


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCatmullRom::CCatmullRom()
{
	mat_ambient_and_diffuse[0]=0.5;
	mat_ambient_and_diffuse[1]=0.5;
	mat_ambient_and_diffuse[2]=0.3;
	mat_ambient_and_diffuse[3]=1;
	mat_specular[0]=0.6f;
	mat_specular[1]=0.6f;
	mat_specular[2]=0.6f;
	mat_specular[3]=1;
	mat_shininess[0]=100;
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_and_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

CCatmullRom::~CCatmullRom()
{
}

void CCatmullRom::computeCurveAndSurface(int& numPoints, CVec3df**& points)
{ 
	n = numPoints; 
	ctrlPoints = points; 
}

void CCatmullRom::displayControlPoints()
{
	glPointSize(POINT_SIZE_CTRLPOINT);
	glColor3f(0,1,0);
	glBegin(GL_POINTS);
	for(int i=0;i<n;i++)
		glVertex3fv(ctrlPoints[i]->getArray());
	glEnd();
}

void CCatmullRom::displayCurve()
{
	// display the Catmull-Rom spline representing the profile curve using
	// a green line (RGB=(0,1,0)) with a width of WIDTH_CURVE 

	double xArray[100];
	double yArray[100];
	for (int i=0; i<n; i++) {
		xArray[i]= ctrlPoints[i]->getArray()[0];
		yArray[i]= ctrlPoints[i]->getArray()[1];
	}
	glColor3f(0,1,0);
	//CATMULL-ROM SPLINE
	glLineWidth(WIDTH_CURVE);
	glBegin(GL_LINE_STRIP);

	for(int i=1;i<n-2;i++) {
		for(int j=0; j<NUM_CIRCUMFERENTIAL_POINTS; j++) { 
			double deltay = 0.5*(yArray[i+1]-yArray[i-1]);
			double deltax = 0.5*(xArray[i+1]-xArray[i-1]);
			CVec3df n = CVec3df(deltax, deltay, 0).cross(CVec3df(0, 1, 0));
			surfaceNormals[i][j][0] = n.getArray()[0];
			surfaceNormals[i][j][2]  = n.getArray()[1];
			surfaceNormals[i][j][1]  = n.getArray()[2];	
			double t = j*0.0200; 
			double xPoint = xArray[i]+(t*(-xArray[i-1]+xArray[i+1])/2)+pow(t,2)*(xArray[i-1]-2.5*xArray[i]+2*xArray[i+1]-0.5*xArray[i+2])+ pow(t,3)*(-0.5*xArray[i-1]+1.5*xArray[i]-1.5*xArray[i+1]+0.5*xArray[i+2]);
			double yPoint = yArray[i]+(t*(-yArray[i-1]+yArray[i+1])/2)+pow(t,2)*(yArray[i-1]-2.5*yArray[i]+2*yArray[i+1]-0.5*yArray[i+2])+ pow(t,3)*(-0.5*yArray[i-1]+1.5*yArray[i]-1.5*yArray[i+1]+0.5*yArray[i+2]);
			curvePoints[i] = CVec3df(xArray[i], yArray[i],0);
			glVertex2f(xPoint, yPoint);
		}
	}	
	glEnd();
	for(int i=0; i<NUM_CURVE_POINTS; i++){
		for(int j=0; j<NUM_CIRCUMFERENTIAL_POINTS; j++){
			float angle = (M_PI*j*2)/NUM_CIRCUMFERENTIAL_POINTS;
			surfacePoints[i][j][0]= curvePoints[i].getArray()[0]*cos(angle);
			surfacePoints[i][j][2]= curvePoints[i].getArray()[0]*sin(angle);
			surfacePoints[i][j][1]= curvePoints[i].getArray()[1];			
		}
	}

}

void CCatmullRom::displaySurface()
{
	// display the surface using the materials properties defined below
	// Remember to use Gouraud shading (GL_SMOOTH) 
	// Please use the arrays "surfacePoints" and "surfaceNormals" and
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular); 
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_and_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess); glBegin(GL_TRIANGLE_STRIP);
	glLineWidth(WIDTH_CURVE);
	glShadeModel(GL_SMOOTH);
	//3D MODEL
	glPushMatrix();
	for(int i=0; i<NUM_CURVE_POINTS; i++){
		for(int j=0; j<NUM_CIRCUMFERENTIAL_POINTS; j++){ 		
			float theta=2.0f*M_PI*((float) j/(float) NUM_CIRCUMFERENTIAL_POINTS);
			float x = (float) cos(theta);		// x = r cos(theta), and r = 1
			float z = (float) sin(theta);	
			glNormal3fv(surfaceNormals[i][j].getArray()); 
			glVertex3fv(surfacePoints[i][j].getArray()); 
			glNormal3fv(surfaceNormals[i][j].getArray());
			glVertex3fv(surfacePoints[i+1][j].getArray());
		}
	}
	glEnd();
	glPopMatrix();
}