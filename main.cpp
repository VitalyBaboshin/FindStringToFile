#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <iterator>
#include <sstream>
#include "getregex.hpp"
#include "metodkmp.hpp"

using namespace std;
namespace fs = experimental::filesystem;

char* GetStr(string fileString);
bool FindStrToFileTxt(string pathFile, char *buf, int lenght1);
void WriteToResult(string pathResult, string foolPathFile);

//indexP путь к обрабатываемой директории с файлами 
//indexM маска имен файлов, в которых осуществляется поиск
//indexI путь к файлу с заданной подстрокой
//indexO путь к файлу с результатами поиска
int indexP, indexM, indexI, indexO;

int main(int argc, char** argv)
{
	//Передача индексов аргументов в переменные 
	if (argc != 9) {
		cout << "input all parametr: " << endl;
		cout << "-p  - path directory" << endl;
		cout << "-m  - mask file *.* " << endl;
		cout << "-i  - path file str" << endl;
		cout << "-o  - path file to result search" << endl;
		return 0;
	}
	else {

		for (int i = 0; i + 1 < argc; ++i)
		{
			switch (argv[i][1])
			{
			case 'p':
				indexP = i + 1;
				break;

			case 'm':
				indexM = i + 1;
				break;

			case 'i':
				indexI = i + 1;
				break;

			case 'o':
				indexO = i + 1;
				break;
			}
		}
	}
	

	//вытаскиваем из файла подстроку с помощью функции GetStr
	char* podStr = GetStr(argv[indexI]);
	int lenghtPodStr = strlen(podStr);

	regex regexFileMask(GetRegexFromFileMask(argv[indexM]));

	fs::recursive_directory_iterator begin(argv[indexP]);
	fs::recursive_directory_iterator end;

	//Фильтруем дерево файлов маской файла
	vector<fs::path> datFiles;
	copy_if(begin, end, back_inserter(datFiles), [regexFileMask](const fs::path& path)
	{
		string putfile = path.stem().string() + path.extension().string();
		return fs::is_regular_file(path) && regex_match(putfile, regexFileMask);
	});

	auto iter = datFiles.begin();
	while (iter != datFiles.end())
	{

		std::ostringstream bstr;
		bstr << *iter;
		string pathFileText = bstr.str();

		if (FindStrToFileTxt(pathFileText, podStr, lenghtPodStr))
		{
			WriteToResult(argv[indexO], pathFileText);
		}
		++iter;
	}

	delete[] podStr;
	cout << "programm successfully completed";
	return 0;
}

//Чтение из файла подстроки
char* GetStr(string fileString)
{
	ifstream fString;
	fString.open(fileString, ios::binary);

	if (!fString.is_open())
	{
		//cout << "Error open file" << endl;
		return 0;
	}
	//cout << "file open " << endl;

	fString.seekg(0, ios_base::end);
	int lenghtStr = fString.tellg();
	fString.seekg(0, ios_base::beg);

	char *bufferStr = new char[lenghtStr + 1];
	fString.read(bufferStr, lenghtStr);
	bufferStr[lenghtStr] = 0;

	fString.close();

	return bufferStr;
}

//Поиск подстроки в файле 
//pathFile путь к файлу где ищем подстроку
bool FindStrToFileTxt(string pathFile, char* bufPodStr, int lenghtPodStr)
{
	ifstream fFile;
	fFile.open(pathFile, ios::binary);

	//проверка на доступ к файлу
	if (!fFile.is_open())
	{
		return 0;
	}

	fFile.seekg(0, ios_base::end);
	long endFile = fFile.tellg();

	//устанавливаем буфер в 10Mb
	int lenght = 10 * 1024 * 1024;
	char *buffer = new char[lenght];

	//устанаваливаем cur на начало файла
	unsigned long currentpos = 0;

	while (currentpos != endFile)
	{
		//Оставшаяся часть не прочитанового файла меньше длины буфера, меняем размер буфера
		if (lenght > endFile - currentpos)
		{
			lenght = endFile - currentpos;
		}

		fFile.seekg(currentpos);
		fFile.read(buffer, lenght);

		//поиск подстроки

		if (KMP(bufPodStr, lenghtPodStr, buffer, lenght))
		{
			delete[] buffer;
			fFile.close();
			return 1;
		}

		//устанавливаем curretpos на новую позицию за вычетом длины подстроки(переход через границу буфера)
		currentpos = fFile.tellg();

		if (currentpos != endFile)
		{
			currentpos = currentpos - (lenghtPodStr - 1);
		}

	}
	delete[] buffer;
	fFile.close();
	return 0;
}

////Записываем путь файла (где есть искомая подстрока) в файл Result
// pathResult		файл с результатами поиска
// fulllPathFile	путь файла где подстрока найдена
void WriteToResult(string pathResult, string fullPathFile)
{
	ofstream fResult;
	fResult.open(pathResult, ios_base::binary | ios_base::app | ios_base::out);

	if (!fResult.is_open())
	{
		//cout << "error open file" << endl;
	}

	fResult.seekp(0, ios::end);
	fResult.write(fullPathFile.c_str(), size(fullPathFile));

	//добавим переход на следующую строку
	string endlLine = "\r\n";
	fResult.write(endlLine.c_str(), size(endlLine));

	fResult.close();
}