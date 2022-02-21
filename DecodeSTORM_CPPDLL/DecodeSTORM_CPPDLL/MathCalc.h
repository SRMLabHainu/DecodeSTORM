#pragma once
#include <vector>
#include <armadillo>
#include"ContainerTrans.h"

using namespace arma;
using namespace std;
typedef long long LL;

struct Unique_Res {
	mat Unique;
	uvec Sort_indA, Sort_indC;
};


template <typename T>
vector<size_t>  sort_index(const vector<T>& matrix) {

	// initialize original index locations
	vector<size_t>  Sort_ind(matrix.size());
	for (size_t i = 0; i != Sort_ind.size(); ++i) Sort_ind[i] = i;

	// sort indexes based on comparing values in v
	sort(Sort_ind.begin(), Sort_ind.end(),
		[&matrix](size_t i1, size_t i2) {return matrix[i1] < matrix[i2]; });

	return Sort_ind;
}


double round(double number, const unsigned int& bits);
vec HistRate(const vec& Vector);
struct Unique_Res uniquerows(const mat& XY);