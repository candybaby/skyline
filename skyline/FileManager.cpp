#include "FileManager.h"


FileManager::FileManager(void)
{
}


FileManager::~FileManager(void)
{
}

// 開啟檔案
bool FileManager::openFile(string path, FileType type)
{
	if (type == Write)
	{
		fout.open(path);
		if (!fout.is_open())
		{
			return false; // 無法寫入檔案
		}
		else
		{
			return true;
		}
	}
	else // type == Read
	{
		fin.open(path);
		if (!fin.is_open())
		{
			return false; // 無法讀取檔案
		}
		else
		{
			return true;
		}
	}
}

// 讀檔案(行)
string FileManager::readFile()
{
	string result, line;
	while (getline(fin, line))
	{
		result += line + "\n";
	}
	return result;
}

vector<string> FileManager::readFileToVector()
{
	vector<string> result_v;
	string result, line;
	int counter = 0;
	while (getline(fin, line))
	{
		counter++;
		result += line + "\n";

		if (counter > 200)
		{
			counter = 0;
			result_v.push_back(result);
			result = "";
		}
	}
	return result_v;
}

// 寫檔案
void FileManager::writeLine(string content)
{
	fout << content.c_str() << endl;
}

void FileManager::write(string content)
{
	fout << content.c_str();
}

// 關閉檔案
void FileManager::closeFile()
{
	if (fout)
	{
		fout.close();
	}
	if (fin)
	{
		fin.close();
	}
}