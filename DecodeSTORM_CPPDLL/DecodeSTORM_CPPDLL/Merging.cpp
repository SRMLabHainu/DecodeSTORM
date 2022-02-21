#include "stdafx.h"
#include "Merging.h"

float MaxOffFrame;
float MaxDis;
mat  Merging(const mat& inLoc)
{
	

	vec x = inLoc.col(1);
	vec y = inLoc.col(2);
	vec inLoc_one;
	inLoc_one.ones(inLoc.n_rows, 1);

	int Dis2_max = pow(MaxDis, 2);
	if (MaxOffFrame > inLoc.col(11).max()) {
		MaxOffFrame = 0;
	}
   //CorDup is a list of links to two points that need to be merged
	uvec consframe, consframe_id, corDup;
	vec xx, yy, radius;
	double minRad; int minRad_id;
	corDup.zeros(size(inLoc.col(1)));
	for (int i = 1; i <= inLoc.col(11).max() - MaxOffFrame; i++)
	{
		consframe = inLoc_one && inLoc.col(11) >= i && inLoc.col(11) <= (i + MaxOffFrame);
		consframe_id = find(consframe == 1);
		xx = x.rows(consframe_id);
		yy = y.rows(consframe_id);
		for (int j = 0; j + 1 < consframe_id.n_rows; j++)
		{
			radius = pow(xx(j) - xx.rows(j + 1, xx.n_rows - 1), 2) + pow(yy(j) - yy.rows(j + 1, yy.n_rows - 1), 2);
			minRad = radius.min();
			minRad_id = radius.index_min();
			if (minRad < Dis2_max) {
				corDup.row(consframe_id(j)) = consframe_id(j + minRad_id) + 1;//nonono
			}
		}
	}
	// Remove it if it links to itself
	vec length = linspace(0, corDup.n_rows - 1, corDup.n_rows);
	uvec index_consLen = find(corDup == length);
	uvec zero;
	zero.zeros(index_consLen.n_rows, 1);
	corDup(index_consLen) = zero;

	//Get Merge ID
	int frame_cur = 1;
	uvec index_fraIdCur_cons;
	uvec MergeFrameID;
	MergeFrameID.ones(corDup.n_rows, 1);
	uvec MergeID = linspace<uvec>(0, corDup.n_rows - 1, corDup.n_rows);
	uvec index_fraIdCur = find(MergeFrameID == frame_cur);
	while (index_fraIdCur.n_rows > 0) {
		index_fraIdCur_cons = MergeFrameID == frame_cur && corDup > 0;
		MergeFrameID.rows(corDup(find(index_fraIdCur_cons == 1))) = MergeFrameID.rows(corDup(find(index_fraIdCur_cons == 1))) + 1;
		MergeID.rows(corDup(find(index_fraIdCur_cons == 1))) = MergeID.rows(find(index_fraIdCur_cons == 1));
		frame_cur = frame_cur + 1;
		index_fraIdCur = find(MergeFrameID == frame_cur);
	}
	mat matrixMerge = inLoc.rows(unique(MergeID));	
	return matrixMerge;
}