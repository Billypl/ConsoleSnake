#include <iostream>

#include "../Headers/customSTD.h"
#include "../Headers/draw.h"
#define FOREGROUND_DEFAULT 7

using namespace customSTD;
using namespace std;

void draw::border::topBorder()
{
    gotoxy(1, 1);
    for (int i = 1; i <= console.WIDTH; i++)
        std::cout << "$";
}

void draw::border::leftBorder()
{
    for (int i = 1; i <= console.HEIGHT; i++)
        gotoxy(1, i, '$');
}

void draw::border::rightBorder()
{
    for (int i = 1; i <= console.HEIGHT; i++)
        gotoxy(console.WIDTH, i, '$');
}

void draw::border::bottomBorder()
{
    gotoxy(1, console.HEIGHT);
    for (int i = 1; i <= console.WIDTH; i++)
        cout << "$";
}

void draw::board()
{
    border::topBorder();
    border::leftBorder();
    border::rightBorder();
    border::bottomBorder();
}

void draw::snake(vector <POINT> BodyCoords, POINT tail)
{
    gotoxy(BodyCoords[0].x, BodyCoords[0].y, '@');
    for (int i = 1; i < BodyCoords.size() - 1; i++)
        gotoxy(BodyCoords[i].x, BodyCoords[i].y, '#');

    gotoxy(tail.x, tail.y, ' ');
    gotoxy(1, 1);
}

void draw::playAgainMenu()
{
    const pair<int, int> MIDDLE = { console.WIDTH / 2 - 5, console.HEIGHT / 2 };
    gotoxy(MIDDLE.first, MIDDLE.second + 2);
    cout << "1. Play again";
    gotoxy(MIDDLE.first, MIDDLE.second + 3);
    cout << "2. Exit";
    gotoxy(MIDDLE.first, MIDDLE.second + 4);
    cout << "3. Menu";

    gotoxy(MIDDLE.first, MIDDLE.second + 5);
    cout << "Choose option: ";
    console.centerWindow(console.WIDTH, console.HEIGHT);
}

void draw::scoresMenu()
{
    system("CLS");
    gotoxy(console.WIDTH / 2 - 6, console.HEIGHT / 2);
    cout << "1. Scoreboard";
    gotoxy(console.WIDTH / 2 - 6, console.HEIGHT / 2 + 1);
    cout << "2. Last scores";
    gotoxy(console.WIDTH / 2 - 6, console.HEIGHT / 2 + 2);
    cout << "3. Settings (not ready)";
    gotoxy(console.WIDTH / 2 - 6, console.HEIGHT / 2 + 3);
    cout << "Choose option: ";
}

void draw::gameOverScreen(vector <POINT> BodyCoords)
{
    system("CLS");
    console.setTextColor(FOREGROUND_RED);
    gotoxy(console.WIDTH / 2 - 7, console.HEIGHT / 2 - 2);
    cout << "G A M E   O V E R";
    gotoxy(console.WIDTH / 2 - 5, console.HEIGHT / 2);
    cout << "Your score: " << BodyCoords.size() - STARTING_LENGTH - 1;
    console.setTextColor(FOREGROUND_DEFAULT);

    playAgainMenu();
}
