#pragma once
#include <vector>
#include "console.h"


namespace draw
{
    static Console console;
    const int STARTING_LENGTH = 4; // fix to Object::STARTING_LENGTH later

    namespace border
    {
        void topBorder();

        void leftBorder();

        void rightBorder();

        void bottomBorder();
    }

    void board();

    void snake(std::vector <POINT> BodyCoords, POINT tail);

    void playAgainMenu();

    void scoresMenu();

    void gameOverScreen(std::vector <POINT> BodyCoords);

}
