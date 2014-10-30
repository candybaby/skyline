#pragma once
#ifndef _INSTANCE_ 
#define _INSTANCE_
#include <vector>
#include <map>
#include <string>

using namespace std;
class Instance
{
public:
	Instance(void);
	~Instance(void);

	void SetName(string);
	string GetName();
	void SetObjectName(string);
	string GetObjectName();

	void SetProbability(double);
	double GetProbability();

	void AddDimensionValue(int);
	vector<int> GetDimensions();

	void AddDominateMeInstance(Instance*);
	void ClearDominateMe();
	double GetSkylineProbability();
	

private:
	string _name;
	string _objectName;
	double _probability;
	vector<int> _dimension;
	vector<Instance*> _dominateMeList;
};
#endif
