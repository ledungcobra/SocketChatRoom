#include "pch.h"
#include "User.h"

CString User::getUsername()
{
	return _username;
}

CString User::getPassword()
{
	return _password;
}

void User::setUserName(CString username)
{
	this->_username = username;

}

void User::setPassword(CString password)
{
	this->_password = password;

}
