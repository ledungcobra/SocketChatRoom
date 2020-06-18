#include "pch.h"
#include "ConvertString.h"
#include "TcpServer.h"
std::string ConvertString::EncodeCStringToString(CString cstring)
{
	/*CT2A t(cstring);
	std::string result(t);*/
	std::string result = "";
	for (int i = 0; i < cstring.GetLength(); i++) {
		result += std::to_string((unsigned int)cstring[i]) + '$';
	}
	return result;
}

CString ConvertString::DecodeStringToCString(std::string input)
{

	CString result = L"";
	auto charArr = stringTokenizer(input, '$',false);
	for (int i = 0; i < charArr.size(); i++) {
		try {
			TCHAR temp(std::stoi(charArr[i]));
			result += temp;
		}
		catch (...) {
			//AfxMessageBox(L"Stoi error");
		}
		
		
	}
	return result;
}

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