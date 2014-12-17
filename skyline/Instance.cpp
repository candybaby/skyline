#include "Instance.h"


Instance::Instance(void)
{
}


Instance::~Instance(void)
{
}

void Instance::SetName(string value)
{
	_name = value;
}

string Instance::GetName()
{
	return _name;
}

void Instance::SetObjectName(string value)
{
	_objectName = value;
}

string Instance::GetObjectName()
{
	return _objectName;
}

void Instance::SetProbability(double value)
{
	_probability = value;
}

double Instance::GetProbability()
{
	return _probability;
}

void Instance::AddDimensionValue(int value)
{
	_dimension.push_back(value);
}

vector<int> Instance::GetDimensions()
{
	return _dimension;
}

void Instance::AddDominateMeInstance(Instance* instance)
{
	_dominateMeList.push_back(instance);
}

void Instance::RemoveDominateMeInstance(Instance* instance)
{
	_dominateMeList.erase(remove(_dominateMeList.begin(), _dominateMeList.end(), instance), _dominateMeList.end());
	//_dominateMeList.erase(find(_dominateMeList.begin(), _dominateMeList.end(), instance));
}

void Instance::ClearDominateMe()
{
	_dominateMeList.clear();
}

double Instance::GetSkylineProbability()
{
	map<string, double> temp;
	for (vector<Instance*>::iterator it = _dominateMeList.begin(); it < _dominateMeList.end(); it++)
	{
		Instance* instance = *it;
		string objectName = instance->GetObjectName();
		if (temp.find(objectName) == temp.end()) {
			temp.insert(pair<std::string, double> (objectName, instance->GetProbability()));
		} else {
			temp.at(objectName) += instance->GetProbability();
		}
	}
	double result = 1;

	typedef map<string, double>::iterator it_type;
	for(it_type iterator = temp.begin(); iterator != temp.end(); iterator++) 
	{
		result *= (1 - iterator->second);
	}

	return result;
}

void Instance::SetCellId(vector<int> cellId)
{
	_cellId = cellId;
}

vector<int> Instance::GetCellId()
{
	return _cellId;
}