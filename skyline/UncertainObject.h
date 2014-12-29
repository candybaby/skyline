#pragma once
#ifndef _UNCERTAIN_OBJECT_ 
#define _UNCERTAIN_OBJECT_
#include <vector>
#include "Instance.h"
#include "Config.h"

using namespace std;
class UncertainObject
{
public:
	UncertainObject(void);
	~UncertainObject(void);

	void SetName(string);
	string GetName();

	void SetTimestamp(int);
	int GetTimestamp();

	void AddInstance(Instance*);
	vector<Instance*> GetInstances();

	double GetSkylineProbability();
	void SetPruned(bool);
	bool GetPruned();

	void SetMax(int*);
	int* GetMax();

	void SetMin(int*);
	int* GetMin();

private:
	string name;
	int timestamp;
	vector<Instance*> instances;
	bool _isPruned;
	int* _max;
	int* _min;
};
#endif
