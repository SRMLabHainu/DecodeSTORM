#pragma once
#include <armadillo>
#include"MathCalc.h"
#include"OpenConsole.h"
#include"CommonPara.h"
using namespace std;
using namespace arma;


extern float MaxAnaDis, RingWidth;
extern mat RipleyH_Result;

class QuantCluster
{
private:
	vec DisMatByXr, Xr;
	double density;
	int N;

public:

	double* Gr_Arr;
	double* Xr_Arr;
	double* Hr_Arr;
	int Xr_Size;
	int RdfBorderMax, RdfBorderMin, RipleyBorderMax, RipleyBorderMin;
	float MaxHrId;

private:

	void GetRdfAndRipVar(const mat& inLoc1, const mat& inLoc2);
public:
	void RDF_Analyze(const mat& inLoc1, const mat& inLoc2);
	void RipleyH_Analyze(const mat& inLoc1, const mat& inLoc2);

};

