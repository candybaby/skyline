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
	virtual void Run();
	virtual void NextTimestamp();
	virtual void Insert(UncertainObject *);
	virtual void Delete(UncertainObject *);
	virtual void InsertObject(UncertainObject *) = 0;
	virtual void DeleteObject(UncertainObject *) = 0;
	virtual void ComputeSkyline() = 0;
	virtual string GetSkylineResult() = 0;
	virtual int GetSkylineCount() = 0;
	virtual int GetUpdateCount() = 0;
	virtual int GetPrunedCount() = 0;
	int GetTimestamp();
	void SetThreshold(double);
	void SetWindowSize(int);

protected:
	int _currentTimestamp;
	int _slideWindowSize;
	int _dimensions;
	double _threshold;
	Model* _model;
	vector<UncertainObject*> _slideWindow;
	vector<int> _maxDimensions;

private:
	void Init();
};
#endif
