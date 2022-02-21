#include"stdafx.h"
#include "DriftCorrection_SRImageXCorr.h"


void SRDriftCorrData_TypeDef::Init(int RawImgWidth, int RawImgHigh, int MaxFrameNum)
{

	cudaError_t err;

	// image information
	ImageWidth = RawImgWidth;
	ImageHigh = RawImgHigh;

	SRImageWidth = RawImgWidth * SRShiftCorr_PixelZoom;
	SRImageHigh = RawImgHigh * SRShiftCorr_PixelZoom;


	// create inf array for each group
	GroupStartFrame = new int[MaxCorrGroupNum]; // start frame of each group
	GroupEndFrame = new int[MaxCorrGroupNum]; // start frame of each group

	GroupFrameStartPos = new int[MaxCorrGroupNum]; // start fluo position of each group
	GroupFrameEndPos = new int[MaxCorrGroupNum]; // start fluo position of each group



	XSliceShift = new float[MaxCorrGroupNum];
	YSliceShift = new float[MaxCorrGroupNum];

	XFrameShift = (float*)malloc(MaxFrameNum * sizeof(float));
	YFrameShift = (float*)malloc(MaxFrameNum * sizeof(float));

	LocArry = (float*)malloc(PointNumTh * 2 * OutParaNumGS2D * sizeof(float));


	FillImg1 = (float*)malloc(SRImageWidth * SRImageHigh * sizeof(float));
	FillImg2 = (float*)malloc(SRImageWidth * SRImageHigh * sizeof(float));

	MulImg = (float*)malloc(SRImageWidth * SRImageHigh * sizeof(float));
	SumLine = (float*)malloc(SRImageWidth * sizeof(float));



}

void SRDriftCorrData_TypeDef::Deinit()
{

	delete[] GroupStartFrame;
	delete[] GroupEndFrame;

	delete[] GroupFrameStartPos;
	delete[] GroupFrameEndPos;


	delete[] XSliceShift;
	delete[] YSliceShift;

	free(XFrameShift);
	free(YFrameShift);

	free(LocArry);


	free(FillImg1);
	free(FillImg2);
	free(MulImg);
	free(SumLine);

}

void SRDriftCorrData_TypeDef::CorrectSampleShift(mat& inLoc, string oFileName, float PixelSize, int CorrFrameNum, int isSave)
{
	// initial information array

	TotalFrame = max(inLoc.col(Pos_Frme));


	int cnt = 0;

	CalcGroupNum(TotalFrame, CorrFrameNum);//CorrGroupNum,GroupStartFrame,GroupEndFrame


	GetCorrGroupFluoPos(inLoc);//GroupFrameStartPos,GroupFrameEndPos

//	printf("group num:%d %d\n", TotalFrame, CorrGroupNum);

	/*
	for (cnt; cnt < CorrGroupNum; cnt++)
	{
	printf("frame pos: %d-%d, %d-%d\n", GroupStartFrame[cnt], GroupEndFrame[cnt], GroupFrameStartPos[cnt], GroupFrameEndPos[cnt]);
	}
	*/

	float ShiftX = 0;
	float ShiftY = 0;
	int CorrShiftBiasX = 0;
	int CorrShiftBiasY = 0;

	// rend  super-resolution image
	// 0th is the reference image and the others are drifted image


	for (cnt = 0; cnt < CorrGroupNum; cnt += 1)
	{
		RenderSlice(inLoc, cnt, PixelSize);

		if (cnt > 0)
		{
			GetSliceShift(&ShiftX, &ShiftY, CorrShiftBiasX, CorrShiftBiasY);
			CorrShiftBiasX = (int)ShiftX;
			CorrShiftBiasY = (int)ShiftY;
		}

		XSliceShift[cnt] = ShiftX / SRShiftCorr_PixelZoom;
		YSliceShift[cnt] = ShiftY / SRShiftCorr_PixelZoom;

		printf("x y shift(pixel):%f %f\n", XSliceShift[cnt], YSliceShift[cnt]);

	}

	ShiftInterpolation();


	ApplyShiftTop(inLoc, oFileName, BIT0 | BIT1, isSave);


	//cudaStreamSynchronize(cstream);

}


void SRDriftCorrData_TypeDef::GetMaxImgSize(mat& inLoc, int* ImageWidth, int* ImageHigh)
{

	int MaxImgWidth = 0;
	int MaxImgHeight = 0;

	int TotalFluoNum = size(inLoc, 0);


	// find image size first


	int findImgWidth = 0, findImgHeight = 0;

	for (int i = 0; i < TotalFluoNum; i++) {
		int xPos = (int)(inLoc(i, Pos_XPos) + 16);
		int yPos = (int)(inLoc(i, Pos_YPos) + 16);

		findImgWidth = max(findImgWidth, xPos);
		findImgHeight = max(findImgHeight, yPos);
	}


	findImgWidth = findImgWidth / 4 * 4;
	findImgHeight = findImgHeight / 4 * 4;

	MaxImgWidth = max(MaxImgWidth, findImgWidth);
	MaxImgHeight = max(MaxImgHeight, findImgHeight);



	*ImageWidth = (MaxImgWidth + 8 - 1) / 8 * 8;
	*ImageHigh = (MaxImgHeight + 8 - 1) / 8 * 8;

}

void SRDriftCorrData_TypeDef::CalcGroupNum(int TotalFrame, int CorrFrameNum)
{
	// note frame is 1 to total frame
	int HalfCorrFrameNum = CorrFrameNum / 2;

	int GroupNum = TotalFrame / CorrFrameNum;

	GroupNum = GroupNum * 2 - 1; // shift with 1/2 overlaping group frames

	int ResidualFrameNum = TotalFrame % CorrFrameNum;

	int cnt;


	for (cnt = 0; cnt < GroupNum; cnt++)
	{
		// frame assign for each group
		GroupStartFrame[cnt] = cnt * HalfCorrFrameNum + 1; // start frame of each group
		GroupEndFrame[cnt] = cnt * HalfCorrFrameNum + CorrFrameNum; // start frame of each group
	}

	// if there are some residual frames
	if (ResidualFrameNum < HalfCorrFrameNum)
	{
		// residual grouped into last group
		GroupEndFrame[GroupNum - 1] = TotalFrame; // start frame of each group

	}
	else
	{
		// residual is a new group
		GroupNum++;
		GroupStartFrame[GroupNum - 1] = TotalFrame - CorrFrameNum + 1; // start frame of each group
		GroupEndFrame[GroupNum - 1] = TotalFrame; // start frame of each group
	}

	CorrGroupNum = GroupNum;
}

void SRDriftCorrData_TypeDef::GetCorrGroupFluoPos(mat& inLoc)
{
	int cnt = 0;

	int BeginPos = 0;
	int EndPos = 0;
	GroupFrameStartPos[0] = 0;

	for (cnt = 0; cnt < CorrGroupNum; cnt++)
	{


		// end fluo position of each group 
		uvec EndPosIndex = inLoc.col(11) <= GroupEndFrame[cnt];
		EndPos = max(find(EndPosIndex == 1));
		GroupFrameEndPos[cnt] = EndPos;

		if (cnt >= 1)
		{

			//start fluo position of each group
			uvec BeginPosIndex = inLoc.col(11) <= GroupStartFrame[cnt] - 1;
			BeginPos = max(find(BeginPosIndex == 1));
			GroupFrameStartPos[cnt] = BeginPos + 1;
		}
	}
}


void SRDriftCorrData_TypeDef::CreateRenderPSF(float oPSFArry[RendROIWidth * RendROIWidth], float RendXPos, float RendYPos, float RendSigma)
{
	float(*RenderPSF)[RendROIWidth] = (float(*)[RendROIWidth])oPSFArry;
	int xcnt, ycnt;

	RendSigma = -0.5f / (RendSigma * RendSigma);

	RendXPos = RendXPos - (int)RendXPos; // shift to 0-0.99
	RendYPos = RendYPos - (int)RendYPos; // shift to 0-0.99
	RendXPos = RendXPos + RendROIWidth_half; // 0 is center
	RendYPos = RendYPos + RendROIWidth_half; // 0 is center

//#pragma unroll
	for (ycnt = 0; ycnt < RendROIWidth; ycnt++)
	{
//#pragma unroll
		for (xcnt = 0; xcnt < RendROIWidth; xcnt++)
		{
			RenderPSF[ycnt][xcnt] = exp(((xcnt - RendXPos) * (xcnt - RendXPos) + (ycnt - RendYPos) * (ycnt - RendYPos)) * RendSigma);

		}
	}
}

void SRDriftCorrData_TypeDef::ShiftInterpolation()
{
	// set shift to 0
	memset(XFrameShift, 0, TotalFrame * sizeof(float));
	memset(YFrameShift, 0, TotalFrame * sizeof(float));

	//int TotalFrame = TotalFrame;


	int CurPos = 0;

	// from 0 frame to the first center
	float LastCenterFrame = 0;
	float LastShiftX = 0;
	float LastShiftY = 0;

	float CurCenterFrame = 0;
	float CurShiftX = 0;
	float CurShiftY = 0;


	int cnt = 0;
	int fcnt = 0;
	float KX = 0, KY = 0;

	for (cnt = 0; cnt <= CorrGroupNum; cnt++)
	{
		if (cnt == CorrGroupNum)
		{
			// end group
			CurCenterFrame = GroupEndFrame[CorrGroupNum - 1]; // the end frame
			// KX KY keep the same with the last group
		}
		else
		{
			// normal group include 0 group
			CurCenterFrame = (GroupStartFrame[cnt] + GroupEndFrame[cnt]) / 2;
			CurShiftX = XSliceShift[cnt];
			CurShiftY = YSliceShift[cnt];

			KX = (CurShiftX - LastShiftX) / (CurCenterFrame - LastCenterFrame);
			KY = (CurShiftY - LastShiftY) / (CurCenterFrame - LastCenterFrame);
		}

		for (fcnt = LastCenterFrame; fcnt < CurCenterFrame; fcnt++)
		{
			XFrameShift[fcnt] = LastShiftX + (fcnt - LastCenterFrame) * KX;
			YFrameShift[fcnt] = LastShiftY + (fcnt - LastCenterFrame) * KY;
		}

		LastCenterFrame = CurCenterFrame;
		LastShiftX = CurShiftX;
		LastShiftY = CurShiftY;

	}
}

void SRDriftCorrData_TypeDef::ApplyShiftTop(mat& inLoc, string oFileName, int ShiftCorrEnable, int isSave)
{

	int TotalFluoNum = size(inLoc, 0);

	/*int ProcNum = TotalFluoNum / PointNumTh + 1;
	int CurFluoNum = PointNumTh;*/

	int cnt;
	for (int i = 0; i < TotalFluoNum; i++)
	{
		int CurFrame = 0;

		if (i < TotalFluoNum)
		{
			CurFrame = inLoc(i, Pos_Frme);

			if (ShiftCorrEnable & 0x01)
			{
				// x,y is pixel
				inLoc(i, Pos_XPos) = inLoc(i, Pos_XPos) - XFrameShift[CurFrame - 1];
			}
			if (ShiftCorrEnable & 0x02)
			{
				// x,y is pixel
				inLoc(i, Pos_YPos) = inLoc(i, Pos_YPos) - YFrameShift[CurFrame - 1];
			}

			if (ShiftCorrEnable & 0x04)
			{
				// z is nm
	//			pLocArry[gid][Pos_ZPos] = pLocArry[gid][Pos_ZPos] - d_ZFrameShift[CurFrame];
			}
		}
	}
	inLoc.cols(1, 2) = inLoc.cols(1, 2) * RawImPixSize;
	if (isSave == 1)
	{
		field<std::string> header(12);
		header = { "peak intensity (photon)","x(nm)","y(nm)","z(nm)","PSFSigmaX (nm)","PSFSigmaY (nm)","Total intensity(photon)",
			"background (photon)","SNR","Localization precision X(nm)", "Localization precision Y(nm)","frame" };
		inLoc.save(csv_name(oFileName, header));
	}

	
}

void SRDriftCorrData_TypeDef::RenderSlice(mat& inLoc, int RendGroup, float PixelSize)
{
	int FirstFluoPos, EndFluoPos;
	float* d_FillImage = NULL;

	FirstFluoPos = GroupFrameStartPos[RendGroup];
	EndFluoPos = GroupFrameEndPos[RendGroup];

	// Render gap
	int TotalFluoNum = EndFluoPos - FirstFluoPos + 1;

	//	printf("cur rend:%d-%d\n", FirstFluoPos, EndFluoPos);

	if (RendGroup == 0)
	{
		d_FillImage = FillImg1;
	}
	else
	{
		d_FillImage = FillImg2;
	}

	// reset this image before rendering
	//ResetFillImage(d_FillImage, SRImageWidth, SRImageHigh, cstream);
	memset(d_FillImage, 0, SRImageWidth * SRImageHigh * sizeof(float));

	//CppBinaryReadFile iLocFile(FileName);

	mat inLocTemp = inLoc.rows(FirstFluoPos, EndFluoPos);

	//iLocFile.Seek(FirstFluoPos * OutParaNumGS2D * sizeof(float), ios::beg);

	//int ProcNum = (TotalFluoNum + PointNumTh - 1) / PointNumTh;
	//int CurFluoNum = PointNumTh;

	int cnt;

	// find image size first
	for (int i = 0; i < inLocTemp.n_rows; i++)
	{
		int gid = i;

		// only for 2d
		float PSFArry[RendROIWidth * RendROIWidth];
		float(*RenderPSF)[RendROIWidth] = (float(*)[RendROIWidth])PSFArry;

		//float(*pLocArry)[OutParaNumGS2D]; // for parameter array

		float RendXPos;
		float RendYPos;
		float PeakPhoton;
		float Sigma;
		float LocPrec;
		float RendSigma;

		int rcnt = 0;
		int Offset;

		// parameters for 2d and 3d
		//pLocArry = (float(*)[OutParaNumGS2D])h_LocArry;

		//PeakPhoton = 1.0f; // same weight 

		if (gid < inLocTemp.n_rows)
		{
			PeakPhoton = inLocTemp(i, Pos_PPho); // peak photon intensity as weight 
			RendXPos = inLocTemp(i, Pos_XPos) * SRShiftCorr_PixelZoom;
			RendYPos = inLocTemp(i, Pos_YPos) * SRShiftCorr_PixelZoom;
			Sigma = inLocTemp(i, Pos_SigX);
			LocPrec = Sigma / sqrt(PeakPhoton);
	
		
			

			// valid fluos
			if ( (PeakPhoton > 1.0f) && (RendXPos > RendROIWidth) && (RendXPos < SRImageWidth - 1 - RendROIWidth) && (RendYPos > RendROIWidth) && (RendYPos < SRImageHigh - 1 - RendROIWidth))
			{

				// localization precision is localization error
				if ((LocPrec > 0.01f) && (LocPrec < 35.0f))
				{
					// avoid NAN precision
				}
				else
				{
					LocPrec = 35.0f; // set maximum 30nm resolution
					PeakPhoton = 0.0f; // do not Render it
				}


				RendSigma = LocPrec / (PixelSize / SRShiftCorr_PixelZoom);

				// calculate rendering PSF
				CreateRenderPSF(PSFArry, RendXPos, RendYPos, RendSigma);

				// use the same weight but not the peak photon
				// seems 1 is better than peak photon
				PeakPhoton = 1.0f;


				for (rcnt = 0; rcnt < RendROIWidth; rcnt++)
				{
					Offset = SRImageWidth * ((int)RendYPos - RendROIWidth_half + rcnt) + (int)RendXPos - RendROIWidth_half;
					for (int ccnt = 0; ccnt < RendROIWidth; ccnt++)
					{

						d_FillImage[Offset + ccnt] += PeakPhoton * RenderPSF[rcnt][ccnt];

					}
				}
			}
		}
	}


		//ImageRender(LocArry, d_LocArry, d_FillImage, QE, SNR_th, PixelSize, SRShiftCorr_PixelZoom, SRImageWidth, SRImageHigh, CurFluoNum, cstream);

	//}

	//iLocFile.Close();
}

void SRDriftCorrData_TypeDef::GetSliceShift(float* ShiftX, float* ShiftY, int CorrShiftBiasX, int CorrShiftBiasY)
{
	// calculate cross correletion
	// don't calculate all, only calculate a useful region

	int CorrShiftX = 0;
	int CorrShiftY = 0;


	//	printf("cor size:%d %d\n", SRImageWidth, SRImageHigh);

	int cnt = 0;
	double CorrResult[CorrSize][CorrSize];

	int xcnt, ycnt;

	double MaxSumDat = 0;
	int MaxPosX = 0, MaxPosY = 0;


	float SumCorrX[2 * FittingRadius + 1]; // sum along x,y direction for fitting region
	float SumCorrY[2 * FittingRadius + 1]; // sum along x,y direction for fitting region

	int FitXS, FitXE, FitYS, FitYE;// X,Y start,end pos


	// cross-correlation calculation for a 51x51 region
	for (ycnt = 0; ycnt < CorrSize; ycnt++)
	{
		for (xcnt = 0; xcnt < CorrSize; xcnt++)
		{
			CorrShiftX = xcnt - HalfCorrSize;
			CorrShiftY = ycnt - HalfCorrSize;

			double TotalSum = 0;
			int TotalPixel = SRImageWidth * SRImageHigh;

			memset(MulImg, 0, SRImageWidth * SRImageHigh * sizeof(float));
			for (int i = 0; i < TotalPixel; i++)
			{
				int gid = i;
				int curx2 = gid % SRImageWidth;
				int cury2 = gid / SRImageWidth;

				int curx1 = curx2 + CorrShiftBiasX + CorrShiftX;
				int cury1 = cury2 + CorrShiftBiasY + CorrShiftY;

				if (gid < SRImageWidth * SRImageHigh)
				{
					if ((curx1 >= 0) && (curx1 < SRImageWidth) && (cury1 >= 0) && (cury1 < SRImageHigh))
					{
						MulImg[gid] = FillImg1[gid] * FillImg2[cury1 * SRImageWidth + curx1];
					}
				}
			}
			TotalPixel = SRImageWidth;
			for (int i = 0; i < TotalPixel; i++)
			{
				int gid = i;

				int ProcNum = SRImageHigh / ThreadsPerBlock;
				int ResidualNum = SRImageHigh % ThreadsPerBlock;

				int pcnt = 0;
				int ycnt = 0;
				int cury;
				int curx = gid;
				float SumDat = 0;


				if (gid < SRImageWidth)
				{
					for (pcnt = 0; pcnt < ProcNum; pcnt++)
					{
						for (ycnt = 0; ycnt < ThreadsPerBlock; ycnt++)
						{
							cury = pcnt * ThreadsPerBlock + ycnt;
							SumDat += MulImg[cury * SRImageWidth + curx];
						}
					}
					for (ycnt = 0; ycnt < ResidualNum; ycnt++)
					{
						cury = pcnt * ThreadsPerBlock + ycnt;
						SumDat += MulImg[cury * SRImageWidth + curx];
					}
					SumLine[curx] = SumDat;
				}
			}
			int cnt = 0;
			TotalSum = 0;
			for (cnt = 0; cnt < SRImageWidth; cnt++)
			{
				TotalSum += SumLine[cnt];
			}
			CorrResult[ycnt][xcnt] = TotalSum;
			//CorrResult[ycnt][xcnt] = CrossCorrelation(CorrShiftX, CorrShiftY, CorrShiftBiasX, CorrShiftBiasY);
			if (MaxSumDat < CorrResult[ycnt][xcnt])
			{
				MaxSumDat = CorrResult[ycnt][xcnt];
				MaxPosX = xcnt;
				MaxPosY = ycnt;
			}
			//			printf("%.1f ", CorrResult[ycnt][xcnt]);
		}
	}
	// normalize the correlation result
	for (ycnt = 0; ycnt < CorrSize; ycnt++)
	{
		for (xcnt = 0; xcnt < CorrSize; xcnt++)
		{

			CorrResult[ycnt][xcnt] = CorrResult[ycnt][xcnt] / MaxSumDat * 1000;
			//			printf("%.1f ", CorrResult[ycnt][xcnt]);
		}
	}


	//	printf("\nMaxPosX:%d %d\n", MaxPosX, MaxPosY);

	// find the fitting region centered with the max pos
	FitXS = max(MaxPosX - FittingRadius, 0);
	FitYS = max(MaxPosY - FittingRadius, 0);
	FitXE = min(MaxPosX + FittingRadius, CorrSize - 1);
	FitYE = min(MaxPosY + FittingRadius, CorrSize - 1);

	int xhlen = (FitXE - FitXS + 1) / 2;
	int yhlen = (FitYE - FitYS + 1) / 2;

	for (cnt = 0; cnt < 2 * FittingRadius + 1; cnt++)
	{
		SumCorrX[cnt] = 0;
		SumCorrY[cnt] = 0;
	}
	// center of mass fitting of gaussian shape cross-correlation
	for (ycnt = FitYS; ycnt <= FitYE; ycnt++)
	{
		for (xcnt = FitXS; xcnt <= FitXE; xcnt++)
		{
			SumCorrX[FittingRadius - xhlen + (xcnt - FitXS)] += CorrResult[ycnt][xcnt];
			SumCorrY[FittingRadius - yhlen + (ycnt - FitYS)] += CorrResult[ycnt][xcnt];
		}
	}

	float wSum = 0, cSum = 0; // weighted sum for center of mass
	float CenterX, CenterY;

	for (cnt = 0; cnt < 2 * FittingRadius + 1; cnt++)
	{
		if (SumCorrX[cnt] == 0)continue;

		wSum += SumCorrX[cnt] * (cnt + 1);
		cSum += SumCorrX[cnt];

	}
	CenterX = wSum / cSum - (FittingRadius + 1);

	wSum = 0;
	cSum = 0;

	for (cnt = 0; cnt < 2 * FittingRadius + 1; cnt++)
	{
		if (SumCorrY[cnt] == 0)continue;

		wSum += SumCorrY[cnt] * (cnt + 1);
		cSum += SumCorrY[cnt];

	}
	CenterY = wSum / cSum - (FittingRadius + 1);


	*ShiftX = CorrShiftBiasX + MaxPosX - HalfCorrSize + CenterX;
	*ShiftY = CorrShiftBiasY + MaxPosY - HalfCorrSize + CenterY;

	//	printf("shift pos:%f %f\n", *ShiftX, *ShiftY);

}