#include "stdafx.h"
#include "GetLocAndRoi.h"

mat inLoc_Ch1, inLoc_Ch2, RoiLoc1, RoiLoc2, RawRoiLoc1, RawRoiLoc2;
double UpLeftX, UpLeftY, LowRightX, LowRightY;
string FilePathCh1, FilePathCh2;
double* inLoc_Ch1X;
double* inLoc_Ch1Y;
double* inLoc_Ch2X;
double* inLoc_Ch2Y;
double* RoiLoc1X;
double* RoiLoc1Y;
double* RoiLoc2X;
double* RoiLoc2Y;
void GetinLoc1() {	
	inLoc_Ch1.load(csv_name(FilePathCh1, csv_opts::no_header));
	inLoc_Ch1.shed_row(0);
	inLoc_Ch1X = ArmaVecToArrayD(inLoc_Ch1.col(1));
	inLoc_Ch1Y = ArmaVecToArrayD(inLoc_Ch1.col(2));
}

void GetinLoc2() {
	inLoc_Ch2.load(csv_name(FilePathCh2, csv_opts::no_header));
	inLoc_Ch2.shed_row(0);
	inLoc_Ch2X = ArmaVecToArrayD(inLoc_Ch2.col(1));
	inLoc_Ch2Y = ArmaVecToArrayD(inLoc_Ch2.col(2));
}

void ExtractRoiLoc1() {
	uvec ind = inLoc_Ch1.col(1) > UpLeftX && inLoc_Ch1.col(1) < LowRightX &&
		inLoc_Ch1.col(2) > UpLeftY && inLoc_Ch1.col(2) < LowRightY;
	RoiLoc1 = inLoc_Ch1.rows(find(ind == 1));
	RoiLoc1X = ArmaVecToArrayD(RoiLoc1.col(1));
	RoiLoc1Y = ArmaVecToArrayD(RoiLoc1.col(2));
	RawRoiLoc1 = RoiLoc1;
}

void ExtractRoiLoc2() {
	uvec ind = inLoc_Ch2.col(1) > UpLeftX && inLoc_Ch2.col(1) < LowRightX &&
		inLoc_Ch2.col(2) > UpLeftY && inLoc_Ch2.col(2) < LowRightY;
	RoiLoc2 = inLoc_Ch2.rows(find(ind == 1));
	RoiLoc2X = ArmaVecToArrayD(RoiLoc2.col(1));
	RoiLoc2Y = ArmaVecToArrayD(RoiLoc2.col(2));
	RawRoiLoc2 = RoiLoc2;
}
