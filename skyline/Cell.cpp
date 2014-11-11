#include "Cell.h"


Cell::Cell(void)
{
}


Cell::~Cell(void)
{
}

void Cell::Insert(Instance* ins)
{
	_instances.push_back(ins);
}

void Cell::Delete(Instance* ins)
{
	_instances.erase(find(_instances.begin(), _instances.end(), ins));
}

vector<Instance*> Cell::GetInstances()
{
	return _instances;
}