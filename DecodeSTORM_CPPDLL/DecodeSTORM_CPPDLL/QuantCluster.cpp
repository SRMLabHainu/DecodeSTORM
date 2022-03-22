#include "stdafx.h"
#include "QuantCluster.h"

float MaxAnaDis = 500;
float RingWidth = 1;
mat RipleyH_Result;

void QuantCluster::GetRdfAndRipVar(const mat& inLoc1, const mat& inLoc2) {

	OpenConsole();
	//Get the variables to calculate RDF and Ripley'H 
	mat XYZ_Ch1 = inLoc1.cols(1, 3);
	mat XYZ_Ch2 = inLoc2.cols(1, 3);

	double EdgePos1 = min(min(XYZ_Ch1.col(0)), min(XYZ_Ch2.col(0)));
	double EdgePos2 = max(max(XYZ_Ch1.col(0)), max(XYZ_Ch2.col(0)));
	double EdgePos3 = min(min(XYZ_Ch1.col(1)), min(XYZ_Ch2.col(1)));
	double EdgePos4 = max(min(XYZ_Ch1.col(1)), max(XYZ_Ch2.col(1)));

	double ImMinLen = min(EdgePos2 - EdgePos1, EdgePos4 - EdgePos3);

	if ((MaxAnaDis / ImMinLen) > 0.1) {
		printf("Warning: The 'MaxAnaDis' parameter is larger than 1 / 10 of the image, lead to abnormal results, please modify it!\n");
	}
	N = XYZ_Ch1.n_rows;
	double area = (EdgePos2 - EdgePos1) * (EdgePos4 - EdgePos3);
	density = N / area;
	//localizations within MaxAnaDis 
	uvec index1 = XYZ_Ch1.col(0) > (EdgePos1 + MaxAnaDis) && XYZ_Ch1.col(0) < (EdgePos2 - MaxAnaDis) && XYZ_Ch1.col(1) > (EdgePos3 + MaxAnaDis) && XYZ_Ch1.col(1) < (EdgePos4 - MaxAnaDis);
	mat xyz1InUnit = XYZ_Ch1.rows(find(index1 == 1));

	if (RingWidth == 0) {
		printf("Warning: The 'MaxAnaDis' parameter is too small to get the result, please modify it!\n");
		exit(0);
	}
	Xr = regspace(0, RingWidth, MaxAnaDis + RingWidth);//For Calculate DisMatByXr
	DisMatByXr = zeros(Xr.n_rows);
	//Calculate Distance Tables
	mat CurXYZ, xyz2InXyz1InUnit;
	vec  CurXYZ_2; uvec index2; rowvec xyz1InUnit_2, CX_Xyz2, DisTab; umat DisHistByXr;
	for (int i = 0; i < xyz1InUnit.n_rows; i++)
	{
		CurXYZ = trans(xyz1InUnit.row(i));
		index2 = XYZ_Ch2.col(0) >= CurXYZ(0) - MaxAnaDis && XYZ_Ch2.col(0) <= CurXYZ(0) + MaxAnaDis && XYZ_Ch2.col(1) >= CurXYZ(1) - MaxAnaDis && XYZ_Ch2.col(1) <= CurXYZ(1) + MaxAnaDis;
		xyz2InXyz1InUnit = trans(XYZ_Ch2.rows(find(index2 == 1)));
		if (~xyz2InXyz1InUnit.is_empty())
		{
			CurXYZ_2 = sum(pow(CurXYZ, 2));
			xyz1InUnit_2 = sum(pow(xyz2InXyz1InUnit, 2));
			CX_Xyz2 = sum(CurXYZ.t() * xyz2InXyz1InUnit);
			DisTab = sqrt(abs(repmat(CurXYZ_2.t(), 1, xyz1InUnit_2.n_cols) + repmat(xyz1InUnit_2, CurXYZ_2.n_cols, 1) - 2 * CX_Xyz2));
			DisHistByXr = hist(DisTab.t(), Xr);
			DisMatByXr = DisMatByXr + DisHistByXr;

		}

	}
	DisMatByXr.shed_row(DisMatByXr.n_rows - 1);
	Xr_Size = Xr.size()-1;
 }


void QuantCluster::RDF_Analyze(const mat& inLoc1, const mat& inLoc2) {

	//string RdfPlot_path = Directory + "\\" + PrefixName + "_RDF.png";

	GetRdfAndRipVar(inLoc1, inLoc2);

	//	Calculate G(r) (RDF)  
	vec Gr = zeros(DisMatByXr.n_rows);
	for (int i = 0; i < DisMatByXr.n_rows; i++)
	{
		Gr(i) = DisMatByXr(i) / (2 * PI * density * RingWidth * (Xr(i + 1)));
	}
	Gr(0) = 0;
	Xr.shed_row(Xr.n_rows - 1);
	RdfBorderMax = ceil(max(Gr.col(0) * 1.1));
	RdfBorderMin = floor(min(Gr.col(0) * 1.1));
	Gr_Arr = ArmaVecToArrayD(Gr);

	Xr_Arr = ArmaVecToArrayD(Xr);

}



void QuantCluster::RipleyH_Analyze(const mat& inLoc1, const mat& inLoc2) {


	GetRdfAndRipVar(inLoc1, inLoc2);

	// Calculate Ripley's H 
	vec Kr = zeros(DisMatByXr.n_rows);
	for (int i = 0; i < DisMatByXr.n_rows; i++)
	{
		Kr(i) = accu(DisMatByXr.rows(0, i)) / N;
	}
	Kr = Kr / density;
	mat Lr = sqrt(Kr / PI);
	mat Hr = Lr.col(0) - Xr.rows(0, Xr.n_rows - 2);


	Xr.shed_row(Xr.n_rows - 1);
	RipleyH_Result = join_rows(Hr, Xr);

	RipleyBorderMax = ceil(max(Hr.col(0) * 1.1));
	RipleyBorderMin = floor(min(Hr.col(0) * 1.1));
	MaxHrId = Xr(index_max(Hr.col(0)));
	Hr_Arr = ArmaVecToArrayD(Hr);
	Xr_Arr = ArmaVecToArrayD(Xr);

}
