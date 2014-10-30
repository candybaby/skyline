#include "Function.h"


bool Function::DominateTest(Instance* a, Instance* b, int dim)
{
	bool dominateTemp=false;
	vector<int> aD = a->GetDimensions();
	vector<int> bD = b->GetDimensions();
	for (int i = 0; i < dim; i++)
	{
		if (aD.at(i) > bD.at(i))
		{
			return false;
		}
		else if(aD.at(i) < bD.at(i))
		{
			dominateTemp=true;
		}
	}
	return dominateTemp;
}

// split
vector<string> Function::split(const std::string &sourceString, char delim, std::vector<std::string> &resultVector) 
{
	stringstream stringStream(sourceString);
	string item;
	while (getline(stringStream, item, delim)) 
	{
		resultVector.push_back(item);
	}
	return resultVector;
}

vector<string> Function::split(const string &sourceString, char delim)
{
	vector<string> resultVector;
	split(sourceString, delim, resultVector);
	return resultVector;
}

// Âà´«IntToString
string Function::convertIntToString(int &targetInt) 
{
	string tempString;
	stringstream stringStream(tempString);
	stringStream << targetInt;
	return stringStream.str();
}

string Function::convertDoubleToString(double &targetDouble)
{
	string tempString;
	stringstream stringStream(tempString);
	stringStream << targetDouble;
	return stringStream.str();
}

string Function::diffclock(clock_t clock1,clock_t clock2)
{
	double diffticks=clock1-clock2;
	double diffms=(diffticks*10)/CLOCKS_PER_SEC;
	return convertDoubleToString(diffms);
}