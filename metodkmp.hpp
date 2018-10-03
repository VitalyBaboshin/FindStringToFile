#pragma once
#include <vector>

//// Алгоритм Кнута-Морриса-Пратта
// podStr   подстрока которую ищем
// strText  строка поиска
// prefix   массив длин префиксов для подстроки 

bool KMP(const char* podStr,const int podStr_size, const char* strText, const int strText_size)
{

	std::vector<int> prefix(podStr_size, 0);
	for (int i = 1; i < podStr_size; ++i) {
		int j = prefix[i - 1];
		while (j > 0 && podStr[j] != podStr[i])
			j = prefix[j - 1];
		if (podStr[j] == podStr[i])
			j++;
		prefix[i] = j;
	}

	prefix[0] = -1;
	int i = 0;
	int k = 0;
	while (i < strText_size)
	{
		if (k == -1)
		{
			i++;
			k = 0;
		}
		else if (strText[i] == podStr[k])
		{
			i++;
			k++;
			if (k == podStr_size)
				return 1;
		}
		else
			k = prefix[k];
	}
	return 0;
}