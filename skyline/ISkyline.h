#pragma once
#ifndef _I_SKYLINE_ 
#define _I_SKYLINE_
#include <algorithm>
#include "Model.h"
#include "Config.h"

using namespace std;
class ISkyline
{
public:
	ISkyline(void);
	ISkyline(Model*);
	virtual ~ISkyline(void);
	virtual void NextTimestamp();
	virtual void Insert(UncertainObject *);
	virtual void Delete(UncertainObject *);
	virtual void InsertObject(UncertainObject *) = 0;
	virtual void DeleteObject(UncertainObject *) = 0;
	virtual void ComputeSkyline() = 0;
	virtual string GetSkylineResult() = 0;
	int GetTimestamp();

protected:
	int _currentTimestamp;
	int _slideWindowSize;
	int _dimensions;
	double _threshold;
	Model* _model;
	vector<UncertainObject*> _slideWindow;
	vector<UncertainObject*> _skylineResult;

private:
	void Init();
};
#endif
