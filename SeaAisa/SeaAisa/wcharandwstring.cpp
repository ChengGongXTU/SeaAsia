#include"wcharandwstring.h"

wstring CharToWstring(const char* word)
{
	wchar_t wword[128];
	MultiByteToWideChar(CP_ACP, 0, word, strlen(word)+1, wword, 128);

	wstring wstr = wword;
	return wstr;
}

void CharToWchar(const char* word,wchar_t* wword)
{
	if (word)
	{
		int number = (int)strlen(word);
		int numberw = (int)MultiByteToWideChar(CP_ACP, 0, word, number, 0, 0);

		MultiByteToWideChar(CP_ACP, 0, word, number, wword, numberw);
	}

}