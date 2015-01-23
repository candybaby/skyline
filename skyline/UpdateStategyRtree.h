#pragma once
#ifndef _UPDATE_STATEGY_RTREE_ 
#define _UPDATE_STATEGY_RTREE_
#include <map>
#include <time.h>
#include "ISkyline.h"
#include "RStarTree.h"

typedef RStarTree<UncertainObject *, DIMENSION, 3, 8> RTree;
typedef RTree::BoundingBox BoundingBox;

struct UVisitor {
	int count;
	vector<UncertainObject*> uObjects;
	bool ContinueVisiting;

	UVisitor() : count(0), ContinueVisiting(true) {};

	void operator()(const RTree::Leaf * const leaf) 
	{
		uObjects.push_back(leaf->leaf);
		count++;
	}
};

struct UVisitorWithoutPruned {
	int count;
	vector<UncertainObject*> uObjects;
	bool ContinueVisiting;

	UVisitorWithoutPruned() : count(0), ContinueVisiting(true) {};

	void operator()(const RTree::Leaf * const leaf) 
	{
		if (!leaf->leaf->GetPruned())
		{
			uObjects.push_back(leaf->leaf);
			count++;
		}
	}
};

using namespace std;

class UpdateStategyRtree : public ISkyline
{
public:
	UpdateStategyRtree(void);
	UpdateStategyRtree(Model*);
	~UpdateStategyRtree(void);

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

	vector<UncertainObject*> _skyline;
	//RTree _slideWindowTree;
	RTree _maybeTree;
	int _updateCount;
	int _prunedCount;
};
#endif