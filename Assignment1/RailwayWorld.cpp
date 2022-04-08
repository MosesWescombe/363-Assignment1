//  ========================================================================
//  COSC363: Computer Graphics (2022);  University of Canterbury.
//
//  FILE NAME: RailwayWorld.cpp
//  ========================================================================

#include <math.h>
#include <GL/freeglut.h>
#include "RailModels.h"
#include <iostream>

#define PI (atan(1)*4)

int count = 0;
int startCount = 0;

float cam_angle = 0;
float cam_rot_spd = 0.05;
float cam_move_spd = 5;
float cam_pos[3] = { 100, 50, 0 };
float cam_dir[3] = { cam_pos[0] + 100 * sin(cam_angle), 0, cam_pos[2] + 100 * cos(cam_angle) };
bool cabView = true;

long moveIncrement = 3; // Larger is faster
long currentPosition = 0;

int wagonSpacing = 50;

long tunnelStart = ((nPoints / 2) / 30 * 2) * 2;
long tunnelLength = 1200;
long tunnelPadding = 10;
long tunnelHeight = 20;
long tunnelSize = 5;

void slowTrain() {
    moveIncrement = 0;
    startCount = (count + 30) % 10000;
}

void speedTrain() {
    moveIncrement = 3;
    startCount = 0;
    currentPosition += 2;
}

void myTimer(int value)
{
    if (startCount != 0 && startCount <= count) {
        speedTrain();
        count = 0;
    }

    if (count % 1 == 0) {
        currentPosition = (currentPosition + moveIncrement * 2) % nPoints;
    }
    glutPostRedisplay();
    count = (count + 1) % 10000;
    glutTimerFunc(1, myTimer, 0);
}

void specialKeys(int key, int x, int y)
{
    if (key == GLUT_KEY_LEFT) cam_angle -= 0.1;  //Change direction
    else if (key == GLUT_KEY_RIGHT) cam_angle += 0.1;
    
    if (key == GLUT_KEY_DOWN)
    {  //Move backward
        cam_pos[0] -= cam_move_spd * sin(cam_angle);
        cam_pos[2] += cam_move_spd * cos(cam_angle);
    }
    else if (key == GLUT_KEY_UP)
    { //Move forward
        cam_pos[0] += cam_move_spd * sin(cam_angle);
        cam_pos[2] -= cam_move_spd * cos(cam_angle);
    }

    if (key == GLUT_KEY_PAGE_UP) cam_pos[1] += cam_move_spd;
    else if (key == GLUT_KEY_PAGE_DOWN) cam_pos[1] -= cam_move_spd;

    cam_dir[0] = cam_pos[0] + 100 * sin(cam_angle);
    cam_dir[2] = cam_pos[2] - 100 * cos(cam_angle);


    glutPostRedisplay();
}

void normalKeys(unsigned char key, int x, int y)
{
    if (key == 'w') cam_dir[1] += cam_move_spd;
    else if (key == 's') cam_dir[1] -= cam_move_spd;

    if (key == 'c') cabView = !cabView;

    glutPostRedisplay();
}


//---------------------------------------------------------------------
void initialize(void)
{
    float grey[4] = {0.2, 0.2, 0.2, 1.0};
    float white[4]  = {1.0, 1.0, 1.0, 1.0};

    loadRoofTexture();
    loadBrickTexture();

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

//  Define light's ambient, diffuse, specular properties
    glLightfv(GL_LIGHT0, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);

    glLightfv(GL_LIGHT1, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT1, GL_SPECULAR, white);

    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT,0.01);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialf(GL_FRONT, GL_SHININESS, 50);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glClearColor (0.0, 0.0, 0.0, 0.0);  //Background colour

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(60., 1.0, 10.0, 1000.0);   //Perspective projection

    // Generate points
    generateTrackPoints();
}

//-------------------------------------------------------------------
void display(void)
{
   float lgt_pos[] = {0.0f, 50.0f, 0.0f, 1.0f};  //light0 position (directly above the origin)
   float headlight[] = {-10, 14, 0.0f, 1};
   float spotDir[] = {-1, -1, 0};

   glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   if (cabView) {
       long vectorPosition = currentPosition + 200 * 2;
       float viewVector[2] = { 0 };
       viewVector[0] = points[(vectorPosition + 2) % nPoints] - points[currentPosition];
       viewVector[1] = points[(vectorPosition + 3) % nPoints] - points[currentPosition + 1];
       normalized2F(viewVector);

       float directionVector[2] = { 0 };
       directionVector[0] = points[(currentPosition + 2) % nPoints] - points[currentPosition];
       directionVector[1] = points[(currentPosition + 3) % nPoints] - points[currentPosition + 1];
       normalized2F(directionVector);

       float lookPoint[2] = {points[currentPosition] + viewVector[0] * 50, points[currentPosition + 1] + viewVector[1] * 50};
       gluLookAt(points[currentPosition] - directionVector[0] * 5, 20, points[currentPosition + 1] - directionVector[1] * 5, lookPoint[0], 20, lookPoint[1], 0.0, 1.0, 0.0);
   }
   else {
       gluLookAt(cam_pos[0], cam_pos[1], cam_pos[2], cam_dir[0], cam_dir[1], cam_dir[2], 0.0, 1.0, 0.0);
   }
   glLightfv(GL_LIGHT0, GL_POSITION, lgt_pos);   //light position

   floor();
   station();

   // Center track
   glTranslatef(-points[nPoints/2], 0, -points[(nPoints / 2) + 1]);
   tunnel(tunnelStart, tunnelLength, tunnelPadding, tunnelSize, tunnelHeight);
   tracks();

   glPushMatrix();
        long previousWagonIndex = currentPosition;
        for (int i = 1; i <= 4; i++)
        {
            long posIndex = 0;
            int spacing = 0;
            float vector[2];
            do {
                posIndex = (previousWagonIndex - ((spacing) * 2)) % nPoints;
                // Fix modulo negative number issue
                if (posIndex < 0) {
                    posIndex = nPoints + posIndex;
                }
                spacing++;
                vector[0] = points[previousWagonIndex] - points[posIndex % nPoints];
                vector[1] = points[previousWagonIndex + 1] - points[(posIndex % nPoints) + 1];
            } while (vectorLength2F(vector) < 20);

            float directionVector[2] = { 0 };

            directionVector[0] = points[(posIndex + 2) % nPoints] - points[posIndex];
            directionVector[1] = points[(posIndex + 3) % nPoints] - points[posIndex + 1];

            glPushMatrix();
                glTranslatef(points[posIndex], 0, points[posIndex + 1]);
                glRotatef(atan2(directionVector[1], -directionVector[0]) * (180 / PI), 0., 1., 0.);
                wagon();
            glPopMatrix();
            previousWagonIndex = posIndex;
        }
        // Create Ui vector 2
        float directionVector[2]= {0};
        directionVector[0] = points[(currentPosition + 2) % nPoints] - points[currentPosition];
        directionVector[1] = points[(currentPosition + 3) % nPoints] - points[currentPosition + 1];


        glTranslatef(points[currentPosition], 0, points[currentPosition + 1]);
        glRotatef(atan2(directionVector[1], -directionVector[0]) * (180/PI), 0., 1., 0.);

        glLightfv(GL_LIGHT1, GL_POSITION, headlight);
        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDir);
        engine(); //locomotive
        if (currentPosition >= (stationPosition + 100) && (currentPosition - (moveIncrement * 2) <= (stationPosition + 200))) {
            slowTrain();
        }

   glPopMatrix();

   glutSwapBuffers();   //Useful for animation
}


//---------------------------------------------------------------------
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE|GLUT_DEPTH);
   glutInitWindowSize (1300, 1000);
   glutInitWindowPosition (50, 50);
   glutCreateWindow ("Assignment 1");
   initialize ();

   glutDisplayFunc(display);
   glutTimerFunc(1, myTimer, 0);
   glutSpecialFunc(specialKeys);
   glutKeyboardFunc(normalKeys);
   glutMainLoop();
   return 0;
}
