#pragma once
#include <armadillo>
#include"OpenConsole.h"
#include"ContainerTrans.h"
#include"CommonPara.h"



using namespace std;
using namespace arma;

extern int MinPts;
extern float Eps, EpsAutoCalc;
extern int* DBSCAN_ImageArray;
extern int DBSCAN_ImageW, DBSCAN_ImageH, ClusterNum;
extern mat DBSCAN_Res;

void AutoCalcEpsDBSCAN(const mat& inLoc);
void DBSCAN(const mat& inLoc);