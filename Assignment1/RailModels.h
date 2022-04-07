//  ========================================================================
//  COSC363: Computer Graphics (2022);  University of Canterbury.
//
//  RailModels.h
//  Header file for a collection of models for a railway scene
//  ========================================================================

extern long nPoints;
extern float points[];

void floor();

void bridge(long startPoint, long length, long padding);

void tracks();

void engine();

void wagon();

void station();

void loadTexture();
