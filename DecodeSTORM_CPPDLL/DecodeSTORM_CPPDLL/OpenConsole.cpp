#include "stdafx.h"
#include"OpenConsole.h"



void OpenConsole()
{
	// create a console
	AllocConsole();

	// 
	FILE* stream;
	freopen_s(&stream, "CON", "r", stdin); //
	freopen_s(&stream, "CON", "w", stdout); //

	SetConsoleTitleA("Information output"); //

	HANDLE _handleOutput;
	_handleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

	// FreeConsole();
}



