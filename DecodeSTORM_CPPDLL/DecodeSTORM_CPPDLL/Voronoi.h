#pragma once
#include <armadillo>
#pragma once
#include <armadillo>
#include"MathCalc.h"
#include"GetLocAndRoi.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace arma;
using namespace cv;

extern  cv::Mat main_img;
extern string VoronoiResSave;
extern int VoronoiClusterNum;
class Voronoi
{
private:
	
	mat uni_inLoc, Csmat;
	cv::Mat img;

private:
	void DrawVoronoiDiagram(const mat& inLoc);
	int VoronoiMonteCarlo(const mat& inLoc);
	int CaculateClusNum(int threshold);
	int Fov(const mat& inLoc);
	vec VorArea_ia(const mat& inLoc);
	mat filterROI(const mat& inLoc);
	vec histcenters(const vec& Vec, int bins);
	vec intersection(const vec& x1, const vec& y1, const vec& x2, const vec& y2);
	vector<mat> VorCluster(const vector<mat>& UniqueCluster, colvec conver);
	mat BsxFunEq(const mat& Mat1, const mat& Mat2);

public:
	void VoronoiDiagram(const mat& inLoc);
	
};
