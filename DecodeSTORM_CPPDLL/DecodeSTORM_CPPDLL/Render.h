#pragma once
#include <armadillo>
#include <afx.h>
#include "CommonPara.h"
#include"DriftCorr.h"
#include <math.h>

using namespace std;
using namespace arma;


extern int MaxImageVal;
extern int SRImageWidth;
extern int SRImageHeight;

float* Render(mat inLoc, int zoomf);

