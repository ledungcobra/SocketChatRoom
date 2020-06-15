#pragma once
enum class FlagClientToServer
{
	SignUp, // Đăng kí
	Login, // Đăng nhập
	PrivateChat, // Tạo private chat
	PublicChat, // Vào public chat
	LogOut, // Đăng xuất
	Send_File_Descriptor, //Gửi mô tả file
	Send_Content, //Gửi nội dung file
	Disconnect_To_Server, // Thoát khỏi server
	Download_Request //Yêu cầu down file
};

