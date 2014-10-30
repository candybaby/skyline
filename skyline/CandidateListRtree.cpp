#include "CandidateListRtree.h"


CandidateListRtree::CandidateListRtree(void)
{
}

CandidateListRtree::CandidateListRtree(Model* model)
	: ISkyline(model)
{
}


CandidateListRtree::~CandidateListRtree(void)
{
}

void CandidateListRtree::InsertObject(UncertainObject* uObject)
{
	BoundingBox mbr = GetMBR(uObject);
	BoundingBox searchDDR = Bounds(mbr.edges[0].second, mbr.edges[1].second, 100, 100);
	Visitor x;
	x = _candidateTree.Query(RTree::AcceptOverlapping(searchDDR), Visitor());
	// 物件MBR 處於新進物件的DDR(有Overlap)
	vector<int> maxDim;
	maxDim.push_back(mbr.edges[0].second);
	maxDim.push_back(mbr.edges[1].second);

	vector<UncertainObject*> pruningObjects = PruningMethod(x.uObjects, maxDim);
	for (vector<UncertainObject*>::iterator It = pruningObjects.begin(); It < pruningObjects.end(); It++)
	{
		UncertainObject* pruningObject = *It;
		_candidateTree.RemoveItem(pruningObject);
	}

	BoundingBox searchDAR = Bounds(0, 0, mbr.edges[0].first, mbr.edges[1].first);
	x = _candidateTree.Query(RTree::AcceptEnclosing(searchDAR), Visitor());
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
			vector<int> maxDim;
			maxDim.push_back(mbr.edges[0].second);
			maxDim.push_back(mbr.edges[1].second);
			vector<UncertainObject*> pruningObjects = PruningMethod(temp, maxDim);
			for (vector<UncertainObject*>::iterator It = pruningObjects.begin(); It < pruningObjects.end(); It++)
			{
				UncertainObject* pruningObject = *It;
				_candidateList.at(indexCL).erase(find(_candidateList.at(indexCL).begin(), _candidateList.at(indexCL).end(), pruningObject));
			}
			_candidateList.at(indexCL).push_back(uObject);
		}

	}	
	
}

void CandidateListRtree::DeleteObject(UncertainObject* uObject)
{
	_candidateTree.RemoveItem(uObject);
	int indexCL = _currentTimestamp;
	if (_candidateList.find(indexCL) != _candidateList.end())
	{
		vector<UncertainObject*> candidateObjects = _candidateList.at(indexCL);
		for (vector<UncertainObject*>::iterator It = candidateObjects.begin(); It < candidateObjects.end(); It++)
		{
			UncertainObject* candidateObject = *It;
			BoundingBox mbr = GetMBR(candidateObject);
			BoundingBox searchDAR = Bounds(0, 0, mbr.edges[0].first, mbr.edges[1].first);
			vector<UncertainObject*> dominateCandidateList = GetEnclosingObject(searchDAR);
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
	//cout << "Tree Size: " << _candidateTree.GetSize() << endl;
	_skyline.clear();
	Visitor x;
	x = _candidateTree.Query(RTree::AcceptAny(), Visitor());
	_skyline = x.uObjects;

	for (vector<UncertainObject*>::iterator it = _skyline.begin(); it < _skyline.end(); it++)
	{
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
		for (vector<UncertainObject*>::iterator it2 = _skyline.begin(); it2 < _skyline.end(); it2++)
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

string CandidateListRtree::GetSkylineResult()
{
	string result = "";
	for (vector<UncertainObject*>::iterator it = _skyline.begin(); it < _skyline.end(); it++)
	{
		string name = (*it)->GetName();
		double pr = (*it)->GetSkylineProbability();
		if (pr > _threshold)
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


BoundingBox CandidateListRtree::Bounds(int minX, int minY, int maxX, int maxY)
{
	BoundingBox bb;

	bb.edges[0].first  = minX;
	bb.edges[0].second = maxX;

	bb.edges[1].first  = minY;
	bb.edges[1].second = maxY;

	return bb;
}

BoundingBox CandidateListRtree::GetMBR(UncertainObject* uObject)
{
	int min[2] = {-1, -1};
	int max[2] = {-1, -1};
	vector<Instance*> instances = uObject->GetInstances();
	for (vector<Instance*>::iterator instamceIt = instances.begin(); instamceIt < instances.end(); instamceIt++)
	{
		vector<int> dimensions = (*instamceIt)->GetDimensions();
		if (min[0] == -1 || min[0] > dimensions.at(0))
		{
			min[0] = dimensions.at(0);
		}
		if (min[1] == -1 || min[1] > dimensions.at(1))
		{
			min[1] = dimensions.at(1);
		}
		if (max[0] == -1 || max[0] < dimensions.at(0))
		{
			max[0] = dimensions.at(0);
		}
		if (max[1] == -1 || max[1] < dimensions.at(1))
		{
			max[1] = dimensions.at(1);
		}
	}
	return Bounds(min[0], min[1], max[0], max[1]);
}

// Overlap
vector<UncertainObject*> CandidateListRtree::GetOverlapObject(BoundingBox bb)
{
	Visitor x;
	x = _candidateTree.Query(RTree::AcceptOverlapping(bb), Visitor());
	return x.uObjects;
}

// Enclosing
vector<UncertainObject*> CandidateListRtree::GetEnclosingObject(BoundingBox bb)
{
	Visitor x;
	x = _candidateTree.Query(RTree::AcceptEnclosing(bb), Visitor());
	return x.uObjects;
}

vector<UncertainObject*> CandidateListRtree::PruningMethod(vector<UncertainObject*> uObjects, vector<int> maxDim)
{
	vector<UncertainObject*> result;
	for (vector<UncertainObject*>::iterator It = uObjects.begin(); It < uObjects.end(); It++)
	{
		UncertainObject* targetObject = *It;
		vector<Instance*> targetInstances = targetObject->GetInstances();
		double tempProbability = 0;
		for (vector<Instance*>::iterator instancesIt = targetInstances.begin(); instancesIt < targetInstances.end(); instancesIt++)
		{
			Instance* targetInstance = *instancesIt;
			Instance* fake = new Instance;
			for (vector<int>::iterator dimIt = maxDim.begin(); dimIt < maxDim.end(); dimIt++)
			{
				fake->AddDimensionValue(*dimIt);
			}
			if (Function::DominateTest(fake, targetInstance, _dimensions))
			{
				tempProbability += targetInstance->GetProbability();
			}
		}
		// targetObject 無法成為skyline
		if (tempProbability > 1 - _threshold)
		{
			result.push_back(targetObject);
		}
	}
	return result;
}