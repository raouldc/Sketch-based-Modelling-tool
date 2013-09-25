//=============================================================
//= Assignment 2                                              =
//= SKetch-based Modelling									  =
//=============================================================

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include "Trackball.h"
#include "Utilities.h"
#include "Geometry.h"
#include "Lighting.h"
#include "SketchManager.h"
#include "CatmullRom.h"

#define LINE_WIDTH_AXIS 3.0

const int windowWidth=800;
const int windowHeight=800;

typedef enum {SKETCH_MODE, SURFACE_MODE} ModeType;

CTrackball trackball;
CLighting lighting;
CSketchManager sketchManager;
CCatmullRom catmullRom;
ModeType mode;

GLfloat mat_specular[4];
GLfloat mat_ambient_and_diffuse[4];
GLfloat mat_shininess[1];




void init(void) 
{
	// select clearing color (for glClear)
	glClearColor (1.0, 1.0, 1.0, 0.0);	// RGB-value for white

	// initialize view (simple perspective projection)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(15.0, (GLfloat) windowWidth/(GLfloat) windowHeight, 35.0, 55.0);
	gluLookAt(0,0,45, 0,0,0, 0,1,0);
	glutPostRedisplay(); 

	lighting.init();

	trackball.tbInit(GLUT_LEFT_BUTTON);
	sketchManager.sketchInit(GLUT_RIGHT_BUTTON, windowWidth, windowHeight);

	mode=SKETCH_MODE;
}


void handleMouseMotion(int x, int y)
{	
	if (mode==SURFACE_MODE) trackball.tbMotion(x, y);
	if (mode==SKETCH_MODE) sketchManager.sketchMotion(x, y);
}

void handleMouseClick(int button, int state, int x, int y)
{
	if (mode==SURFACE_MODE) trackball.tbMouse(button, state, x, y);
	if (mode==SKETCH_MODE) sketchManager.sketchMouse(button, state, x, y);
}

void handleKeyboardEvent(unsigned char key, int x, int y)
{
	if((key == (int) 'm') || (key == (int) 'M')){
		if (mode == SKETCH_MODE) mode = SURFACE_MODE;
		else if (mode == SURFACE_MODE) mode = SKETCH_MODE;
		cout << "\n mode=" << mode;
	}
	if((key == (int) 's') || (key == (int) 's')){ 
		mode = SURFACE_MODE;
		// construct control points for Catmull-Rom spline if sketch has been drawn
		// iterate through all points, remove points with positive x coordinate.
		// make sure first and last point is on axis, Add bogus point at end
		CVec3df** points;
		int numPoints;
		sketchManager.getControlPoints(points, numPoints);
		// create Catmull-Rom spline with control points (first and last are bogus points)
		catmullRom.computeCurveAndSurface(numPoints, points);
		glutPostRedisplay();
	}
	if((key == (int) 'r') || (key == (int) 'r')){ 
		// reset the application to allow sketching of a new profile curve
		init();
		glutPostRedisplay();
	}
}



void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode( GL_MODELVIEW );	// Set the view matrix ...
	glLoadIdentity();				// ... to identity.
	trackball.tbMatrix();


	glEnable(GL_DEPTH_TEST);

	// draw sketch
	sketchManager.draw();

	// draw Catmull-Rom control points
	catmullRom.displayControlPoints();
	catmullRom.displayCurve();


	// draw rotation axis (y-axis)
	glColor3f(0,0,1);
	glLineWidth(LINE_WIDTH_AXIS);
	glBegin(GL_LINES);
	glVertex3d(0, -5, 0);
	glVertex3d(0, 5, 0);
	glEnd();

	if (mode==SURFACE_MODE){
		// draw parametric surface representing surface of revolution obtained from sketched curve
		lighting.enable();
		catmullRom.displaySurface();
		lighting.disable();
	}

	glFlush ();
	glutSwapBuffers();
}

void reshape(int width, int height ) {
	// Called at start, and whenever user resizes component
	int size = min(width, height);
	glViewport(0, 0, size, size);  // Largest possible square
	trackball.tbReshape(width, height);
}



// create a single buffered colour window
int main(int argc, char** argv)
{
	glutInit(&argc, argv);	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(windowWidth, windowHeight); 
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Assignment 2 - Sketch-based Modelling");
	init ();						// initialise view
	glutMouseFunc(handleMouseClick);		// Set function to handle mouse clicks
	glutMotionFunc(handleMouseMotion);		// Set function to handle mouse motion
	glutKeyboardFunc(handleKeyboardEvent);	// Set function to handle keyboard input
	glutDisplayFunc(display);		// Set function to draw scene
	glutReshapeFunc(reshape);		// Set function called if window gets resized
	glutMainLoop();
	return 0;
}
