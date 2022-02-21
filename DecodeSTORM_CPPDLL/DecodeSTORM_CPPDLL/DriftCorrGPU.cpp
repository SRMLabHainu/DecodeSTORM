#include "stdafx.h"
#include"DriftCorrGPU.h"


int GPUID_1Best = 0;


void DriftCorrGPU(const string& DataBinaryTxtPath,const string& PrefixName)
{
	cudaSetDevice(GPUID_1Best);

	//IsLocRunning = true;

	OpenConsole(); // open console window to display printf

	int TotalFluoNum;


	int ImageWidth = 0;
	int ImageHigh = 0;

	int TotalFrame = 0;

	
	SRDriftCorrData_TypeDef_GPU::GetMaxImgSize(DataBinaryTxtPath, &ImageWidth, &ImageHigh);
	TotalFrame = SRDriftCorrData_TypeDef_GPU::GetTotalFrame(DataBinaryTxtPath);
	string FindImSize = "find " + PrefixName+ " img size : % d % d % d\n";
	printf(FindImSize.c_str(), ImageWidth, ImageHigh, TotalFrame);

	

	if (DriftCorrGroupFrameNum > TotalFrame / 2)
	{
		printf("\n\nDrift corr frame number is too large\n\n");
		IsDriftCorrection = 0;
	}

	
	if (IsDriftCorrection)
	{
		CorrectedResPath = Directory + PrefixName + "_DriftCorrected_g" + to_string(DriftCorrGroupFrameNum) + "f.txt";
	}

	// find image size from loc arry first
	string DriftCorrBegin = PrefixName + " drift correction begin\n";
	string DriftCorrEnd = PrefixName + " drift correction finish\n";
	string CorrTime = PrefixName + " cor time:%d\n";

	if (IsDriftCorrection)
	{
		printf(DriftCorrBegin.c_str());

		cudaStream_t CurStream1;
		cudaStreamCreate(&CurStream1);

		SRDriftCorrData_TypeDef_GPU h_SRDriftCorrData;
		h_SRDriftCorrData.Init(ImageWidth, ImageHigh, TotalFrame);

		// 500 frame as a rendering group for cross-correlation


		int time1, time2;

		time1 = clock();


		h_SRDriftCorrData.CorrectSampleShift(DataBinaryTxtPath, CorrectedResPath, RawImPixSize, DriftCorrGroupFrameNum, CurStream1);


		time2 = clock();

		printf(CorrTime.c_str(), time2 - time1);

		h_SRDriftCorrData.Deinit();
		cudaStreamDestroy(CurStream1);

		printf(DriftCorrEnd.c_str());
	}

}

