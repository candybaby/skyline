#include "UpdateStategyRtree.h"


UpdateStategyRtree::UpdateStategyRtree(void)
{
}

UpdateStategyRtree::UpdateStategyRtree(Model* model)
	: ISkyline(model)
{
	_updateCount = 0;
	_prunedCount = 0;
}

UpdateStategyRtree::~UpdateStategyRtree(void)
{
	_maybeTree.~RStarTree();
}

void UpdateStategyRtree::InsertObject(UncertainObject* uObject)
{

	BoundingBox mbr = GetMBR(uObject);
	int* mbr_min;
	int* mbr_max;
	mbr_min = new int[DIMENSION];
	mbr_max = new int[DIMENSION];
	int minDDR[DIMENSION], maxDDR[DIMENSION];
	for (int i=0; i< DIMENSION;i++)
	{
		minDDR[i] = mbr.edges[i].second;
		maxDDR[i] = _maxDimensions.at(i);
		mbr_min[i] = mbr.edges[i].first;
		mbr_max[i] = mbr.edges[i].second;
	}
	uObject->SetMin(mbr_min);
	uObject->SetMax(mbr_max);

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
		_prunedCount++;
		//_maybeTree.RemoveItem(pruningObject);
	}
	///////////////////////////////////////////////////////////////////////////////////
	int minPDR[DIMENSION], maxPDR[DIMENSION];
	for (int i=0; i< DIMENSION;i++)
	{
		minPDR[i] = mbr.edges[i].first;
		maxPDR[i] = _maxDimensions.at(i);
	}

	BoundingBox searchPDR = Bounds(minPDR, maxPDR);
	x = _maybeTree.Query(RTree::AcceptOverlapping(searchPDR), UVisitorWithoutPruned());
	//_updateList.insert(_updateList.end(), x.uObjects.begin(), x.uObjects.end());
	// fix
	for (vector<UncertainObject*>::iterator it = x.uObjects.begin(); it < x.uObjects.end(); it++)
	{
		_updateCount++;
		UncertainObject* uObjectTest = *it;
		vector<Instance*> instances = uObjectTest->GetInstances();
		vector<Instance*> instancesEnter = uObject->GetInstances();
		for (vector<Instance*>::iterator instamceIt = instances.begin(); instamceIt < instances.end(); instamceIt++)
		{
			Instance* instance = *instamceIt;
			for (vector<Instance*>::iterator instamceIt2 = instancesEnter.begin(); instamceIt2 < instancesEnter.end(); instamceIt2++)
			{
				if (Function::DominateTest(*instamceIt2, instance, _dimensions))
				{
					instance->AddDominateMeInstance(*instamceIt2);
				}
			}
		}
		uObjectTest->NeedReCompute();
	}
	// fix end
	////////////////////////////////////////////////////////////////////////////////////

	int minPAR[DIMENSION], maxPAR[DIMENSION];
	for (int i=0; i< DIMENSION;i++)
	{
		minPAR[i] = 0;
		maxPAR[i] = mbr.edges[i].second;
	}

	BoundingBox searchPAR = Bounds(minPAR, maxPAR);
	UVisitor y;
	y = _maybeTree.Query(RTree::AcceptOverlapping(searchPAR), UVisitor());
	if (y.count != 0)
	{
		//_updateList.push_back(uObject);
		// fix
		//_updateCount++;
		for (vector<UncertainObject*>::iterator it2 = y.uObjects.begin(); it2 < y.uObjects.end(); it2++)
		{
			_updateCount++;
			vector<Instance*> instances = uObject->GetInstances();
			UncertainObject* uObject2 = *it2;
			//if (Function::FullDominateTest(uObject2, uObject, _dimensions)) // 2 dominate 1
			BoundingBox mbr = GetMBR(uObject);
			Instance* fake1 = new Instance;
			for (int i = 0; i< DIMENSION; i++)
			{
				fake1->AddDimensionValue(mbr.edges[i].first);
			}

			BoundingBox mbr2 = GetMBR(uObject2);
			Instance* fake2 = new Instance;
			for (int i = 0; i< DIMENSION; i++)
			{
				fake2->AddDimensionValue(mbr2.edges[i].second);
			}
			if (Function::DominateTest(fake2, fake1, _dimensions)) // 2 dominate 1
			{
				for (vector<Instance*>::iterator instamceIt = instances.begin(); instamceIt < instances.end(); instamceIt++)
				{
					Instance* instance = *instamceIt;
					vector<Instance*> instances2 = uObject2->GetInstances();

					for (vector<Instance*>::iterator instamceIt2 = instances2.begin(); instamceIt2 < instances2.end(); instamceIt2++)
					{
						instance->AddDominateMeInstance(*instamceIt2);
					}
				}
			}
			else
			{
				for (vector<Instance*>::iterator instamceIt = instances.begin(); instamceIt < instances.end(); instamceIt++)
				{
					Instance* instance = *instamceIt;
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

			delete fake1;
			delete fake2;
		}
		
		/*vector<Instance*> instances = uObject->GetInstances();
		for (vector<Instance*>::iterator instamceIt = instances.begin(); instamceIt < instances.end(); instamceIt++)
		{
			Instance* instance = *instamceIt;
			for (vector<UncertainObject*>::iterator it2 = y.uObjects.begin(); it2 < y.uObjects.end(); it2++)
			{
				UncertainObject* uObject2 = *it2;
				vector<Instance*> instances2 = uObject2->GetInstances();

				for (vector<Instance*>::iterator instamceIt2 = instances2.begin(); instamceIt2 < instances2.end(); instamceIt2++)
				{
					if (Function::DominateTest(*instamceIt2, instance, _dimensions))
					{
						instance->AddDominateMeInstance(*instamceIt2);
					}
				}
				
			}
		}*/
		// fix end
	}
	//////////////////////////////////////////////////////////////////////////////////////
	_maybeTree.Insert(uObject, mbr);
}

void UpdateStategyRtree::DeleteObject(UncertainObject* uObject)
{
	BoundingBox mbr = GetMBR(uObject);
	int minPDR[DIMENSION], maxPDR[DIMENSION];
	for (int i=0; i< DIMENSION;i++)
	{
		minPDR[i] = mbr.edges[i].first;
		maxPDR[i] = _maxDimensions.at(i);
	}

	BoundingBox searchPDR = Bounds(minPDR, maxPDR);
	UVisitorWithoutPruned x;
	x = _maybeTree.Query(RTree::AcceptOverlapping(searchPDR), UVisitorWithoutPruned());
	//_updateList.insert(_updateList.end(), x.uObjects.begin(), x.uObjects.end());
	// fix
	for (vector<UncertainObject*>::iterator it = x.uObjects.begin(); it < x.uObjects.end(); it++)
	{
		_updateCount++;
		UncertainObject* uObjectTest = *it;
		vector<Instance*> instances = uObjectTest->GetInstances();
		vector<Instance*> instancesLeaves = uObject->GetInstances();
		for (vector<Instance*>::iterator instamceIt = instances.begin(); instamceIt < instances.end(); instamceIt++)
		{
			Instance* instance = *instamceIt;
			
			for (vector<Instance*>::iterator instamceIt2 = instancesLeaves.begin(); instamceIt2 < instancesLeaves.end(); instamceIt2++)
			{
				instance->RemoveDominateMeInstance(*instamceIt2);
			}
		}
		uObjectTest->NeedReCompute();
	}
	// fix end
	///////////////////////////////////////////////////////////////////////////////////////
	_maybeTree.RemoveItem(uObject);
}

void UpdateStategyRtree::ComputeSkyline()
{
	/*
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
	
	//ofstream resultFile;
	//resultFile.open("Mine_Update_Count.txt", ios::out | ios::app);
	//resultFile << "Time:\t" << _currentTimestamp << "\tCount:\t" << _updateList.size() << "\tTreeSize:\t" << _maybeTree.GetSize() << endl;
	//resultFile.close();
	
	_updateList.clear();
	*/
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
		//BoundingBox bb = GetMBR(targetObject);

		Instance* fake1 = new Instance;
		for (int i = 0; i< DIMENSION; i++)
		{
			fake1->AddDimensionValue(targetObject->GetMin()[i]);
		}

		if (Function::DominateTest(fake, fake1, _dimensions))
		{
			needCompute = false;
			result.push_back(targetObject);
		}
		else
		{
			for (int i = 0; i< DIMENSION; i++)
			{
				if (targetObject->GetMin()[i] < minDim.at(i))
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
				if (Function::isBigger(tempProbability, 1 - _threshold, OFFSET))
				{
					result.push_back(targetObject);
				}
			}
		}
	}
	delete fake;
	return result;
}

int UpdateStategyRtree::GetSkylineCount()
{
	int result = 0;
	UVisitorWithoutPruned x;
	x = _maybeTree.Query(RTree::AcceptAny(), UVisitorWithoutPruned());
	vector<UncertainObject*> temp = x.uObjects;
	for (vector<UncertainObject*>::iterator it = temp.begin(); it < temp.end(); it++)
	{
		double pr = (*it)->GetSkylineProbability();
		if (Function::isBiggerEqual(pr, _threshold, OFFSET))
		{
			result++;
		}
	}
	return result;
}

int UpdateStategyRtree::GetUpdateCount()
{
	int result = _updateCount;
	_updateCount = 0;
	return result;
}

int UpdateStategyRtree::GetPrunedCount()
{
	int result = _prunedCount;
	_prunedCount = 0;
	return result;
}