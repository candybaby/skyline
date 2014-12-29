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

// true if a maybe dominate b
bool Function::DominateTest(UncertainObject* a, UncertainObject* b, int dim)
{
	bool dominateTemp=false;
	
	int* aD = a->GetMin();
	int* bD = b->GetMax();

	for (int i = 0; i < dim; i++)
	{
		if (aD[i] > bD[i])
		{
			return false;
		}
		else if(aD[i] < bD[i])
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
	double diffms=(diffticks);
	return convertDoubleToString(diffms);
}

bool Function::isBigger(double a, double b, double offset)
{
	bool result = false;
	double c = a - b;
	if (c > offset)
	{
		result = true;
	}
	return result;
}

bool Function::isEqual(double a, double b, double offset)
{
	bool result = false;
	double c = a - b;
	if (std::abs(c) <= offset)
	{
		result = true;
	}
	return result;
}

bool Function::isBiggerEqual(double a, double b, double offset)
{
	return isBigger(a, b, offset) || isEqual(a, b, offset);
}