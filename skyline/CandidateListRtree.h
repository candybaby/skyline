#pragma once
#ifndef _CANDIDATE_LIST_RTREE_ 
#define _CANDIDATE_LIST_RTREE_
#include <map>
#include "ISkyline.h"
#include "RTree.h"
#include <time.h>

typedef RTree<UncertainObject *, DIMENSION, 3, 8> RTree_T;
typedef RTree_T::BoundingBox BoundingBox;

struct Visitor {
	int count;
	vector<UncertainObject*> uObjects;
	bool ContinueVisiting;

	Visitor() : count(0), ContinueVisiting(true) {};

	void operator()(const RTree_T::Leaf * const leaf)
	{
		uObjects.push_back(leaf->leaf);
		count++;
	}
};


using namespace std;
class CandidateListRtree : public ISkyline
{
public:
	CandidateListRtree(void);
	CandidateListRtree(Model*,bool);
	~CandidateListRtree(void);
	void InsertObject(UncertainObject *);
	void DeleteObject(UncertainObject *);
	void ComputeSkyline();

	string GetSkylineResult();

	int GetSkylineCount();
	int GetUpdateCount();
	int GetPrunedCount();

private:
	BoundingBox Bounds(int[DIMENSION], int[DIMENSION]);
	BoundingBox GetMBR(UncertainObject *);
	vector<UncertainObject*> PruningMethod(vector<UncertainObject*>, vector<int>, vector<int>);
	void Group();
	void Normal();

	bool _IsGroup;
	map<int, vector<UncertainObject*>> _candidateList;
	vector<UncertainObject*> _skyline;
	vector<UncertainObject*> _prunedObject;
	RTree_T _candidateTree;
	int _updateCount;
	int _prunedCount;
};
#endif
