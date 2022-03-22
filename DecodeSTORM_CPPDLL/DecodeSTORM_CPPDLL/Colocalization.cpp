#include "stdafx.h"
#include "Colocalization.h"

float  Rmax, Step, ColoThres;
double width, height;
double* CBC1Array, * CBC2Array;
int CBC1Size, CBC2Size;
float CBC1Range, CBC2Range;
rowvec CBC_Results;

vec Colocalization::NumOfRangeSearch(const mat& matrix_1, const mat& matrix_2, double radius) {
	vec nParts;
	nParts.zeros(matrix_2.n_rows, 1);
	vec D;
 
	for (int i = 0; i < matrix_2.n_rows; i++)
	{
		D.zeros(matrix_2.n_rows, 1);
		D = sqrt(pow(matrix_1.col(1) - matrix_2(i, 1), 2) + pow(matrix_1.col(2) - matrix_2(i, 2), 2) + pow(matrix_1.col(3) - matrix_2(i, 3), 2));
		uvec indicate = find(D <= radius);
		nParts(i, 0) = indicate.n_rows;
	}
	return nParts;
}





vector<double> Colocalization::R_nearestSearch(vector<double>& x1, vector<double>& y1, vector<double>& x2, vector<double>& y2) {
	int N1, N2;
	double r2;
	N1 = x1.size();
	N2 = x2.size();
	r2 = r * r;
	vector<double> num_points(N2);
	array2dfloat input_data;
	for (int i = 0; i < N1; i++)
	{
		vector<float> point(2);
		point[0] = (float)x1[i];
		point[1] = (float)y1[i];
		input_data.push_back(point);
	}
	kdtree2* tree = new kdtree2(input_data, false);
	for (int i = 0; i < N2; i++)
	{
		vector<float> point(2);
		point[0] = (float)x2[i];
		point[1] = (float)y2[i];

		kdtree2_result_vector result_vec;

		tree->r_nearest(point, r2, result_vec);

		num_points[i] = result_vec.size();
	}

	input_data.clear();

	// delete tree
	delete tree;
	return num_points;

}

double Colocalization::Corr_Spearman(const mat& X, const mat& Y) {
	
	if (size(X, 0) != size(Y, 0))
	{
		printf("Dimensions are not equal");
		exit(0);
	}
	int N = size(X, 0);
	mat Xrank = zeros(1, N);
	mat Yrank = zeros(1, N);

	for (int i = 0; i < N; i++)
	{
		int cont1_x = 1;
		int cont2_x = -1;
		int cont1_y = 1;
		int cont2_y = -1;
		for (int j = 0; j < N; j++)
		{
			if (X(i) < X(j)) {
				cont1_x = cont1_x + 1;
			}
			else if (X(i) == X(j)) {
				cont2_x = cont2_x + 1;
			}
			if (Y(i) < Y(j)) {
				cont1_y = cont1_y + 1;
			}
			else if (Y(i) == Y(j)) {
				cont2_y = cont2_y + 1;
			}
		}
		Xrank(i) = cont1_x + mean(linspace(0, cont2_x, cont2_x + 1));
		Yrank(i) = cont1_y + mean(linspace(0, cont2_y, cont2_y + 1));
	}
	double dd = 6 * accu(pow(Xrank - Yrank, 2));
	double SpCorr = 1 - (dd / (N * (pow(N, 2) - 1)));
	return SpCorr;
}

vec Colocalization::NearestSearch(const mat& matrix_1, const mat& matrix_2) {
	vec nearest;
	nearest.zeros(matrix_2.n_rows, 1);
	vec D;
	  
	for (int i = 0; i < matrix_2.n_rows; i++)
	{
		D.zeros(matrix_2.n_rows, 1);
		D = sqrt(pow(matrix_1.col(1) - matrix_2(i, 1), 2) + pow(matrix_1.col(2) - matrix_2(i, 2), 2) + pow(matrix_1.col(3) - matrix_2(i, 3), 2));
		nearest(i, 0) = D.min();
	}
	return nearest;
}

void Colocalization::ColocalizationCal(const mat& inLoc1, const mat& inLoc2, const string& PrefixName)
{


	SizeROI = width * height;
	//Caculate Lr and Density from localization of all channel



	//Lr_r above threshold


	//mat thre_channel_1 = inLoc1Ext.rows(index_thre_channel_1);
	//mat thre_channel_2 = inLoc2Ext.rows(index_thre_channel_2);
	vec x1 = inLoc1.col(1);
	vec y1 = inLoc1.col(2);
	vec x2 = inLoc2.col(1);
	vec y2 = inLoc2.col(2);


	double D1max = inLoc1.n_rows / pow(SizeROI, 2);
	double D2maxCh1Ch2 = inLoc2.n_rows / pow(SizeROI, 2);
	vec D2maxCh2Ch1 = NumOfRangeSearch(inLoc1, inLoc2, Rmax) / pow(Rmax, 2);
	mat N11 = zeros(inLoc1.n_rows, ceil(Rmax / Step));
	mat N12 = zeros(inLoc1.n_rows, ceil(Rmax / Step));
	mat N22 = zeros(inLoc2.n_rows, ceil(Rmax / Step));
	mat N21 = zeros(inLoc2.n_rows, ceil(Rmax / Step));


	vector<double> x1_ = conv_to< vector<double> >::from(x1);
	vector<double> y1_ = conv_to< vector<double> >::from(y1);
	vector<double> x2_ = conv_to< vector<double> >::from(x2);
	vector<double> y2_ = conv_to< vector<double> >::from(y2);
	//Calculate Ch1->Ch2 CBC and Ch2->Ch1 CBC
	mat num_points;
	for (int i = 1; i <= ceil(Rmax / Step); i++)
	{
		r = Step * i;
		num_points = conv_to<mat>::from(R_nearestSearch(x1_, y1_, x1_, y1_)) - 1;
		N11.col(i - 1) = num_points / (D1max * pow(r, 2));

		num_points = conv_to<mat>::from(R_nearestSearch(x2_, y2_, x1_, y1_));
		N12.col(i - 1) = num_points / (D2maxCh1Ch2 * pow(r, 2));

		num_points = conv_to<mat>::from(R_nearestSearch(x2_, y2_, x2_, y2_)) - 1;
		N22.col(i - 1) = num_points / (D1max * pow(r, 2));

		num_points = conv_to<mat>::from(R_nearestSearch(x1_, y1_, x2_, y2_));
		N21.col(i - 1) = num_points / (D2maxCh2Ch1 * pow(r, 2));
	}
	//Correlating coefficients
	vec SA1 = zeros(x1.n_rows, 1);
	vec SA2 = zeros(x2.n_rows, 1);
	N11.replace(datum::nan, 0);
	N12.replace(datum::nan, 0);
	N22.replace(datum::nan, 0);
	N21.replace(datum::nan, 0);
	for (int i = 0; i < size(x1, 0); i++)
	{
		SA1(i) = Corr_Spearman(N11.row(i).t(), N12.row(i).t());
		if (i < size(x2, 0))
		{
			SA2(i) = Corr_Spearman(N22.row(i).t(), N21.row(i).t());
		}
	}

	vec SA1_(size(SA1));
	for (int i = 0; i < size(SA1, 0); i++)
	{
		SA1_(i) = round(SA1(i), 4);
	}
	uvec index_SA1 = SA1_ == 0.5;
	uvec index_SA1_id = find(index_SA1 == 1);
	vec zero_SA1_ = zeros(index_SA1_id.n_rows, 1);
	SA1.rows(index_SA1_id) = zero_SA1_;

	vec SA2_(size(SA2));
	for (int i = 0; i < size(SA2, 0); i++)
	{
		SA2_(i) = round(SA2(i), 4);
	}
	uvec index_SA2 = SA2_ == 0.5;
	uvec index_SA2_id = find(index_SA2 == 1);
	vec zero_SA2_ = zeros(index_SA2_id.n_rows, 1);
	SA2.rows(index_SA2_id) = zero_SA2_;

	vec channel_1_CBC = zeros(inLoc1.n_rows, 1);
	vec channel_2_CBC = zeros(inLoc2.n_rows, 1);
	//Caculate CBC
	vec NND1 = NearestSearch(inLoc2, inLoc1);
	channel_1_CBC = SA1 % exp(-NND1 / Rmax);
	vec NND2 = NearestSearch(inLoc1, inLoc2);
	channel_2_CBC = SA2 % exp(-NND2 / Rmax);

	vec CBC_1 = channel_1_CBC;
	vec CBC_2 = channel_2_CBC;
	CBC1Range = max(CBC_1) - min(CBC_1);
	CBC2Range = max(CBC_2) - min(CBC_2);
	CBC1Size = CBC_1.size();
	CBC2Size = CBC_2.size();
	CBC1Array = ArmaVecToArrayD(CBC_1);
	CBC2Array = ArmaVecToArrayD(CBC_2);

	//Save percentage of colocalised Ch1 molecules and Percentage of colocalised Ch2 molecules
	float Ch1_ColocAbove = sum(CBC_1 >= ColoThres);
	float Ch1_Num = inLoc1.n_rows;
	float Ch2_ColocAbove = sum(CBC_2 >= ColoThres);
	float Ch2_Num = inLoc2.n_rows;
	double Percent_Ch1_ColocAbove = Ch1_ColocAbove / Ch1_Num;
	double Percent_Ch2_ColocAbove = Ch2_ColocAbove / Ch2_Num;
	CBC_Results.zeros(2);
	CBC_Results(0) = Percent_Ch1_ColocAbove;
	CBC_Results(1) = Percent_Ch2_ColocAbove;

}

