#include"stdafx.h"
#include"DriftCorrGPU.h"

using namespace std;
#define PointNumTh					20480



int main()
{

	clock_t start, end;
	start = clock();

	//FilePathCh1 = "D:\\Desktop\\TubulinAF647_result2D7_M.txt";
	//DriftCorrGPU(FilePathCh1, Chan1PrefixName);

	end = clock();
	cout << "time:" << (double)(end - start) / CLOCKS_PER_SEC << "s" << endl;
	cout << "time:" << (double)clock() / CLOCKS_PER_SEC << "s" << endl;

}