#include "Model.h"


Model::Model(void)
{
	_streamFile = NULL;
	_size = 0;
}

Model::Model(string dataSet)
{
	_streamFile = new FileManager;
	_streamFile->openFile(dataSet, FileManager::Read);
	_tempContent = "";
	_timestamp = 0;
	InitSize(dataSet);
}

Model::~Model(void)
{
	objectsHashByName.clear();
	objectsHash.clear();
	//while (objects.size() > 0)
	//{
	//	UncertainObject* delData = objects.back();
	//	objects.pop_back();
	//	delete delData;
	//}
	if (_streamFile != NULL)
	{
		_streamFile->closeFile();
		delete _streamFile;
	}
}

void Model::InitSize(string fileName)
{
	FileManager* file = new FileManager;
	file->openFile(fileName, FileManager::Read);
	string content = file->readLastLine();
	vector<string> temp = Function::split(content, '\t');
	file->closeFile();
	delete file;
	// 爆掉的話 最後一行是 空的要刪除
	int timestamp = std::stoi(temp[1]);

	_size = timestamp + 1;

	
}

void Model::LoadData(string dataSet)
{
	string fileName = dataSet;
	FileManager* file = new FileManager;
	file->openFile(fileName, FileManager::Read);
	string content = file->readFile();
	file->closeFile();

	string tempName = "";
	UncertainObject* object = NULL;
	vector<string> lines = Function::split(content, CHAR_ENDL);
	lines.erase(lines.begin());
	for (vector<string>::iterator it = lines.begin(); it < lines.end(); it++)
	{
		vector<string> temp = Function::split(*it, '\t');
		if (temp.size() < 5) break;
		string oName = temp[0];
		int timestamp = std::stoi(temp[1]);
		string iName = temp[2];
		double probability = std::stod(temp[temp.size() - 1]);

		if (tempName != oName)
		{
			tempName = oName;
			if (object != NULL)
			{
				objects.push_back(object);
			}
			object = new UncertainObject;
			object->SetName(oName);
			object->SetTimestamp(timestamp);
		}

		Instance* instance = new Instance;
		instance->SetName(iName);
		instance->SetObjectName(oName);
		instance->SetProbability(probability);
		for (int i = 0; i < temp.size() - 4; i++)
		{
			int value = std::stoi(temp[ i + 3]);
			instance->AddDimensionValue(value);
			if (_maxDimensions.size() != temp.size() - 4)
			{
				_maxDimensions.push_back(value);
			}
			else
			{
				if (_maxDimensions.at(i) < value)
				{
					_maxDimensions.at(i) = value;
				}
			}
		}
		object->AddInstance(instance);
	}
	objects.push_back(object);

	for (vector<UncertainObject*>::iterator it = objects.begin(); it < objects.end(); it++)
	{
		int timestamp = (*it)->GetTimestamp();
		objectsHash.insert(pair<int, UncertainObject*> (timestamp, *it));
	}

	for (vector<UncertainObject*>::iterator it = objects.begin(); it < objects.end(); it++)
	{
		string name = (*it)->GetName();
		objectsHashByName.insert(pair<string, UncertainObject*> (name, *it));
	}
}

void Model::LoadNextData()
{
	string objectString;
	string content;
	if (_tempContent == "")
	{
		content = _streamFile->readLine(); // skip title
		_tempContent = content;
		content = _streamFile->readLine();
	}
	else
	{
		content = _tempContent;
	}
		
	while (content != "END")
	{
		vector<string> temp = Function::split(content, '\t');
		if (temp.size() < 5) break;
		int timestamp = std::stoi(temp[1]);
		if (timestamp != _timestamp)
		{
			_tempContent = content;
			break;
		}
		else
		{
			objectString += content + "\n";
			content = _streamFile->readLine();
		}
	}
		
	string tempName = "";
	UncertainObject* object = NULL;
	vector<string> lines = Function::split(objectString, CHAR_ENDL);
	for (vector<string>::iterator it = lines.begin(); it < lines.end(); it++)
	{
		vector<string> temp = Function::split(*it, '\t');
		if (temp.size() < 5) break;
		string oName = temp[0];
		int timestamp = std::stoi(temp[1]);
		string iName = temp[2];
		double probability = std::stod(temp[temp.size() - 1]);

		if (tempName != oName)
		{
			tempName = oName;
			if (object != NULL)
			{
				objects.push_back(object);
			}
			object = new UncertainObject;
			object->SetName(oName);
			object->SetTimestamp(timestamp);
		}

		Instance* instance = new Instance;
		instance->SetName(iName);
		instance->SetObjectName(oName);
		instance->SetProbability(probability);
		for (int i = 0; i < temp.size() - 4; i++)
		{
			int value = std::stoi(temp[i + 3]);
			instance->AddDimensionValue(value);
			if (_maxDimensions.size() != temp.size() - 4)
			{
				_maxDimensions.push_back(value);
			}
			else
			{
				if (_maxDimensions.at(i) < value)
				{
					_maxDimensions.at(i) = value;
				}
			}
		}
		object->AddInstance(instance);
	}
	objects.push_back(object);
	objectsHash.insert(pair<int, UncertainObject*>(_timestamp, object));
	objectsHashByName.insert(pair<string, UncertainObject*>(tempName, object));
	_timestamp++;
}

UncertainObject* Model::GetObjectByTimestamp(int timestamp)
{
	UncertainObject* result = NULL;
	if (objectsHash.find(timestamp) == objectsHash.end()) {
		// not found
	} else {
		result = objectsHash.at(timestamp);
	}
	
	return result;
}

UncertainObject* Model::GetObjectByName(string name)
{
	UncertainObject* result = NULL;
	if (objectsHashByName.find(name) == objectsHashByName.end()) {
		// not found
	} else {
		result = objectsHashByName.at(name);
	}

	return result;
}

int Model::GetSize()
{
	//return objects.size();
	return _size;
}

vector<int> Model::GetMaxDimension()
{
	return _maxDimensions;
}