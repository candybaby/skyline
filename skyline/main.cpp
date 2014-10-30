#include <iostream>
#include <string>
#include <time.h>
#include "Instance.h"
#include "FileManager.h"
#include "Function.h"
#include "Model.h"
#include "BruteForce.h"
#include "CandidateListRtree.h"
#include "Config.h"


using namespace std;
int main(int argc, char *argv[])
{
	cout << "------------------------Setting------------------------" << endl;
	cout << "	DataSet:	" << DATASET << endl;
	cout << "	Dimensions:	" << DIMENSION << endl;
	cout << "	WindowSize:	" << SLIDE_WINDOW_SIZE << endl;
	cout << "	Threshold:	" << THRESHOLD << endl;
	cout << "	TestCount:	" << TESTCOUNT << endl;
	cout << "-------------------------------------------------------" << endl;


	Model* model = new Model;
	model->LoadData();
	int dataCount = model->GetSize();
	int testTimestamp = dataCount + SLIDE_WINDOW_SIZE;
	
	BruteForce* bf = new BruteForce(model);

	clock_t begin = clock();
	for (int i = 0; i<testTimestamp;i++)
	{
		bf->NextTimestamp();

		//int cT = bf->GetTimestamp();
		//cout << "Timestamp: " << cT << ", Object: " << model->GetObjectByTimestamp(cT)->GetName() << endl;
		//cout << bf->GetSkylineResult() << endl;
	}
	clock_t end = clock();
	cout << "BF Time elapsed: " << Function::diffclock(end,begin) << " ms"<< endl;

	
	CandidateListRtree* CLR = new CandidateListRtree(model);

	begin = clock();
	for (int i = 0; i<testTimestamp;i++)
	{
		CLR->NextTimestamp();

		//int cT = CLR->GetTimestamp();
		//cout << "Timestamp: " << cT << endl;
		//cout << CLR->GetSkylineResult() << endl;
	}
	end = clock();
	cout << "CL Time elapsed: " << Function::diffclock(end,begin) << " ms"<< endl;

	system("pause");
	return 0;
}


