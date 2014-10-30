#include "ISkyline.h"


ISkyline::ISkyline(void)
{
	Init();
}

ISkyline::ISkyline(Model* model)
{
	Init();
	_model = model;
}

void ISkyline::Init()
{
	_currentTimestamp = -1;
	_slideWindowSize = SLIDE_WINDOW_SIZE;
	_threshold = THRESHOLD;
	_dimensions = DIMENSION;
}

ISkyline::~ISkyline(void)
{
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
