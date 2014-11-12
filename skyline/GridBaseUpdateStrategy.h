#pragma once
#ifndef _CANDIDATE_LIST_QTREE_ 
#define _CANDIDATE_LIST_QTREE_
#include <map>
#include <time.h>
#include "ISkyline.h"
#include "CellManager.h"

using namespace std;
class GridBaseUpdateStrategy : public ISkyline
{
public:
	GridBaseUpdateStrategy(void);
	GridBaseUpdateStrategy(Model*);
	~GridBaseUpdateStrategy(void);
	void InsertObject(UncertainObject *);
	void DeleteObject(UncertainObject *);
	void ComputeSkyline();
	string GetSkylineResult();
private:
	vector<int> GetMaxDim(UncertainObject*);
	vector<int> GetMinDim(UncertainObject*);
	vector<UncertainObject*> GetCanPruningObject(vector<Instance*>);
	bool CanDominate(UncertainObject*, UncertainObject*);

	CellManager* _maybeList;
	vector<Instance*> _updateList;
};
#endif
