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

/// Convert a wide string to a standard string
extern std::string ws2s(const std::wstring& s);

/// Convert a standard string to a wide string
extern std::wstring s2ws(const std::string& s);

/// Splits a string by the delimeter into a vector of strings.
void Split(const std::string& str, StringVec& vec, char delimiter);



/**
	A hashed string. This object retains the intial string in addition to 
	the hashed value for easy referece.
*/
class HashedString
{
public:
	/// Constructor taking a string
	explicit HashedString(const char* pIdentString);

	/// Return the hashed value of the string
	unsigned long GetHashValue() const;

	/// Return the original string
	const std::string& GetStr() const;

	/// Hash a string and return the hash
	static void* Hash_Name(char const* pIdentStr);

	/// Less than comparison function
	bool operator<(const HashedString& rhs) const;

	/// Is equal to comparison function
	bool operator==(const HashedString& rhs) const;

private:
	/// Hashed value of a string. Stored as void* so it shows up in hex in the debugger
	void* m_Ident;

	/// The original string to be hashed
	std::string m_IdentStr;
};
