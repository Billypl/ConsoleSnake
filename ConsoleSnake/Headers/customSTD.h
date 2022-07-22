#pragma once
#include "console.h"

namespace customSTD
{
	bool operator== (POINT a, POINT b);
	bool operator!= (POINT a, POINT b);
	int random(int rangeStart, int rangeEnd);

	void gotoxy(int x, int y);
	void gotoxy(int x, int y, char character);
}
