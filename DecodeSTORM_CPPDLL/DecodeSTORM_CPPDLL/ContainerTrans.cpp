#include "stdafx.h"
#include "ContainerTrans.h"


vector<vector<double>> Arma2Std_2D(const mat& InMatrixArma) {
	vector<vector<double>> OutMatrixStd;
	for (int i = 0; i < InMatrixArma.n_rows; ++i) {
		vector<double> vectorRow;
		for (int j = 0; j < InMatrixArma.n_cols; j++)
		{
			vectorRow.push_back(InMatrixArma(i, j));

		}
		OutMatrixStd.push_back(vectorRow);
	}
	return OutMatrixStd;
}


mat Std2Arma_2D(vector<vector<double>>& InMatrixStd) {
	mat OutMatrixArma(InMatrixStd.size(), InMatrixStd[0].size());
	for (int i = 0; i < InMatrixStd.size(); i++) {
		for (int j = 0; j < InMatrixStd[0].size(); j++)
		{
			OutMatrixArma(i, j) = InMatrixStd[i][j];
		}

	}
	return OutMatrixArma;
}


double* ArmaVecToArrayD(vec Vector) {
	int n = Vector.size();
	double* Array = NULL;
	Array = new double[n];
	for (size_t i = 0; i < n; i++)
	{
		Array[i] = Vector(i);
	}
	return Array;
}

int* ArmaVecToArrayI(vec Vector) {
	int n = Vector.size();
	int* Array = NULL;
	Array = new int[n];
	for (size_t i = 0; i < n; i++)
	{
		Array[i] = Vector(i);
	}
	return Array;
}

vec ArmaMatToArmaVec(mat& matrix) {
	vec vector(matrix.n_rows * matrix.n_cols);
	int i = 0;
	for (size_t j = 0; j < matrix.n_rows; j++)
	{
		for (size_t k = 0; k < matrix.n_cols; k++)
		{
			vector(i) = matrix(j, k);
			i++;
		}
	}
	return vector;
}