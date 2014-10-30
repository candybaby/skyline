#pragma once
#ifndef _FUNCTION_ 
#define _FUNCTION_
#include <iostream>
#include <sstream>
#include <vector>
#include <time.h>
#include "Instance.h"

using namespace std;
class Function
{
public:
	static vector<string> split(const string &, char, vector<string> &);
	static vector<string> split(const string &, char);
	static string convertIntToString(int &);
	static string convertDoubleToString(double &);
	static bool DominateTest(Instance*, Instance*, int);
	static string diffclock(clock_t ,clock_t);
};
#endif
