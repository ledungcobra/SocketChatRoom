﻿#pragma once

enum class FlagServerToClient
{
	Send_Active_User, //Gửi danh sách người dùng online
	Fail_Sign_Up, // Đăng kí thất bại
	Fail_Login, // Đăng nhập thất bại
	Login_Success, //Đăng nhập thành công
	SignUp_Success, //Đăng kí thành công
	Send_File_Desc, //Gửi miêu tả file
	Send_File_Content //Gửi content của file
};
