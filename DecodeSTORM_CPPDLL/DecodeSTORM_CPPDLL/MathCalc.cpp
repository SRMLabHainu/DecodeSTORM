#include "stdafx.h"
#include"MathCalc.h"

double round(double number, const unsigned int& bits) {
	LL integerPart = number;
	number -= integerPart;
	for (unsigned int i = 0; i < bits; ++i)
		number *= 10;
	number = (LL)(number + 0.5);
	for (unsigned int i = 0; i < bits; ++i)
		number /= 10;
	return integerPart + number;
}

vec HistRate(const vec& Vector) {
	int rows, n;
	int count = 1;
	rows = Vector.n_rows;
	vec Hist;
	Hist.zeros(rows, 1);
	for (int i = 1; i < rows; i++)
	{
		n = i - 1;
		if (Vector(i) == Vector(n))
		{
			count += 1;
		}
		else
		{
			Hist(i) = count;
			count = 1;
		}
		if (i == (rows - 1)) {
			Hist(i) = count;
		}
	}
	uvec index = find(Hist != 0);
	Hist = Hist(index);
	return Hist;

}

struct Unique_Res uniquerows(const mat& XY) {
	int numRows = size(XY, 0);
	bool isSortedA = XY.is_sorted();

	mat sortA;
	uvec indSortA;

	if (isSortedA)
	{
		sortA = XY;
		vec indSortA_Vec = linspace(0, numRows - 1, numRows);
		indSortA = conv_to<uvec>::from(indSortA_Vec);
	}
	else
	{
		vector<vector<double>> XY_Cvector = Arma2Std_2D(XY);
		vector<size_t> indSortA_Cvector = sort_index(XY_Cvector);
		indSortA = conv_to<uvec>::from(indSortA_Cvector);
		sort(XY_Cvector.begin(), XY_Cvector.end());
		sortA = Std2Arma_2D(XY_Cvector);
	}

	umat groupsSortA = sortA.rows(0, numRows - 2) != sortA.rows(1, numRows - 1);
	groupsSortA = any(groupsSortA, 1);
	umat True{ 1 };
	groupsSortA.insert_rows(0, True);
	mat uniqueRes = sortA.rows(find(groupsSortA == 1));// Create unique list by indexing into sorted list.
	//Find indA.
	uvec indA = indSortA(find(groupsSortA == 1));//Find the indices of the sorted logical.
	//Find indC.
	uvec indC = cumsum(groupsSortA);
	indC(indSortA) = indC - 1;

	Unique_Res res;
	res.Unique = uniqueRes;
	res.Sort_indA = indA;
	res.Sort_indC = indC;
	return res;
}