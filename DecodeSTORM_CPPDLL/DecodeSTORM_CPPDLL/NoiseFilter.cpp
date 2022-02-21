#include "stdafx.h"
#include"NoiseFilter.h"

float StDev;  
float distThre;
mat NoiseFilter(const mat& inLoc) //distFilt is radius£¨calculate regional density in an area of radius£©,StDev is number of std
{
	mat tempLoc;
	vec distance;
	fvec nPosInDis;
	tempLoc.zeros(inLoc.n_rows, 3);
	distance.zeros(inLoc.n_rows - 1, 1);
	nPosInDis.zeros(inLoc.n_rows, 1);
	//2D or 3D? 
	if (inLoc.col(3).is_zero()) {

		for (int i = 0; i < inLoc.n_rows; i++)
		{
			tempLoc = inLoc.cols(1, 3);
			tempLoc.shed_row(i); //remove self-loclization
			distance = sqrt(pow(tempLoc.col(0) - inLoc(i, 1), 2) + pow(tempLoc.col(1) - inLoc(i, 2), 2));// The distance between the current localization and other localizations
			nPosInDis(i) = sum(distance <= distThre);//Statistics the number of localizations within the threshold
		}
	}
	else {

		for (int i = 0; i < inLoc.n_rows; i++)
		{
			tempLoc = inLoc.cols(1, 3);
			tempLoc.shed_row(i);
			distance = sqrt(pow(tempLoc.col(0) - inLoc(i, 1), 2) + pow(tempLoc.col(1) - inLoc(i, 2), 2) + pow(tempLoc.col(2) - inLoc(i, 3), 2));
			nPosInDis(i) = sum(distance <= distThre);
		}
	}
	//Filter noise loclization 
	float cutOff = median(nPosInDis) + (stddev(nPosInDis) * StDev);
	mat out_inLoc = inLoc.rows(find(nPosInDis >= cutOff));
	return out_inLoc;
}


