#pragma once
#ifndef _CELL_MANAGER_ 
#define _CELL_MANAGER_
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include "Config.h"
#include "Function.h"
#include "Cell.h"
#include "UncertainObject.h"

class CellManager
{
public:
	CellManager(int p, vector<int>);
	~CellManager(void);
	void Insert(UncertainObject*);
	void Delete(UncertainObject*);
	Cell* GetCellByKey(vector<int>);
	vector<UncertainObject*> GetObjects();

	vector<Instance*> GetCompleteDominate(vector<int>);
	vector<Instance*> GetMaybeDominate(vector<int>);

	vector<Instance*> GetMaybeDominateMe(vector<int>);
	void ComputeSkyline(double);
	void Clear();
private:
	void Insert(Instance*);
	void Delete(Instance*);

	vector<vector<int>> GetDDR(vector<int>);
	vector<vector<int>> GetDAR(vector<int>);
	vector<vector<int>> GetPAR(vector<int>);

	int _partNumber;
	map<vector<int>, Cell*> _cellsMap;
	map<vector<int>, vector<vector<int>>> _DDRMap;
	map<vector<int>, vector<vector<int>>> _DARMap;
	map<vector<int>, vector<vector<int>>> _PARMap;
	vector<UncertainObject*> _objects;
	vector<int> _unitLengthDim;
};
#endif