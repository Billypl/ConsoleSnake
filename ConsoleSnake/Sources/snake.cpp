#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <string>
#include <cstdlib>
#include <vector>
#include <utility>
#include <ctime>
#include <fstream>
#include <cwchar>
#include <direct.h>
#include <algorithm>
#include <array>
#define FOREGROUND_DEFAULT 7

#include "../Headers/customSTD.h"
#include "../Headers/dateFormatter.h"
#include "../Headers/console.h"
#include "../Headers/draw.h"

using namespace std;
using namespace customSTD;
Console console;



const int H_SPEED = 150;
const int V_SPEED = H_SPEED * 1.4;

POINT currPos = { console.WIDTH / 4, console.HEIGHT / 2 };

char direction = 'D';
char prevDirection = direction;


bool comp(pair <int, string> i, pair <int, string> j)
{
    return (i.first > j.first);
}

class Object
{
private:
    POINT head()
    {
        return BodyCoords[0];
    }

public:
    const static int STARTING_LENGTH = 4;
    vector <POINT> BodyCoords;

    int gameOverTicks = 2; //ticks when player bumps into the wall

    POINT fruit;
    POINT tail = { currPos.x - STARTING_LENGTH, currPos.y }; //tail is actually 1 block behind the visible tail

    int centredScore()
    {
        return -log10((BodyCoords.size() - 1 - STARTING_LENGTH)) - 3;
    }
    void updateScore()
    {
        string formatedScore = "Score: ";
        if (-centredScore() - 3)
            formatedScore += " ";
        formatedScore += to_string(BodyCoords.size() - STARTING_LENGTH - 1);

        console.setTextColor(FOREGROUND_GREEN);
        gotoxy(console.WIDTH / 2 + centredScore(), 1);
        cout << formatedScore;
        console.setTextColor(FOREGROUND_DEFAULT);
    }
    void checkIfEaten()
    {
        if (head() != fruit)
            return;

        BodyCoords.push_back(prevTail);
        updateScore();
        spawnFruit();
    }
    bool isSpawnedInsideBody()
    {
        for (int i = 0; i < BodyCoords.size(); i++)
            if (fruit == BodyCoords[i])
                return true;
        return false;
    }
    void displayFruit()
    {
        console.setTextColor(FOREGROUND_RED);
        gotoxy(fruit.x, fruit.y, '*');
        console.setTextColor(FOREGROUND_DEFAULT);
    }
    void spawnFruit()
    {
        do {
            fruit.x = random(2, console.WIDTH - 2);
            fruit.y = random(2, console.HEIGHT - 2);
        } while (isSpawnedInsideBody());

        displayFruit();
    }


    void checkColision()
    {
        for (int i = 1; i < BodyCoords.size(); i++)
        {
            if (head() != BodyCoords[i])
                continue;

            bool again = false;
            while (!again)
            {
                draw::gameOverScreen(BodyCoords);
                chooseMenuOption(again);
            }
            return;
        }
    }

    void chooseScoresMenuOption()
    {
        char menuOption;
        while (true)
        {
            cin >> menuOption;
            switch (menuOption)
            {
            case '1':
                loadScoresToScreen("Highscores");
                return;
            case '2':
                loadScoresToScreen("Last_Scores");
                return;
            }
        }
    }

    void sayGoodbye()
    {
        system("CLS");
        gotoxy(console.WIDTH / 2 - 3, console.HEIGHT / 2);
        cout << "Żegnaj! :)";
        Sleep(1500);
        exit(0);
    }

    void chooseMenuOption(bool& again)
    {
        while (true)
        {
            char option;
            cin >> option;
            switch (option)
            {
            case '1':
                again = true;
                restart();
                return;
            case '2':
                sayGoodbye();
                return;
            case '3':
                draw::scoresMenu();
                chooseScoresMenuOption();

                _getch();
                system("CLS");
                draw::board();
                return;
            }
        }
    }

    void checkGameOver()
    {
        if (gameOverTicks > 2)
        {
            saveLastScore();
            saveHighscore();
            bool again = false;
            while (!again)
            {
                draw::gameOverScreen(BodyCoords);
                chooseMenuOption(again);
            }
        }
        else
            gameOverTicks++;
    }

    void moveCoords()
    {
        prevTail = tail;
        POINT prevPos1 = BodyCoords[0];
        if (BodyCoords[0] == currPos) //probably death delay (commented after a year)
            return;
        else
        {
            BodyCoords[0] = currPos;
            for (int i = 1; i < BodyCoords.size(); i++)
                swap(prevPos1, BodyCoords[i]);

            checkIfEaten();
            checkColision();
            tail = BodyCoords[BodyCoords.size() - 1];
        }
    }

    void preventGettingIntoOwnBody(char& key, char& prevKey)
    {
        switch (key)
        {
        case 'W':
            if (prevKey == 'S')
                key = 'S';
            break;
        case 'S':
            if (prevKey == 'W')
                key = 'W';
            break;
        case 'A':
            if (prevKey == 'D')
                key = 'D';
            break;
        case 'D':
            if (prevKey == 'A')
                key = 'A';
            break;
        }
    }


    void moveCoord(LONG& coord, bool isInRange, int distanceToMove)
    {
        if (!isInRange)
        {
            checkGameOver();
            return;
        }

        coord += distanceToMove;
        gameOverTicks = 0;
    }


    void moving(int key)
    {
        switch (key)
        {
        case 'W':
            moveCoord(currPos.y, currPos.y > 2, -1);
            Sleep(V_SPEED);
            break;

        case 'A':
            moveCoord(currPos.x, currPos.x > 2, -1);
            Sleep(H_SPEED);
            break;

        case 'S':
            moveCoord(currPos.y, currPos.y < console.HEIGHT - 1, 1);
            Sleep(V_SPEED);
            break;

        case 'D':
            moveCoord(currPos.x, currPos.x < console.WIDTH - 1, 1);
            Sleep(H_SPEED);
            break;
        }
    }

    void move()
    {
        char key = toupper(direction);
        char prevKey = toupper(prevDirection);

        if (key != 'W' && key != 'S' && key != 'A' && key != 'D')
            key = prevKey;

        preventGettingIntoOwnBody(key, prevKey);
        direction = key;

        moving(key);
        moveCoords();
        draw::snake(BodyCoords, tail);
    }


    void saveLastScore()
    {
        int scores[10];
        string dates[10];

        //saving scores in arrays to have the newest one on top
        ifstream lastScoreRead("Pliki/Last_Scores.txt");
        _mkdir("Pliki");

        int i = 0; string line;
        while (getline(lastScoreRead, line, '.')) //deleting number (ex. "1.")
        {
            getline(lastScoreRead, line, ':'); //deleting "Score:" word
            getline(lastScoreRead, line, '>'); //reading and converting score to a number
            scores[i] = stoi(line);
            getline(lastScoreRead, dates[i]); //reading date
            getline(lastScoreRead, line); //deleting empty line
            i++;
            if (i > 7) break;
        }

        lastScoreRead.close();

        //writing scores
        ofstream lastScore("Pliki/Last_Scores.txt");

        int score = BodyCoords.size() - STARTING_LENGTH - 1;
        lastScore << "1. Score: " << score << " -----> " << "Date: " << date::getDate() << endl << endl;
        for (int j = 2; j <= i + 1; j++)
            lastScore << j << ". " << "Score: " << scores[j - 2] << " -" << dates[j - 2] << endl << endl;

        lastScore.close();
    }

    void readingScores(string filename, array<pair<int, string>, 10>& scores, int& i)
    {
        ifstream scoresFile(filename);
        string line;
        while (getline(scoresFile, line, '.')) //deleting number (ex. "1.")
        {
            getline(scoresFile, line, ':'); //deleting "Score:" word
            getline(scoresFile, line, '>'); //reading and converting score to a number
            scores[i].first = stoi(line);
            getline(scoresFile, scores[i].second); //reading date
            getline(scoresFile, line); //deleting empty line
            i++;
            if (i > 9) break;
        }
        scoresFile.close();
    }

    void saveHighscore()
    {
        int score = BodyCoords.size() - STARTING_LENGTH - 1;

        if (score > 0)
        {
            array<pair <int, string>, 10> scores;
            int n = sizeof(scores) / sizeof(scores[0]);

            //saving scores in array of pairs to have the highest one on top
            _mkdir("Pliki");
            int i = 1;
            readingScores("Pliki/Highscores.txt", scores, i);

            sort(scores.begin(), scores.end(), comp);


            string date = " Date: " + date::getDate();
            if (scores[9].first < score)
            {
                scores[9].first = score;
                scores[9].second = date;
            }

            sort(scores.begin(), scores.end(), comp);

            fstream lastScore;
            lastScore.open("Pliki/Highscores.txt", ios::out);

            for (int j = 1; j <= i; j++)
                lastScore << j << ". " << "Score: " << scores[j - 1].first << " ----->" << scores[j - 1].second << endl << endl;

            lastScore.close();
        }
    }

    void loadScoresToScreen(string plik)
    {
        system("CLS");

        ifstream fileScoreRead;
        fileScoreRead.open("Pliki/" + plik + ".txt");
        float p = 1.5;
        int width = 40 * p, height = 15 * p - 1;
        console.centerWindow(width, height);

        //BORDERS
        gotoxy(1, 1); for (int i = 0; i < width - 1; i++) cout << "#";
        for (int i = 0; i < height; i++) cout << "#" << endl;
        for (int i = 0; i < height; i++) { gotoxy(width, i); cout << "#"; }
        gotoxy(1, height); for (int i = 0; i < width; i++) cout << "#";

        int i = 0; string line;
        while (getline(fileScoreRead, line)) //reading score
        {
            int Score_x = width / 2 - 22;
            int Score_y = 3;

            for (auto& c : plik) c = toupper(c);
            gotoxy(width / 2 - plik.length() / 2 + 1, 2); cout << plik;

            gotoxy(Score_x, Score_y + i * 2);
            cout << line << endl;

            gotoxy(Score_x, Score_y + i * 2 + 1);

            getline(fileScoreRead, line); //deleting empty line
            i++; if (i > 8) break;
        }

        fileScoreRead.close();
    }


    //PREPARATION STUFF
    Object()
    {
        draw::board();
        fillBodyCoords();
        draw::snake(BodyCoords, tail);
        spawnFruit();
        gotoxy(console.WIDTH / 2 - 3, 1);
        cout << "Score: " << 0;
    }

    void restart()
    {
        system("CLS");
        currPos = { console.WIDTH / 4, console.HEIGHT / 2 };
        //snake reset
        direction = 'D'; prevDirection = 'D';
        tail = { currPos.x - STARTING_LENGTH, currPos.y };

        draw::board();
        BodyCoords.clear();
        fillBodyCoords();
        gotoxy(console.WIDTH / 2 - 3, 1); cout << "Score: " << BodyCoords.size() - STARTING_LENGTH - 1;
        spawnFruit();
    }


private:
    POINT prevTail;
    void fillBodyCoords()
    {
        BodyCoords.push_back({ console.WIDTH / 4, console.HEIGHT / 2 });
        for (int i = 1; i < STARTING_LENGTH; i++)
            BodyCoords.push_back({ head().x - i, head().y });
        BodyCoords.push_back(tail);
    }

} snake;

int main()
{
    srand(time(NULL));
    while (true)
    {
        while (_kbhit())
            direction = _getch();

        //direction output /////DO DOPRACOWANIA!!!/////
            //gotoxy(10, 20); 
            //cout << "PD: " << prevDirection << " D: " << direction;

        snake.move();
        prevDirection = direction;
    }
}