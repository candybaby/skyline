#pragma once
#ifndef _BRUTEFORCE_ 
#define _BRUTEFORCE_
#include "ISkyline.h"

using namespace std;
class BruteForce : public ISkyline
{
public:
	BruteForce(void);
	BruteForce(Model*);
	~BruteForce(void);
	void InsertObject(UncertainObject *);
	void DeleteObject(UncertainObject *);
	void ComputeSkyline();
	string GetSkylineResult();

	int GetSkylineCount();
	int GetUpdateCount();

};
#endif
