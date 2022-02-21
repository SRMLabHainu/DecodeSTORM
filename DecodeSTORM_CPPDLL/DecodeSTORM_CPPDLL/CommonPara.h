#pragma once
#include <direct.h>
#include <string>
#include <io.h>
#include <armadillo>



#define ImgRend_MaxDatHistLen 400

// stored parameter number for each types of fitting for each fitted molecule

#define OutParaNumGS2D			12 // gaussian 2d

#define RendROIWidth				7
#define RendROIWidth_half			((int)RendROIWidth/2)
// loc results information order for OutParaNumGS2D and OutParaNumAS3D and OutParaNumDH3D

#define Pos_PPho			0 // peak photon
#define Pos_XPos			1 // may have 0.5 or 1 pixel offset compared with other software
#define Pos_YPos			2 // may have 0.5 or 1 pixel offset compared with other software
#define Pos_ZPos			3 // may have 0.5 or 1 pixel offset compared with other software
#define Pos_SigX			4 // sigma x
#define Pos_SigY			5 // sigma y
#define Pos_TPho			6 // total photon
#define Pos_Bakg			7 // background
#define Pos_PSNR			8 // peak photon to background noise snr
#define Pos_CrbX			9 // crlb of x  
#define Pos_CrbY			10 // crlb of Y
#define Pos_Frme			11 // frame



// maximum molecules in allocated memory for each class
#define PointNumTh					20480



#define WhiteValue 16777215
#define BlueValue 65535
#define BlackValue 0

#define PI acos(-1)
using namespace std;
using namespace arma;

extern float RawImPixSize;
extern vec ColorVec;
extern string Chan1PrefixName;
extern string Chan2PrefixName;
extern string Directory;
extern int IsDriftCorrection;
extern int DriftCorrGroupFrameNum;
extern string CorrectedResPath;
