#include "pch.h"
#include "User.h"

CString User::getUsername()
{
	return username;
}

CString User::getPassword()
{
	return _password;
}

void User::setUserName(CString username)
{
	this->username = username;

}

void User::setPassword(CString password)
{
	this->_password = password;

}
