#pragma once
#include <armadillo>
#include"MathCalc.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace arma;
using namespace cv;

extern  cv::Mat main_img;
extern string VoronoiResSave;
void VoronoiDiagram(const mat& inLoc);
