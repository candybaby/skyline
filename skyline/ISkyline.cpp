#include "ISkyline.h"


ISkyline::ISkyline(void)
{
	Init();
}

ISkyline::ISkyline(Model* model)
{
	Init();
	_model = model;
	_maxDimensions = model->GetMaxDimension();
}

void ISkyline::Init()
{
	_currentTimestamp = -1;
	_slideWindowSize = 60;
	_threshold = 0.4;
	_dimensions = DIMENSION;
}

ISkyline::~ISkyline(void)
{
	_slideWindow.clear();
}

void ISkyline::NextTimestamp()
{
	_currentTimestamp++;
	Delete(_model->GetObjectByTimestamp(_currentTimestamp - _slideWindowSize));
	Insert(_model->GetObjectByTimestamp(_currentTimestamp));
	ComputeSkyline();

}

void ISkyline::Insert(UncertainObject* uObject)
{
	if (uObject != NULL)
	{
		_slideWindow.push_back(uObject);
		InsertObject(uObject);
	}
}

void ISkyline::Delete(UncertainObject* uObject)
{
	if (uObject != NULL)
	{
		_slideWindow.erase(find(_slideWindow.begin(), _slideWindow.end(), uObject));
		DeleteObject(uObject);
	}
}

int ISkyline::GetTimestamp()
{
	return _currentTimestamp;
}

void ISkyline::SetThreshold(double th)
{
	_threshold = th;
}

void ISkyline::SetWindowSize(int value)
{
	_slideWindowSize = value;
}