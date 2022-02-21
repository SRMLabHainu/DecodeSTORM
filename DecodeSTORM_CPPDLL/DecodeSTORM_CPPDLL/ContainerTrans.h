#pragma once
#include <vector>
#include <armadillo>
using namespace std;
using namespace arma;


vector<vector<double>> Arma2Std_2D(const mat& inMatrixArma);

mat Std2Arma_2D(vector<vector<double>>& inMatrixStd);

double* ArmaVecToArrayD(vec Vector);

int* ArmaVecToArrayI(vec Vector);

vec ArmaMatToArmaVec(mat& matrix);


