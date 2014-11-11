#include "UncertainObject.h"


UncertainObject::UncertainObject(void)
{
	_isPruned = false;
}


UncertainObject::~UncertainObject(void)
{
	while (instances.size() > 0)
	{
		Instance* delData = instances.back();
		instances.pop_back();
		delete delData;
	}
}

void UncertainObject::SetName(string value)
{
	name = value;
}

string UncertainObject::GetName()
{
	return name;
}

void UncertainObject::SetTimestamp(int value)
{
	timestamp = value;
}

int UncertainObject::GetTimestamp()
{
	return timestamp;
}

void UncertainObject::AddInstance(Instance* value)
{
	instances.push_back(value);
}

vector<Instance*> UncertainObject::GetInstances()
{
	return instances;
}

double UncertainObject::GetSkylineProbability()
{
	double result = 0;
	for (vector<Instance*>::iterator it = instances.begin(); it < instances.end(); it++)
	{
		Instance* instance = *it;
		result += instance->GetSkylineProbability() * instance->GetProbability(); 
	}
	return result;
}

void UncertainObject::SetPruned(bool flag)
{
	_isPruned = flag;
}

bool UncertainObject::GetPruned()
{
	return _isPruned;
}