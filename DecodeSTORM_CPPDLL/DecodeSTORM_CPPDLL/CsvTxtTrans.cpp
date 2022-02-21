#include"stdafx.h"
#include"CsvTxtTrans.h"

string BinaryTxtCh1Path;
string BinaryTxtCh2Path;
string oFileNameStr;

 //csv transition to binary txt for drift correction
void Csv2TxtBinaryCh1() {
	BinaryTxtCh1Path = Directory + Chan1PrefixName + ".txt";

	mat XYCh1Pix = RoiLoc1.cols(1, 2) / RawImPixSize;

	ofstream  binaryfile(BinaryTxtCh1Path, ios_base::binary | ios_base::trunc);

	for (size_t i = 0; i < RoiLoc1.n_rows; i++)
	{
		float line[12] = { RoiLoc1(i, 0),XYCh1Pix(i, 0),XYCh1Pix(i, 1),RoiLoc1(i, 3),RoiLoc1(i, 4),RoiLoc1(i, 5),
			RoiLoc1(i, 6),RoiLoc1(i, 7),RoiLoc1(i, 8),RoiLoc1(i,9) ,RoiLoc1(i, 10),RoiLoc1(i, 11) };

		for (int j = 0; j < sizeof(line); j++) {
			binaryfile << ((char*)line)[j];

		}
	}
	binaryfile.close();

}

void Csv2TxtBinaryCh2() {
	BinaryTxtCh2Path = Directory + Chan2PrefixName + ".txt";

	mat XYCh2Pix = RoiLoc2.cols(1, 2) / RawImPixSize;

	ofstream  binaryfile(BinaryTxtCh2Path, ios_base::binary | ios_base::trunc);

	for (size_t i = 0; i < RoiLoc2.n_rows; i++)
	{
		float line[12] = { RoiLoc2(i, 0),XYCh2Pix(i, 0),XYCh2Pix(i, 1),RoiLoc2(i, 3),RoiLoc2(i, 4),RoiLoc2(i, 5),
			RoiLoc2(i, 6),RoiLoc2(i, 7),RoiLoc2(i, 8),RoiLoc2(i,9) ,RoiLoc2(i, 10),RoiLoc2(i, 11) };

		for (int j = 0; j < sizeof(line); j++) {
			binaryfile << ((char*)line)[j];
		}

	}
	binaryfile.close();
}

// txt transition to csv txt for save
void TxtBinary2Csv(const string& TxtBinaryFile) {
	int cnt = 0;
	CString CurFile;
	CString oFileName;

	CurFile = TxtBinaryFile.c_str();
	oFileName = CurFile;

	string::size_type iPos = TxtBinaryFile.find_last_of('\\') + 1;
	string FileName = TxtBinaryFile.substr(iPos, TxtBinaryFile.length() - iPos);
	FileName = FileName.substr(0, FileName.rfind("."));
	oFileNameStr = Directory + FileName+"_DecodeSTORM.csv";
	oFileName = oFileNameStr.c_str();

	CFile fraw(CurFile.GetBuffer(), CFile::modeRead);
	CFile fText(oFileName.GetBuffer() , CFile::modeWrite | CFile::modeCreate);

	int FluoNum = fraw.GetLength() / sizeof(float) / OutParaNumGS2D;

	float* pRawData = new float[OutParaNumGS2D];

	char* tbuf = new char[2048];

	sprintf(tbuf, "%s\n", "peak intensity (photon),x (nm),y (nm),z (nm),PSFSigmaX (nm),PSFSigmaY (nm),Total intensity (photon),background (photon),SNR (peak to background e-),Localization precision X(nm),Localization precision Y(nm),frame");

	fText.Write(tbuf, strlen(tbuf));


	for (cnt = 0; cnt < FluoNum; cnt++)
	{
		fraw.Read(pRawData, OutParaNumGS2D * sizeof(float));
		
		sprintf(tbuf, "%.15f,%.15f,%.15f,%.15f,%.15f,%.15f,%.15f,%.15f,%.15f,%.15f,%.15f,%.15f\n", pRawData[0], pRawData[1] * RawImPixSize, pRawData[2] * RawImPixSize, pRawData[3], pRawData[4], pRawData[5] , pRawData[6], pRawData[7], pRawData[8], pRawData[9], pRawData[10], pRawData[11]);

		fText.Write(tbuf, strlen(tbuf));

	}

	delete[] pRawData;
	delete[] tbuf;

	fraw.Close();
	fText.Close();
}

