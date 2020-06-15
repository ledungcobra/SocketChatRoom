#include "pch.h"
#include "User.h"

void User::setUsername(CString username)
{
	_username = username;
}

void User::setPassword(CString password)
{
	_password = password;

}

CString User::getUsername()
{
	return _username;
}

CString User::getPassword()
{
	return _password;
}
