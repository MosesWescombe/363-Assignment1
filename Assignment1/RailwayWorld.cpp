//  ========================================================================
//  COSC363: Computer Graphics (2022);  University of Canterbury.
//
//  FILE NAME: RailwayWorld.cpp
//  ========================================================================

#include <math.h>
#include <GL/freeglut.h>
#include "RailModels.h"

float theta = 0;
float cam_rot_spd = 0.1;
float cam_move_spd = 2;
float cam_y = 50;
float cam_dst = 200;
float cam_angle = 0;

void myTimer(int value)
{
    theta += 0.5;
    glutPostRedisplay();
    glutTimerFunc(30, myTimer, 0);
}

void specialKeys(int key, int x, int y)
{
    if (key == GLUT_KEY_UP) cam_y += cam_move_spd;
    else if (key == GLUT_KEY_DOWN) cam_y -= cam_move_spd;
    else if (key == GLUT_KEY_LEFT) cam_angle -= cam_rot_spd;
    else if (key == GLUT_KEY_RIGHT) cam_angle += cam_rot_spd;
    glutPostRedisplay();
}

void normalKeys(unsigned char key, int x, int y)
{
    if (key == 'w') cam_dst -= cam_move_spd;
    else if (key == 's') cam_dst += cam_move_spd;
    glutPostRedisplay();
}


//---------------------------------------------------------------------
void initialize(void)
{
    float grey[4] = {0.2, 0.2, 0.2, 1.0};
    float white[4]  = {1.0, 1.0, 1.0, 1.0};

    loadTexture();

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

   gluLookAt (cam_dst * sin(cam_angle), cam_y, cam_dst * cos(cam_angle), 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
   glLightfv(GL_LIGHT0, GL_POSITION, lgt_pos);   //light position



   floor();
   station();
   tracks();  //mean radius 120 units, width 10 units

   glPushMatrix();

        glRotatef(theta, 0., 1., 0.);

        for (int i = 0; i < 4; i++)
        {
            glPushMatrix();
                glRotatef((i+1)*-10.5, 0., 1., 0.);
                glTranslatef(0, 1,-120);
                wagon();
            glPopMatrix();
        }
        glTranslatef(0, 1,-120);
        glLightfv(GL_LIGHT1, GL_POSITION, headlight);
        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDir);
        engine(); //locomotive

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
   glutTimerFunc(50, myTimer, 0);
   glutSpecialFunc(specialKeys);
   glutKeyboardFunc(normalKeys);
   glutMainLoop();
   return 0;
}
