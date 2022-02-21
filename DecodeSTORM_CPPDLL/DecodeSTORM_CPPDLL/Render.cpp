#include "stdafx.h"
#include"Render.h"
int MaxImageVal = 0;
int SRImageWidth = 0;
int SRImageHeight = 0;
float LightRatio = 0.1f;
float* HistDat = (float*)calloc(ImgRend_MaxDatHistLen, sizeof(float));


float* Render(mat inLoc, int PixelZoom) {

	inLoc.cols(1, 2) = inLoc.cols(1, 2) / RawImPixSize;
	int ImageWidth;
	int ImageHeight;
	SRDriftCorrData_TypeDef::GetMaxImgSize(inLoc, &ImageWidth, &ImageHeight);
	SRImageWidth = ImageWidth * PixelZoom;
	SRImageHeight = ImageHeight * PixelZoom;

	int SRImageSize = SRImageWidth * SRImageHeight;

	float* SRIntensityImg = (float*)calloc(SRImageSize, sizeof(float));

	int TotalFluoNum = size(inLoc, 0);

	float* PSFArry = (float*)calloc(RendROIWidth * RendROIWidth, sizeof(float));
	float(*RenderPSF)[RendROIWidth] = (float(*)[RendROIWidth])PSFArry;

	float PeakPhoton;
	float RendXPos;
	float RendYPos;
	float Sigma;
	float LocPrec;

	for (int i = 0; i < TotalFluoNum; i++) {


		PeakPhoton = inLoc(i, Pos_PPho); // peak photon intensity as weight 
		RendXPos = inLoc(i, Pos_XPos) * PixelZoom;
		RendYPos = inLoc(i, Pos_YPos) * PixelZoom;
		Sigma = inLoc(i, Pos_SigX);
		LocPrec = Sigma / sqrt(PeakPhoton);

		PeakPhoton = isnan(LocPrec) ? 0 : PeakPhoton;
		PeakPhoton = (LocPrec <= 0) ? 0 : PeakPhoton;
		if ((PeakPhoton > 1.0f) && (RendXPos > RendROIWidth) && (RendXPos < SRImageWidth - 1 - RendROIWidth) && (RendYPos > RendROIWidth) && (RendYPos < SRImageHeight - 1 - RendROIWidth)) {

			int Offset;
			if (LocPrec > 30.0f) {
				LocPrec = 30.0f; // set maximum resolution
				PeakPhoton = 0.0f; // do not Render it
			}
			LocPrec = max(LocPrec, RawImPixSize / PixelZoom / 2);

			float RendSigma = LocPrec / (RawImPixSize / PixelZoom);
	
			SRDriftCorrData_TypeDef::CreateRenderPSF(PSFArry, RendXPos, RendYPos, RendSigma);

			for (int rcnt = 0; rcnt < RendROIWidth; rcnt++)
			{
				Offset = SRImageWidth * ((int)RendYPos - RendROIWidth_half + rcnt) + (int)RendXPos - RendROIWidth_half;
				for (int ccnt = 0; ccnt < RendROIWidth; ccnt++)
				{

					SRIntensityImg[Offset + ccnt] += PeakPhoton * RenderPSF[rcnt][ccnt];

				}
			}

			Offset = SRImageWidth * ((int)RendYPos) + (int)RendXPos;
			MaxImageVal = max(MaxImageVal, (int)SRIntensityImg[Offset]);

			int HistPos = SRIntensityImg[Offset] * ImgRend_MaxDatHistLen / MaxImageVal;
			if (HistPos > 0 && HistPos < ImgRend_MaxDatHistLen) {
				HistDat[HistPos]++;
			}
		}

	}

	float CurDat = 0.0f;
	float TotalDat = 0;

	for (int i = 0; i < ImgRend_MaxDatHistLen; i++) {
		TotalDat += HistDat[i];
	}
	int cnt;
	for (cnt = 0; cnt < ImgRend_MaxDatHistLen; cnt++) {
		CurDat += HistDat[cnt];
		if (CurDat / TotalDat > 0.95f) {
			break;
		}
	}

	if (cnt < 2) cnt = 2;

	LightRatio = (float)cnt / (float)ImgRend_MaxDatHistLen;
	MaxImageVal = (int)(MaxImageVal * LightRatio);


	free(PSFArry);

	return SRIntensityImg;
	
}