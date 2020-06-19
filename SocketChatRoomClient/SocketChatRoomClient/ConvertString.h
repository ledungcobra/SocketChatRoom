#pragma once
#include <string>
#include <afxstr.h>
class ConvertString
{
public:
	static std::string EncodeCStringToString(CString cstring);
	static CString DecodeStringToCString(std::string input);
	static std::string ConvertCStringToString(CString cstring);
	static CString ConvertStringToCString(std::string input);
};

