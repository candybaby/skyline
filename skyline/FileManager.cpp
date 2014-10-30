#include "FileManager.h"


FileManager::FileManager(void)
{
}


FileManager::~FileManager(void)
{
}

// �}���ɮ�
bool FileManager::openFile(string path, FileType type)
{
	if (type == Write)
	{
		fout.open(path);
		if (!fout.is_open())
		{
			return false; // �L�k�g�J�ɮ�
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
			return false; // �L�kŪ���ɮ�
		}
		else
		{
			return true;
		}
	}
}

// Ū�ɮ�(��)
string FileManager::readFile()
{
	string result, line;
	while (getline(fin, line))
	{
		result += line + "\n";
	}
	return result;
}

// �g�ɮ�
void FileManager::writeLine(string content)
{
	fout << content.c_str() << endl;
}

void FileManager::write(string content)
{
	fout << content.c_str();
}

// �����ɮ�
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