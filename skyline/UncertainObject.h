#pragma once
#ifndef _UNCERTAIN_OBJECT_ 
#define _UNCERTAIN_OBJECT_
#include <vector>
#include "Instance.h"

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

private:
	string name;
	int timestamp;
	vector<Instance*> instances;
	bool _isPruned;
};
#endif
