#pragma once
#include <afxstr.h>
class User
{
public:
	void setUsername(CString);
	void setPassword(CString);
	CString getUsername();
	CString getPassword();
private:
	CString _username;
	CString _password;

};

