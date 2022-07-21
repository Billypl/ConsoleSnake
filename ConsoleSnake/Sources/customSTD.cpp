#include <iostream>
#include <Windows.h>
#include "../Headers/customSTD.h"

bool customSTD::operator== (POINT a, POINT b) { return ((a.x == b.x) && (a.y == b.y)); }
bool customSTD::operator!= (POINT a, POINT b) { return !(a == b); }
int customSTD::random(int rangeStart, int rangeEnd) { return (rand() % rangeEnd) + rangeStart; }

void customSTD::gotoxy(int x, int y)
{
	COORD c;
	c.X = x - 1;
	c.Y = y - 1;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void customSTD::gotoxy(int x, int y, char character)
{
	gotoxy(x, y);
	std::cout << character;
}
