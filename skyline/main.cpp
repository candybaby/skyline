#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include "Instance.h"
#include "FileManager.h"
#include "Function.h"
#include "Model.h"
#include "BruteForce.h"
#include "CandidateListRtree.h"
#include "Config.h"
#include "CellManager.h"
#include "GridBaseUpdateStrategy.h"
#include "UpdateStategyRtree.h"

ofstream resultFile;

double RunUSR(string dataSet, double threshold, int windowSize)
{
	clock_t begin = clock();
	Model* model = new Model;
	model->LoadData(dataSet);
	clock_t end = clock();
	//cout << "Load Data Time elapsed: " << Function::diffclock(end,begin) << " ms"<< endl;

	int dataCount = model->GetSize();
	int testTimestamp = dataCount + windowSize;

	begin = clock();
	UpdateStategyRtree* usr = new UpdateStategyRtree(model);
	usr->SetThreshold(threshold);
	usr->SetWindowSize(windowSize);
	for (int i = 0; i<testTimestamp;i++)
	{
		usr->NextTimestamp();

		/*int cT = usr->GetTimestamp();
		resultFile << "Timestamp: " << cT << endl;
		resultFile << usr->GetSkylineResult() << endl;*/
		//system("pause");
	}
	end = clock();
	delete usr;
	delete model;
	//cout << "USR Time elapsed: " << Function::diffclock(end, begin) << " ms"<< endl;
	return (end - begin);
}

double RunCLRG(string dataSet, double threshold, int windowSize)
{
	clock_t begin = clock();
	Model* model = new Model;
	model->LoadData(dataSet);
	clock_t end = clock();
	//cout << "Load Data Time elapsed: " << Function::diffclock(end,begin) << " ms"<< endl;
	int dataCount = model->GetSize();
	int testTimestamp = dataCount + windowSize;
	begin = clock();

	CandidateListRtree* CLRG = new CandidateListRtree(model, true);
	CLRG->SetThreshold(threshold);
	CLRG->SetWindowSize(windowSize);
	for (int i = 0; i<testTimestamp;i++)
	{
		CLRG->NextTimestamp();

		/*int cT = CLRG->GetTimestamp();
		resultFile << "Timestamp: " << cT << endl;
		resultFile << CLRG->GetSkylineResult() << endl;*/
		//system("pause");
	}
	end = clock();
	delete CLRG;
	delete model;
	//cout << "CL group Time elapsed: " << Function::diffclock(end,begin) << " ms"<< endl;
	return (end - begin);
}

double RunCLR(string dataSet, double threshold, int windowSize)
{
	clock_t begin = clock();
	Model* model = new Model;
	model->LoadData(dataSet);
	clock_t end = clock();
	//cout << "Load Data Time elapsed: " << Function::diffclock(end,begin) << " ms"<< endl;
	int dataCount = model->GetSize();
	int testTimestamp = dataCount + windowSize;


	begin = clock();
	CandidateListRtree* CLR = new CandidateListRtree(model, false);
	CLR->SetThreshold(threshold);
	CLR->SetWindowSize(windowSize);
	for (int i = 0; i<testTimestamp;i++)
	{
		CLR->NextTimestamp();

		/*int cT = CLR->GetTimestamp();
		resultFile << "Timestamp: " << cT << endl;
		resultFile << CLR->GetSkylineResult() << endl;*/
	}
	end = clock();
	delete CLR;
	delete model;
	//cout << "CL normal Time elapsed: " << Function::diffclock(end,begin) << " ms"<< endl;
	return (end - begin);
}

double RunBF(string dataSet, double threshold, int windowSize)
{
	clock_t begin = clock();
	Model* model = new Model;
	model->LoadData(dataSet);
	clock_t end = clock();
	//cout << "Load Data Time elapsed: " << Function::diffclock(end,begin) << " ms"<< endl;
	int dataCount = model->GetSize();
	int testTimestamp = dataCount + windowSize;

	begin = clock();
	BruteForce* bf = new BruteForce(model);
	bf->SetThreshold(threshold);
	bf->SetWindowSize(windowSize);
	for (int i = 0; i<testTimestamp;i++)
	{
		bf->NextTimestamp();

		//int cT = bf->GetTimestamp();
		//resultFile << "Timestamp: " << cT << endl;
		//resultFile << bf->GetSkylineResult() << endl;
	}
	end = clock();
	delete bf;
	delete model;
	//cout << "BF Time elapsed: " << Function::diffclock(end,begin) << " ms"<< endl;
	return (end - begin);
}

double RunGBUS(string dataSet, double threshold, int windowSize)
{
	clock_t begin = clock();
	Model* model = new Model;
	model->LoadData(dataSet);
	clock_t end = clock();
	//cout << "Load Data Time elapsed: " << Function::diffclock(end,begin) << " ms"<< endl;
	int dataCount = model->GetSize();
	int testTimestamp = dataCount + windowSize;

	begin = clock();
	GridBaseUpdateStrategy* gbus = new GridBaseUpdateStrategy(model);
	gbus->SetThreshold(threshold);
	gbus->SetWindowSize(windowSize);
	for (int i = 0; i<testTimestamp;i++)
	{
		gbus->NextTimestamp();

		//int cT = gbus->GetTimestamp();
		//resultFile << "Timestamp: " << cT << endl;
		//resultFile << gbus->GetSkylineResult() << endl;
	}
	end = clock();
	delete gbus;
	delete model;
	//cout << "GBUS Time elapsed: " << Function::diffclock(end,begin) << " ms"<< endl;
	return (end - begin);
}


using namespace std;
int main(int argc, char *argv[])
{
	string dataSet = "";
	int windowSize = 20;
	int testCount = 5;
	string fileName = "";
	if (argc == 5)
	{
		dataSet = argv[1];
		windowSize =  std::stoi(argv[2]);
		testCount =  std::stoi(argv[3]);
		fileName = argv[4];
	}
	else
	{
		cout << "DataSet : ";
		cin >> dataSet;
		system("cls");
		cout << "Window Size : ";
		cin >> windowSize;
		system("cls");
		cout << "Test Count : ";
		cin >> testCount;
		system("cls");
		cout << "Save as (.txt): ";
		cin >> fileName;
		system("cls");
	}
	

	resultFile.open(fileName.append(".txt"));
	
	double _timeUSR = 0;
	double _timeCLR = 0;
	double _timeCLRG = 0;
	double _timeBF = 0;
	double _timeGBUS = 0;

	double threshod[11] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
	//double threshod[1] = {0.9};
	int processingRate = 0;
	double processUnit = (100.0 / 11) / testCount;

	system("cls");
	cout << "Now : " << processingRate << "%" << endl;

	for (int j = 0; j< 11;j++)
	{
		/*cout << "------------------------Setting------------------------" << endl;
		cout << "	DataSet:	" << dataSet << endl;
		cout << "	Dimensions:	" << DIMENSION << endl;
		cout << "	WindowSize:	" << windowSize << endl;
		cout << "	Threshold:	" << threshod[j] << endl;
		cout << "	TestCount:	" << testCount << endl;
		cout << "-------------------------------------------------------" << endl;*/

		resultFile << "------------------------Setting------------------------" << endl;
		resultFile << "	DataSet:	" << dataSet << endl;
		resultFile << "	Dimensions:	" << DIMENSION << endl;
		resultFile << "	WindowSize:	" << windowSize << endl;
		resultFile << "	Threshold:	" << threshod[j] << endl;
		resultFile << "	TestCount:	" << testCount << endl;
		resultFile << "-------------------------------------------------------" << endl;

		for (int i = 0 ;i < testCount; i++)
		{
			_timeUSR += RunUSR(dataSet, threshod[j], windowSize);
			_timeCLR += RunCLR(dataSet, threshod[j], windowSize);
			_timeCLRG += RunCLRG(dataSet, threshod[j], windowSize);
			_timeBF += RunBF(dataSet, threshod[j], windowSize);
			_timeGBUS += RunGBUS(dataSet, threshod[j], windowSize);
			processingRate += processUnit;
			system("cls");
			cout << "Now : " << processingRate << "%" << endl;
		}

		//cout << "USR Average Time : " << _timeUSR / testCount << endl;
		////cout << "CLR Average Time : " << _timeCLR / testCount << endl;
		//cout << "CLRG Average Time : " << _timeCLRG / testCount << endl;
		//cout << "BF Average Time : " << _timeBF / testCount << endl;

		resultFile << "USR Average Time : " << _timeUSR / testCount << endl;
		resultFile << "CLR Average Time : " << _timeCLR / testCount << endl;
		resultFile << "CLRG Average Time : " << _timeCLRG / testCount << endl;
		resultFile << "BF Average Time : " << _timeBF / testCount << endl;
		resultFile << "GBUS Average Time : " << _timeGBUS / testCount << endl;
		
		//cout << "BF Average Time : " << _timeBF / TESTCOUNT << endl;
		_timeUSR = 0;
		_timeCLR = 0;
		_timeCLRG = 0;
		_timeBF = 0;
		_timeGBUS = 0;
	}
	

	resultFile.close();
	//system("pause");
	return 0;
}
