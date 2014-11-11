#include "GridBaseUpdateStrategy.h"


GridBaseUpdateStrategy::GridBaseUpdateStrategy(void)
{
}

GridBaseUpdateStrategy::GridBaseUpdateStrategy(Model* model)
	: ISkyline(model)
{
	_maybeList = new CellManager(4, model->GetMaxDimension());
}


GridBaseUpdateStrategy::~GridBaseUpdateStrategy(void)
{
	_updateList.clear();
}

void GridBaseUpdateStrategy::InsertObject(UncertainObject* uObject)
{
	vector<UncertainObject*> canPruningObject = GetCanPruningObject(_maybeList->GetCompleteDominate(GetMaxDim(uObject)));

	for (vector<UncertainObject*>::iterator it = canPruningObject.begin(); it < canPruningObject.end(); it++)
	{
		UncertainObject* temp = (*it);
		_maybeList->Delete(temp);
	}

	vector<Instance*> temp = _maybeList->GetMaybeDominate(GetMinDim(uObject));
	_updateList.insert(_updateList.end(), temp.begin(), temp.end());

	

	temp = _maybeList->GetMaybeDominateMe(GetMaxDim(uObject));
	//_updateList.insert(_updateList.end(), temp.begin(), temp.end());

	if (temp.size() != 0)
	{
		vector<Instance*> temp1 = uObject->GetInstances();
		_updateList.insert(_updateList.end(), temp1.begin(), temp1.end());
	}

	_maybeList->Insert(uObject);
}

void GridBaseUpdateStrategy::DeleteObject(UncertainObject* uObject)
{
	vector<Instance*> temp = _maybeList->GetMaybeDominate(GetMinDim(uObject));
	_updateList.insert(_updateList.end(), temp.begin(), temp.end());
	_maybeList->Delete(uObject);
	
}

void GridBaseUpdateStrategy::ComputeSkyline()
{
	sort(_updateList.begin(), _updateList.end());
	_updateList.erase(unique(_updateList.begin(), _updateList.end()), _updateList.end());
	for (vector<Instance*>::iterator instamceIt = _updateList.begin(); instamceIt < _updateList.end(); instamceIt++)
	{
		Instance* instance = *instamceIt;
		instance->ClearDominateMe();
		vector<Instance*> needDominateTestList = _maybeList->GetMaybeDominateMe(instance->GetDimensions());
		
		for (vector<Instance*>::iterator instamceIt2 = needDominateTestList.begin(); instamceIt2 < needDominateTestList.end(); instamceIt2++)
		{
			if (instance->GetObjectName() != (*instamceIt2)->GetObjectName())
			{
				if (Function::DominateTest(*instamceIt2, instance, _dimensions))
				{
					instance->AddDominateMeInstance(*instamceIt2);
				}
			}
		}
	}
	_updateList.clear();
}

string GridBaseUpdateStrategy::GetSkylineResult()
{
	string result = "";
	vector<UncertainObject*> temp = _maybeList->GetObjects();
	for (vector<UncertainObject*>::iterator it = temp.begin(); it < temp.end(); it++)
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

vector<int> GridBaseUpdateStrategy::GetMaxDim(UncertainObject* uObject)
{
	vector<int> maxDim;
	for (int i=0; i< DIMENSION;i++)
	{
		maxDim.push_back(-1);
	}

	vector<Instance*> instances = uObject->GetInstances();
	for (vector<Instance*>::iterator instamceIt = instances.begin(); instamceIt < instances.end(); instamceIt++)
	{
		vector<int> dimensions = (*instamceIt)->GetDimensions();
		for (int i=0; i< DIMENSION;i++)
		{
			if (maxDim.at(i) == -1 || maxDim.at(i) < dimensions.at(i))
			{
				maxDim.at(i) = dimensions.at(i);
			}

		}
	}
	return maxDim;
}

vector<int> GridBaseUpdateStrategy::GetMinDim(UncertainObject* uObject)
{
	vector<int> minDim;
	for (int i=0; i< DIMENSION;i++)
	{
		minDim.push_back(-1);
	}

	vector<Instance*> instances = uObject->GetInstances();
	for (vector<Instance*>::iterator instamceIt = instances.begin(); instamceIt < instances.end(); instamceIt++)
	{
		vector<int> dimensions = (*instamceIt)->GetDimensions();
		for (int i=0; i< DIMENSION;i++)
		{
			if (minDim.at(i) == -1 || minDim.at(i) > dimensions.at(i))
			{
				minDim.at(i) = dimensions.at(i);
			}

		}
	}
	return minDim;
}

vector<UncertainObject*> GridBaseUpdateStrategy::GetCanPruningObject(vector<Instance*> instances)
{
	map<string, double> temp;
	for (vector<Instance*>::iterator instamceIt = instances.begin(); instamceIt < instances.end(); instamceIt++)
	{
		Instance* instance = (*instamceIt);
		if (temp.find(instance->GetObjectName()) == temp.end()) {
			// not found
			temp.insert(pair<string, double> (instance->GetObjectName(), instance->GetProbability()));
		} else {
			temp.at(instance->GetObjectName()) += instance->GetProbability();
		}
	}
	vector<UncertainObject*> result;
	for(map<string, double>::iterator tIt = temp.begin(); tIt != temp.end(); tIt++)
	{
		if (Function::isBigger(tIt->second , 1 - _threshold, OFFSET))
		{
			result.push_back(_model->GetObjectByName(tIt->first));
		}
	}
	return result;
}