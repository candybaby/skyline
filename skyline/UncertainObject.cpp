#include "UncertainObject.h"


UncertainObject::UncertainObject(void)
{
}


UncertainObject::~UncertainObject(void)
{
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