#pragma once
#ifndef _MODEL_ 
#define _MODEL_

#include <vector>
#include <map>
#include "Config.h"
#include "UncertainObject.h"
#include "FileManager.h"
#include "Function.h"

#define CHAR_ENDL '\n'
#define EMPTY_TEXT ""


class Model
{
public:
	Model(void);
	~Model(void);
	void LoadData(string);
	UncertainObject* GetObjectByTimestamp(int );
	UncertainObject* GetObjectByName(string );
	int GetSize();
	vector<int> GetMaxDimension();

private:
	vector<UncertainObject *> objects;
	map<int, UncertainObject *> objectsHash;
	map<string, UncertainObject *> objectsHashByName;
	vector<int> _maxDimensions;
};
#endif