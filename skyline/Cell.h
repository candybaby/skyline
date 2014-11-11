#pragma once
#ifndef _CELL_ 
#define _CELL_
#include <vector>
#include <algorithm>
#include "Config.h"
#include "Instance.h"

class Cell
{
public:
	Cell(void);
	~Cell(void);
	void Insert(Instance*);
	void Delete(Instance*);
	vector<Instance*> GetInstances();

private:
	vector<Instance*> _instances;
};
#endif