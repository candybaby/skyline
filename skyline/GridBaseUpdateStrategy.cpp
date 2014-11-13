#include "GridBaseUpdateStrategy.h"


GridBaseUpdateStrategy::GridBaseUpdateStrategy(void)
{
}

GridBaseUpdateStrategy::GridBaseUpdateStrategy(Model* model)
	: ISkyline(model)
{
	_maybeList = new CellManager(3, model->GetMaxDimension());
}


GridBaseUpdateStrategy::~GridBaseUpdateStrategy(void)
{
	_updateList.clear();
}

void GridBaseUpdateStrategy::InsertObject(UncertainObject* uObject)
{
	//pruning 條件

	// updateList add A可能dominate的物件的Cell
	vector<vector<int>> temp = _maybeList->GetMaybeDominate(GetMinDim(uObject));
	_needUpdateCell.insert(_needUpdateCell.end(), temp.begin(), temp.end());

	// updateList add A
	vector<Instance*> tempInstances = _maybeList->GetMaybeDominateMe(GetMaxDim(uObject));
	if (tempInstances.size() != 0)
	{
		temp = _maybeList->GetCell(uObject);
		_needUpdateCell.insert(_needUpdateCell.end(), temp.begin(), temp.end());
	}
	
	_maybeList->Insert(uObject);
}

void GridBaseUpdateStrategy::DeleteObject(UncertainObject* uObject)
{
	// updateList add B可能dominate的物件的Cell
	vector<vector<int>> temp = _maybeList->GetMaybeDominate(GetMinDim(uObject));
	_needUpdateCell.insert(_needUpdateCell.end(), temp.begin(), temp.end());
	_maybeList->Delete(uObject);
}

void GridBaseUpdateStrategy::ComputeSkyline()
{
	//_maybeList->Clear();
	//vector<UncertainObject*> tempObjects;
	//tempObjects.insert(tempObjects.end(), _slideWindow.begin(), _slideWindow.end());
	//for (vector<UncertainObject*>::iterator it = _slideWindow.begin(); it < _slideWindow.end(); it++)
	//{
	//	UncertainObject* uObject = *it;
	//	for (vector<UncertainObject*>::iterator it2 = _slideWindow.begin(); it2 < _slideWindow.end(); it2++)
	//	{
	//		UncertainObject* uObject2 = *it2;
	//		if (uObject->GetName() != uObject2->GetName())
	//		{
	//			if (CanDominate(uObject, uObject2)) // uObject dominate uObject2
	//			{
	//				if (find(tempObjects.begin(), tempObjects.end(), uObject2) != tempObjects.end())
	//				{
	//					tempObjects.erase(find(tempObjects.begin(), tempObjects.end(), uObject2));
	//				}
	//			}
	//		}
	//	}
	//}

	//for (vector<UncertainObject*>::iterator it = tempObjects.begin(); it < tempObjects.end(); it++)
	//{
	//	_maybeList->Insert(*it);
	//}
	sort(_needUpdateCell.begin(), _needUpdateCell.end());
	_needUpdateCell.erase(unique(_needUpdateCell.begin(), _needUpdateCell.end()), _needUpdateCell.end());

	_maybeList->ComputeSkyline(_needUpdateCell);
	_needUpdateCell.clear();
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

bool GridBaseUpdateStrategy::CanDominate(UncertainObject* uObject1, UncertainObject* uObject2)
{
	vector<int> uObject1Max = GetMaxDim(uObject1);
	vector<int> uObject2Min = GetMinDim(uObject2);
	Instance* fake = new Instance;
	for (vector<int>::iterator dimIt = uObject1Max.begin(); dimIt < uObject1Max.end(); dimIt++)
	{
		fake->AddDimensionValue(*dimIt);
	}
	Instance* fake2 = new Instance;
	for (vector<int>::iterator dimIt = uObject2Min.begin(); dimIt < uObject2Min.end(); dimIt++)
	{
		fake2->AddDimensionValue(*dimIt);
	}

	bool result = Function::DominateTest(fake, fake2, _dimensions);
	delete fake;
	delete fake2;
	return result;
}