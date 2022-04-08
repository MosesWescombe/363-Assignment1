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
#include <fstream>

#define PI (atan(1)*4)

GLuint txId;
GLuint txId2;

const long nPoints = 3500 * 2;
float points[nPoints] = {0};

// Track variables
long stationPosition = (nPoints / 32) * 30;

float trackWidth = 1.5;
float innerTrackW = 3;
float outerTrackW = innerTrackW + trackWidth;
float trackHeight = 1;

float sleeperHeight = 0.5;
long sleeperSpacing = 10;
float sleeperExtraLength = 1.5;
float sleeperWidth = 2;

// Bezier curve equation
float bez_point(double t, float p1, float p2, float p3)
{
    return pow(t, 2) * p3 + 2 * p2 * t * (1 - t) + pow((1 - t), 2) * p1;
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

float vectorLength2F(float vector[2]) {
    return sqrt(pow(vector[0], 2) + pow(vector[1], 2));
}

float vectorLength3F(float vector[3]) {
    return sqrt(pow(vector[0], 2) + pow(vector[1], 2) + pow(vector[2], 2));
}

void normalized2F(float vector[2]) {
    float length = vectorLength2F(vector);
    vector[0] = vector[0] / length;
    vector[1] = vector[1] / length;
}

void normalized3F(float vector[3]) {
    float length = vectorLength3F(vector);
    vector[0] = vector[0] / length;
    vector[1] = vector[1] / length;
    vector[2] = vector[2] / length;
}

float functionY(const int a, double t) {
    return a * sin(t);
}

float functionX(const int a, double t) {
    return a * sin(t) * cos(t);
}

void generateTrackPoints() {
    const int scale = 200;
    long i = 0;
    for (double t = 0; t < 360*2; t += ((double)(360 * 2)/nPoints)) {
        float x = functionX(scale, t * (PI/180));
        float y = functionY(scale, t * (PI/180));
        points[i] = x;
        points[i + 1] = y;
        i = (i + 2) % nPoints;
    }
}

//------Function to load a texture in bmp format  ------------------------
void loadRoofTexture()
{
    glGenTextures(1, &txId);                // Create a Texture object
    glBindTexture(GL_TEXTURE_2D, txId);     //Use this texture
    loadBMP("roof.bmp");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   //Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

//------Function to load a texture in bmp format  ------------------------
void loadBrickTexture()
{
    glGenTextures(1, &txId2);                // Create a Texture object
    glBindTexture(GL_TEXTURE_2D, txId2);     //Use this texture
    loadBMP("brick.bmp");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   //Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
    glColor4f(0.2078, 0.6353, 0.0382, 1.0);  //The floor is gray in colour
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
//-----------------------------------------------------------------
void tracks()
{
    glBegin(GL_QUADS);
    for (long i=0; i < nPoints; i += 2)
    {
        float directionVector1[2] = {0};
        float directionVector2[2] = {0};

        // Create Ui + 1 vector 1
        directionVector1[0] = points[i] - points[(i + 2) % nPoints];
        directionVector1[1] = points[i + 1] - points[(i + 3) % nPoints];

        // Create Ui vector 2
        directionVector2[0] = points[(i+2) % nPoints] - points[(i + 4) % nPoints];
        directionVector2[1] = points[(i + 3) % nPoints] - points[(i + 5) % nPoints];

        // Create perpendicular vector for Pi
        float perpVector1[2] = {0};
        perpVector1[0] = directionVector1[1];
        perpVector1[1] = -directionVector1[0];
        normalized2F(perpVector1);

        // Create perpendicular vector for Pi + 1
        float perpVector2[2] = {0};
        perpVector2[0] = directionVector2[1];
        perpVector2[1] = -directionVector2[0];
        normalized2F(perpVector2);

        // Calculate track points
        float A1[2] = {points[i] + perpVector1[0] * innerTrackW, points[i+1] + perpVector1[1] * innerTrackW};
        float A2[2] = {points[i] + perpVector1[0] * outerTrackW, points[i+1] + perpVector1[1] * outerTrackW};
        float B1[2] = {points[(i+2) % nPoints] + perpVector2[0] * innerTrackW, points[(i+3) % nPoints] + perpVector2[1] * innerTrackW};
        float B2[2] = {points[(i+2) % nPoints] + perpVector2[0] * outerTrackW, points[(i+3) % nPoints] + perpVector2[1] * outerTrackW};

        float C1[2] = {points[i] - perpVector1[0] * innerTrackW, points[i+1] - perpVector1[1] * innerTrackW};
        float C2[2] = {points[i] - perpVector1[0] * outerTrackW, points[i+1] - perpVector1[1] * outerTrackW};
        float D1[2] = {points[(i+2) % nPoints]- perpVector2[0] * innerTrackW, points[(i+3) % nPoints] - perpVector2[1] * innerTrackW};
        float D2[2] = {points[(i+2) % nPoints] - perpVector2[0] * outerTrackW, points[(i+3) % nPoints] - perpVector2[1] * outerTrackW};

        // Draw top of tracks
        glPushMatrix();
            glColor4f(0.2, 0.2, 0.2, 1.0);   //Base 
            glNormal3f(0.0, 1.0, 0.0);

            // Outer
            glVertex3f(A1[0], trackHeight, A1[1]);
            glVertex3f(A2[0], trackHeight, A2[1]);
            glVertex3f(B2[0], trackHeight, B2[1]);
            glVertex3f(B1[0], trackHeight, B1[1]);

            // Inner
            glVertex3f(C1[0], trackHeight, C1[1]);
            glVertex3f(C2[0], trackHeight, C2[1]);
            glVertex3f(D2[0], trackHeight, D2[1]);
            glVertex3f(D1[0], trackHeight, D1[1]);
        glPopMatrix();

        // Draw sides
        glPushMatrix();
            glColor4f(0.2, 0.2, 0.2, 1.0);   //Base color
            // Outer
            normal(A1[0], trackHeight, A1[1],
                   B1[0], trackHeight, B1[1],
                   B1[0], 0, B1[1]);
            glVertex3f(A1[0], trackHeight, A1[1]);
            glVertex3f(B1[0], trackHeight, B1[1]);
            glVertex3f(B1[0], 0, B1[1]);
            glVertex3f(A1[0], 0, A1[1]);

            normal(B2[0], trackHeight, B2[1],
                   A2[0], trackHeight, A2[1],
                   A2[0], 0, A2[1]);
            glVertex3f(A2[0], trackHeight, A2[1]);
            glVertex3f(B2[0], trackHeight, B2[1]);
            glVertex3f(B2[0], 0, B2[1]);
            glVertex3f(A2[0], 0, A2[1]);


        // Inner
            normal(D1[0], trackHeight, D1[1],
                   C1[0], trackHeight, C1[1],
                   C1[0], 0, C1[1]);
            glVertex3f(C1[0], trackHeight, C1[1]);
            glVertex3f(D1[0], trackHeight, D1[1]);
            glVertex3f(D1[0], 0, D1[1]);
            glVertex3f(C1[0], 0, C1[1]);


            normal(C2[0], trackHeight, C1[1],
                   D2[0], trackHeight, D1[1],
                   D2[0], 0, D1[1]);
            glVertex3f(C2[0], trackHeight, C2[1]);
            glVertex3f(D2[0], trackHeight, D2[1]);
            glVertex3f(D2[0], 0, D2[1]);
            glVertex3f(C2[0], 0, C2[1]);
        glPopMatrix();

        if (i % (sleeperSpacing * 2) == 0) {
            float S1[2] = {points[i] - perpVector1[0] * (outerTrackW + sleeperExtraLength), points[i+1] - perpVector1[1] * (outerTrackW + sleeperExtraLength)};
            float S2[2] = {S1[0] + directionVector2[0] * sleeperWidth, S1[1] + directionVector2[1] * sleeperWidth};
            float S4[2] = { points[i] + perpVector1[0] * (outerTrackW + sleeperExtraLength), points[i + 1] + perpVector1[1] * (outerTrackW + sleeperExtraLength) };
            float S3[2] = {S4[0] + directionVector2[0] * sleeperWidth, S4[1] + directionVector2[1] * sleeperWidth};

            // Draw sleepers
            glPushMatrix();
                glColor4f(0.28, 0., 0., 1.0);   //Base color
                glNormal3f(0.0, -1.0, 0.0); // Face normal down so that no fancy lighting is applied. Gives dull look


                // Top of sleeper
                glVertex3f(S1[0], sleeperHeight, S1[1]);
                glVertex3f(S2[0], sleeperHeight, S2[1]);
                glVertex3f(S3[0], sleeperHeight, S3[1]);
                glVertex3f(S4[0], sleeperHeight, S4[1]);

                // Front
                glVertex3f(S1[0], sleeperHeight, S1[1]);
                glVertex3f(S4[0], sleeperHeight, S4[1]);
                glVertex3f(S4[0], 0, S4[1]);
                glVertex3f(S1[0], 0, S1[1]);

                // Back
                glVertex3f(S3[0], sleeperHeight, S3[1]);
                glVertex3f(S2[0], sleeperHeight, S2[1]);
                glVertex3f(S2[0], 0, S2[1]);
                glVertex3f(S3[0], 0, S3[1]);

                // Left
                glVertex3f(S3[0], sleeperHeight, S3[1]);
                glVertex3f(S4[0], sleeperHeight, S4[1]);
                glVertex3f(S4[0], 0, S4[1]);
                glVertex3f(S3[0], 0, S3[1]);

                // Right
                glVertex3f(S2[0], sleeperHeight, S2[1]);
                glVertex3f(S1[0], sleeperHeight, S1[1]);
                glVertex3f(S1[0], 0, S1[1]);
                glVertex3f(S2[0], 0, S2[1]);
            glPopMatrix();
        }
    }
    glEnd();
}

//--------------- RAILWAY TRACK ------------------------------------
//-----------------------------------------------------------------
void tunnel(long startPoint, long length, long padding, long tunnelSize, long straightHeight)
{
    startPoint *= 2;
    length *= 2;
    float tunnelWidth = padding + outerTrackW;

    glColor4f(0.5333, 0.549, 0.5529, 1.0);   //Base 

    // Left Inner wall
    glBegin(GL_QUAD_STRIP);
    for (long i = 0; i < nPoints; i += 2)
    {
        // Only draw within a certain range
        if (!(i >= startPoint && i <= startPoint + length)) {
            continue;
        }

        float directionVector[2] = { 0 };

        // Create Ui vector 1
        directionVector[0] = points[i] - points[(i + 2) % nPoints];
        directionVector[1] = points[i + 1] - points[(i + 3) % nPoints];

        // Create perpendicular vector for Pi
        float perpVector[2] = { 0 };
        perpVector[0] = directionVector[1];
        perpVector[1] = -directionVector[0];
        normalized2F(perpVector);

        // Calculate track points
        float A1[2] = { points[i] + perpVector[0] * tunnelWidth, points[i + 1] + perpVector[1] * tunnelWidth };


        glPushMatrix();
            glNormal3f(perpVector[0], 0.0, perpVector[1]);

            glVertex3f(A1[0], 0, A1[1]);
            glVertex3f(A1[0], straightHeight, A1[1]);
        glPopMatrix();
    }
    glEnd();

    // Right Inner wall
    glBegin(GL_QUAD_STRIP);
    for (long i = 0; i < nPoints; i += 2)
    {
        // Only draw within a certain range
        if (!(i >= startPoint && i <= startPoint + length)) {
            continue;
        }

        // Create Ui vector 1
        float directionVector[2] = { 0 };
        directionVector[0] = points[i] - points[(i + 2) % nPoints];
        directionVector[1] = points[i + 1] - points[(i + 3) % nPoints];

        // Create perpendicular vector for Pi
        float perpVector[2] = { 0 };
        perpVector[0] = directionVector[1];
        perpVector[1] = -directionVector[0];
        normalized2F(perpVector);

        // Calculate wall points
        float B1[2] = { points[i] - perpVector[0] * tunnelWidth, points[i + 1] - perpVector[1] * tunnelWidth };

        glPushMatrix();
            glNormal3f(-perpVector[0], 0.0, -perpVector[1]);

            glVertex3f(B1[0], 0, B1[1]);
            glVertex3f(B1[0], straightHeight, B1[1]);
        glPopMatrix();
    }
    glEnd();

    // Left Outer wall
    glBegin(GL_QUAD_STRIP);
    for (long i = 0; i < nPoints; i += 2)
    {
        // Only draw within a certain range
        if (!(i >= startPoint && i <= startPoint + length)) {
            continue;
        }

        float directionVector[2] = { 0 };

        // Create Ui vector 1
        directionVector[0] = points[i] - points[(i + 2) % nPoints];
        directionVector[1] = points[i + 1] - points[(i + 3) % nPoints];

        // Create perpendicular vector for Pi
        float perpVector[2] = { 0 };
        perpVector[0] = directionVector[1];
        perpVector[1] = -directionVector[0];
        normalized2F(perpVector);

        // Calculate track points
        float A1[2] = { points[i] + perpVector[0] * (tunnelWidth + tunnelSize), points[i + 1] + perpVector[1] * (tunnelWidth + tunnelSize) };


        glPushMatrix();
        glNormal3f(perpVector[0], 0.0, perpVector[1]);

        glVertex3f(A1[0], 0, A1[1]);
        glVertex3f(A1[0], straightHeight, A1[1]);
        glPopMatrix();
    }
    glEnd();

    // Right Outer wall
    glBegin(GL_QUAD_STRIP);
    for (long i = 0; i < nPoints; i += 2)
    {
        // Only draw within a certain range
        if (!(i >= startPoint && i <= startPoint + length)) {
            continue;
        }

        // Create Ui vector 1
        float directionVector[2] = { 0 };
        directionVector[0] = points[i] - points[(i + 2) % nPoints];
        directionVector[1] = points[i + 1] - points[(i + 3) % nPoints];

        // Create perpendicular vector for Pi
        float perpVector[2] = { 0 };
        perpVector[0] = directionVector[1];
        perpVector[1] = -directionVector[0];
        normalized2F(perpVector);

        // Calculate wall points
        float B1[2] = { points[i] - perpVector[0] * (tunnelWidth + tunnelSize), points[i + 1] - perpVector[1] * (tunnelWidth + tunnelSize) };

        glPushMatrix();
        glNormal3f(-perpVector[0], 0.0, -perpVector[1]);

        glVertex3f(B1[0], 0, B1[1]);
        glVertex3f(B1[0], straightHeight, B1[1]);
        glPopMatrix();
    }
    glEnd();

    // Roof
    int p3_hgt = straightHeight + 15;
    for (long i = 0; i < nPoints; i += 2)
    {
        // Only draw within a certain range
        if (!(i >= startPoint && i < startPoint + length)) { // finishes one early
            continue;
        }

        // Create Ui vector 1
        float directionVector[2] = { 0 };
        directionVector[0] = points[i] - points[(i + 2) % nPoints];
        directionVector[1] = points[i + 1] - points[(i + 3) % nPoints];

        // Create Ui vector 2
        float directionVector2[2] = { 0 };
        directionVector2[0] = points[(i + 2) % nPoints] - points[(i + 4) % nPoints];
        directionVector2[1] = points[(i + 3) % nPoints] - points[(i + 5) % nPoints];

        // Create perpendicular vector for Pi
        float perpVector[2] = { 0 };
        perpVector[0] = directionVector[1];
        perpVector[1] = -directionVector[0];
        normalized2F(perpVector);

        // Create perpendicular2 vector for Pi
        float perpVector2[2] = { 0 };
        perpVector2[0] = directionVector2[1];
        perpVector2[1] = -directionVector2[0];
        normalized2F(perpVector2);

        // Calculate wall points
        float A1[2] = { points[i] + perpVector[0] * tunnelWidth, points[i + 1] + perpVector[1] * tunnelWidth };
        float B1[2] = { points[i] - perpVector[0] * tunnelWidth, points[i + 1] - perpVector[1] * tunnelWidth };
        float A2[2] = { points[(i + 2) % nPoints] + perpVector2[0] * tunnelWidth, points[(i + 3) % nPoints] + perpVector2[1] * tunnelWidth };
        float B2[2] = { points[(i + 2) % nPoints] - perpVector2[0] * tunnelWidth, points[(i + 3) % nPoints] - perpVector2[1] * tunnelWidth };

        float outer_A1[2] = { points[i] + perpVector[0] * (tunnelWidth + tunnelSize), points[i + 1] + perpVector[1] * (tunnelWidth + tunnelSize) };
        float outer_B1[2] = { points[i] - perpVector[0] * (tunnelWidth + tunnelSize),points[i + 1] - perpVector[1] * (tunnelWidth + tunnelSize) };
        float outer_A2[2] = { points[(i + 2) % nPoints] + perpVector2[0] * (tunnelWidth + tunnelSize),points[(i + 3) % nPoints] + perpVector2[1] * (tunnelWidth + tunnelSize) };
        float outer_B2[2] = { points[(i + 2) % nPoints] - perpVector2[0] * (tunnelWidth + tunnelSize),points[(i + 3) % nPoints] - perpVector2[1] * (tunnelWidth + tunnelSize) };

        // Inner roof
        int steps = 20;
        glBegin(GL_QUAD_STRIP);
        for (long j = 0; j < steps; j += 1)
        {
            double t = ((double)j) / ((double)(steps - 1));

            float x1 = bez_point(t, A1[0], points[i], B1[0]);
            float y1 = bez_point(t, straightHeight, p3_hgt, straightHeight);
            float z1 = bez_point(t, A1[1], points[i + 1], B1[1]);

            float x2 = bez_point(t, A2[0], points[(i + 2) % nPoints], B2[0]);
            float y2 = bez_point(t, straightHeight, p3_hgt, straightHeight);
            float z2 = bez_point(t, A2[1], points[(i + 3) % nPoints], B2[1]);

            float normal[3] = { x1 - points[i], y1 - straightHeight, z1 - points[i + 1] };
            normalized3F(normal);
            glNormal3f(normal[0], normal[1], normal[2]);

            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y2, z2);
        }
        glEnd();

        // Outer roof
        glBegin(GL_QUAD_STRIP);
        for (long j = 0; j < steps; j += 1)
        {
            double t = ((double)j) / ((double)(steps - 1));

            float x1 = bez_point(t, outer_A1[0], points[i], outer_B1[0]);
            float y1 = bez_point(t, straightHeight, p3_hgt + (tunnelSize * 2), straightHeight);
            float z1 = bez_point(t, outer_A1[1], points[i + 1], outer_B1[1]);

            float x2 = bez_point(t, outer_A2[0], points[(i + 2) % nPoints], outer_B2[0]);
            float y2 = bez_point(t, straightHeight, p3_hgt + (tunnelSize * 2), straightHeight);
            float z2 = bez_point(t, outer_A2[1], points[(i + 3) % nPoints], outer_B2[1]);

            float normal[3] = { x1 - points[i], y1 - straightHeight, z1 - points[i + 1] };
            normalized3F(normal);
            glNormal3f(normal[0], normal[1], normal[2]);

            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y2, z2);
        }
        glEnd();

        // Ends
        if (i == startPoint || i == startPoint + length - 2) {
            glBegin(GL_QUAD_STRIP);
            for (long j = 0; j < steps; j += 1)
            {
                double t = ((double)j) / ((double)(steps - 1));
                float x1, x2, y1, y2, z1, z2;
                if (i == startPoint) {
                    x1 = bez_point(t, A1[0], points[i], B1[0]);
                    y1 = bez_point(t, straightHeight, p3_hgt, straightHeight);
                    z1 = bez_point(t, A1[1], points[i + 1], B1[1]);
                    x2 = bez_point(t, outer_A1[0], points[i], outer_B1[0]);
                    y2 = bez_point(t, straightHeight, p3_hgt + (tunnelSize * 2), straightHeight);
                    z2 = bez_point(t, outer_A1[1], points[i + 1], outer_B1[1]);
                }
                else {
                    x1 = bez_point(t, A2[0], points[(i + 2) % nPoints], B2[0]);
                    y1 = bez_point(t, straightHeight, p3_hgt, straightHeight);
                    z1 = bez_point(t, A2[1], points[(i + 3) % nPoints], B2[1]);
                    x2 = bez_point(t, outer_A2[0], points[(i + 2) % nPoints], outer_B2[0]);
                    y2 = bez_point(t, straightHeight, p3_hgt + (tunnelSize * 2), straightHeight);
                    z2 = bez_point(t, outer_A2[1], points[(i + 3) % nPoints], outer_B2[1]);
                }

                glNormal3f(-directionVector[0], 0, -directionVector[1]);

                glVertex3f(x1, y1, z1);
                glVertex3f(x2, y2, z2);
            }
            glEnd();

            // Draw a second and third end to patch weird criss cross stuff
            glBegin(GL_QUAD_STRIP);
            for (long j = 0; j < steps; j += 1)
            {
                double t = ((double)j) / ((double)(steps - 1));
                float x1, x2, y1, y2, z1, z2;
                x1 = bez_point(t, A1[0], points[i], B1[0]);
                y1 = bez_point(t, straightHeight, p3_hgt, straightHeight);
                z1 = bez_point(t, A1[1], points[i + 1], B1[1]);
                x2 = bez_point(t, outer_A2[0], points[(i + 2) % nPoints], outer_B2[0]);
                y2 = bez_point(t, straightHeight, p3_hgt + (tunnelSize * 2), straightHeight);
                z2 = bez_point(t, outer_A2[1], points[(i + 3) % nPoints], outer_B2[1]);

                glNormal3f(-directionVector[0], 0, -directionVector[1]);

                glVertex3f(x1, y1, z1);
                glVertex3f(x2, y2, z2);
            }
            glEnd();
            glBegin(GL_QUAD_STRIP);
            for (long j = 0; j < steps; j += 1)
            {
                double t = ((double)j) / ((double)(steps - 1));
                float x1, x2, y1, y2, z1, z2;
                x1 = bez_point(t, A2[0], points[(i + 2) % nPoints], B2[0]);
                y1 = bez_point(t, straightHeight, p3_hgt, straightHeight);
                z1 = bez_point(t, A2[1], points[(i + 3) % nPoints], B2[1]);
                x2 = bez_point(t, outer_A1[0], points[i], outer_B1[0]);
                y2 = bez_point(t, straightHeight, p3_hgt + (tunnelSize * 2), straightHeight);
                z2 = bez_point(t, outer_A1[1], points[i + 1], outer_B1[1]);

                glNormal3f(-directionVector[0], 0, -directionVector[1]);

                glVertex3f(x1, y1, z1);
                glVertex3f(x2, y2, z2);
            }
            glEnd();

            glBegin(GL_QUADS);
            for (long j = 0; j < steps; j += 1)
            {
                glNormal3f(-directionVector[0], 0, -directionVector[1]);
                
                glVertex3f(outer_B1[0], straightHeight, outer_B1[1]);
                glVertex3f(B1[0], straightHeight, B1[1]);
                glVertex3f(B1[0], 0, B1[1]);
                glVertex3f(outer_B1[0], 0, outer_B1[1]);

                glVertex3f(outer_A1[0], straightHeight, outer_A1[1]);
                glVertex3f(A1[0], straightHeight, A1[1]);
                glVertex3f(A1[0], 0, A1[1]);
                glVertex3f(outer_A1[0], 0, outer_A1[1]);

                glVertex3f(outer_B2[0], straightHeight, outer_B2[1]);
                glVertex3f(B2[0], straightHeight, B2[1]);
                glVertex3f(B2[0], 0, B2[1]);
                glVertex3f(outer_B2[0], 0, outer_B2[1]);

                glVertex3f(outer_A2[0], straightHeight, outer_A2[1]);
                glVertex3f(A2[0], straightHeight, A2[1]);
                glVertex3f(A2[0], 0, A2[1]);
                glVertex3f(outer_A2[0], 0, outer_A2[1]);
            }
            glEnd();
        }
    }
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

    glPushMatrix();                 //Connector between wagons
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

//--------------- Station -----------------------------------------
//-----------------------------------------------------------------
void station()
{
    float height = 40.0;
    float width = 120.0;
    float length = 20.0;

    float p3_hgt = 30;
    float p3_z = -length / 2;

    glPushMatrix();
    float directionVector[2] = { 0 };
    directionVector[0] = points[(stationPosition + 2) % nPoints] - points[stationPosition];
    directionVector[1] = points[(stationPosition + 3) % nPoints] - points[stationPosition + 1];

    float perpVector[2] = { 0 };
    perpVector[0] = directionVector[1];
    perpVector[1] = -directionVector[0];
    normalized2F(perpVector);

    glTranslatef(points[stationPosition] - (outerTrackW + 7) * perpVector[0], 0, points[stationPosition + 1] - (outerTrackW + 7) * perpVector[1]);
    glRotatef((atan2(directionVector[1], -directionVector[0]) * (180 / PI)) + 180, 0., 1., 0.);
    glScalef(1, 0.5, 0.5);


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
        glColor4f(0.2, 0.2, 0.2, 1.0);
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
        float frontLeft[2] = { -width/2,10 };
        float frontRight[2] = { width/2,10 };
        float backRight[2] = { width/2, 30 };
        float backLeft[2] = { -width/2, 30 };

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, txId2);
        glColor4f(0.8, 0.8, 0.8, 1.0);

        glBegin(GL_QUADS);
        for (int i = 0; i < steps; i++) {
            glNormal3f(0, 0, -1);
            glTexCoord2f(0, 0); glVertex3f(frontLeft[0], 0, frontLeft[1]);
            glTexCoord2f(12, 0); glVertex3f(frontRight[0], 0, frontRight[1]);
            glTexCoord2f(12, 3); glVertex3f(frontRight[0], height, frontRight[1]);
            glTexCoord2f(0, 3); glVertex3f(frontLeft[0], height, frontLeft[1]);

            glNormal3f(0, 0, 1);
            glTexCoord2f(0, 0); glVertex3f(backLeft[0], 0, backLeft[1]);
            glTexCoord2f(12, 0); glVertex3f(backRight[0], 0, backRight[1]);
            glTexCoord2f(12, 3); glVertex3f(backRight[0], height, backRight[1]);
            glTexCoord2f(0, 3); glVertex3f(backLeft[0], height, backLeft[1]);

            glNormal3f(-1, 0, 0);
            glTexCoord2f(0, 0); glVertex3f(backLeft[0], 0, backLeft[1]);
            glTexCoord2f(12, 0); glVertex3f(frontLeft[0], 0, frontLeft[1]);
            glTexCoord2f(12, 3); glVertex3f(frontLeft[0], height, frontLeft[1]);
            glTexCoord2f(0, 3); glVertex3f(backLeft[0], height, backLeft[1]);

            glNormal3f(1, 0, 0);
            glTexCoord2f(0, 0); glVertex3f(backRight[0], 0, backRight[1]);
            glTexCoord2f(12, 0); glVertex3f(frontRight[0], 0, frontRight[1]);
            glTexCoord2f(12, 3); glVertex3f(frontRight[0], height, frontRight[1]);
            glTexCoord2f(0, 3); glVertex3f(backRight[0], height, backRight[1]);
        }
        glEnd();
        glDisable(GL_TEXTURE_2D);


    glPopMatrix();

    glPopMatrix();
}
