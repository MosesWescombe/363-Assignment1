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
    glGenTextures(1, &txId);                // Create a Texture object
    glBindTexture(GL_TEXTURE_2D, txId);     //Use this texture
    loadBMP("roof.bmp");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   //Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

// Bezier curve equation
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

float vectorLength2F(float vector[2]) {
    return sqrt(pow(vector[0], 2) + pow(vector[1], 2));
}

void normalized2F(float vector[2]) {
    float length = vectorLength2F(vector);
    float normalVector[2] = {vector[0] / length, vector[1] / length};
    vector[0] = normalVector[0];
    vector[1] = normalVector[1];
}

//--------------- RAILWAY TRACK ------------------------------------
// A circular railway track of specified median radius and width
// The tracks are modelled using small quadrilateral segments each covering 5 deg arc
// The tracks have a height of 1 unit (refer to lab notes)
//-----------------------------------------------------------------
void tracks()
{
    long nPoints = 492 * 2;
    float points[] = {0.0, -40.0, -1.0, -40.0, -2.0, -40.0, -3.0, -40.0, -4.0, -40.0, -5.0, -40.0, -6.0, -40.0, -7.0, -40.0, -8.0, -40.0, -9.0, -40.0, -10.0, -40.0, -11.0, -40.0, -12.0, -40.0, -13.0, -40.0, -14.0, -40.0, -15.0, -40.0, -16.0, -40.0, -17.0, -40.0, -18.0, -40.0, -19.0, -40.0, -20.0, -40.0, -21.0, -40.0, -22.0, -40.0, -23.0, -40.0, -24.0, -40.0, -25.0, -40.0, -26.0, -40.0, -27.0, -40.0, -28.0, -40.0, -29.0, -40.0, -30.0, -40.0, -31.0, -40.0, -32.0, -40.0, -33.0, -40.0, -34.0, -40.0, -35.0, -40.0, -36.0, -40.0, -37.0, -40.0, -38.0, -40.0, -39.0, -40.0, -40.0, -40.0, -41.0, -40.0, -42.0, -40.0, -43.0, -40.0, -44.0, -40.0, -45.0, -40.0, -46.0, -40.0, -47.0, -40.0, -48.0, -40.0, -49.0, -40.0, -50.0, -40.0, -51.0, -40.0, -52.0, -40.0, -53.0, -40.0, -54.0, -40.0, -55.0, -40.0, -56.0, -40.0, -57.0, -40.0, -58.0, -40.0, -59.0, -40.0, -60.0, -40.0, -60.9999, -39.9875, -61.9992, -39.95, -62.9972, -39.8876, -63.9933, -39.8002, -64.987, -39.6879, -65.9775, -39.5508, -66.9643, -39.3891, -67.9468, -39.2027, -68.9243, -38.9918, -69.8962, -38.7565, -70.8619, -38.497, -71.8208, -38.2135, -72.7724, -37.906, -73.7159, -37.5749, -74.6509, -37.2203, -75.5767, -36.8424, -76.4928, -36.4415, -77.3986, -36.0179, -78.2935, -35.5717, -79.177, -35.1033, -80.0485, -34.613, -80.9075, -34.101, -81.7534, -33.5677, -82.5857, -33.0134, -83.4039, -32.4385, -84.2075, -31.8434, -84.9959, -31.2283, -85.7687, -30.5937, -86.5254, -29.94, -87.2655, -29.2676, -87.9886, -28.5768, -88.6942, -27.8683, -89.3819, -27.1423, -90.0512, -26.3993, -90.7017, -25.6399, -91.3331, -24.8644, -91.9448, -24.0734, -92.5366, -23.2673, -93.1081, -22.4467, -93.6588, -21.6121, -94.1886, -20.764, -94.6969, -19.9029, -95.1836, -19.0293, -95.6483, -18.1439, -96.0907, -17.2471, -96.5105, -16.3395, -96.9076, -15.4217, -97.2816, -14.4943, -97.6322, -13.5579, -97.9594, -12.6129, -98.2628, -11.6601, -98.5423, -10.7, -98.7977, -9.73318, -99.0289, -8.76029, -99.2357, -7.78194, -99.418, -6.79872, -99.5756, -5.81124, -99.7085, -4.82014, -99.8166, -3.82603, -99.8998, -2.82952, -99.9581, -1.83125, -99.9913, -0.831828, -99.9996, 0.16811, -99.9829, 1.16794, -99.9413, 2.16705, -99.8746, 3.1648, -99.783, 4.16057, -99.6666, 5.15374, -99.5254, 6.14369, -99.3594, 7.1298, -99.1689, 8.11145, -98.9539, 9.08804, -98.7146, 10.0589, -98.451, 11.0236, -98.1634, 11.9813, -97.852, 12.9315, -97.5169, 13.8737, -97.1584, 14.8072, -96.7767, 15.7314, -96.3719, 16.6458, -95.9445, 17.5498, -95.4945, 18.4429, -95.0224, 19.3244, -94.5284, 20.1938, -94.0128, 21.0506, -93.476, 21.8943, -92.9182, 22.7243, -92.3399, 23.54, -91.7413, 24.3411, -91.1229, 25.1269, -90.4851, 25.8971, -89.8282, 26.651, -89.1527, 27.3883, -88.4589, 28.1085, -87.7474, 28.8111, -87.0185, 29.4958, -86.2728, 30.1619, -85.5106, 30.8093, -84.7325, 31.4373, -83.9389, 32.0458, -83.1303, 32.6342, -82.3073, 33.2022, -81.4704, 33.7494, -80.62, 34.2756, -79.7568, 34.7803, -78.8812, 35.2633, -77.9938, 35.7243, -77.0952, 36.1629, -76.1858, 36.5789, -75.2664, 36.9721, -74.3374, 37.3422, -73.3995, 37.6889, -72.4531, 38.0121, -71.4991, 38.3115, -70.5378, 38.587, -69.5699, 38.8383, -68.596, 39.0654, -67.6168, 39.2681, -66.6328, 39.4462, -65.6447, 39.5997, -64.653, 39.7284, -63.6585, 39.8323, -62.6616, 39.9114, -61.6631, 39.9654, -60.6636, 39.9945, -60.0, 40.0, -59.0, 40.0, -58.0, 40.0, -57.0, 40.0, -56.0, 40.0, -55.0, 40.0, -54.0, 40.0, -53.0, 40.0, -52.0, 40.0, -51.0, 40.0, -50.0, 40.0, -49.0, 40.0, -48.0, 40.0, -47.0, 40.0, -46.0, 40.0, -45.0, 40.0, -44.0, 40.0, -43.0, 40.0, -42.0, 40.0, -41.0, 40.0, -40.0, 40.0, -39.0, 40.0, -38.0, 40.0, -37.0, 40.0, -36.0, 40.0, -35.0, 40.0, -34.0, 40.0, -33.0, 40.0, -32.0, 40.0, -31.0, 40.0, -30.0, 40.0, -29.0, 40.0, -28.0, 40.0, -27.0, 40.0, -26.0, 40.0, -25.0, 40.0, -24.0, 40.0, -23.0, 40.0, -22.0, 40.0, -21.0, 40.0, -20.0, 40.0, -19.0, 40.0, -18.0, 40.0, -17.0, 40.0, -16.0, 40.0, -15.0, 40.0, -14.0, 40.0, -13.0, 40.0, -12.0, 40.0, -11.0, 40.0, -10.0, 40.0, -9.0, 40.0, -8.0, 40.0, -7.0, 40.0, -6.0, 40.0, -5.0, 40.0, -4.0, 40.0, -3.0, 40.0, -2.0, 40.0, -1.0, 40.0, 0.0, 40.0, 1.0, 40.0, 2.0, 40.0, 3.0, 40.0, 4.0, 40.0, 5.0, 40.0, 6.0, 40.0, 7.0, 40.0, 8.0, 40.0, 9.0, 40.0, 10.0, 40.0, 11.0, 40.0, 12.0, 40.0, 13.0, 40.0, 14.0, 40.0, 15.0, 40.0, 16.0, 40.0, 17.0, 40.0, 18.0, 40.0, 19.0, 40.0, 20.0, 40.0, 21.0, 40.0, 22.0, 40.0, 23.0, 40.0, 24.0, 40.0, 25.0, 40.0, 26.0, 40.0, 27.0, 40.0, 28.0, 40.0, 29.0, 40.0, 30.0, 40.0, 31.0, 40.0, 32.0, 40.0, 33.0, 40.0, 34.0, 40.0, 35.0, 40.0, 36.0, 40.0, 37.0, 40.0, 38.0, 40.0, 39.0, 40.0, 40.0, 40.0, 41.0, 40.0, 42.0, 40.0, 43.0, 40.0, 44.0, 40.0, 45.0, 40.0, 46.0, 40.0, 47.0, 40.0, 48.0, 40.0, 49.0, 40.0, 50.0, 40.0, 51.0, 40.0, 52.0, 40.0, 53.0, 40.0, 54.0, 40.0, 55.0, 40.0, 56.0, 40.0, 57.0, 40.0, 58.0, 40.0, 59.0, 40.0, 60.0, 40.0, 60.9999, 39.9875, 61.9992, 39.95, 62.9972, 39.8876, 63.9933, 39.8002, 64.987, 39.6879, 65.9775, 39.5508, 66.9643, 39.3891, 67.9468, 39.2027, 68.9243, 38.9918, 69.8962, 38.7565, 70.8619, 38.497, 71.8208, 38.2135, 72.7724, 37.906, 73.7159, 37.5749, 74.6509, 37.2203, 75.5767, 36.8424, 76.4928, 36.4415, 77.3986, 36.0179, 78.2935, 35.5717, 79.177, 35.1033, 80.0485, 34.613, 80.9075, 34.101, 81.7534, 33.5677, 82.5857, 33.0134, 83.4039, 32.4385, 84.2075, 31.8434, 84.9959, 31.2283, 85.7687, 30.5937, 86.5254, 29.94, 87.2655, 29.2676, 87.9886, 28.5768, 88.6942, 27.8683, 89.3819, 27.1423, 90.0512, 26.3993, 90.7017, 25.6399, 91.3331, 24.8644, 91.9448, 24.0734, 92.5366, 23.2673, 93.1081, 22.4467, 93.6588, 21.6121, 94.1886, 20.764, 94.6969, 19.9029, 95.1836, 19.0293, 95.6483, 18.1439, 96.0907, 17.2471, 96.5105, 16.3395, 96.9076, 15.4217, 97.2816, 14.4943, 97.6322, 13.5579, 97.9594, 12.6129, 98.2628, 11.6601, 98.5423, 10.7, 98.7977, 9.73318, 99.0289, 8.76029, 99.2357, 7.78194, 99.418, 6.79872, 99.5756, 5.81124, 99.7085, 4.82014, 99.8166, 3.82603, 99.8998, 2.82952, 99.9581, 1.83125, 99.9913, 0.831828, 99.9996, -0.16811, 99.9829, -1.16794, 99.9413, -2.16705, 99.8746, -3.1648, 99.783, -4.16057, 99.6666, -5.15374, 99.5254, -6.14369, 99.3594, -7.1298, 99.1689, -8.11145, 98.9539, -9.08804, 98.7146, -10.0589, 98.451, -11.0236, 98.1634, -11.9813, 97.852, -12.9315, 97.5169, -13.8737, 97.1584, -14.8072, 96.7767, -15.7314, 96.3719, -16.6458, 95.9445, -17.5498, 95.4945, -18.4429, 95.0224, -19.3244, 94.5284, -20.1938, 94.0128, -21.0506, 93.476, -21.8943, 92.9182, -22.7243, 92.3399, -23.54, 91.7413, -24.3411, 91.1229, -25.1269, 90.4851, -25.8971, 89.8282, -26.651, 89.1527, -27.3883, 88.4589, -28.1085, 87.7474, -28.8111, 87.0185, -29.4958, 86.2728, -30.1619, 85.5106, -30.8093, 84.7325, -31.4373, 83.9389, -32.0458, 83.1303, -32.6342, 82.3073, -33.2022, 81.4704, -33.7494, 80.62, -34.2756, 79.7568, -34.7803, 78.8812, -35.2633, 77.9938, -35.7243, 77.0952, -36.1629, 76.1858, -36.5789, 75.2664, -36.9721, 74.3374, -37.3422, 73.3995, -37.6889, 72.4531, -38.0121, 71.4991, -38.3115, 70.5378, -38.587, 69.5699, -38.8383, 68.596, -39.0654, 67.6168, -39.2681, 66.6328, -39.4462, 65.6447, -39.5997, 64.653, -39.7284, 63.6585, -39.8323, 62.6616, -39.9114, 61.6631, -39.9654, 60.6636, -39.9945, 60.0, -40.0, 59.0, -40.0, 58.0, -40.0, 57.0, -40.0, 56.0, -40.0, 55.0, -40.0, 54.0, -40.0, 53.0, -40.0, 52.0, -40.0, 51.0, -40.0, 50.0, -40.0, 49.0, -40.0, 48.0, -40.0, 47.0, -40.0, 46.0, -40.0, 45.0, -40.0, 44.0, -40.0, 43.0, -40.0, 42.0, -40.0, 41.0, -40.0, 40.0, -40.0, 39.0, -40.0, 38.0, -40.0, 37.0, -40.0, 36.0, -40.0, 35.0, -40.0, 34.0, -40.0, 33.0, -40.0, 32.0, -40.0, 31.0, -40.0, 30.0, -40.0, 29.0, -40.0, 28.0, -40.0, 27.0, -40.0, 26.0, -40.0, 25.0, -40.0, 24.0, -40.0, 23.0, -40.0, 22.0, -40.0, 21.0, -40.0, 20.0, -40.0, 19.0, -40.0, 18.0, -40.0, 17.0, -40.0, 16.0, -40.0, 15.0, -40.0, 14.0, -40.0, 13.0, -40.0, 12.0, -40.0, 11.0, -40.0, 10.0, -40.0, 9.0, -40.0, 8.0, -40.0, 7.0, -40.0, 6.0, -40.0, 5.0, -40.0, 4.0, -40.0, 3.0, -40.0, 2.0, -40.0, 1.0, -40.0};

    float trackWidth = 1.5;
    float innerTrackW = 3;
    float outerTrackW = innerTrackW + trackWidth;
    float trackHeight = 1;

    float sleeperHeight = 0.5;
    long sleeperSpacing = 2;
    float sleeperExtraLength = 1;
    float sleeperWidth = 2;

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

        if (i % sleeperSpacing == 0) {
            float S1[2] = {points[i] - perpVector1[0] * (outerTrackW + sleeperExtraLength), points[i+1] - perpVector1[1] * (outerTrackW + sleeperExtraLength)};
            float S2[2] = {points[i] + perpVector1[0] * (outerTrackW + sleeperExtraLength), points[i+1] + perpVector1[1] * (outerTrackW + sleeperExtraLength)};
            float S3[2] = {S1[0] + directionVector1[0] * sleeperWidth, S1[1] + directionVector1[1] * sleeperWidth};

            // Draw top of sleepers
            glPushMatrix();
                glNormal3f(0.0, 1.0, 0.0);

                glVertex3f(A1[0], sleeperHeight, A1[1]);
                glVertex3f(A2[0], sleeperHeight, A2[1]);
                glVertex3f(B2[0], sleeperHeight, B2[1]);
                glVertex3f(B1[0], sleeperHeight, B1[1]);;
            glPopMatrix();

            // Draw sides of sleepers
            glPushMatrix();
                glVertex3f(A1[0], trackHeight, A1[1]);
                glVertex3f(B1[0], trackHeight, B1[1]);
                glVertex3f(B1[0], 0, B1[1]);
                glVertex3f(A1[0], 0, A1[1]);

                glVertex3f(A2[0], trackHeight, A2[1]);
                glVertex3f(B2[0], trackHeight, B2[1]);
                glVertex3f(B2[0], 0, B2[1]);
                glVertex3f(A2[0], 0, A2[1]);
            glPopMatrix();
        }
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
    float p3_z = -length;

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
