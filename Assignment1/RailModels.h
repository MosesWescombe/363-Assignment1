//  ========================================================================
//  COSC363: Computer Graphics (2022);  University of Canterbury.
//
//  RailModels.h
//  Header file for a collection of models for a railway scene
//  ========================================================================

extern const long nPoints;
extern float points[];
extern long stationPosition;

void generateTrackPoints();

float vectorLength2F(float vector[2]);
void normalized2F(float vector[2]);

void floor();

void tunnel(long startPoint, long length, long padding, long tunnelSize, long straightHeight);

void tracks();

void engine();

void wagon();

void station();

void loadRoofTexture();

void loadBrickTexture();
