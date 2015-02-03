#pragma once
#ifndef _FILEMANAGER_ 
#define _FILEMANAGER_
#include <iostream> 
#include <fstream> 
#include <string>
#include <vector>

using namespace std;
class FileManager
{
public:
	enum FileType
	{
		Read,
		Write
	};
	FileManager(void);
	~FileManager(void);
	bool openFile(string, FileType);
	void writeLine(string);
	void write(string);
	string readFile();
	vector<string> readFileToVector();
	void closeFile();
	
private:
	ofstream fout;
	ifstream fin;
};
#endif