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

vector<Instance*> CellManager::GetMaybeDominate(vector<int> dimVal)
{
	vector<Instance*> temp;
	
	vector<int> oDimVal;
	for (int i = 0; i < DIMENSION; i++)
	{
		int temp = dimVal.at(i) / _unitLengthDim.at(i);
		oDimVal.push_back(temp);
	}

	vector<vector<int>> cellID = _DARMap.at(oDimVal);
	for (vector<vector<int>>::iterator it = cellID.begin(); it < cellID.end(); it++)
	{
		vector<int> cId = *it;
		vector<Instance*> cellInstances = _cellsMap[cId]->GetInstances();
		temp.insert(temp.end(), cellInstances.begin(), cellInstances.end());
	}

	return temp;
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
	vector<int> index;
	for (int i = 0; i < DIMENSION; i++)
	{
		index.push_back(ins->GetDimensions().at(i) / _unitLengthDim.at(i));
	}
	_cellsMap[index]->Insert(ins);
}

void CellManager::Delete(Instance* ins)
{
	vector<int> index;
	for (int i = 0; i < DIMENSION; i++)
	{
		index.push_back(ins->GetDimensions().at(i) / _unitLengthDim.at(i));
	}
	_cellsMap[index]->Delete(ins);
}