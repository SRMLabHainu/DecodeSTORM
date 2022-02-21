#pragma once
#include <armadillo>
#include <omp.h>
using namespace arma;

extern float StDev;
extern float distThre;

mat NoiseFilter(const mat& inLoc);