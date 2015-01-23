#include "UncertainObject.h"


UncertainObject::UncertainObject(void)
{
	_isPruned = false;
	_cache = 0;
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
	if (_cache == 0)
	{
		for (vector<Instance*>::iterator it = instances.begin(); it < instances.end(); it++)
		{
			Instance* instance = *it;
			result += instance->GetSkylineProbability() * instance->GetProbability();
		}
		_cache = result;
	}
	else
	{
		result = _cache;
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


void UncertainObject::SetMax(int* max)
{
	_max = max;
}
int* UncertainObject::GetMax()
{
	return _max;
}

void UncertainObject::SetMin(int* min)
{
	_min = min;
}
int* UncertainObject::GetMin()
{
	return _min;
}

void UncertainObject::NeedReCompute()
{
	_cache = 0;
}