//  ========================================================================
//  COSC363: Computer Graphics (2022);  University of Canterbury.
//
//  RailModels.cpp
//  A collection of functions for generating the models for a railway scene
//  ========================================================================

#include <cmath>
#include <GL/freeglut.h>
#include "RailModels.h"
#include "LoadBMP.h"
#include <iostream>

using namespace std;

GLuint txId;

//------Function to load a texture in bmp format  ------------------------
void loadTexture()
{
	glGenTextures(1, &txId); 				// Create a Texture object
	glBindTexture(GL_TEXTURE_2D, txId);		//Use this texture
	loadBMP("roof.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

//--------------- GROUND PLANE ------------------------------------
// This is a square shaped region on the xz-plane of size 400x400 units
// centered at the origin.  This region is constructed using small quads
// of unit size, to facilitate rendering of spotlights
//-----------------------------------------------------------------
void floor()
{
	float white[4] = {1., 1., 1., 1.};
	float black[4] = {0};
	glColor4f(0.7, 0.7, 0.7, 1.0);  //The floor is gray in colour
	glNormal3f(0.0, 1.0, 0.0);
    glMaterialfv(GL_FRONT, GL_SPECULAR, black);

    //The floor is made up of several tiny squares on a 800 x 800 grid. Each square has a unit size.
	glBegin(GL_QUADS);
    for(int i = -400; i < 400; i++)
	{
        for(int j = -400;  j < 400; j++)
		{
			glVertex3f(i, 0, j);
			glVertex3f(i, 0, j+1);
			glVertex3f(i+1, 0, j+1);
			glVertex3f(i+1, 0, j);
		}
	}
	glEnd();

    glMaterialfv(GL_FRONT, GL_SPECULAR, white);

}

//--------------- RAILWAY TRACK ------------------------------------
// A circular railway track of specified median radius and width
// The tracks are modelled using small quadrilateral segments each covering 5 deg arc
// The tracks have a height of 1 unit (refer to lab notes)
//-----------------------------------------------------------------
void tracks(float medRadius, float width)  
{
	float inRad  = medRadius - width * 0.5;
	float outRad = medRadius + width * 0.5;
	float angle1,angle2, ca1,sa1, ca2,sa2;
	float x1,z1, x2,z2, x3,z3, x4,z4;  //four points of a quad
    float toRad = 3.14159265/180.0;  //Conversion from degrees to radians

	glColor4f(0.0, 0.0, 0.3, 1.0);
    glBegin(GL_QUADS);
	float radius = inRad;
	for (int i = 0; i < 2; i++)   //Two parallel tracks (radius = inRad, outRad)
	{
		for (int i = 0; i < 360; i += 5)    //5 deg intervals
		{
			angle1 = i * toRad;       //Computation of angles, cos, sin etc
			angle2 = (i + 5) * toRad;
			ca1 = cos(angle1); ca2 = cos(angle2);
			sa1 = sin(angle1); sa2 = sin(angle2);
			x1 = (radius - 0.5)*sa1; z1 = (radius - 0.5)*ca1;
			x2 = (radius + 0.5)*sa1; z2 = (radius + 0.5)*ca1;
			x3 = (radius + 0.5)*sa2; z3 = (radius + 0.5)*ca2;
			x4 = (radius - 0.5)*sa2; z4 = (radius - 0.5)*ca2;

			glNormal3f(0., 1., 0.);       //Quad 1 facing up
			glVertex3f(x1, 1.0, z1);
			glVertex3f(x2, 1.0, z2);
			glVertex3f(x3, 1.0, z3);
			glVertex3f(x4, 1.0, z4);

			glNormal3f(-sa1, 0.0, -ca1);   //Quad 2 facing inward
			glVertex3f(x1, 0.0, z1);
			glVertex3f(x1, 1.0, z1);
			glNormal3f(-sa2, 0.0, -ca2);
			glVertex3f(x4, 1.0, z4);
			glVertex3f(x4, 0.0, z4);

			glNormal3f(sa1, 0.0, ca1);   //Quad 3 facing outward
			glVertex3f(x2, 1.0, z2);
			glVertex3f(x2, 0.0, z2);
			glNormal3f(sa2, 0.0, ca2);
			glVertex3f(x3, 0.0, z3);
			glVertex3f(x3, 1.0, z3);
		}
		radius = outRad;
	}
	glEnd();
}


//--------------- MODEL BASE --------------------------------------
// This is a common base for the locomotive and wagons
// The base is of rectangular shape (20 length x 10 width x 2 height)
// and has wheels and connectors attached.
//-----------------------------------------------------------------
void base()
{
    glColor4f(0.2, 0.2, 0.2, 1.0);   //Base color
    glPushMatrix();
      glTranslatef(0.0, 4.0, 0.0);
      glScalef(20.0, 2.0, 10.0);     //Size 20x10 units, thickness 2 units.
      glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();					//Connector between wagons
      glTranslatef(11.0, 4.0, 0.0);
      glutSolidCube(2.0);
    glPopMatrix();

    //4 Wheels (radius = 2 units)
	//x, z positions of wheels:
	float wx[4] = {  -8,   8,   -8,    8 }; 
	float wz[4] = { 5.1, 5.1, -5.1, -5.1 };
    glColor4f(0.5, 0., 0., 1.0);    //Wheel color
	GLUquadric *q = gluNewQuadric();   //Disc

	for (int i = 0; i < 4; i++)
	{
		glPushMatrix();
		glTranslatef(wx[i], 2.0, wz[i]);
		gluDisk(q, 0.0, 2.0, 20, 2);
		glPopMatrix();
	}
}

//--------------- LOCOMOTIVE --------------------------------
// This simple model of a locomotive consists of the base,
// cabin and boiler
//-----------------------------------------------------------
void engine()
{
    base();

    //Cab
    glColor4f(0.8, 0.8, 0.0, 1.0);
    glPushMatrix();
      glTranslatef(7.0, 8.5, 0.0);
      glScalef(6.0, 7.0, 10.0);
      glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
      glTranslatef(6.0, 14.0, 0.0);
      glScalef(4.0, 4.0, 8.0);
      glutSolidCube(1.0);
    glPopMatrix();

    //Boiler
	GLUquadric *q = gluNewQuadric();   //Cylinder
    glPushMatrix();
      glColor4f(0.5, 0., 0., 1.0);
      glTranslatef(4.0, 10.0, 0.0);
      glRotatef(-90.0, 0., 1., 0.);
      gluCylinder(q, 5.0, 5.0, 14.0, 20, 5);
      glTranslatef(0.0, 0.0, 14.0);
      gluDisk(q, 0.0, 5.0, 20, 3);
      glColor4f(1.0, 1.0, 0.0, 1.0);
      glTranslatef(0.0, 4.0, 0.2);
      gluDisk(q, 0.0, 1.0, 20,2);  //headlight!
    glPopMatrix();



}

//--------------- WAGON ----------------------------------
// This simple model of a rail wagon consists of the base,
// and a cube(!)
//--------------------------------------------------------
void wagon()
{
    base();

    glColor4f(0.0, 1.0, 1.0, 1.0);
    glPushMatrix();
      glTranslatef(0.0, 10.0, 0.0);
      glScalef(18.0, 10.0, 10.0);
      glutSolidCube(1.0);
    glPopMatrix();
}

// Besier curve equation
float bez_point(double t, float p1, float p2, float p3)
{
	return pow(t, 2)*p3 + 2*p2*t*(1-t) + pow((1-t), 2)*p1;
}

void normal(float x1, float y1, float z1,
	float x2, float y2, float z2,
	float x3, float y3, float z3)
{
	float nx, ny, nz;
	nx = y1 * (z2 - z3) + y2 * (z3 - z1) + y3 * (z1 - z2);
	ny = z1 * (x2 - x3) + z2 * (x3 - x1) + z3 * (x1 - x2);
	nz = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);

	glNormal3f(nx, ny, nz);
}

//--------------- Station -----------------------------------------
// This is a common base for the locomotive and wagons
// The base is of rectangular shape (20 length x 10 width x 2 height)
// and has wheels and connectors attached.
//-----------------------------------------------------------------
void station()
{
	float height = 40.0;
	float width = 120.0;
	float length = 20.0;

	float p3_hgt = 30;
	float p3_z = -length / 2;

	// Ground
	glPushMatrix();
		glColor4f(0.2, 0.2, 0.2, 1.0);   //Base color
		glTranslatef(0.0, 2.5, 0);
		glScalef(width, 5.0, 20.0);
		glutSolidCube(1.0);
	glPopMatrix();

	
	// Roof
	glPushMatrix();
		glColor4f(1, 1, 1, 1.0);
		glTranslatef(0.0, 0.0, 1.5 * length);
		int sections = 6;
		int steps = sections * 2 + 1;

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, txId);

		// Curved top
		glBegin(GL_QUAD_STRIP);
		double prev_t = 0;
		for (int i = 0; i < steps; i++) {
			double t = ((double)i)/((double)(steps - 1));
			float y = bez_point(t, height, height + p3_hgt, height);
			float z = bez_point(t, 0.0, p3_z, -2 * length);

			if (i > 0) {
				normal(-1 * width / 2, y, z,
					width / 2, y, z,
					width / 2, bez_point(prev_t, height, height + p3_hgt, height), bez_point(prev_t, 0.0, p3_z, -2 * length));
			}

			glTexCoord2f(0.0,0.0); glVertex3f(-1 * width / 2, y, z);
			glTexCoord2f(0.1,1.0); glVertex3f(width / 2, y, z);
			prev_t = t;
		}
		glEnd();

		glDisable(GL_TEXTURE_2D);

		// Left wall
		glNormal3f(-1., 0., 0.);
		glBegin(GL_QUAD_STRIP);
		for (int i = 0; i < steps; i++) {
			double t = ((double)i) / ((double)(steps - 1));
			float y = bez_point(t, height, height + p3_hgt, height);
			float z = bez_point(t, 0.0, p3_z, -2 * length);
			glVertex3f(-1 * width / 2, y, z);
			glVertex3f(-1 * width / 2, height, z);
		}
		glEnd();

		// Right wall
		glNormal3f(1., 0., 0.);
		glBegin(GL_QUAD_STRIP);
		for (int i = 0; i < steps; i++) {
			double t = ((double)i) / ((double)(steps - 1));
			float y = bez_point(t, height, height + p3_hgt, height);
			float z = bez_point(t, 0.0, p3_z, -2 * length);
			glVertex3f(width / 2, y, z);
			glVertex3f(width / 2, height, z);
		}
		glEnd();

		// Ceiling
		glNormal3f(0., -1., 0.);
		glBegin(GL_QUADS);
			glVertex3f(-1 * width / 2, height, 0);
			glVertex3f(-1 * width / 2, height, -2 * length);
			glVertex3f(width / 2, height, -2 * length);
			glVertex3f(width / 2, height, 0);
		glEnd();
	glPopMatrix();

	// Building
	glPushMatrix();
		glTranslatef(0.0, height/2, length);
		glScalef(width, height, length);
		glutSolidCube(1.0);
	glPopMatrix();
}