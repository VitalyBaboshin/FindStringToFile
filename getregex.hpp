#pragma once
#include <regex>

using namespace std;
//Из маски файла делаем фильтр для regex
string GetRegexFromFileMask(string fileMask)
{
	regex rx1("[*]{1}");
	regex rx2("[\\.]");
	regex rx3("\\?");

	fileMask = regex_replace(fileMask, rx1, "[a-zA-Z_0-9]{0,}");
	fileMask = regex_replace(fileMask, rx2, "\\\.");
	fileMask = regex_replace(fileMask, rx3, "[0-9]{1}");

	return fileMask;
}