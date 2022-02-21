#pragma once
#include <armadillo>
using namespace arma;

extern float ImpThre;
mat ImprecisionFilter(const mat& inLoc);
