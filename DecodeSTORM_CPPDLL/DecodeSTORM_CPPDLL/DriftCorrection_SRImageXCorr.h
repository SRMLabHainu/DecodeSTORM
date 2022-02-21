#pragma once

#include"stdafx.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include"CommonPara.h"
#include <stdio.h>
#include <string>
#include <armadillo>

using namespace std;
using namespace arma;





#define CorrSize					47
#define HalfCorrSize				((int)(CorrSize/2))

#define FittingRadius				12

#define SRShiftCorr_PixelZoom		8

#define MaxCorrGroupNum				512


#define BIT0						0X01
#define BIT1						0X02
#define BIT2						0X04

#define ThreadsPerBlock				32	//Threads Per Block




// drift correction by super-resoluction image cross-correlation 
class SRDriftCorrData_TypeDef
{

public:

	void Init(int RawImgWidth, int RawImgHigh, int MaxFrameNum);
	void Deinit();

	void CorrectSampleShift(mat& inLoc, string oFileName, float PixelSize, int CorrFrameNum, int isSave);


public:
	static void GetMaxImgSize(mat& inLoc, int* ImageWidth, int* ImageHigh);

	static void CreateRenderPSF(float oPSFArry[RendROIWidth * RendROIWidth], float RendXPos, float RendYPos, float RendSigma);

private:
	int ImageWidth; // raw image width
	int ImageHigh; // raw image high
	int SRImageWidth; // raw image width
	int SRImageHigh; // raw image high


	int TotalFrame;

	int CorrGroupNum;

	int* GroupStartFrame; // start frame of each group
	int* GroupEndFrame; // start frame of each group

	int* GroupFrameStartPos; // start fluo position of each group
	int* GroupFrameEndPos; // start fluo position of each group


	float* XSliceShift;
	float* YSliceShift;

	float* XFrameShift;
	float* YFrameShift;


	float* LocArry; // Render super-resolution image for corss-correlation


	float* FillImg1; // reference image
	float* FillImg2; // shifted image

	float* MulImg;  // temporal use for correlation
	float* SumLine; // temporal use for correlation


private:

	// low level

	void CalcGroupNum(int TotalFrame, int CorrFrameNum);

	void GetCorrGroupFluoPos(mat& inLoc);

	void ShiftInterpolation();

	void ApplyShiftTop(mat& inLoc, string oFileName, int ShiftCorrEnable, int isSave);


	void RenderSlice(mat& inLoc, int RendGroup, float PixelSize);

	void GetSliceShift(float* ShiftX, float* ShiftY, int CorrShiftBiasX, int CorrShiftBiasY);

};