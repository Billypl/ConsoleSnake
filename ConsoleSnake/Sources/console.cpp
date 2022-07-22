#include "../Headers/console.h" 
#include <utility>


RECT Console::GetWindowRectangle()
{
    RECT desktop;
    GetWindowRect(hDesktop, &desktop);
    return desktop;
}

void Console::makeScrollAppear()
{
    COORD c;
    c.X = WIDTH + 100; c.Y = 9000;
    SetConsoleScreenBufferSize(hConsole, c);
}

Console::Console()
{
    changeFont();
    centerWindow(WIDTH, HEIGHT);
    makeScrollAppear(); //doesnt work
}

void Console::setTextColor(UINT color)
{
    SetConsoleTextAttribute(hConsole, color | FOREGROUND_INTENSITY);
}

void Console::centerWindow(int width, int height)
{
    if (CENTERING != true)
        return;

    const std::pair<int, int> MARGIN = { 36, 60 };
    width = width * cfi.dwFontSize.X + MARGIN.first;
    height = height * cfi.dwFontSize.Y + MARGIN.second;

    MoveWindow(console, (desktop.right / 2) - (width / 2), (desktop.bottom / 2) - (height / 2), width, height, TRUE);
}

void Console::changeFont()
{
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 13;
    cfi.dwFontSize.Y = 24;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_BOLD;
    wcscpy_s(cfi.FaceName, L"Liberation Mono");
    SetCurrentConsoleFontEx(hConsole, FALSE, &cfi);
}

