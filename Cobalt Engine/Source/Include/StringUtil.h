/*
	StringUtil.h

	Useful string utilities.
	
	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <string>
#include <vector>
#include <Windows.h>

#define MAX_DIGITS_IN_INT 12

typedef std::vector<std::string> StringVec;

/// Does a classic * & ? pattern match on a file name - this is case sensitive!
extern bool WildcardMatch(const char *pat, const char *str);

/// Convert an ascii string to unicode string
extern HRESULT AnsiToWideCch(WCHAR* dest, const CHAR* src, int charCount);

/// Convert an integer to string
extern std::string ToStr(int num, int base = 10);

/// Convert an unsigned int to a string
extern std::string ToStr(unsigned int num, int base = 10);

/// Convert an unsigned long to a string
extern std::string ToStr(unsigned long num, int base = 10);

/// Convert a float to a string
extern std::string ToStr(float num);

/// Splits a string by the delimeter into a vector of strings.
void Split(const std::string& str, StringVec& vec, char delimiter);

class HashedString
{
public:
	static void* hash_name(char const* pStr);
};
