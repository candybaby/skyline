#include "BruteForce.h"


BruteForce::BruteForce(void)
{
}

BruteForce::BruteForce(Model* model)
	: ISkyline(model)
{

}


BruteForce::~BruteForce(void)
{
}


void BruteForce::InsertObject(UncertainObject *)
{

}
void BruteForce::DeleteObject(UncertainObject *)
{

}

void BruteForce::ComputeSkyline()
{
	for (vector<UncertainObject*>::iterator it = _slideWindow.begin(); it < _slideWindow.end(); it++)
	{
		UncertainObject* uObject = *it;
		vector<Instance*> instances = uObject->GetInstances();
		for (vector<Instance*>::iterator instamceIt = instances.begin(); instamceIt < instances.end(); instamceIt++)
		{
			(*instamceIt)->ClearDominateMe();
		}
	}

	for (vector<UncertainObject*>::iterator it = _slideWindow.begin(); it < _slideWindow.end(); it++)
	{
		UncertainObject* uObject = *it;
		for (vector<UncertainObject*>::iterator it2 = _slideWindow.begin(); it2 < _slideWindow.end(); it2++)
		{
			UncertainObject* uObject2 = *it2;
			if (uObject != uObject2)
			{
				vector<Instance*> instances = uObject->GetInstances();

				for (vector<Instance*>::iterator instamceIt = instances.begin(); instamceIt < instances.end(); instamceIt++)
				{
					vector<Instance*> instances2 = uObject2->GetInstances();
					Instance* instance = *instamceIt;
					for (vector<Instance*>::iterator instamceIt2 = instances2.begin(); instamceIt2 < instances2.end(); instamceIt2++)
					{
						if (Function::DominateTest(instance, *instamceIt2, _dimensions))
						{
							(*instamceIt2)->AddDominateMeInstance(instance);
						}
					}
				}
			}
		}
	}
}

string BruteForce::GetSkylineResult()
{
	string result = "";
	for (vector<UncertainObject*>::iterator it = _slideWindow.begin(); it < _slideWindow.end(); it++)
	{
		string name = (*it)->GetName();
		double pr = (*it)->GetSkylineProbability();
		if (Function::isBiggerEqual(pr , _threshold, OFFSET))
		{
			result.append("Name: ").append(name).append(", Pr: ").append(Function::convertDoubleToString(pr)).append("\n");
		}
	}
	return result;
}