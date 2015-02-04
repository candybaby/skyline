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

string FileManager::readLine()
{
	string result, line;
	if (getline(fin, line))
	{
		result = line;
	}
	else
	{
		result = "END";
	}
	return result;
}

string FileManager::readLastLine()
{
	fin.seekg(-1, ios_base::end);                // go to one spot before the EOF

	bool keepLooping = true;
	while (keepLooping) {
		char ch;
		fin.get(ch);                            // Get current byte's data

		if ((int)fin.tellg() <= 1) {             // If the data was at or before the 0th byte
			fin.seekg(0);                       // The first line is the last line
			keepLooping = false;                // So stop there
		}
		else if (ch == '\n') {                   // If the data was a newline
			keepLooping = false;                // Stop at the current position.
		}
		else {                                  // If the data was neither a newline nor at the 0 byte
			fin.seekg(-2, ios_base::cur);        // Move to the front of that data, then to the front of the data before it
		}
	}

	string lastLine;
	getline(fin, lastLine);                      // Read the current line
	return lastLine;
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