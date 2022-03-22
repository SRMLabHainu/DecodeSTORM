#pragma once
#include <armadillo>
#include "kdtree2.hpp"
#include"MathCalc.h"
#include"CommonPara.h"

using namespace arma;


extern float Rmax, Step, ColoThres;
extern double width, height;
extern double* CBC1Array, * CBC2Array;
extern int CBC1Size, CBC2Size;
extern float CBC1Range, CBC2Range;
extern rowvec CBC_Results;


class Colocalization
{
private:

	double SizeROI, r;

private:

	vec NumOfRangeSearch(const mat& matrix_1, const mat& matrix_2, double radius);
	struct Res Lr_Subfun(const mat& matrix_1, const mat& matrix_2);
	vector<double> R_nearestSearch(vector<double>& x1, vector<double>& y1, vector<double>& x2, vector<double>& y2);
	double Corr_Spearman(const mat& X, const mat& Y);
	vec NearestSearch(const mat& matrix_1, const mat& matrix_2);

public:

	void ColocalizationCal(const mat& inLoc1, const mat& inLoc2, const string& PrefixName);
};

