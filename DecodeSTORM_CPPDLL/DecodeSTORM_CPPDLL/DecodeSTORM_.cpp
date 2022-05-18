#include "stdafx.h"
#include"DecodeSTORM_.h"
#include"QuantCluster.h"
#include"Colocalization.h"
#include"GetLocAndRoi.h"
#include"CppFileWrapper.h"
#include"NoiseFilter.h"
#include"DuplicateFilter.h"
#include"ImprecisionFilter.h"
#include"Merging.h"
#include"DBSCAN.h"
#include"VoronoiDiagram.h"
#include"Render.h"
#include"CommonPara.h"
#include"DriftCorr.h"
#include"CsvTxtTrans.h"
#include "DriftCorrGPU.h"
QuantCluster quantCluster;
Colocalization Coloc;


/*
 * Class:     DecodeSTORM_
 * Method:    lm_Convert
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_DecodeSTORM_1_lm_1Convert
(JNIEnv* env, jclass obj, jstring jConvertFilePath, jint FileFormatSelect)
{
	string ConvertFileName = jstring2str(env, jConvertFilePath);
	string::size_type iPos = ConvertFileName.find_last_of('\\') + 1;
	string FileName = ConvertFileName.substr(iPos, ConvertFileName.length() - iPos);
	FileName = FileName.substr(0, FileName.rfind("."));

	field<std::string> header(12);
	header = { "peak intensity (photon)","x(nm)","y(nm)","z(nm)","PSFSigmaX (nm)","PSFSigmaY (nm)","Total intensity(photon)",
		"background (photon)","SNR","Localization precision X(nm)", "Localization precision Y(nm)","frame" };

	if (FileFormatSelect == 0)
	{
		fstream RawFile(ConvertFileName);
		string line;
		int count = 0;
		//Remove the header of the QuickPALM's localization table
		ofstream OutTempFile(Directory + FileName + "_temp.xls", ios::out | ios::trunc);
		while (!RawFile.eof()) {
			getline(RawFile, line);
			if (count != 0)
				OutTempFile << line << endl;
			count++;
		}
		OutTempFile.close();
		RawFile.close();
		ofstream OutRawFile(ConvertFileName, ios::out | ios::trunc);
		fstream TempFile(Directory + FileName + "_temp.xls");
		while (!TempFile.eof()) {
			getline(TempFile, line);
			OutRawFile << line << endl;
		}
		OutRawFile.close();
		TempFile.close();
		string TempFilePath = Directory + FileName + "_temp.xls";
		remove(TempFilePath.c_str());
		//Convert format
		vector<vector<double>> LocVec;
		vector<double>::iterator it;
		fstream Data(ConvertFileName);
		double value;
		int i = 0;
		vector<double> LocVecRow;
		while (Data >> value) {
			i++;
			LocVecRow.push_back(value);
			if (i == 15)
			{
				i = 0;
				LocVec.push_back(LocVecRow);
				LocVecRow.clear();
			}

		}
		Data.close();

		mat QuickLoc = Std2Arma_2D(LocVec);
		float p = QuickLoc(0, 5) / QuickLoc(0, 3);
		mat Loc(QuickLoc.n_rows, 12, fill::zeros);
		Loc.col(0) = QuickLoc.col(1);
		Loc.cols(1, 3) = QuickLoc.cols(4, 6);
		Loc.col(4) = (QuickLoc.col(7) + QuickLoc.col(8)) * p;
		Loc.col(5) = (QuickLoc.col(9) + QuickLoc.col(10)) * p;
		Loc.col(11) = sort(QuickLoc.col(14));
		Loc.save(csv_name(Directory + FileName + "_DecodeSTORM.csv", header));
	}
	else if (FileFormatSelect == 1) {
		fstream RawFile(ConvertFileName);
		string line;
		int count = 0;
		//Remove the header of the RapidSTORM's localization table
		ofstream OutTempFile(Directory + FileName + "_temp.txt", ios::out | ios::trunc);
		while (!RawFile.eof()) {
			getline(RawFile, line);
			if (count != 0)//如果要修改内容就在这修改line的内容,再存到文件中就行了
				OutTempFile << line << endl;
			count++;
		}
		OutTempFile.close();
		RawFile.close();
		ofstream OutRawFile(ConvertFileName, ios::out | ios::trunc);
		fstream TempFile(Directory + FileName + "_temp.txt");
		while (!TempFile.eof()) {
			getline(TempFile, line);
			OutRawFile << line << endl;
		}
		OutRawFile.close();
		TempFile.close();
		string TempFilePath = Directory + FileName + "_temp.txt";
		remove(TempFilePath.c_str());//删除中间文件
		//Convert format
		vector<vector<double>> LocVec;
		vector<double>::iterator it;
		fstream Data(ConvertFileName);
		double value;
		int i = 0;
		vector<double> LocVecRow;
		while (Data >> value) {
			i++;
			LocVecRow.push_back(value);		//将数据压入堆栈。//
			if (i == 6)
			{
				i = 0;
				LocVec.push_back(LocVecRow);
				LocVecRow.clear();
			}

		}
		Data.close();

		mat RapidLoc = Std2Arma_2D(LocVec);
		mat Loc(RapidLoc.n_rows, 12, fill::zeros);
		Loc.cols(1, 2) = RapidLoc.cols(0, 1);
		Loc.col(6) = RapidLoc.col(3);
		Loc.col(7) = RapidLoc.col(5);
		Loc.col(11) = RapidLoc.col(2) + 1;
		Loc.save(csv_name(Directory + FileName + "_DecodeSTORM.csv", header));
	}
	else if (FileFormatSelect == 2) {
		mat ThunderLoc;
		ThunderLoc.load(csv_name(ConvertFileName, csv_opts::no_header));
		ThunderLoc.shed_row(0);
		mat Loc(ThunderLoc.n_rows, 12, fill::zeros);
		Loc.col(0) = ThunderLoc.col(6);
		Loc.cols(1, 2) = ThunderLoc.cols(2, 3);
		Loc.col(4) = ThunderLoc.col(4);
		Loc.col(5) = ThunderLoc.col(4);
		Loc.col(6) = ThunderLoc.col(5);
		Loc.col(7) = ThunderLoc.col(7);
		Loc.col(9) = ThunderLoc.col(8);
		Loc.col(10) = ThunderLoc.col(8);
		Loc.col(11) = ThunderLoc.col(1);
		Loc.save(csv_name(Directory + FileName + "_DecodeSTORM.csv", header));
	}
	else if (FileFormatSelect == 3) {
		TxtBinary2Csv(ConvertFileName);
	}
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetInLoc1
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_DecodeSTORM_1_lm_1GetInLoc1
(JNIEnv* env, jclass obj, jstring FilePathChannel1)
{
	FilePathCh1 = jstring2str(env, FilePathChannel1);
	GetinLoc1();
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetInLoc2
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_DecodeSTORM_1_lm_1GetInLoc2
(JNIEnv* env, jclass obj, jstring FilePathChannel2)
{
	FilePathCh2 = jstring2str(env, FilePathChannel2);
	GetinLoc2();
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_SetRoiLocPara
 * Signature: (DDDD)V
 */
JNIEXPORT void JNICALL Java_DecodeSTORM_1_lm_1SetRoiLocPara
(JNIEnv* env, jclass obj, jdouble jUpLeftX, jdouble jUpLeftY, jdouble jLowRightX, jdouble jLowRightY)
{
	UpLeftX = jUpLeftX;
	UpLeftY = jUpLeftY;
	LowRightX = jLowRightX;
	LowRightY = jLowRightY;
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetRoiLoc1
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_DecodeSTORM_1_lm_1GetRoiLoc1
(JNIEnv* env, jclass obj)
{
	ExtractRoiLoc1();
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetRoiLoc2
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_DecodeSTORM_1_lm_1GetRoiLoc2
(JNIEnv* env, jclass obj)
{
	ExtractRoiLoc2();
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_SetSaveDir
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_DecodeSTORM_1_lm_1SetSaveDir
(JNIEnv* env, jclass obj, jstring ResultSavePath)
{
	string iResultSavePath = jstring2str(env, ResultSavePath);
	Directory = iResultSavePath;
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_SetFilteringPara
 * Signature: (FFFFFF)V
 */
JNIEXPORT void JNICALL Java_DecodeSTORM_1_lm_1SetFilteringPara
(JNIEnv* env, jclass obj, jfloat jMinPts_NoiseFilter, jfloat jdistThre, jfloat jRadThre, jfloat jImpThres, jfloat jMaxOffFrame, jfloat jMaxDis)
{
	MinPts_NoiseFilter = jMinPts_NoiseFilter;
	distThre = jdistThre;

	RadThre = jRadThre;

	ImpThre = jImpThres;

	MaxOffFrame = jMaxOffFrame;
	MaxDis = jMaxDis;
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_SetDriftCorrPara
 * Signature: (IIF)V
 */
JNIEXPORT void JNICALL Java_DecodeSTORM_1_lm_1SetDriftCorrPara
(JNIEnv* env, jclass obj, jint isDriftCorr, jint jDriftCorrGroupFrameNum, jfloat jRawImPixSzie)
{
	IsDriftCorrection = isDriftCorr;
	DriftCorrGroupFrameNum = jDriftCorrGroupFrameNum;
	RawImPixSize = jRawImPixSzie;
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_SetDBSCANPara
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_DecodeSTORM_1_lm_1SetDBSCANPara
(JNIEnv* env, jclass obj, jint jMinPts, jfloat jEps)
{
	MinPts = jMinPts;
	Eps = jEps;
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_SetSpatialStatisticsPara
 * Signature: (FF)V
 */
JNIEXPORT void JNICALL Java_DecodeSTORM_1_lm_1SetSpatialStatisticsPara
(JNIEnv* env, jclass obj, jfloat jMaxAnaDis, jfloat jRingWidth)
{
	MaxAnaDis = jMaxAnaDis;
	RingWidth = jRingWidth;
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_SetColocalizationPara
 * Signature: (FFFDD)V
 */
JNIEXPORT void JNICALL Java_DecodeSTORM_1_lm_1SetColocalizationPara
(JNIEnv*, jclass,  jfloat jRmax, jfloat jStep, jfloat jColoThres, jdouble jwidth, jdouble jheight)
{
	Rmax = jRmax;
	Step = jStep;
	ColoThres = jColoThres;
	width = jwidth;
	height = jheight;
}


/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetChan1Loc
 * Signature: ()[[D
 */
JNIEXPORT jobjectArray JNICALL Java_DecodeSTORM_1_lm_1GetChan1Loc
(JNIEnv* env, jclass obj)
{
	jobjectArray result;
	jclass intArrCls = env->FindClass("[D");
	result = env->NewObjectArray(2 * inLoc_Ch1.n_rows, intArrCls, NULL);

	jdoubleArray inLocCh1X = env->NewDoubleArray(inLoc_Ch1.n_rows);
	env->SetDoubleArrayRegion(inLocCh1X, 0, inLoc_Ch1.n_rows, inLoc_Ch1X);
	env->SetObjectArrayElement(result, 0, inLocCh1X);

	jdoubleArray inLocCh1Y = env->NewDoubleArray(inLoc_Ch1.n_rows);
	env->SetDoubleArrayRegion(inLocCh1Y, 0, inLoc_Ch1.n_rows, inLoc_Ch1Y);
	env->SetObjectArrayElement(result, 1, inLocCh1Y);
	return result;

}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetChan2Loc
 * Signature: ()[[D
 */
JNIEXPORT jobjectArray JNICALL Java_DecodeSTORM_1_lm_1GetChan2Loc
(JNIEnv* env, jclass obj)
{
	jobjectArray result;
	jclass intArrCls = env->FindClass("[D");
	result = env->NewObjectArray(2 * inLoc_Ch2.n_rows, intArrCls, NULL);

	jdoubleArray inLocCh2X = env->NewDoubleArray(inLoc_Ch2.n_rows);
	env->SetDoubleArrayRegion(inLocCh2X, 0, inLoc_Ch2.n_rows, inLoc_Ch2X);
	env->SetObjectArrayElement(result, 0, inLocCh2X);

	jdoubleArray inLocCh2Y = env->NewDoubleArray(inLoc_Ch2.n_rows);
	env->SetDoubleArrayRegion(inLocCh2Y, 0, inLoc_Ch2.n_rows, inLoc_Ch2Y);
	env->SetObjectArrayElement(result, 1, inLocCh2Y);
	return result;

}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetChan1RoiLoc
 * Signature: ()[[D
 */
JNIEXPORT jobjectArray JNICALL Java_DecodeSTORM_1_lm_1GetChan1RoiLoc
(JNIEnv* env, jclass obj)
{
	jobjectArray result;
	jclass intArrCls = env->FindClass("[D");
	result = env->NewObjectArray(2 * RoiLoc1.n_rows, intArrCls, NULL);

	jdoubleArray Chan1RoiLocX = env->NewDoubleArray(RoiLoc1.n_rows);
	env->SetDoubleArrayRegion(Chan1RoiLocX, 0, RoiLoc1.n_rows, RoiLoc1X);
	env->SetObjectArrayElement(result, 0, Chan1RoiLocX);

	jdoubleArray Chan1RoiLocY = env->NewDoubleArray(RoiLoc1.n_rows);
	env->SetDoubleArrayRegion(Chan1RoiLocY, 0, RoiLoc1.n_rows, RoiLoc1Y);
	env->SetObjectArrayElement(result, 1, Chan1RoiLocY);
	return result;

}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetChan2RoiLoc
 * Signature: ()[[D
 */
JNIEXPORT jobjectArray JNICALL Java_DecodeSTORM_1_lm_1GetChan2RoiLoc
(JNIEnv* env, jclass obj)
{
	jobjectArray result;
	jclass intArrCls = env->FindClass("[D");
	result = env->NewObjectArray(2 * RoiLoc2.n_rows, intArrCls, NULL);

	jdoubleArray Chan2RoiLocX = env->NewDoubleArray(RoiLoc2.n_rows);
	env->SetDoubleArrayRegion(Chan2RoiLocX, 0, RoiLoc2.n_rows, RoiLoc2X);
	env->SetObjectArrayElement(result, 0, Chan2RoiLocX);

	jdoubleArray Chan2RoiLocY = env->NewDoubleArray(RoiLoc2.n_rows);
	env->SetDoubleArrayRegion(Chan2RoiLocY, 0, RoiLoc2.n_rows, RoiLoc2Y);
	env->SetObjectArrayElement(result, 1, Chan2RoiLocY);
	return result;

}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetScatImInfo
 * Signature: (ZZ)[F
 */
JNIEXPORT jfloatArray JNICALL Java_DecodeSTORM_1_lm_1GetScatImInfo
(JNIEnv* env, jclass obj, jboolean Ch1Exist, jboolean Ch2Exist)
{
	jfloatArray result = env->NewFloatArray(4);

	float ScatImInf[4];
	if (Ch1Exist) {
		ScatImInf[0] = inLoc_Ch1.col(1).max();
		ScatImInf[1] = inLoc_Ch1.col(1).min();
		ScatImInf[2] = inLoc_Ch1.col(2).max();
		ScatImInf[3] = inLoc_Ch1.col(2).min();
	}
	if (Ch2Exist) {
		ScatImInf[0] = inLoc_Ch2.col(1).max();
		ScatImInf[1] = inLoc_Ch2.col(1).min();
		ScatImInf[2] = inLoc_Ch2.col(2).max();
		ScatImInf[3] = inLoc_Ch2.col(2).min();
	}
	if (Ch1Exist && Ch2Exist) {
		ScatImInf[0] = max(inLoc_Ch1.col(1).max(), inLoc_Ch2.col(1).max());
		ScatImInf[1] = min(inLoc_Ch1.col(1).min(), inLoc_Ch2.col(1).min());
		ScatImInf[2] = max(inLoc_Ch1.col(2).max(), inLoc_Ch2.col(2).max());
		ScatImInf[3] = min(inLoc_Ch2.col(2).min(), inLoc_Ch2.col(2).min());

	}

	env->SetFloatArrayRegion(result, 0, 4, ScatImInf);

	return result;
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetScatRoiImInfo
 * Signature: (ZZ)[F
 */
JNIEXPORT jfloatArray JNICALL Java_DecodeSTORM_1_lm_1GetScatRoiImInfo
(JNIEnv* env, jclass obj, jboolean Ch1Exist, jboolean Ch2Exist)
{
	jfloatArray result = env->NewFloatArray(4);

	float ScatRoiImInf[4];
	if (Ch1Exist) {
		ScatRoiImInf[0] = RoiLoc1.col(1).max();
		ScatRoiImInf[1] = RoiLoc1.col(1).min();
		ScatRoiImInf[2] = RoiLoc1.col(2).max();
		ScatRoiImInf[3] = RoiLoc1.col(2).min();
	}
	if (Ch2Exist) {
		ScatRoiImInf[0] = RoiLoc2.col(1).max();
		ScatRoiImInf[1] = RoiLoc2.col(1).min();
		ScatRoiImInf[2] = RoiLoc2.col(2).max();
		ScatRoiImInf[3] = RoiLoc2.col(2).min();
	}
	if (Ch1Exist && Ch2Exist) {
		ScatRoiImInf[0] = max(RoiLoc1.col(1).max(), RoiLoc2.col(1).max());
		ScatRoiImInf[1] = min(RoiLoc1.col(1).min(), RoiLoc2.col(1).min());
		ScatRoiImInf[2] = max(RoiLoc1.col(2).max(), RoiLoc2.col(2).max());
		ScatRoiImInf[3] = min(RoiLoc1.col(2).min(), RoiLoc2.col(2).min());

	}

	env->SetFloatArrayRegion(result, 0, 4, ScatRoiImInf);

	return result;
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetCh1RoiSMLMImage
 * Signature: (FI)[F
 */
JNIEXPORT jfloatArray JNICALL Java_DecodeSTORM_1_lm_1GetCh1RoiSMLMImage
(JNIEnv* env, jclass obj, jfloat JRawImPixSize, jint ZoomF) {

	RawImPixSize = JRawImPixSize;

	jfloatArray result = NULL;

	float* receiver = Render(RawRoiLoc1, ZoomF);

	int ImgSize = SRImageWidth * SRImageHeight;
	result = env->NewFloatArray(ImgSize);

	env->SetFloatArrayRegion(result, 0, ImgSize, receiver);

	free(receiver);

	return result;

}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetCh2RoiSMLMImage
 * Signature: (FI)[F
 */
JNIEXPORT jfloatArray JNICALL Java_DecodeSTORM_1_lm_1GetCh2RoiSMLMImage
(JNIEnv* env, jclass obj, jfloat JRawImPixSize, jint ZoomF) {

	RawImPixSize = JRawImPixSize;

	jfloatArray result = NULL;

	float* receiver = Render(RawRoiLoc2, ZoomF);

	int ImgSize = SRImageWidth * SRImageHeight;
	result = env->NewFloatArray(ImgSize);

	env->SetFloatArrayRegion(result, 0, ImgSize, receiver);

	free(receiver);

	return result;

}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetCh1ArtifactCorrRoiSMLMImage
 * Signature: (FI)[F
 */
JNIEXPORT jfloatArray JNICALL Java_DecodeSTORM_1_lm_1GetCh1ArtifactCorrRoiSMLMImage
(JNIEnv* env, jclass obj, jfloat JRawImPixSize, jint ZoomF) {

	RawImPixSize = JRawImPixSize;

	jfloatArray result = NULL;

	float* receiver = Render(RoiLoc1, ZoomF);

	int ImgSize = SRImageWidth * SRImageHeight;
	result = env->NewFloatArray(ImgSize);

	env->SetFloatArrayRegion(result, 0, ImgSize, receiver);

	free(receiver);

	return result;

}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetCh2ArtifactCorrRoiSMLMImage
 * Signature: (FI)[F
 */
JNIEXPORT jfloatArray JNICALL Java_DecodeSTORM_1_lm_1GetCh2ArtifactCorrRoiSMLMImage
(JNIEnv* env, jclass obj, jfloat JRawImPixSize, jint ZoomF) {

	RawImPixSize = JRawImPixSize;

	jfloatArray result = NULL;

	float* receiver = Render(RoiLoc2, ZoomF);

	int ImgSize = SRImageWidth * SRImageHeight;
	result = env->NewFloatArray(ImgSize);

	env->SetFloatArrayRegion(result, 0, ImgSize, receiver);

	free(receiver);

	return result;

}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetCh1SMLMImage
 * Signature: (FI)[F
 */
JNIEXPORT jfloatArray JNICALL Java_DecodeSTORM_1_lm_1GetCh1SMLMImage
(JNIEnv* env, jclass obj, jfloat JRawImPixSize, jint ZoomF) {

	RawImPixSize = JRawImPixSize;

	jfloatArray result = NULL;

	float* receiver = Render(inLoc_Ch1, ZoomF);

	int ImgSize = SRImageWidth * SRImageHeight;
	result = env->NewFloatArray(ImgSize);

	env->SetFloatArrayRegion(result, 0, ImgSize, receiver);

	free(receiver);

	return result;

}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetCh2SMLMImage
 * Signature: (FI)[F
 */
JNIEXPORT jfloatArray JNICALL Java_DecodeSTORM_1_lm_1GetCh2SMLMImage
(JNIEnv* env, jclass obj, jfloat JRawImPixSize, jint ZoomF) {

	RawImPixSize = JRawImPixSize;
	jfloatArray result = NULL;

	float* receiver = Render(inLoc_Ch2, ZoomF);

	int ImgSize = SRImageWidth * SRImageHeight;
	result = env->NewFloatArray(ImgSize);

	env->SetFloatArrayRegion(result, 0, ImgSize, receiver);

	free(receiver);

	return result;
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetMaxDispVal
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_DecodeSTORM_1_lm_1GetMaxDispVal
(JNIEnv* env, jclass obj) {
	return MaxImageVal;
}


/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetRenderImageInf
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL Java_DecodeSTORM_1_lm_1GetRenderImageInf
(JNIEnv* env, jclass obj) {

	jintArray result = NULL;
	result = env->NewIntArray(2);

	int* ImgInf = new int[2];
	ImgInf[0] = SRImageWidth;
	ImgInf[1] = SRImageHeight;

	env->SetIntArrayRegion(result, 0, 2, (jint*)ImgInf);

	delete[]ImgInf;

	return result;
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_StartDriftCorr
 * Signature: (IIII)V
 */
JNIEXPORT void JNICALL Java_DecodeSTORM_1_lm_1StartDriftCorr
(JNIEnv* env, jclass obj, jint isDriftCorrCh1, jint isDriftCorrCh2, jint isGPU, jint isSave)
{
	if (isGPU)
	{
		if (isDriftCorrCh1)
		{
			Csv2TxtBinaryCh1();
			DriftCorrGPU(BinaryTxtCh1Path, Chan1PrefixName);
			if (IsDriftCorrection)
			{
				TxtBinary2Csv(CorrectedResPath);
				RoiLoc1.load(csv_name(oFileNameStr, csv_opts::no_header));
				RoiLoc1.shed_row(0);
				if (isSave == 0)
				{
					remove(oFileNameStr.c_str());
				}
				remove(BinaryTxtCh1Path.c_str());
				remove(CorrectedResPath.c_str());
			}
			
		}

		if (isDriftCorrCh2)
		{
			Csv2TxtBinaryCh2();
			DriftCorrGPU(BinaryTxtCh2Path, Chan2PrefixName);
			if (IsDriftCorrection)
			{
				TxtBinary2Csv(CorrectedResPath);
				RoiLoc2.load(csv_name(oFileNameStr, csv_opts::no_header));
				RoiLoc2.shed_row(0);
				if (isSave == 0)
				{
					remove(oFileNameStr.c_str());
				}
				remove(BinaryTxtCh2Path.c_str());
				remove(CorrectedResPath.c_str());
			}
			
		}

	}
	else
	{
		if (isDriftCorrCh1)
		{
			DriftCorr(RoiLoc1, Chan1PrefixName, isSave);
		}

		if (isDriftCorrCh2)
		{
			DriftCorr(RoiLoc2, Chan2PrefixName, isSave);
		}
	}


}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_Filtering
 * Signature: (IIIIIII)V
 */
JNIEXPORT void JNICALL Java_DecodeSTORM_1_lm_1Filtering
(JNIEnv* env, jclass obj, jint isImpFilter, jint isNoiFilt, jint isDupFilt, jint isMerging, jint isChan1, jint isChan2, jint isSave)
{

	field<std::string> header(12);
	header = { "peak intensity (photon)","x(nm)","y(nm)","z(nm)","PSFSigmaX (nm)","PSFSigmaY (nm)","Total intensity(photon)",
		"background (photon)","SNR","Localization precision X(nm)", "Localization precision Y(nm)","frame" };
	if (isSave == 1)
	{
		string FilteringSavePath = Directory + "FilteringResults\\";
		if (isImpFilter && isNoiFilt == 0 && isDupFilt == 0 && isMerging == 0) {
			if (isChan1) {
				RoiLoc1 = ImprecisionFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_Imp.csv", header));
			}
			if (isChan2) {
				RoiLoc2 = ImprecisionFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_Imp.csv", header));
			}
		}

		if (isImpFilter == 0 && isNoiFilt && isDupFilt == 0 && isMerging == 0) {
			if (isChan1) {
				RoiLoc1 = NoiseFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_Noi.csv", header));
			}
			if (isChan2) {
				RoiLoc2 = NoiseFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_Noi.csv", header));
			}
		}

		if (isImpFilter == 0 && isNoiFilt == 0 && isDupFilt && isMerging == 0) {
			if (isChan1) {
				RoiLoc1 = DuplicateFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_Dup.csv", header));
			}
			if (isChan2) {
				RoiLoc2 = DuplicateFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_Dup.csv", header));
			}
		}


		if (isImpFilter == 0 && isNoiFilt == 0 && isDupFilt == 0 && isMerging) {
			if (isChan1) {
				RoiLoc1 = Merging(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_Merging.csv", header));
			}
			if (isChan2) {
				RoiLoc2 = Merging(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_Merging.csv", header));
			}
		}

		if (isImpFilter && isNoiFilt && isDupFilt == 0 && isMerging == 0) {
			if (isChan1) {
				RoiLoc1 = ImprecisionFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_Imp.csv", header));
				RoiLoc1 = NoiseFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_ImpNoi.csv", header));
			}
			if (isChan2) {
				RoiLoc2 = ImprecisionFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_Imp.csv", header));
				RoiLoc2 = NoiseFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_ImpNoi.csv", header));
			}
		}

		if (isImpFilter && isNoiFilt == 0 && isDupFilt && isMerging == 0) {
			if (isChan1) {
				RoiLoc1 = ImprecisionFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_Imp.csv", header));
				RoiLoc1 = DuplicateFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_ImpDup.csv", header));
			}
			if (isChan2) {
				RoiLoc2 = ImprecisionFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_Imp.csv", header));
				RoiLoc2 = DuplicateFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_ImpDup.csv", header));
			}
		}

		if (isImpFilter && isNoiFilt == 0 && isDupFilt == 0 && isMerging) {
			if (isChan1) {
				RoiLoc1 = ImprecisionFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_Imp.csv", header));
				RoiLoc1 = Merging(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_ImpMerging.csv", header));
			}
			if (isChan2) {
				RoiLoc2 = ImprecisionFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_Imp.csv", header));
				RoiLoc2 = Merging(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_ImpMerging.csv", header));
			}
		}

		if (isImpFilter == 0 && isNoiFilt && isDupFilt && isMerging == 0) {
			if (isChan1) {
				RoiLoc1 = NoiseFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_Noi.csv", header));
				RoiLoc1 = DuplicateFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_NoiDup.csv", header));
			}
			if (isChan2) {
				RoiLoc2 = NoiseFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_Noi.csv", header));
				RoiLoc2 = DuplicateFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_NoiDup.csv", header));
			}
		}


		if (isImpFilter == 0 && isNoiFilt && isDupFilt == 0 && isMerging) {
			if (isChan1) {
				RoiLoc1 = NoiseFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_Noi.csv", header));
				RoiLoc1 = Merging(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_NoiMerging.csv", header));
			}
			if (isChan2) {
				RoiLoc2 = NoiseFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_Noi.csv", header));
				RoiLoc2 = Merging(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_NoiMerging.csv", header));
			}
		}

		if (isImpFilter == 0 && isNoiFilt == 0 && isDupFilt && isMerging) {
			if (isChan1) {
				RoiLoc1 = DuplicateFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_Dup.csv", header));
				RoiLoc1 = Merging(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_DupMerging.csv", header));
			}
			if (isChan2) {
				RoiLoc2 = DuplicateFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_Dup.csv", header));
				RoiLoc2 = Merging(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_DupMerging.csv", header));
			}
		}

		if (isImpFilter && isNoiFilt && isDupFilt && isMerging == 0) {
			if (isChan1) {
				RoiLoc1 = ImprecisionFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_Imp.csv", header));
				RoiLoc1 = NoiseFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_ImpNoi.csv", header));
				RoiLoc1 = DuplicateFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_ImpNoiDup.csv", header));
			}
			if (isChan2) {
				RoiLoc2 = ImprecisionFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_Imp.csv", header));
				RoiLoc2 = NoiseFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_ImpNoi.csv", header));
				DuplicateFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_ImpNoiDup.csv", header));
			}
		}

		if (isImpFilter && isNoiFilt && isDupFilt == 0 && isMerging) {
			if (isChan1) {
				RoiLoc1 = ImprecisionFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_Imp.csv", header));
				RoiLoc1 = NoiseFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_ImpNoi.csv", header));
				RoiLoc1 = Merging(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_ImpNoiMerging.csv", header));
			}
			if (isChan2) {
				RoiLoc2 = ImprecisionFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_Imp.csv", header));
				RoiLoc2 = NoiseFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_ImpNoi.csv", header));
				RoiLoc2 = Merging(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_ImpNoiMerging.csv", header));
			}
		}

		if (isImpFilter && isNoiFilt == 0 && isDupFilt && isMerging) {
			if (isChan1) {
				RoiLoc1 = ImprecisionFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_Imp.csv", header));
				RoiLoc1 = DuplicateFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_ImpDup.csv", header));
				RoiLoc1 = Merging(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_ImpDupMerging.csv", header));
			}
			if (isChan2) {
				RoiLoc2 = ImprecisionFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_Imp.csv", header));
				RoiLoc2 = DuplicateFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_ImpDup.csv", header));
				RoiLoc2 = Merging(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_ImpDupMerging.csv", header));
			}
		}

		if (isImpFilter == 0 && isNoiFilt && isDupFilt && isMerging) {
			if (isChan1) {
				RoiLoc1 = NoiseFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_Noi.csv", header));
				RoiLoc1 = DuplicateFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_NoiDup.csv", header));
				RoiLoc1 = Merging(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_NoiDupMerging.csv", header));
			}
			if (isChan2) {
				RoiLoc2 = NoiseFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_Noi.csv", header));
				RoiLoc2 = DuplicateFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_NoiDup.csv", header));
				RoiLoc2 = Merging(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_NoiDupMerging.csv", header));
			}
		}


		if (isImpFilter && isNoiFilt && isDupFilt && isMerging) {
			if (isChan1) {
				RoiLoc1 = ImprecisionFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_Imp.csv", header));
				RoiLoc1 = NoiseFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_ImpNoi.csv", header));
				RoiLoc1 = DuplicateFilter(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_ImpNoiDup.csv", header));
				RoiLoc1 = Merging(RoiLoc1);
				RoiLoc1.save(csv_name(FilteringSavePath + Chan1PrefixName + "_Filter_ImpNoiDupMerging.csv", header));
			}
			if (isChan2) {
				RoiLoc2 = ImprecisionFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_Imp.csv", header));
				RoiLoc2 = NoiseFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_ImpNoi.csv", header));
				RoiLoc2 = DuplicateFilter(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_ImpNoiDup.csv", header));
				RoiLoc2 = Merging(RoiLoc2);
				RoiLoc2.save(csv_name(FilteringSavePath + Chan2PrefixName + "_Filter_ImpNoiDupMerging.csv", header));
			}
		}
	}
		else {
			if (isImpFilter && isNoiFilt == 0 && isDupFilt == 0 && isMerging == 0) {
				if (isChan1) {
					RoiLoc1 = ImprecisionFilter(RoiLoc1);
				}
				if (isChan2) {
					RoiLoc2 = ImprecisionFilter(RoiLoc2);
				}
			}

			if (isImpFilter == 0 && isNoiFilt && isDupFilt == 0 && isMerging == 0) {
				if (isChan1) {
					RoiLoc1 = NoiseFilter(RoiLoc1);
				}
				if (isChan2) {
					RoiLoc2 = NoiseFilter(RoiLoc2);
				}
			}

			if (isImpFilter == 0 && isNoiFilt == 0 && isDupFilt && isMerging == 0) {
				if (isChan1) {
					RoiLoc1 = DuplicateFilter(RoiLoc1);
				}
				if (isChan2) {
					RoiLoc2 = DuplicateFilter(RoiLoc2);
				}
			}


			if (isImpFilter == 0 && isNoiFilt == 0 && isDupFilt == 0 && isMerging) {
				if (isChan1) {
					RoiLoc1 = Merging(RoiLoc1);
				}
				if (isChan2) {
					RoiLoc2 = Merging(RoiLoc2);
				}
			}

			if (isImpFilter && isNoiFilt && isDupFilt == 0 && isMerging == 0) {
				if (isChan1) {
					RoiLoc1 = ImprecisionFilter(RoiLoc1);
					RoiLoc1 = NoiseFilter(RoiLoc1);
				}
				if (isChan2) {
					RoiLoc2 = ImprecisionFilter(RoiLoc2);
					RoiLoc2 = NoiseFilter(RoiLoc2);
				}
			}

			if (isImpFilter && isNoiFilt == 0 && isDupFilt && isMerging == 0) {
				if (isChan1) {
					RoiLoc1 = ImprecisionFilter(RoiLoc1);
					RoiLoc1 = DuplicateFilter(RoiLoc1);
				}
				if (isChan2) {
					RoiLoc2 = ImprecisionFilter(RoiLoc2);
					RoiLoc2 = DuplicateFilter(RoiLoc2);
				}
			}

			if (isImpFilter && isNoiFilt == 0 && isDupFilt == 0 && isMerging) {
				if (isChan1) {
					RoiLoc1 = ImprecisionFilter(RoiLoc1);
					RoiLoc1 = Merging(RoiLoc1);
				}
				if (isChan2) {
					RoiLoc2 = ImprecisionFilter(RoiLoc2);
					RoiLoc2 = Merging(RoiLoc2);
				}
			}

			if (isImpFilter == 0 && isNoiFilt && isDupFilt && isMerging == 0) {
				if (isChan1) {
					RoiLoc1 = NoiseFilter(RoiLoc1);
					RoiLoc1 = DuplicateFilter(RoiLoc1);
				}
				if (isChan2) {
					RoiLoc2 = NoiseFilter(RoiLoc2);
					RoiLoc2 = DuplicateFilter(RoiLoc2);
				}
			}


			if (isImpFilter == 0 && isNoiFilt && isDupFilt == 0 && isMerging) {
				if (isChan1) {
					RoiLoc1 = NoiseFilter(RoiLoc1);
					RoiLoc1 = Merging(RoiLoc1);
				}
				if (isChan2) {
					RoiLoc2 = NoiseFilter(RoiLoc2);
					RoiLoc2 = Merging(RoiLoc2);
				}
			}

			if (isImpFilter == 0 && isNoiFilt == 0 && isDupFilt && isMerging) {
				if (isChan1) {
					RoiLoc1 = DuplicateFilter(RoiLoc1);
					RoiLoc1 = Merging(RoiLoc1);
				}
				if (isChan2) {
					RoiLoc2 = DuplicateFilter(RoiLoc2);
					RoiLoc2 = Merging(RoiLoc2);
				}
			}

			if (isImpFilter && isNoiFilt && isDupFilt && isMerging == 0) {
				if (isChan1) {
					RoiLoc1 = ImprecisionFilter(RoiLoc1);
					RoiLoc1 = NoiseFilter(RoiLoc1);
					RoiLoc1 = DuplicateFilter(RoiLoc1);
				}
				if (isChan2) {
					RoiLoc2 = ImprecisionFilter(RoiLoc2);
					RoiLoc2 = NoiseFilter(RoiLoc2);
					DuplicateFilter(RoiLoc2);
				}
			}

			if (isImpFilter && isNoiFilt && isDupFilt == 0 && isMerging) {
				if (isChan1) {
					RoiLoc1 = ImprecisionFilter(RoiLoc1);
					RoiLoc1 = NoiseFilter(RoiLoc1);
					RoiLoc1 = Merging(RoiLoc1);
				}
				if (isChan2) {
					RoiLoc2 = ImprecisionFilter(RoiLoc2);
					RoiLoc2 = NoiseFilter(RoiLoc2);
					RoiLoc2 = Merging(RoiLoc2);
				}
			}

			if (isImpFilter && isNoiFilt == 0 && isDupFilt && isMerging) {
				if (isChan1) {
					RoiLoc1 = ImprecisionFilter(RoiLoc1);
					RoiLoc1 = DuplicateFilter(RoiLoc1);
					RoiLoc1 = Merging(RoiLoc1);
				}
				if (isChan2) {
					RoiLoc2 = ImprecisionFilter(RoiLoc2);
					RoiLoc2 = DuplicateFilter(RoiLoc2);
					RoiLoc2 = Merging(RoiLoc2);
				}
			}

			if (isImpFilter == 0 && isNoiFilt && isDupFilt && isMerging) {
				if (isChan1) {
					RoiLoc1 = NoiseFilter(RoiLoc1);
					RoiLoc1 = DuplicateFilter(RoiLoc1);
					RoiLoc1 = Merging(RoiLoc1);
				}
				if (isChan2) {
					RoiLoc2 = NoiseFilter(RoiLoc2);
					RoiLoc2 = DuplicateFilter(RoiLoc2);
					RoiLoc2 = Merging(RoiLoc2);
				}
			}


			if (isImpFilter && isNoiFilt && isDupFilt && isMerging) {
				if (isChan1) {
					RoiLoc1 = ImprecisionFilter(RoiLoc1);
					RoiLoc1 = NoiseFilter(RoiLoc1);
					RoiLoc1 = DuplicateFilter(RoiLoc1);
					RoiLoc1 = Merging(RoiLoc1);
				}
				if (isChan2) {
					RoiLoc2 = ImprecisionFilter(RoiLoc2);
					RoiLoc2 = NoiseFilter(RoiLoc2);
					RoiLoc2 = DuplicateFilter(RoiLoc2);
					RoiLoc2 = Merging(RoiLoc2);
				}
			}
		}
	if (isChan1) {
		RoiLoc1X = ArmaVecToArrayD(RoiLoc1.col(1));
		RoiLoc1Y = ArmaVecToArrayD(RoiLoc1.col(2));
	}
	if (isChan2)
	{
		RoiLoc2X = ArmaVecToArrayD(RoiLoc2.col(1));
		RoiLoc2Y = ArmaVecToArrayD(RoiLoc2.col(2));
	}

}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetRdfIm
 * Signature: (III)[[D
 */
JNIEXPORT jobjectArray JNICALL Java_DecodeSTORM_1_lm_1GetRdfIm
(JNIEnv* env, jclass obj, jint ChanSele_1, jint ChanSele_2, jint isLinkPostStatistic)
{
	if (isLinkPostStatistic) {
		if (ChanSele_1 == 0 && ChanSele_2 == 0)
		{
			quantCluster.RDF_Analyze(RoiLoc1, RoiLoc1);
		}
		else if (ChanSele_1 == 0 && ChanSele_2 == 1)
		{
			quantCluster.RDF_Analyze(RoiLoc1, RoiLoc2);
		}
		else if (ChanSele_1 == 1 && ChanSele_2 == 0)
		{
			quantCluster.RDF_Analyze(RoiLoc2, RoiLoc1);
		}
		else if (ChanSele_1 == 1 && ChanSele_2 == 1)
		{
			quantCluster.RDF_Analyze(RoiLoc2, RoiLoc2);
		}
	}
	else {
		if (ChanSele_1 == 0 && ChanSele_2 == 0)
		{
			quantCluster.RDF_Analyze(RawRoiLoc1, RawRoiLoc1);
		}
		else if (ChanSele_1 == 0 && ChanSele_2 == 1)
		{
			quantCluster.RDF_Analyze(RawRoiLoc1, RawRoiLoc2);
		}
		else if (ChanSele_1 == 1 && ChanSele_2 == 0)
		{
			quantCluster.RDF_Analyze(RawRoiLoc2, RawRoiLoc1);
		}
		else if (ChanSele_1 == 1 && ChanSele_2 == 1)
		{
			quantCluster.RDF_Analyze(RawRoiLoc2, RawRoiLoc2);
		}

	}
	jobjectArray result;
	jclass intArrCls = env->FindClass("[D");
	result = env->NewObjectArray(2 * quantCluster.Xr_Size, intArrCls, NULL);

	jdoubleArray RdfResultX = env->NewDoubleArray(quantCluster.Xr_Size);
	env->SetDoubleArrayRegion(RdfResultX, 0, quantCluster.Xr_Size, quantCluster.Xr_Arr);
	env->SetObjectArrayElement(result, 0, RdfResultX);

	jdoubleArray RdfResultY = env->NewDoubleArray(quantCluster.Xr_Size);
	env->SetDoubleArrayRegion(RdfResultY, 0, quantCluster.Xr_Size, quantCluster.Gr_Arr);
	env->SetObjectArrayElement(result, 1, RdfResultY);
	return result;

}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetRdfImInfo
 * Signature: ()[F
 */
JNIEXPORT jfloatArray JNICALL Java_DecodeSTORM_1_lm_1GetRdfImInfo
(JNIEnv* env, jclass obj) {

	jfloatArray result = env->NewFloatArray(3);

	float RdfImInf[3];

	RdfImInf[0] = quantCluster.RdfBorderMax;
	RdfImInf[1] = quantCluster.RdfBorderMin;
	RdfImInf[2] = quantCluster.MaxGrId;

	env->SetFloatArrayRegion(result, 0, 3, RdfImInf);

	return result;

}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_SaveRdfResult
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_DecodeSTORM_1_lm_1SaveRdfResult
(JNIEnv* env, jclass obj, jint ChanSele_1, jint ChanSele_2) {
	string SaveNameRDF;
	if (ChanSele_1 == 0 && ChanSele_2 == 0) {
		SaveNameRDF = Chan1PrefixName + "_RDF Result.csv";
	}
	else if (ChanSele_1 == 0 && ChanSele_2 == 1) {
		SaveNameRDF = Chan1PrefixName + "-" + Chan2PrefixName + "_RDF Result.csv";
	}
	else if (ChanSele_1 == 1 && ChanSele_2 == 0) {
		SaveNameRDF = Chan2PrefixName + "-" + Chan1PrefixName + "_RDF Result.csv";
	}
	else {
		SaveNameRDF = Chan2PrefixName + " _RDF Result.csv";;
	}
	string RipCsv_path = Directory + "\\SpatialStatisticsResults\\" + SaveNameRDF;
	field<std::string> header(RDF_Result.n_cols);
	header = { "G(r)","r" };
	RDF_Result.save(csv_name(RipCsv_path, header));

}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetRipleyHIm
 * Signature: (III)[[D
 */
JNIEXPORT jobjectArray JNICALL Java_DecodeSTORM_1_lm_1GetRipleyHIm
(JNIEnv* env, jclass obj, jint ChanSele_1, jint ChanSele_2, jint isLinkPostStatistic)
{
	if (isLinkPostStatistic)
	{
		if (ChanSele_1 == 0 && ChanSele_2 == 0)
		{
			quantCluster.RipleyH_Analyze(RoiLoc1, RoiLoc1);
		}
		else if (ChanSele_1 == 0 && ChanSele_2 == 1)
		{
			quantCluster.RipleyH_Analyze(RoiLoc1, RoiLoc2);
		}
		else if (ChanSele_1 == 1 && ChanSele_2 == 0)
		{
			quantCluster.RipleyH_Analyze(RoiLoc2, RoiLoc1);
		}
		else if (ChanSele_1 == 1 && ChanSele_2 == 1)
		{
			quantCluster.RipleyH_Analyze(RoiLoc2, RoiLoc2);
		}
	}
	else {
		if (ChanSele_1 == 0 && ChanSele_2 == 0)
		{
			quantCluster.RipleyH_Analyze(RawRoiLoc1, RawRoiLoc1);
		}
		else if (ChanSele_1 == 0 && ChanSele_2 == 1)
		{
			quantCluster.RipleyH_Analyze(RawRoiLoc1, RawRoiLoc2);
		}
		else if (ChanSele_1 == 1 && ChanSele_2 == 0)
		{
			quantCluster.RipleyH_Analyze(RawRoiLoc2, RawRoiLoc1);
		}
		else if (ChanSele_1 == 1 && ChanSele_2 == 1)
		{
			quantCluster.RipleyH_Analyze(RawRoiLoc2, RawRoiLoc2);
		}


	}
	jobjectArray result;
	jclass intArrCls = env->FindClass("[D");
	result = env->NewObjectArray(2 * quantCluster.Xr_Size, intArrCls, NULL);

	jdoubleArray RipleyResultX = env->NewDoubleArray(quantCluster.Xr_Size);
	env->SetDoubleArrayRegion(RipleyResultX, 0, quantCluster.Xr_Size, quantCluster.Xr_Arr);
	env->SetObjectArrayElement(result, 0, RipleyResultX);

	jdoubleArray RipleyResultY = env->NewDoubleArray(quantCluster.Xr_Size);
	env->SetDoubleArrayRegion(RipleyResultY, 0, quantCluster.Xr_Size, quantCluster.Hr_Arr);
	env->SetObjectArrayElement(result, 1, RipleyResultY);
	return result;

}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetRipleyImInfo
 * Signature: ()[F
 */
JNIEXPORT jfloatArray JNICALL Java_DecodeSTORM_1_lm_1GetRipleyImInfo
(JNIEnv* env, jclass obj)
{

	jfloatArray result = env->NewFloatArray(3);

	float RipleyImInf[3];

	RipleyImInf[0] = quantCluster.RipleyBorderMax;
	RipleyImInf[1] = quantCluster.RipleyBorderMin;
	RipleyImInf[2] = quantCluster.MaxHrId;
	env->SetFloatArrayRegion(result, 0, 3, RipleyImInf);

	return result;
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_SaveRipleyResult
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_DecodeSTORM_1_lm_1SaveRipleyResult
(JNIEnv* env, jclass obj, jint ChanSele_1, jint ChanSele_2) {
	string SaveNameRipleyH;
	if (ChanSele_1 == 0 && ChanSele_2 == 0) {
		SaveNameRipleyH = Chan1PrefixName + "_RipleyH Result.csv";
	}
	else if (ChanSele_1 == 0 && ChanSele_2 == 1) {
		SaveNameRipleyH = Chan1PrefixName + "-" + Chan2PrefixName + "_RipleyH Result.csv";
	}
	else if (ChanSele_1 == 1 && ChanSele_2 == 0) {
		SaveNameRipleyH = Chan2PrefixName + "-" + Chan1PrefixName + "_RipleyH Result.csv";
	}
	else {
		SaveNameRipleyH = Chan2PrefixName + " _RipleyH Result.csv";;
	}
	string RipCsv_path = Directory + "\\SpatialStatisticsResults\\" + SaveNameRipleyH;
	field<std::string> header(RipleyH_Result.n_cols);
	header = { "H(r)","r"};
	RipleyH_Result.save(csv_name(RipCsv_path, header));

}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetDbscanImCh1
 * Signature: (II)[I
 */
JNIEXPORT jintArray JNICALL Java_DecodeSTORM_1_lm_1GetDbscanImCh1
(JNIEnv* env, jclass obj, jint isEps, jint isLinkPostCluster)
{
	if (isLinkPostCluster) {
		if (isEps)
		{
			DBSCAN(RoiLoc1);

			jintArray result = env->NewIntArray(DBSCAN_ImageW * DBSCAN_ImageH);

			env->SetIntArrayRegion(result, 0, DBSCAN_ImageW * DBSCAN_ImageH, (jint*)DBSCAN_ImageArray);

			return result;
		}
		else {
			AutoCalcEpsDBSCAN(RoiLoc1);

			jintArray result = env->NewIntArray(DBSCAN_ImageW * DBSCAN_ImageH);


			env->SetIntArrayRegion(result, 0, DBSCAN_ImageW * DBSCAN_ImageH, (jint*)DBSCAN_ImageArray);

			return result;
		}
	}
	else {
		if (isEps)
		{
			DBSCAN(RawRoiLoc1);

			jintArray result = env->NewIntArray(DBSCAN_ImageW * DBSCAN_ImageH);

			env->SetIntArrayRegion(result, 0, DBSCAN_ImageW * DBSCAN_ImageH, (jint*)DBSCAN_ImageArray);

			return result;
		}
		else {
			AutoCalcEpsDBSCAN(RawRoiLoc1);

			jintArray result = env->NewIntArray(DBSCAN_ImageW * DBSCAN_ImageH);


			env->SetIntArrayRegion(result, 0, DBSCAN_ImageW * DBSCAN_ImageH, (jint*)DBSCAN_ImageArray);

			return result;
		}
	}
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetDbscanImCh2
 * Signature: (II)[I
 */
JNIEXPORT jintArray JNICALL Java_DecodeSTORM_1_lm_1GetDbscanImCh2
(JNIEnv* env, jclass obj, jint isEps, jint isLinkPostCluster) {
	if (isLinkPostCluster) {
		if (isEps)
		{
			DBSCAN(RoiLoc2);

			jintArray result = env->NewIntArray(DBSCAN_ImageW * DBSCAN_ImageH);

			env->SetIntArrayRegion(result, 0, DBSCAN_ImageW * DBSCAN_ImageH, (jint*)DBSCAN_ImageArray);

			return result;
		}
		else {
			AutoCalcEpsDBSCAN(RoiLoc2);

			jintArray result = env->NewIntArray(DBSCAN_ImageW * DBSCAN_ImageH);


			env->SetIntArrayRegion(result, 0, DBSCAN_ImageW * DBSCAN_ImageH, (jint*)DBSCAN_ImageArray);

			return result;
		}
	}
	else {
		if (isEps)
		{
			DBSCAN(RawRoiLoc2);

			jintArray result = env->NewIntArray(DBSCAN_ImageW * DBSCAN_ImageH);

			env->SetIntArrayRegion(result, 0, DBSCAN_ImageW * DBSCAN_ImageH, (jint*)DBSCAN_ImageArray);

			return result;
		}
		else {
			AutoCalcEpsDBSCAN(RawRoiLoc2);

			jintArray result = env->NewIntArray(DBSCAN_ImageW * DBSCAN_ImageH);


			env->SetIntArrayRegion(result, 0, DBSCAN_ImageW * DBSCAN_ImageH, (jint*)DBSCAN_ImageArray);

			return result;
		}

	}
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetDbscanImInfo
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL Java_DecodeSTORM_1_lm_1GetDbscanImInfo
(JNIEnv* env, jclass obj)
{
	jintArray result = env->NewIntArray(4);

	int DbscanImInfo[4];
	DbscanImInfo[0] = DBSCAN_ImageW;
	DbscanImInfo[1] = DBSCAN_ImageH;
	DbscanImInfo[2] = (int)round(EpsAutoCalc);
	DbscanImInfo[3] = ClusterNum;
	env->SetIntArrayRegion(result, 0, 4, (jint*)DbscanImInfo);

	return result;
}
/*
 * Class:     DecodeSTORM_
 * Method:    lm_SaveDbscanResult
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_DecodeSTORM_1_lm_1SaveDbscanResult
(JNIEnv* env, jclass obj, jstring DbscanSavePath) {

	string DBSACAN_Res_save = jstring2str(env, DbscanSavePath);
	//DBSACAN_Res_save = DBSACAN_Res_save + ".csv";
	field<std::string> header(DBSCAN_Res.n_cols);
	header(0) = "x(nm)";
	header(1) = "y(nm)";
	header(2) = "z(nm)";
	header(3) = "clusters";
	header(4) = "LocType";
	DBSCAN_Res.save(csv_name(DBSACAN_Res_save, header));
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_VoronoiDiagramCh1
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_DecodeSTORM_1_lm_1VoronoiDiagramCh1
(JNIEnv* env, jclass obj, jstring VoronoiSavePath, jint isLinkPostCluster)
{
	VoronoiResSave = jstring2str(env, VoronoiSavePath);
	if (isLinkPostCluster) {
		VoronoiDiagram(RoiLoc1);
	}
	else {
		VoronoiDiagram(RawRoiLoc1);
	}
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_VoronoiDiagramCh2
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_DecodeSTORM_1_lm_1VoronoiDiagramCh2
(JNIEnv* env, jclass obj, jstring VoronoiSavePath, jint isLinkPostCluster) {
	VoronoiResSave = jstring2str(env, VoronoiSavePath);
	if (isLinkPostCluster) {
		VoronoiDiagram(RoiLoc2);
	}
	else {
		VoronoiDiagram(RawRoiLoc2);
	}
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_IsSaveVoronoiResult
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_DecodeSTORM_1_lm_1IsSaveVoronoiResult
(JNIEnv* env, jclass obj, jint isSave) {
	if (isSave == 0)
	{
		remove(VoronoiResSave.c_str());
	}
	
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_Colocalization
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_DecodeSTORM_1_lm_1Colocalization
(JNIEnv* env, jclass obj, jint isLinkPostColoc)
{
	if (isLinkPostColoc) {
		Coloc.ColocalizationCal(RoiLoc1, RoiLoc2, Chan1PrefixName + "-" + Chan2PrefixName);
	}
	else {
		Coloc.ColocalizationCal(RawRoiLoc1, RawRoiLoc2, Chan1PrefixName + "-" + Chan2PrefixName);
	}
}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetCBC1Hist
 * Signature: ()[D
 */
JNIEXPORT jdoubleArray JNICALL Java_DecodeSTORM_1_lm_1GetCBC1Hist
(JNIEnv* env, jclass obj)
{
	jdoubleArray result = env->NewDoubleArray(CBC1Size);

	env->SetDoubleArrayRegion(result, 0, CBC1Size, CBC1Array);

	return result;

}

/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetCBC2Hist
 * Signature: ()[D
 */
JNIEXPORT jdoubleArray JNICALL Java_DecodeSTORM_1_lm_1GetCBC2Hist
(JNIEnv* env, jclass obj)
{
	jdoubleArray result = env->NewDoubleArray(CBC2Size);

	env->SetDoubleArrayRegion(result, 0, CBC2Size, CBC2Array);

	return result;
}
/*
 * Class:     DecodeSTORM_
 * Method:    lm_GetCBCHistInfo
 * Signature: ()[F
 */
JNIEXPORT jfloatArray JNICALL Java_DecodeSTORM_1_lm_1GetCBCHistInfo
(JNIEnv* env, jclass obj)
{
	jfloatArray result = env->NewFloatArray(2);

	float CBCHistInfo[2];
	CBCHistInfo[0] = CBC1Range;
	CBCHistInfo[1] = CBC2Range;
	env->SetFloatArrayRegion(result, 0, 2, CBCHistInfo);

	return result;
}



/*
 * Class:     DecodeSTORM_
 * Method:    lm_SaveColoclizationResult
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_DecodeSTORM_1_lm_1SaveColoclizationResult
(JNIEnv* env, jclass obj, jstring CBCScoreResSave) {
	string CBC_Results_save = jstring2str(env, CBCScoreResSave);
	//CBC_Results_save = CBC_Results_save + ".csv";
	field<std::string> header(CBC_Results.n_cols);
	header(0) = "Percentage of colocalised Ch1 molecules";
	header(1) = "Percentage of colocalised Ch2 molecules";
	CBC_Results.save(csv_name(CBC_Results_save, header));
}