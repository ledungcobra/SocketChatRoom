#pragma once

enum class FlagServerToClient
{
	Send_Active_User, //Gửi danh sách người dùng online
	Fail_Sign_Up, // Đăng kí thất bại
	Fail_Login, // Đăng nhập thất bại
	Login_Success, //Đăng nhập thành công
	SignUp_Success, //Đăng kí thành công
	Send_File_Desc, //Gửi miêu tả file
	Send_File_Content, //Gửi content của file
	Already_Login, // đã có người sử dụng tài khoản
	Send_Private_Message, //flag NULL sender(username - thêm map user vào bên client) NULL message NULL
	Send_Public_Message, //flag NULL sender(username) NULL message NULL
	Close_All_Connection//flag NULL
};

