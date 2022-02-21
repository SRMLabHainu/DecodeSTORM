#include "stdafx.h"
#include "DuplicateFilter.h"

float RadThre;
mat DuplicateFilter(const mat& inLoc) {

	vec Frame = inLoc.col(11);
	vec X = inLoc.col(1);
	vec Y = inLoc.col(2);
	vec LocPrecision = inLoc.col(9);
	std::vector<double> FilterPos;
	vec Hist = HistRate(Frame);//Count the number of elements

	int count = 0;
	int t;
	//get the position of duplicate localization 
	for (int i = 0; i < Hist.n_rows; i++) {
		t = Hist(i);
		for (int j = count; j < (t + count); j++)
		{
			for (int k = count; k < (t + count); k++)
			{

				if (j < k) {
					if (sqrt(pow((X(j) - X(k)), 2) + pow((Y(j) - Y(k)), 2)) < RadThre) {
						if (LocPrecision(j) > LocPrecision(k)) //Filter points of poor localization precision
							FilterPos.push_back(j);
						else
							FilterPos.push_back(k);
					}
				}
			}
		}
		count = count + t;
	}
	uvec inLoc_ID = linspace<uvec>(0, inLoc.n_rows - 1, inLoc.n_rows);
	uvec FilterPos_ID = conv_to< uvec >::from(FilterPos);
	inLoc_ID.shed_rows(unique(FilterPos_ID));//remove duplicate localization
	mat outLoc = inLoc.rows(inLoc_ID);
	return outLoc;
}

