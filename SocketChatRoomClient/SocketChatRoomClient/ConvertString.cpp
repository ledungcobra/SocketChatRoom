#include "pch.h"
#include "ConvertString.h"

std::string ConvertString::ConvertCStringToString(CString cstring)
{
	CT2A t(cstring);
	std::string result(t);
	return result;
}

CString ConvertString::ConvertStringToCString(std::string input)
{
	return CString(input.c_str());
}
