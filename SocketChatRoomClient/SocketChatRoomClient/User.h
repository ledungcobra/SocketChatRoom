#pragma once
#include <afx.h>
class User
{

public:
	CString getUsername();
	CString getPassword();
	void setUserName(CString username);
	void setPassword(CString password);

private:
	CString username;
	CString _password;
	

};

