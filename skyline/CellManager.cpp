#include "CellManager.h"


CellManager::CellManager(int p, vector<int> maxDim)
{
	_partNumber = p;
	
	for (int i = 0; i < pow((double)_partNumber,DIMENSION); i++)
	{
		int x[DIMENSION];
		int temp = i;
		for (int y = DIMENSION - 1; y >= 0; y--)
		{
			x[y] = temp / pow((double)_partNumber, y);//0~3
			temp = temp % (int)pow((double)_partNumber, y);
		}
		vector<int> cId;
		for (int z = 0; z < DIMENSION; z++)
		{
			cId.push_back(x[z]);
		}
		
		_cellsMap.insert(pair<vector<int>, Cell*> (cId, new Cell));
		_DDRMap.insert(pair<vector<int>, vector<vector<int>>> (cId, GetDDR(cId)));
		_DARMap.insert(pair<vector<int>, vector<vector<int>>> (cId, GetDAR(cId)));
		_PARMap.insert(pair<vector<int>, vector<vector<int>>> (cId, GetPAR(cId)));
		_PDRMap.insert(pair<vector<int>, vector<vector<int>>> (cId, GetPDR(cId)));
	}

	for (int i=0;i<DIMENSION;i++)
	{
		_unitLengthDim.push_back((maxDim.at(i) / _partNumber) + 1);
	}

}

CellManager::~CellManager(void)
{
}

void CellManager::Insert(UncertainObject* uObject)
{
	_objects.push_back(uObject);
	vector<Instance*> instances = uObject->GetInstances();
	for (vector<Instance*>::iterator it = instances.begin(); it < instances.end(); it++)
	{
		Instance* instance = (*it);
		Insert(instance);
	}
}

vector<vector<int>> CellManager::GetCell(UncertainObject* uObject)
{
	vector<vector<int>> result;
	vector<Instance*> instances = uObject->GetInstances();
	for (vector<Instance*>::iterator it = instances.begin(); it < instances.end(); it++)
	{
		Instance* instance = (*it);
		vector<int> index = instance->GetCellId();
		if (index.size() == 0)
		{
			for (int i = 0; i < DIMENSION; i++)
			{
				index.push_back(instance->GetDimensions().at(i) / _unitLengthDim.at(i));
			}
		}
		result.push_back(index);
	}
	sort(result.begin(), result.end());
	result.erase(unique(result.begin(), result.end()), result.end());

	return result;
}

vector<vector<int>> CellManager::GetCell(vector<Instance*> instances)
{
	vector<vector<int>> result;
	for (vector<Instance*>::iterator it = instances.begin(); it < instances.end(); it++)
	{
		Instance* instance = (*it);
		vector<int> index = instance->GetCellId();
		if (index.size() == 0)
		{
			for (int i = 0; i < DIMENSION; i++)
			{
				index.push_back(instance->GetDimensions().at(i) / _unitLengthDim.at(i));
			}
		}
		result.push_back(index);
	}
	sort(result.begin(), result.end());
	result.erase(unique(result.begin(), result.end()), result.end());

	return result;
}

void CellManager::Delete(UncertainObject* uObject)
{
	if(find(_objects.begin(), _objects.end(), uObject) != _objects.end())
	{
		vector<Instance*> instances = uObject->GetInstances();
		for (vector<Instance*>::iterator it = instances.begin(); it < instances.end(); it++)
		{
			Instance* instance = (*it);
			Delete(instance);
		}
		_objects.erase(find(_objects.begin(), _objects.end(), uObject));
	}
}


Cell* CellManager::GetCellByKey(vector<int> key)
{
	return _cellsMap[key];
}

vector<UncertainObject*> CellManager::GetObjects()
{
	return _objects;
}

vector<vector<int>> CellManager::GetDDR(vector<int> dimVal)
{
	vector<vector<int>> result;
	int count = 1;
	for (int i = 0; i < DIMENSION; i++)
	{
		count *= (_partNumber - (dimVal.at(i) + 1));
	}

	for (int i = 0; i < count; i++)
	{
		vector<int> cId;
		cId.assign(dimVal.begin(), dimVal.end());
		int overflow = i;
		for (int dim = 0; dim < DIMENSION; dim++)
		{
			int aaa = _partNumber - dimVal.at(dim); // 3
			int dimValue = cId.at(dim) + overflow;  //5
			cId.at(dim) += dimValue % aaa;
			overflow = dimValue / aaa;
		}
		result.push_back(cId);
	}
	return result;
}

vector<vector<int>> CellManager::GetDAR(vector<int> dimVal)
{
	vector<vector<int>> result;
	int count = 1;
	for (int i = 0; i < DIMENSION; i++)
	{
		count *= (_partNumber - dimVal.at(i));
	}

	for (int i = 0; i < count; i++)
	{
		vector<int> cId;
		cId.assign(dimVal.begin(), dimVal.end());
		int overflow = i;
		for (int dim = 0; dim < DIMENSION; dim++)
		{
			int aaa = _partNumber - dimVal.at(dim); // 3
			int dimValue = cId.at(dim) + overflow;  //5
			cId.at(dim) += dimValue % aaa;
			overflow = dimValue / aaa;
		}
		result.push_back(cId);
	}
	return result;
}

vector<vector<int>> CellManager::GetPAR(vector<int> dimVal)
{
	vector<vector<int>> result;
	int count = 1;
	for (int i = 0; i < DIMENSION; i++)
	{
		count *= (dimVal.at(i) + 1);
	}

	// 8

	for (int i = 0; i < count; i++)
	{
		vector<int> cId;
		cId.assign(dimVal.begin(), dimVal.end());
		int tempVal = i;
		for (int y = 0; y < DIMENSION; y++)
		{
			int aaa = dimVal.at(y) + 1; //2
			cId.at(y) = tempVal % aaa;
			tempVal = tempVal / aaa;
		}
		result.push_back(cId);
	}
	return result;
}

vector<vector<int>> CellManager::GetPDR(vector<int> dimVal)
{
	vector<vector<int>> result;
	int count = 1;
	for (int i = 0; i < DIMENSION; i++)
	{
		count *= (dimVal.at(i));
	}

	// 8
	// 2 1

	for (int i = 0; i < count; i++)
	{
		vector<int> cId;
		cId.assign(dimVal.begin(), dimVal.end());
		int tempVal = i;
		for (int y = 0; y < DIMENSION; y++)
		{
			int aaa = dimVal.at(y); //2
			cId.at(y) = tempVal % aaa;  //   0
			tempVal = tempVal / aaa;    //   0
		}
		result.push_back(cId);
	}
	return result;
}

vector<Instance*> CellManager::GetCompleteDominate(vector<int> dimVal)
{
	vector<Instance*> temp;
	vector<int> oDimVal;
	for (int i = 0; i < DIMENSION; i++)
	{
		int temp = dimVal.at(i) / _unitLengthDim.at(i);
		oDimVal.push_back(temp + 1);
	}
	vector<vector<int>> cellID;
	if (_DDRMap.find(oDimVal) == _DDRMap.end()) {
		// not found
	} else {
		cellID = _DDRMap.at(oDimVal);
	}
	for (vector<vector<int>>::iterator it = cellID.begin(); it < cellID.end(); it++)
	{
		vector<int> cId = *it;
		vector<Instance*> cellInstances = _cellsMap[cId]->GetInstances();
		temp.insert(temp.end(), cellInstances.begin(), cellInstances.end());
	}

	return temp;
}

vector<vector<int>> CellManager::GetMaybeDominate(vector<int> dimVal)
{
	vector<int> oDimVal;
	for (int i = 0; i < DIMENSION; i++)
	{
		int temp = dimVal.at(i) / _unitLengthDim.at(i);
		oDimVal.push_back(temp);
	}

	return _DARMap.at(oDimVal);
}

vector<vector<int>> CellManager::GetMaybeDominateNotComplete(vector<int> minDimVal, vector<int> maxDimVal)
{
	vector<int> oMinDimVal, oMaxDimVal;
	for (int i = 0; i < DIMENSION; i++)
	{
		int temp = minDimVal.at(i) / _unitLengthDim.at(i);
		oMinDimVal.push_back(temp);
		temp = maxDimVal.at(i) / _unitLengthDim.at(i);
		oMaxDimVal.push_back(temp);
	}
	//******************************
	vector<vector<int>> cellID = _DARMap.at(oMinDimVal); // ¦h
	vector<vector<int>> cellID2 = _DDRMap.at(oMaxDimVal);
	vector<vector<int>> diffCellID;
	for (vector<vector<int>>::iterator cellId = cellID.begin(); cellId < cellID.end(); cellId++)
	{
		if (find(cellID2.begin(), cellID2.end(), *cellId) == cellID2.end())
		{
			diffCellID.push_back(*cellId);
		}
	}
	/*vector<Instance*> temp;
	for (vector<vector<int>>::iterator it = cellID2.begin(); it < cellID2.end(); it++)
	{
		vector<int> cId = *it;
		vector<Instance*> cellInstances = _cellsMap[cId]->GetInstances();
		temp.insert(temp.end(), cellInstances.begin(), cellInstances.end());
	}
	for (vector<Instance*>::iterator instamceIt = temp.begin(); instamceIt < temp.end(); instamceIt++)
	{
		(*instamceIt)->SetPruned(true);
	}*/

	return diffCellID;
}

vector<Instance*> CellManager::GetMaybeDominateMe(vector<int> dimVal)
{
	vector<Instance*> temp;

	vector<int> oDimVal;
	for (int i = 0; i < DIMENSION; i++)
	{
		int temp = dimVal.at(i) / _unitLengthDim.at(i);
		oDimVal.push_back(temp);
	}
	vector<vector<int>> cellID = _PARMap.at(oDimVal);
	for (vector<vector<int>>::iterator it = cellID.begin(); it < cellID.end(); it++)
	{
		vector<int> cId = *it;
		vector<Instance*> cellInstances = _cellsMap[cId]->GetInstances();
		temp.insert(temp.end(), cellInstances.begin(), cellInstances.end());
	}

	return temp;
}

void CellManager::Insert(Instance* ins)
{
	vector<int> index = ins->GetCellId();
	if (index.size() == 0)
	{
		for (int i = 0; i < DIMENSION; i++)
		{
			index.push_back(ins->GetDimensions().at(i) / _unitLengthDim.at(i));
		}
	}
	_cellsMap[index]->Insert(ins);
}

void CellManager::Delete(Instance* ins)
{
	vector<int> index = ins->GetCellId();
	if (index.size() == 0)
	{
		for (int i = 0; i < DIMENSION; i++)
		{
			index.push_back(ins->GetDimensions().at(i) / _unitLengthDim.at(i));
		}
	}
	_cellsMap[index]->Delete(ins);
}

void CellManager::ComputeSkyline(double threshold)
{
	for (map<vector<int>, Cell*>::iterator it = _cellsMap.begin(); it != _cellsMap.end(); ++it)
	{
		vector<int> index = it->first;
		vector<Instance*> instances = it->second->GetInstances();
		if (instances.size() > 0)
		{
			vector<Instance*> instances2;
			vector<Instance*> instances3;
			vector<vector<int>> cellIds = _PDRMap.at(index);
			for (vector<vector<int>>::iterator cellId = cellIds.begin(); cellId < cellIds.end(); cellId++)
			{
				vector<Instance*> tempInstances = _cellsMap.at(*cellId)->GetInstances();
				instances2.insert(instances2.end(), tempInstances.begin(), tempInstances.end());
			}
			vector<vector<int>> celldifferenceIds;
			vector<vector<int>> temp = _PARMap.at(index);
			// ®t¤Î
			for (vector<vector<int>>::iterator cellId = temp.begin(); cellId < temp.end(); cellId++)
			{
				if (find(cellIds.begin(), cellIds.end(), *cellId) == cellIds.end())
				{
					celldifferenceIds.push_back(*cellId);
				}
			}

			for (vector<vector<int>>::iterator cellId = celldifferenceIds.begin(); cellId < celldifferenceIds.end(); cellId++)
			{
				vector<Instance*> tempInstances = _cellsMap.at(*cellId)->GetInstances();
				instances3.insert(instances3.end(), tempInstances.begin(), tempInstances.end());
			}

			for (vector<Instance*>::iterator instamceIt = instances.begin(); instamceIt < instances.end(); instamceIt++)
			{
				Instance* instance = *instamceIt;
				instance->ClearDominateMe();
				for (vector<Instance*>::iterator instamceIt2 = instances2.begin(); instamceIt2 < instances2.end(); instamceIt2++)
				{
					Instance* instance2 = *instamceIt2;
					if (instance->GetObjectName() != instance2->GetObjectName())
					{
						instance->AddDominateMeInstance(instance2);
					}
				}
				for (vector<Instance*>::iterator instamceIt2 = instances3.begin(); instamceIt2 < instances3.end(); instamceIt2++)
				{
					Instance* instance2 = *instamceIt2;
					if (instance->GetObjectName() != instance2->GetObjectName())
					{
						if (Function::DominateTest(instance2, instance, DIMENSION))
						{
							instance->AddDominateMeInstance(instance2);
						}
					}
				}
			}
		}
	}
}


void CellManager::ComputeSkyline(vector<vector<int>> needUpdateIds)
{
	for (vector<vector<int>>::iterator updateId = needUpdateIds.begin(); updateId < needUpdateIds.end(); updateId++)
	{
		vector<int> index = *updateId;
		vector<Instance*> instances = _cellsMap.at(index)->GetInstances();
	
		if (instances.size() > 0)
		{
			vector<Instance*> instances2;
			vector<Instance*> instances3;
			vector<vector<int>> cellIds = _PDRMap.at(index);
			for (vector<vector<int>>::iterator cellId = cellIds.begin(); cellId < cellIds.end(); cellId++)
			{
				vector<Instance*> tempInstances = _cellsMap.at(*cellId)->GetInstances();
				instances2.insert(instances2.end(), tempInstances.begin(), tempInstances.end());
			}
			vector<vector<int>> celldifferenceIds;
			vector<vector<int>> temp = _PARMap.at(index);
			// ®t¤Î
			for (vector<vector<int>>::iterator cellId = temp.begin(); cellId < temp.end(); cellId++)
			{
				if (find(cellIds.begin(), cellIds.end(), *cellId) == cellIds.end())
				{
					celldifferenceIds.push_back(*cellId);
				}
			}

			for (vector<vector<int>>::iterator cellId = celldifferenceIds.begin(); cellId < celldifferenceIds.end(); cellId++)
			{
				vector<Instance*> tempInstances = _cellsMap.at(*cellId)->GetInstances();
				instances3.insert(instances3.end(), tempInstances.begin(), tempInstances.end());
			}

			for (vector<Instance*>::iterator instamceIt = instances.begin(); instamceIt < instances.end(); instamceIt++)
			{
				Instance* instance = *instamceIt;
				instance->ClearDominateMe();
				for (vector<Instance*>::iterator instamceIt2 = instances2.begin(); instamceIt2 < instances2.end(); instamceIt2++)
				{
					Instance* instance2 = *instamceIt2;
					if (instance->GetObjectName() != instance2->GetObjectName())
					{
						instance->AddDominateMeInstance(instance2);
					}
				}
				for (vector<Instance*>::iterator instamceIt2 = instances3.begin(); instamceIt2 < instances3.end(); instamceIt2++)
				{
					Instance* instance2 = *instamceIt2;
					if (instance->GetObjectName() != instance2->GetObjectName())
					{
						if (Function::DominateTest(instance2, instance, DIMENSION))
						{
							instance->AddDominateMeInstance(instance2);
						}
					}
				}
			}
		}
	}
}

void CellManager::Clear()
{
	_objects.clear();
	for (map<vector<int>, Cell*>::iterator it = _cellsMap.begin(); it != _cellsMap.end(); ++it)
	{
		delete it->second;
		it->second = new Cell;
	}
}