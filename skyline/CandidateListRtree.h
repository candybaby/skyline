#pragma once
#ifndef _CANDIDATE_LIST_RTREE_ 
#define _CANDIDATE_LIST_RTREE_
#include <map>
#include "ISkyline.h"
#include "RStarTree.h"

typedef RStarTree<UncertainObject *, DIMENSION, 3, 8> RTree;
typedef RTree::BoundingBox BoundingBox;

struct Visitor {
	int count;
	vector<UncertainObject*> uObjects;
	bool ContinueVisiting;

	Visitor() : count(0), ContinueVisiting(true) {};

	void operator()(const RTree::Leaf * const leaf) 
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
	CandidateListRtree(Model*);
	~CandidateListRtree(void);
	void InsertObject(UncertainObject *);
	void DeleteObject(UncertainObject *);
	void ComputeSkyline();
	string GetSkylineResult();

private:
	BoundingBox Bounds(int[DIMENSION], int[DIMENSION]);
	BoundingBox Bounds(int, int, int, int);
	BoundingBox GetMBR(UncertainObject *);
	vector<UncertainObject*> GetOverlapObject(BoundingBox); // Overlap
	vector<UncertainObject*> GetEnclosingObject(BoundingBox); // Enclosing
	vector<UncertainObject*> PruningMethod(vector<UncertainObject*>, vector<int>);

	map<int, vector<UncertainObject*>> _candidateList;
	vector<UncertainObject*> _skyline;
	RTree _candidateTree;
};
#endif
