#include "stdafx.h"
#include"DBSCAN.h"

int MinPts = 10;
float Eps = 20, EpsAutoCalc = 0;
int DBSCAN_ImageW, DBSCAN_ImageH, ClusterNum;
int* DBSCAN_ImageArray;
mat DBSCAN_Res;

void AutoCalcEpsDBSCAN(const mat& inLoc) {

	mat XYZ = inLoc.cols(1, 3);
	if (XYZ.col(2).is_zero())
	{
		XYZ.shed_col(2);
	}

	mat XYZ_Raw = XYZ;
	int m = size(XYZ, 0);
	float n = size(XYZ, 1);

	EpsAutoCalc = pow(((prod(max(XYZ) - min(XYZ)) * MinPts * tgamma(0.5 * n + 1)) / (m * sqrt(pow(PI, n)))), 1 / n);

	vec Sort = linspace(0, m - 1, m);
	XYZ = join_rows(Sort, XYZ);
	vec LocType = zeros(m, 1);
	int no = 1;
	vec act = zeros(m, 1);
	vec clusters = zeros(m, 1);
	rowvec CurPoiRow, EucDis; vec one;
	uvec ind, ind_1, ind_1_i;
	//Cluster, label the cluster ID and localization category
	for (int i = 0; i < m; i++)
	{
		if (act(i) == 0) {
			CurPoiRow = XYZ.row(i);
			one = ones(m, 1);
			EucDis = sqrt(sum(trans(pow(XYZ.cols(1, n) - one * CurPoiRow.cols(1, n), 2))));
			ind = find(EucDis <= EpsAutoCalc);

			if (size(ind, 0) > 1 && size(ind, 0) < MinPts + 1) {
				LocType(i) = 0;
				clusters(i) = 0;
			}
			else if (size(ind, 0) == 1) {
				LocType(i) = -1;
				clusters(i) = -1;
				act(i) = 1;
			}
			else if (size(ind, 0) >= MinPts + 1) {
				LocType(i) = 1;
				clusters(ind) = ones(size(ind, 0), 1) * max(no);
				while (!ind.is_empty())
				{
					CurPoiRow = XYZ.row(ind(0));
					act(ind(0)) = 1;
					ind.shed_row(0);
					EucDis = sqrt(sum(trans(pow(XYZ.cols(1, n) - one * CurPoiRow.cols(1, n), 2))));
					ind_1 = find(EucDis <= EpsAutoCalc);
					vec no_vec(ind_1.n_rows, 1);
					no_vec.fill(no);
					if (size(ind_1, 0) > 1) {
						clusters(ind_1) = no_vec;
						if (size(ind_1, 0) >= MinPts + 1)
						{
							LocType(CurPoiRow(0)) = 1;
						}
						else {
							LocType(CurPoiRow(0)) = 0;
						}

						for (int i = 0; i < size(ind_1, 0); i++)
						{
							if (act(ind_1(i)) == 0) {
								act(ind_1(i)) = 1;
								ind_1_i = ind_1(i);
								ind = join_cols(ind, ind_1_i);
								clusters(ind_1(i)) = no;
							}
						}
					}
				}
				no = no + 1;
			}

		}
	}

	uvec ind_2 = find(clusters == 0);
	vec Fu1(ind_2.n_rows, 1, fill::value(-1));
	clusters(ind_2) = Fu1;
	LocType(ind_2) = Fu1;

	uvec NoisePos_ind = find(clusters == -1);
	mat NoisePos = XYZ_Raw.rows(NoisePos_ind);

	if (n == 2) {
		vec zero = zeros(XYZ_Raw.n_rows, 1);
		XYZ_Raw = join_rows(XYZ_Raw, zero);
	}
	DBSCAN_Res.zeros(XYZ_Raw.n_rows, 5);
	DBSCAN_Res.cols(0, 2) = XYZ_Raw;
	DBSCAN_Res.col(3) = clusters;
	DBSCAN_Res.col(4) = LocType;

	/*save DBSCAN Cluster Result
	 1-3 columns: x / y / z coordinates
	 4 columns: localization belongs to cluster ID. cluster ID = -1 is unclustered localization.
	 5 columns: 1 is cluster localization, 0 is edge localization, -1 is unclustered localization*/

	
	//DBSCAN map infomation
	ClusterNum = max(clusters);
	string minpts = to_string(MinPts);
	string Str_Eps = to_string(EpsAutoCalc);
	string NoCluster = "DBSCAN, No identified clusters. MinPts: " + minpts + " Eps: " + Str_Eps + "\n";
	if (max(clusters) == -1) {
		OpenConsole();
		printf(NoCluster.c_str());
	}
	else {
		XYZ_Raw.shed_rows(NoisePos_ind);
		clusters.shed_rows(NoisePos_ind);
		vec ClusterPixel(size(clusters));
		for (int i = 1; i < max(clusters)+1; i++)
		{
			uvec clustersID = find(clusters == i);
			vec Pixel(size(clustersID));
			Pixel.fill(ColorVec(i - 1));
			ClusterPixel(clustersID) = Pixel;
		}
		mat XYZ_RawRound = round(XYZ_Raw / 10);//
		int MaxX = max(XYZ_RawRound.col(0));
		int MaxY = max(XYZ_RawRound.col(1));
		mat DBSCAN_Image(MaxY + 1, MaxX + 1);
		DBSCAN_Image.fill(WhiteValue);
		for (size_t i = 0; i < XYZ_RawRound.n_rows; i++)
		{
			DBSCAN_Image(XYZ_RawRound(i, 1), XYZ_RawRound(i, 0)) = ClusterPixel(i);
		}
		
		int MinX = min(XYZ_RawRound.col(0));
		int MinY = min(XYZ_RawRound.col(1));
		DBSCAN_Image.shed_rows(0, MinY - 1);
		DBSCAN_Image.shed_cols(0, MinX - 1);
		DBSCAN_ImageW = DBSCAN_Image.n_cols;
		DBSCAN_ImageH = DBSCAN_Image.n_rows;

		vec DBSCAN_ImageVec = ArmaMatToArmaVec(DBSCAN_Image);
		DBSCAN_ImageArray = ArmaVecToArrayI(DBSCAN_ImageVec);
	}


}



void DBSCAN(const mat& inLoc) {



	mat XYZ = inLoc.cols(1, 3);
	if (XYZ.col(2).is_zero())
	{
		XYZ.shed_col(2);
	}

	mat XYZ_Raw = XYZ;
	int m = size(XYZ, 0);
	float n = size(XYZ, 1);

	vec Sort = linspace(0, m - 1, m);
	XYZ = join_rows(Sort, XYZ);
	vec LocType = zeros(m, 1);
	int no = 1;
	vec act = zeros(m, 1);
	vec clusters = zeros(m, 1);
	rowvec CurPoiRow, EucDis; vec one;
	uvec ind, ind_1, ind_1_i;
	//Cluster, label the cluster ID and localization category
	for (int i = 0; i < m; i++)
	{
		if (act(i) == 0) {
			CurPoiRow = XYZ.row(i);
			one = ones(m, 1);
			EucDis = sqrt(sum(trans(pow(XYZ.cols(1, n) - one * CurPoiRow.cols(1, n), 2))));
			ind = find(EucDis <= Eps);

			if (size(ind, 0) > 1 && size(ind, 0) < MinPts + 1) {
				LocType(i) = 0;
				clusters(i) = 0;
			}
			else if (size(ind, 0) == 1) {
				LocType(i) = -1;
				clusters(i) = -1;
				act(i) = 1;
			}
			else if (size(ind, 0) >= MinPts + 1) {
				LocType(i) = 1;
				clusters(ind) = ones(size(ind, 0), 1) * max(no);
				while (!ind.is_empty())
				{
					CurPoiRow = XYZ.row(ind(0));
					act(ind(0)) = 1;
					ind.shed_row(0);
					EucDis = sqrt(sum(trans(pow(XYZ.cols(1, n) - one * CurPoiRow.cols(1, n), 2))));
					ind_1 = find(EucDis <= Eps);
					vec no_vec(ind_1.n_rows, 1);
					no_vec.fill(no);
					if (size(ind_1, 0) > 1) {
						clusters(ind_1) = no_vec;
						if (size(ind_1, 0) >= MinPts + 1)
						{
							LocType(CurPoiRow(0)) = 1;
						}
						else {
							LocType(CurPoiRow(0)) = 0;
						}

						for (int i = 0; i < size(ind_1, 0); i++)
						{
							if (act(ind_1(i)) == 0) {
								act(ind_1(i)) = 1;
								ind_1_i = ind_1(i);
								ind = join_cols(ind, ind_1_i);
								clusters(ind_1(i)) = no;
							}
						}
					}
				}
				no = no + 1;
			}

		}
	}


	uvec ind_2 = find(clusters == 0);
	vec Fu1(ind_2.n_rows, 1, fill::value(-1));
	clusters(ind_2) = Fu1;
	LocType(ind_2) = Fu1;

	uvec NoisePos_ind = find(clusters == -1);
	mat NoisePos = XYZ_Raw.rows(NoisePos_ind);

	if (n == 2) {
		vec zero = zeros(XYZ_Raw.n_rows, 1);
		XYZ_Raw = join_rows(XYZ_Raw, zero);
	}
	DBSCAN_Res.zeros(XYZ_Raw.n_rows, 5);
	DBSCAN_Res.cols(0, 2) = XYZ_Raw;
	DBSCAN_Res.col(3) = clusters;
	DBSCAN_Res.col(4) = LocType;

	/*save DBSCAN Cluster Result
	 1-3 columns: x / y / z coordinates
	 4 columns: localization belongs to cluster ID. cluster ID = -1 is unclustered localization.
	 5 columns: 1 is cluster localization, 0 is edge localization, -1 is unclustered localization*/
	
	//DBSCAN map infomation
	ClusterNum = max(clusters);
	string minpts = to_string(MinPts);
	string Str_Eps = to_string(Eps);
	string NoCluster = "DBSCAN, No identified clusters. MinPts: " + minpts + " Eps: " + Str_Eps + "\n";
	if (max(clusters) == -1) {
		OpenConsole();
		printf(NoCluster.c_str());
	}
	else {
		XYZ_Raw.shed_rows(NoisePos_ind);
		clusters.shed_rows(NoisePos_ind);
		vec ClusterPixel(size(clusters));
		for (int i = 1; i < max(clusters) + 1; i++)
		{
			uvec clustersID = find(clusters == i);
			vec Pixel(size(clustersID));
			Pixel.fill(ColorVec(i - 1));
			ClusterPixel(clustersID) = Pixel;
		}
		mat XYZ_RawRound = round(XYZ_Raw / 10);//
		int MaxX = max(XYZ_RawRound.col(0));
		int MaxY = max(XYZ_RawRound.col(1));
		mat DBSCAN_Image(MaxY + 1, MaxX + 1);
		DBSCAN_Image.fill(WhiteValue);

		for (size_t i = 0; i < XYZ_RawRound.n_rows; i++)
		{
			DBSCAN_Image(XYZ_RawRound(i, 1), XYZ_RawRound(i, 0)) = ClusterPixel(i);
		}

		int MinX = min(XYZ_RawRound.col(0));
		int MinY = min(XYZ_RawRound.col(1));
		DBSCAN_Image.shed_rows(0, MinY - 1);
		DBSCAN_Image.shed_cols(0, MinX - 1);
		DBSCAN_ImageW = DBSCAN_Image.n_cols;
		DBSCAN_ImageH = DBSCAN_Image.n_rows;

		vec DBSCAN_ImageVec = ArmaMatToArmaVec(DBSCAN_Image);
		DBSCAN_ImageArray = ArmaVecToArrayI(DBSCAN_ImageVec);

	}


}