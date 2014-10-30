#include "Model.h"


Model::Model(void)
{
}


Model::~Model(void)
{
}

void Model::LoadData()
{
	string fileName = DATASET;
	FileManager* file = new FileManager;
	file->openFile("../Debug/" + fileName, FileManager::Read);
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
			instance->AddDimensionValue(std::stoi(temp[ i + 3]));
		}
		object->AddInstance(instance);
	}
	objects.push_back(object);

	for (vector<UncertainObject*>::iterator it = objects.begin(); it < objects.end(); it++)
	{
		int timestamp = (*it)->GetTimestamp();
		objectsHash.insert(pair<int, UncertainObject*> (timestamp, *it));
	}

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

int Model::GetSize()
{
	return objects.size();
}
