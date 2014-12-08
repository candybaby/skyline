#include "CandidateListRtree.h"


CandidateListRtree::CandidateListRtree(void)
{
}

CandidateListRtree::CandidateListRtree(Model* model, bool flag)
	: ISkyline(model)
{
	_IsGroup = flag;
}


CandidateListRtree::~CandidateListRtree(void)
{
	_candidateList.clear();
	_skyline.clear();
	_candidateTree.~RStarTree();
}

void CandidateListRtree::InsertObject(UncertainObject* uObject)
{
	BoundingBox mbr = GetMBR(uObject);
	int minDDR[DIMENSION], maxDDR[DIMENSION];
	for (int i=0; i< DIMENSION;i++)
	{
		minDDR[i] = mbr.edges[i].second;
		maxDDR[i] = _maxDimensions.at(i);
	}

	BoundingBox searchDDR = Bounds(minDDR, maxDDR);
	Visitor x;
	x = _candidateTree.Query(RTree::AcceptOverlapping(searchDDR), Visitor());
	// 物件MBR 處於新進物件的DDR(有Overlap)
	vector<int> maxDim, minDim;
	for (int i=0;i<DIMENSION;i++)
	{
		maxDim.push_back(mbr.edges[i].second);
		minDim.push_back(mbr.edges[i].first);
	}

	///*int maxDim[DIMENSION];
	//for (int i=0;i<DIMENSION;i++)
	//{
	//maxDim[i] = mbr.edges[i].second;
	//}*/
	// 可能會影響skyline結果
	vector<UncertainObject*> pruningObjects = PruningMethod(x.uObjects, maxDim, minDim);

	for (vector<UncertainObject*>::iterator It = pruningObjects.begin(); It < pruningObjects.end(); It++)
	{
		UncertainObject* pruningObject = *It;
		_candidateTree.RemoveItem(pruningObject);
		_prunedObject.push_back(pruningObject);
	}

	int minPAR[DIMENSION], maxPAR[DIMENSION];
	for (int i=0; i< DIMENSION;i++)
	{
		minPAR[i] = 0;
		maxPAR[i] = mbr.edges[i].first;
	}

	BoundingBox searchPAR = Bounds(minPAR, maxPAR);
	x = _candidateTree.Query(RTree::AcceptEnclosing(searchPAR), Visitor());
	//cout << x.count << endl;
	if (x.uObjects.size() == 0)
	{
		_candidateTree.Insert(uObject, mbr);
	}
	else
	{
		int tsMax = 0;
		for (vector<UncertainObject*>::iterator It = x.uObjects.begin(); It < x.uObjects.end(); It++)
		{
			// 找timestamp最大的 t 將uObject放到t+w的CL中
			int ts = (*It)->GetTimestamp();
			if (tsMax < ts)
			{
				tsMax = ts;
			}
		}
		int indexCL = tsMax + _slideWindowSize;
		if ( _candidateList.find(indexCL) == _candidateList.end() ) {
			vector<UncertainObject*> temp;
			temp.push_back(uObject);
			_candidateList.insert(pair<int, vector<UncertainObject*>> (indexCL, temp));
		} else {
			vector<UncertainObject*> temp = _candidateList.at(indexCL);
			vector<int> maxDim, minDim;
			for (int i=0;i<DIMENSION;i++)
			{
				maxDim.push_back(mbr.edges[i].second);
				minDim.push_back(mbr.edges[i].first);
			}
			
			vector<UncertainObject*> pruningObjects = PruningMethod(temp, maxDim, minDim);
			for (vector<UncertainObject*>::iterator It = pruningObjects.begin(); It < pruningObjects.end(); It++)
			{
				UncertainObject* pruningObject = *It;
				_candidateList.at(indexCL).erase(find(_candidateList.at(indexCL).begin(), _candidateList.at(indexCL).end(), pruningObject));
				_prunedObject.push_back(pruningObject);
			}
			_candidateList.at(indexCL).push_back(uObject);
		}

	}	
	
}

void CandidateListRtree::DeleteObject(UncertainObject* uObject)
{
	_candidateTree.RemoveItem(uObject);
	if (find(_prunedObject.begin(), _prunedObject.end(),uObject) != _prunedObject.end())
	{
		_prunedObject.erase(find(_prunedObject.begin(), _prunedObject.end(), uObject));
	}
	int indexCL = _currentTimestamp;
	if (_candidateList.find(indexCL) != _candidateList.end())
	{
		vector<UncertainObject*> candidateObjects = _candidateList.at(indexCL);
		for (vector<UncertainObject*>::iterator It = candidateObjects.begin(); It < candidateObjects.end(); It++)
		{
			UncertainObject* candidateObject = *It;
			BoundingBox mbr = GetMBR(candidateObject);
			int minDAR[DIMENSION], maxDAR[DIMENSION];
			for (int i=0; i< DIMENSION;i++)
			{
				minDAR[i] = 0;
				maxDAR[i] = mbr.edges[i].first;
			}
			BoundingBox searchDAR = Bounds(minDAR, maxDAR);
			Visitor x;
			x = _candidateTree.Query(RTree::AcceptEnclosing(searchDAR), Visitor());
			vector<UncertainObject*> dominateCandidateList = x.uObjects;
			if (dominateCandidateList.size() == 0)
			{
				_candidateTree.Insert(candidateObject, mbr);
			}
		}
		_candidateList.erase(indexCL);
	}
		
}


void CandidateListRtree::ComputeSkyline()
{
	if (_IsGroup)
	{
		Group();
	}
	else
	{
		Normal();
	}
}

void CandidateListRtree::Group()
{
	//cout << "Tree Size: " << _candidateTree.GetSize() << endl;
	_skyline.clear();
	Visitor x;
	x = _candidateTree.Query(RTree::AcceptAny(), Visitor());
	_skyline = x.uObjects;

	//ofstream resultFile;
	//resultFile.open("CLR_Count.txt", ios::out | ios::app);
	//resultFile << "Time:\t" << _currentTimestamp << "\tCount:\t" << _skyline.size() << endl;
	//resultFile.close();

	map<string, vector<UncertainObject*>> groups;
	map<string, string> tempMap;
	groups.clear();
	tempMap.clear();


	for (vector<UncertainObject*>::iterator it = x.uObjects.begin(); it < x.uObjects.end(); it++)
	{
		// 清空我被誰Dominate的陣列
		UncertainObject* uObject = *it;
		vector<Instance*> instances = uObject->GetInstances();
		for (vector<Instance*>::iterator instamceIt = instances.begin(); instamceIt < instances.end(); instamceIt++)
		{
			(*instamceIt)->ClearDominateMe();
		}

		// 分群組
		BoundingBox mbr = GetMBR(uObject);

		int minAR[DIMENSION], maxAR[DIMENSION];
		for (int i=0; i< DIMENSION;i++)
		{
			minAR[i] = 0;
			maxAR[i] = mbr.edges[i].second;
		}

		BoundingBox searchAR = Bounds(minAR, maxAR);

		Visitor x;
		x = _candidateTree.Query(RTree::AcceptOverlapping(searchAR), Visitor());

		if (tempMap.find(uObject->GetName()) == tempMap.end()) {
			// not found
			tempMap.insert(pair<string, string> (uObject->GetName(), uObject->GetName()));
			groups.insert(pair<string, vector<UncertainObject*>> (uObject->GetName(), vector<UncertainObject*>()));
			groups.at(uObject->GetName()).push_back(uObject);
		} else {

		}

		for (vector<UncertainObject*>::iterator it1 = x.uObjects.begin(); it1 < x.uObjects.end(); it1++)
		{
			UncertainObject* uObject1 = *it1;
			if (tempMap.find(uObject1->GetName()) == tempMap.end()) {
				// not found 無群組 加到我的群組
				tempMap.insert(pair<string, string> (uObject1->GetName(),tempMap.at(uObject->GetName())));
				groups.at(tempMap.at(uObject->GetName())).push_back(uObject1);
			} else {
				// 有群組 合併群組
				string groupName = tempMap.at(uObject1->GetName());
				string groupName2 = tempMap.at(uObject->GetName());
				if (groupName != groupName2) 
				{
					// 合併group
					for (vector<UncertainObject*>::iterator it2 = groups.at(groupName).begin(); it2 < groups.at(groupName).end(); it2++)
					{
						tempMap.at((*it2)->GetName()) = groupName2;
						groups.at(groupName2).push_back(*it2);
					}
					groups.at(groupName).clear();
					groups.erase(groupName);
				}
			}

		}

	}
	//cout << "group : " << groups.size() << endl;
	//groups.push_back(_skyline);

	for(map<string, vector<UncertainObject*>>::iterator gIt = groups.begin(); gIt != groups.end(); gIt++)
	{
		vector<UncertainObject*> tempSkyline = gIt->second;
		vector<UncertainObject*> tempSkyline2;
		tempSkyline2.insert(tempSkyline2.begin(),tempSkyline.begin(),tempSkyline.end());
		tempSkyline2.insert(tempSkyline2.begin(),_prunedObject.begin(),_prunedObject.end());
		for (vector<UncertainObject*>::iterator it = tempSkyline.begin(); it < tempSkyline.end(); it++)
		{
			UncertainObject* uObject = *it;
			for (vector<UncertainObject*>::iterator it2 = tempSkyline2.begin(); it2 < tempSkyline2.end(); it2++)
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
							if (Function::DominateTest(*instamceIt2, instance, _dimensions))
							{
								instance->AddDominateMeInstance(*instamceIt2);
							}
						}
					}
				}
			}
		}
	}
}

void CandidateListRtree::Normal()
{
	//cout << "Tree Size: " << _candidateTree.GetSize() << endl;
	_skyline.clear();
	Visitor x;
	x = _candidateTree.Query(RTree::AcceptAny(), Visitor());
	_skyline = x.uObjects;

	for (vector<UncertainObject*>::iterator it = _skyline.begin(); it < _skyline.end(); it++)
	{
		// 清空我被誰Dominate的陣列
		UncertainObject* uObject = *it;
		vector<Instance*> instances = uObject->GetInstances();
		for (vector<Instance*>::iterator instamceIt = instances.begin(); instamceIt < instances.end(); instamceIt++)
		{
			(*instamceIt)->ClearDominateMe();
		}
	}

	for (vector<UncertainObject*>::iterator it = _skyline.begin(); it < _skyline.end(); it++)
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
						if (Function::DominateTest(*instamceIt2, instance, _dimensions))
						{
							instance->AddDominateMeInstance(*instamceIt2);
						}
					}
				}
			}
		}
	}	
}

string CandidateListRtree::GetSkylineResult()
{
	string result = "";
	for (vector<UncertainObject*>::iterator it = _skyline.begin(); it < _skyline.end(); it++)
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

BoundingBox CandidateListRtree::Bounds(int min[DIMENSION] , int max[DIMENSION])
{
	BoundingBox bb;

	for(int i = 0; i< DIMENSION; i++)
	{
		bb.edges[i].first  = min[i];
		bb.edges[i].second = max[i];
	}

	return bb;
}


BoundingBox CandidateListRtree::GetMBR(UncertainObject* uObject)
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

vector<UncertainObject*> CandidateListRtree::PruningMethod(vector<UncertainObject*> uObjects, vector<int> maxDim, vector<int> minDim)
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

		Instance* fake1 = new Instance;
		for (int i = 0; i< DIMENSION; i++)
		{
			fake1->AddDimensionValue(bb.edges[i].first);
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
