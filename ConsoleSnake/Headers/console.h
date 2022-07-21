#pragma once
#include <Windows.h>

class Console
{
    const bool CENTERING = true;
    CONSOLE_FONT_INFOEX cfi{};
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    HWND hDesktop = GetDesktopWindow();
    HWND console = GetConsoleWindow();
    RECT desktop = GetWindowRectangle();

    RECT GetWindowRectangle();

    void makeScrollAppear();

public:

    const int WIDTH = 40, HEIGHT = 15;

    Console();

    void setTextColor(UINT color);

    void centerWindow(int width, int height);

    void changeFont();

};
