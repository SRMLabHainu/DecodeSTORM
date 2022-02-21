 #include "stdafx.h"
#include"DriftCorr.h"



void DriftCorr(mat& inLoc,const string& PrefixName,int isSave)
{
	inLoc.cols(1, 2) = inLoc.cols(1, 2) / RawImPixSize;

	OpenConsole(); // open console window to display printf

	int TotalFluoNum;


	int ImageWidth = 0;
	int ImageHigh = 0;

	int TotalFrame = 0;

	
	SRDriftCorrData_TypeDef::GetMaxImgSize(inLoc, &ImageWidth, &ImageHigh);
	TotalFrame = max(inLoc.col(Pos_Frme));
	string FindImSize = "find " + PrefixName+ " img size : % d % d % d\n";
	printf(FindImSize.c_str(), ImageWidth, ImageHigh, TotalFrame);

	

	if (DriftCorrGroupFrameNum > TotalFrame / 2)
	{
		printf("\n\nDrift corr frame number is too large\n\n");
		IsDriftCorrection = 0;
	}

	
	if (IsDriftCorrection)
	{
		CorrectedResPath = Directory + PrefixName + "_DriftCorrected_g" + to_string(DriftCorrGroupFrameNum) + "f.csv";
	}

	// find image size from loc arry first
	string DriftCorrBegin = PrefixName + " drift correction begin\n";
	string DriftCorrEnd = PrefixName + " drift correction finish\n";
	string CorrTime = PrefixName + " cor time:%d\n";

	if (IsDriftCorrection)
	{
		printf(DriftCorrBegin.c_str());

		SRDriftCorrData_TypeDef h_SRDriftCorrData;
		h_SRDriftCorrData.Init(ImageWidth, ImageHigh, TotalFrame);

		// 500 frame as a rendering group for cross-correlation


		int time1, time2;

		time1 = clock();


		h_SRDriftCorrData.CorrectSampleShift(inLoc, CorrectedResPath, RawImPixSize, DriftCorrGroupFrameNum, isSave);


		time2 = clock();

		printf(CorrTime.c_str(), time2 - time1);

		h_SRDriftCorrData.Deinit();

		printf(DriftCorrEnd.c_str());
	}

}

