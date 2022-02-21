#include "stdafx.h"
#include "ImprecisionFilter.h"

float ImpThre;
mat ImprecisionFilter(const mat& inLoc)
{
	//Remove the localization with poor localization precision
	uvec indicate = find(inLoc.col(9) <= ImpThre);
	mat outLoc = inLoc.rows(indicate);
	return outLoc;
}