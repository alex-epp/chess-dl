// CppChessLib.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <Windows.h>

#include "chess.h"


void enable_unicode() {
	CONSOLE_FONT_INFOEX old_font, new_font;
	GetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &old_font);
	new_font.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	new_font.FontWeight = FW_NORMAL;
	new_font.dwFontSize.X = 14;
	new_font.dwFontSize.Y = 14;
	wcscpy_s(new_font.FaceName, L"NSimSun");
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &new_font);
	_setmode(_fileno(stdout), _O_U16TEXT);
}

int main()
{
	enable_unicode();

	auto b = chess::Board();
	b.push_move("e2e4");
	std::wcout << b;
}
