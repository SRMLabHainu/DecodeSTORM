/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU LESSER GENERAL PUBLIC LICENSE for more details.

You should have received a copy of the GNU LESSER GENERAL PUBLIC LICENSE
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#include "cuda_runtime.h"
#include "device_launch_parameters.h"



#include <string>
#include <iostream>
#include <vector>
#include"CommonPara.h"
using namespace std;


#pragma comment(lib,"cudart.lib")
#pragma comment(lib,"DriftCorrGPU_dll.lib")



// drift correction by super-resoluction image cross-correlation 
class SRDriftCorrData_TypeDef_GPU
{

public:

	void Init(int RawImgWidth, int RawImgHigh, int MaxFrameNum);
	void Deinit();

	void CorrectSampleShift(string FileName, string oFileName, float PixelSize, int CorrFrameNum, cudaStream_t cstream);


public:
	static int GetTotalFrame(string FileName);
	static void GetMaxImgSize(string FileName, int *ImageWidth, int *ImageHigh);
	static void GetMaxImgSizeFromLocArry(float* h_LocArry, float* d_LocArry, int* MaxImgWidth, int* MaxImgHigh, int FluoNum, cudaStream_t cstream);

private:
	int ImageWidth; // raw image width
	int ImageHigh; // raw image high
	int SRImageWidth; // raw image width
	int SRImageHigh; // raw image high


	int TotalFrame;

	int CorrGroupNum;

	int *GroupStartFrame; // start frame of each group
	int *GroupEndFrame; // start frame of each group

	int *GroupFrameStartPos; // start fluo position of each group
	int *GroupFrameEndPos; // start fluo position of each group


	float *XSliceShift;
	float *YSliceShift;

	float *XFrameShift;
	float *YFrameShift;

	float *d_XFrameShift;
	float *d_YFrameShift;

	// super-resolution image rendering
	float *h_LocArry; // render super-resolution image for corss-correlation
	float *d_LocArry; // render super-resolution image for corss-correlation

	float *h_FillImg1; // reference image
	float *h_FillImg2; // shifted image

	float *h_SumLine; // temporal use for correlation

	float *d_FillImg1; // reference image
	float *d_FillImg2; // shifted image

	float *d_MulImg;  // temporal use for correlation
	float *d_SumLine; // temporal use for correlation


private:

	// low level

	void CalcGroupNum(int TotalFrame, int CorrFrameNum);

	int GetAFrameEndFluoPos(string FileName, int OffsetFluoNum, int FindFrame);
	void GetCorrGroupFluoPos(string FileName);


	void ShiftInterpolation();
	void ApplyShiftTop(string FileName, string oFileName, int ShiftCorrEnable, cudaStream_t cstream);


	void RenderSlice(string FileName, int RendGroup, float PixelSize, float QE, float SNR_th, cudaStream_t cstream);

	void GetSliceShift(float *ShiftX, float *ShiftY, int CorrShiftBiasX, int CorrShiftBiasY, cudaStream_t cstream);

	void ResetFillImage(float *d_SRIntensityImg, int SRImageWidth, int SRImageHigh, cudaStream_t cstream);

	double CrossCorrelation(int ShiftX, int ShiftY, int CorrShiftBiasX, int CorrShiftBiasY, cudaStream_t cstream);

	// render 20480 molecules
	void ImageRender(float *h_LocArry, float *d_LocArry, float *d_SRIntensityImg, float QE, float SNR_th, float PixelSize, float PixelZoom, int SRImageWidth, int SRImageHigh, int FluoNum, cudaStream_t cstream);


	void gpuApplyShift(float *h_LocArry, int ShiftCorrEnable, int FluoNum, cudaStream_t cstream);


};


