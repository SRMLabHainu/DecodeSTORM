#pragma once
#include <armadillo>
#include"GetLocAndRoi.h"
#include"CommonPara.h"


extern string BinaryTxtCh1Path;
extern string BinaryTxtCh2Path;
extern string oFileNameStr;

void Csv2TxtBinaryCh1();
void Csv2TxtBinaryCh2();
void TxtBinary2Csv(const string& TxtBinaryFile);
