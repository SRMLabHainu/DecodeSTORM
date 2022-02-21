#pragma once
#include <armadillo>
#include"ContainerTrans.h"
using namespace arma;
using namespace std;

extern mat inLoc_Ch1, inLoc_Ch2, RoiLoc1, RoiLoc2, RawRoiLoc1, RawRoiLoc2;
extern double UpLeftX, UpLeftY, LowRightX, LowRightY;
extern string FilePathCh1, FilePathCh2;
extern double* inLoc_Ch1X; 
extern double* inLoc_Ch1Y; 
extern double* inLoc_Ch2X;
extern double* inLoc_Ch2Y;
extern double* RoiLoc1X;
extern double* RoiLoc1Y;
extern double* RoiLoc2X;
extern double* RoiLoc2Y;
void GetinLoc1();
void GetinLoc2();
void ExtractRoiLoc1();
void ExtractRoiLoc2();

