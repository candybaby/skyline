#include "UpdateStategyRtree.h"


UpdateStategyRtree::UpdateStategyRtree(void)
{
}

UpdateStategyRtree::UpdateStategyRtree(Model* model)
	: ISkyline(model)
{

}

UpdateStategyRtree::~UpdateStategyRtree(void)
{
	_maybeTree.~RStarTree();
	_updateList.clear();
}

void UpdateStategyRtree::InsertObject(UncertainObject* uObject)
{
	BoundingBox mbr = GetMBR(uObject);
	int minDDR[DIMENSION], maxDDR[DIMENSION];
	for (int i=0; i< DIMENSION;i++)
	{
		minDDR[i] = mbr.edges[i].second;
		maxDDR[i] = _maxDimensions.at(i);
	}

	BoundingBox searchDDR = Bounds(minDDR, maxDDR);
	UVisitorWithoutPruned x;
	x = _maybeTree.Query(RTree::AcceptOverlapping(searchDDR), UVisitorWithoutPruned());
	// 物件MBR 處於新進物件的DDR(有Overlap)
	vector<int> maxDim, minDim;
	for (int i=0;i<DIMENSION;i++)
	{
		maxDim.push_back(mbr.edges[i].second);
		minDim.push_back(mbr.edges[i].first);
	}

	vector<UncertainObject*> pruningObjects = PruningMethod(x.uObjects, maxDim, minDim);

	for (vector<UncertainObject*>::iterator It = pruningObjects.begin(); It < pruningObjects.end(); It++)
	{
		UncertainObject* pruningObject = *It;
		pruningObject->SetPruned(true);
		//_maybeTree.RemoveItem(pruningObject);
	}
	///////////////////////////////////////////////////////////////////////////////////
	int minDAR[DIMENSION], maxDAR[DIMENSION];
	for (int i=0; i< DIMENSION;i++)
	{
		minDAR[i] = mbr.edges[i].first;
		maxDAR[i] = _maxDimensions.at(i);
	}

	BoundingBox searchDAR = Bounds(minDAR, maxDAR);
	x = _maybeTree.Query(RTree::AcceptOverlapping(searchDAR), UVisitorWithoutPruned());
	_updateList.insert(_updateList.end(), x.uObjects.begin(), x.uObjects.end());
	////////////////////////////////////////////////////////////////////////////////////

	int minPAR[DIMENSION], maxPAR[DIMENSION];
	for (int i=0; i< DIMENSION;i++)
	{
		minPAR[i] = 0;
		maxPAR[i] = mbr.edges[i].second;
	}

	BoundingBox searchPAR = Bounds(minPAR, maxPAR);
	x = _maybeTree.Query(RTree::AcceptOverlapping(searchPAR), UVisitorWithoutPruned());
	if (x.count != 0)
	{
		_updateList.push_back(uObject);
	}
	//////////////////////////////////////////////////////////////////////////////////////
	_maybeTree.Insert(uObject, mbr);
}

void UpdateStategyRtree::DeleteObject(UncertainObject* uObject)
{
	BoundingBox mbr = GetMBR(uObject);
	int minDAR[DIMENSION], maxDAR[DIMENSION];
	for (int i=0; i< DIMENSION;i++)
	{
		minDAR[i] = mbr.edges[i].first;
		maxDAR[i] = _maxDimensions.at(i);
	}

	BoundingBox searchDAR = Bounds(minDAR, maxDAR);
	UVisitorWithoutPruned x;
	x = _maybeTree.Query(RTree::AcceptOverlapping(searchDAR), UVisitorWithoutPruned());
	_updateList.insert(_updateList.end(), x.uObjects.begin(), x.uObjects.end());
	///////////////////////////////////////////////////////////////////////////////////////
	_maybeTree.RemoveItem(uObject);
}

void UpdateStategyRtree::ComputeSkyline()
{
	sort(_updateList.begin(), _updateList.end());
	_updateList.erase(unique(_updateList.begin(), _updateList.end()), _updateList.end());
	for (vector<UncertainObject*>::iterator it = _updateList.begin(); it < _updateList.end(); it++)
	{
		UncertainObject* uObject = *it;
		//////////////////////////////////////////////////////////////////////
		BoundingBox mbr = GetMBR(uObject);
		int minPAR[DIMENSION], maxPAR[DIMENSION];
		for (int i=0; i< DIMENSION;i++)
		{
			minPAR[i] = 0;
			maxPAR[i] = mbr.edges[i].second;
		}
		UVisitor x;
		BoundingBox searchPAR = Bounds(minPAR, maxPAR);
		x = _maybeTree.Query(RTree::AcceptOverlapping(searchPAR), UVisitor());
		//////////////////////////////////////////////////////////////////////
		vector<UncertainObject*> maybeDominateMeList = x.uObjects;
		vector<Instance*> instances = uObject->GetInstances();
		for (vector<Instance*>::iterator instamceIt = instances.begin(); instamceIt < instances.end(); instamceIt++)
		{
			// 清空我被誰Dominate的陣列
			Instance* instance = *instamceIt;
			(*instamceIt)->ClearDominateMe();
			for (vector<UncertainObject*>::iterator it2 = maybeDominateMeList.begin(); it2 < maybeDominateMeList.end(); it2++)
			{
				UncertainObject* uObject2 = *it2;
				if (uObject != uObject2)
				{
					vector<Instance*> instances2 = uObject2->GetInstances();
					
					for (vector<Instance*>::iterator instamceIt2 = instances2.begin(); instamceIt2 < instances2.end(); instamceIt2++)
					{
						if (Function::DominateTest(*instamceIt2, instance, _dimensions))
						{
								instance->AddDominateMeInstance(*instamceIt2);
						}
					}
				}
			}
		}
		
		
	}
	
	/*ofstream resultFile;
	resultFile.open("Mine_Update_Count.txt", ios::out | ios::app);
	resultFile << "Time:\t" << _currentTimestamp << "\tCount:\t" << _updateList.size() << "\tTreeSize:\t" << _maybeTree.GetSize() << endl;
	resultFile.close();*/
	
	_updateList.clear();
}

string UpdateStategyRtree::GetSkylineResult()
{
	string result = "";
	UVisitorWithoutPruned x;
	x = _maybeTree.Query(RTree::AcceptAny(), UVisitorWithoutPruned());
	vector<UncertainObject*> temp = x.uObjects;
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


BoundingBox UpdateStategyRtree::Bounds(int min[DIMENSION] , int max[DIMENSION])
{
	BoundingBox bb;

	for(int i = 0; i< DIMENSION; i++)
	{
		bb.edges[i].first  = min[i];
		bb.edges[i].second = max[i];
	}

	return bb;
}


BoundingBox UpdateStategyRtree::GetMBR(UncertainObject* uObject)
{
	int min[DIMENSION];
	int max[DIMENSION];
	for (int i=0; i< DIMENSION;i++)
	{
		min[i] = -1;
		max[i] = -1;
	}
	vector<Instance*> instances = uObject->GetInstances();
	for (vector<Instance*>::iterator instamceIt = instances.begin(); instamceIt < instances.end(); instamceIt++)
	{
		vector<int> dimensions = (*instamceIt)->GetDimensions();
		for (int i=0; i< DIMENSION;i++)
		{
			if (min[i] == -1 || min[i] > dimensions.at(i))
			{
				min[i] = dimensions.at(i);
			}
			if (max[i] == -1 || max[i] < dimensions.at(i))
			{
				max[i] = dimensions.at(i);
			}

		}
	}
	return Bounds(min, max);
}

vector<UncertainObject*> UpdateStategyRtree::PruningMethod(vector<UncertainObject*> uObjects, vector<int> maxDim, vector<int> minDim)
{
	Instance* fake = new Instance;
	for (vector<int>::iterator dimIt = maxDim.begin(); dimIt < maxDim.end(); dimIt++)
	{
		fake->AddDimensionValue(*dimIt);
	}

	vector<UncertainObject*> result;
	for (vector<UncertainObject*>::iterator It = uObjects.begin(); It < uObjects.end(); It++)
	{
		UncertainObject* targetObject = *It;
		bool needCompute = true;
		BoundingBox bb = GetMBR(targetObject);
		for (int i=0; i< DIMENSION;i++)
		{
			if (bb.edges[i].first < minDim.at(i))
			{
				needCompute = false;
			}
		}
		if (needCompute)
		{
			vector<Instance*> targetInstances = targetObject->GetInstances();
			double tempProbability = 0;
			for (vector<Instance*>::iterator instancesIt = targetInstances.begin(); instancesIt < targetInstances.end(); instancesIt++)
			{
				Instance* targetInstance = *instancesIt;

				if (Function::DominateTest(fake, targetInstance, _dimensions))
				{
					tempProbability += targetInstance->GetProbability();
				}
			}
			// targetObject 無法成為skyline
			if (Function::isBigger(tempProbability , 1 - _threshold, OFFSET))
			{
				result.push_back(targetObject);
			}
		}
	}
	delete fake;
	return result;
}

