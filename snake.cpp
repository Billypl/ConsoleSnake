#include <iostream>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <string>
#include <cstdlib>
#include <vector>
#include <utility>
#include <ctime>
#include <fstream>
#include <cwchar>
#include <direct.h>
#include <algorithm>
#pragma warning(disable : 4996)
#define FOREGROUND_DEFAULT 7

using namespace std;

namespace date
{
    string fDate(int date)
    {
        string F_date = to_string(date);
        if (date < 10)
            F_date = "0" + to_string(date);

        return F_date;
    }

    string getDate()
    {
        time_t now = time(0);
        tm ltm = (*localtime(&now));

        int day = ltm.tm_mday;
        int month = 1 + ltm.tm_mon;
        int year = 1900 + ltm.tm_year;

        int hour = ltm.tm_hour;
        int min = ltm.tm_min; 
        int sec = ltm.tm_sec; 

        string date = fDate(day) + "-" 
            + fDate(month) + "-" 
            + fDate(year) + "   ";
        
        date += fDate(hour) + ":" 
            + fDate(min) + ":" 
            + fDate(sec);

        return date;
    }
}

void gotoxy(int x, int y)
{
    COORD c;
    c.X = x - 1;
    c.Y = y - 1;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

class Console
{
    const bool CENTERING = true;
    CONSOLE_FONT_INFOEX cfi;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    HWND hDesktop = GetDesktopWindow();
    HWND console = GetConsoleWindow();
    RECT desktop = GetWindowRectangle();

    RECT GetWindowRectangle()
    {
        RECT desktop;
        GetWindowRect(hDesktop, &desktop);
        return desktop;
    }

    void makeScrollAppear()
    {
        COORD c;
        c.X = WIDTH + 100; c.Y = 9000;
        SetConsoleScreenBufferSize(hConsole, c);
    }

public:

    const int WIDTH = 40, const HEIGHT = 15;

    Console()
    {
        changeFont();
        centerWindow(WIDTH, HEIGHT);
        makeScrollAppear(); //doesnt work
    }

    void setTextColor(UINT color)
    {
        SetConsoleTextAttribute(hConsole, color);
    }

    void centerWindow(int width, int height)
    {
        if (CENTERING != true)
            return;
        
        const pair<int, int> MARGIN = {36, 60};
        width = width * cfi.dwFontSize.X + MARGIN.first;
        height = height * cfi.dwFontSize.Y + MARGIN.second;

        MoveWindow(console, (desktop.right / 2) - (width / 2), (desktop.bottom / 2) - (height / 2), width, height, TRUE);
    }

    void changeFont()
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

} console;

const int H_SPEED = 150;
const int V_SPEED = H_SPEED * 1.4;

POINT current = { console.WIDTH / 4, console.HEIGHT / 2 };

char direction = 'D';
char prevDirection = direction;


bool comp(pair <int, string> i, pair <int, string> j)
{
    return (i.first > j.first);
}

class Object
{
public:
    const int STARTING_LENGTH = 4;
    vector <POINT> BodyCoords;

    int gameOverTicks = 2; //ticks when player bumps into the wall

    POINT fruit;
    POINT head = current;
    POINT tail = { current.x - STARTING_LENGTH, current.y }; //tail is actually 1 block behind the visible tail

    void checkIfEaten()
    {
        if (head.x == fruit.x && head.y == fruit.y)//prevTailX == TfruitX && prevTailY == TfruitY)
        {
            //creating new tail
            BodyCoords.emplace_back(POINT{ prevTailX, prevTailY });
            //"deleting" fruit
            TfruitX = 0; TfruitY = 0;
            ///} 

                //if to avoid spawning multiple fruits at the same time
                //and to not make infinite snake 
                //(fruit coords staying it the same place/dissapearing immidiately and not counting as snake segments)
            ///if (headX == fruitX && headY == fruitY)
            ///{
            TfruitX = fruit.x;
            TfruitY = fruit.y;

            ///centred SCORE text///
            int centred = -(int)log10((BodyCoords.size() - 1 - 2)) - 3;
            string word = "Score: ";
            if ((-centred) - 3)
                word += " ";
            console.setTextColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            gotoxy(console.WIDTH / 2 + centred, 1); cout << word << BodyCoords.size() - STARTING_LENGTH - 1;
            console.setTextColor(FOREGROUND_DEFAULT);
            spawnFruit();
        }
    }
    void spawnFruit()
    {
        srand(time(NULL));

        while (true) //if fruit coords are the same as any of snake segments, draw coords again
        {
            fruit.x = rand() % (console.WIDTH - 2) + 2;
            fruit.y = rand() % (console.HEIGHT - 2) + 2;
            bool out = false;

            for (int i = 0; i < BodyCoords.size(); i++)
            {
                if (fruit.x != BodyCoords[i].x && fruit.y != BodyCoords[i].y)
                {
                    out = true;
                    break;
                }
            }
            if (out) 
                break;
        }
        console.setTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        gotoxy(fruit.x, fruit.y);
        cout << "*";
        console.setTextColor(FOREGROUND_DEFAULT);
    }


    void checkColision()
    {
        for (int i = 1; i < BodyCoords.size(); i++)
        {
            if (head.x == BodyCoords[i].x && head.y == BodyCoords[i].y)
            {
                bool again = false;
                while (!again)
                {
                    drawGameOverScreen();
                    chooseMenuOption(again);
                }
                break;
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
                drawGameOverScreen();
                chooseMenuOption(again);
            }
        }
        else
            gameOverTicks++;
    }


    void moveCoords()
    {
        prevTailX = tail.x;
        prevTailY = tail.y; 
        POINT prevPos1 = BodyCoords[0];
        POINT prevPos2;
        if (BodyCoords[0].x == current.x && BodyCoords[0].y == current.y) //probably death delay (comment after a year)
            return;
        else
        {
            BodyCoords[0] = current;
            for (int i = 1; i < BodyCoords.size(); i++)
            {
                prevPos2 = BodyCoords[i];
                BodyCoords[i] = prevPos1;
                prevPos1 = prevPos2;
            }
            head = current;
            checkIfEaten();
            checkColision();
            tail.x = BodyCoords[BodyCoords.size() - 1].x;
            tail.y = BodyCoords[BodyCoords.size() - 1].y;

        }
    }
    void move()//char& key, char& prevKey)
    {
        char key = direction;
        char prevKey = prevDirection;
        key = toupper(key);
        prevKey = toupper(prevKey);

        //if any other letter - continue current direvtion
        if (key != 'W' && key != 'S' && key != 'A' && key != 'D')
            key = prevKey;

        //if direction is inverted, keep current dirrection (sneak can't get into it's own body)
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
        direction = key;


        //moving
        switch (key)
        {
        case 'W':
            if (current.y > 2)
            {
                current.y -= 1;
                gameOverTicks = 0;
            }
            else
                checkGameOver();

            Sleep(V_SPEED);
            break;

        case 'A':
            if (current.x > 2)
            {
                current.x -= 1;
                gameOverTicks = 0;
            }
            else
                checkGameOver();

            Sleep(H_SPEED);
            break;

        case 'S':
            if (current.y < console.HEIGHT - 1)
            {
                current.y += 1;
                gameOverTicks = 0;
            }
            else
                checkGameOver();

            Sleep(V_SPEED);
            break;

        case 'D':
            if (current.x < console.WIDTH - 1)
            {
                current.x += 1;
                gameOverTicks = 0;
            }
            else
                checkGameOver();

            Sleep(H_SPEED);
            break;
        }

        moveCoords();
        drawSnake();
    }


    void drawBoard()
    {
        //bottom border
        gotoxy(1, 1);
        for (int i = 1; i <= console.WIDTH; i++)
            cout << "$";
        //left border
        for (int i = 1; i <= console.HEIGHT; i++)
        {
            gotoxy(1, i);
            cout << "$";
        }
        //right border
        for (int i = 1; i <= console.HEIGHT; i++)
        {
            gotoxy(console.WIDTH, i);
            cout << "$";
        }
        //top border
        gotoxy(1, console.HEIGHT);
        for (int i = 1; i <= console.WIDTH; i++)
            cout << "$";
    }
    void drawSnake()
    {
        //clearing space after tail
        gotoxy(tail.x, tail.y);
        cout << " ";

        //drawing snake
        gotoxy(BodyCoords[0].x, BodyCoords[0].y);
        cout << "@";
        for (int i = 1; i < BodyCoords.size() - 1; i++)
        {
            gotoxy(BodyCoords[i].x, BodyCoords[i].y);
            cout << "#";
        }
        gotoxy(1, 1);
    }
    void drawPlayAgainMenu()
    {
        gotoxy(console.WIDTH / 2 - 5, console.HEIGHT / 2 + 2);
        cout << "1. Play again";
        gotoxy(console.WIDTH / 2 - 5, console.HEIGHT / 2 + 3);
        cout << "2. Exit";
        gotoxy(console.WIDTH / 2 - 5, console.HEIGHT / 2 + 4);
        cout << "3. Menu";

        gotoxy(console.WIDTH / 2 - 5, console.HEIGHT / 2 + 5);
        cout << "Choose option: ";
        console.centerWindow(console.WIDTH, console.HEIGHT);
    }
    void drawGameOverScreen()
    {
        //clearing board
        clearBoard();
        //outputing "GAME OVER"
        console.setTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        gotoxy(console.WIDTH / 2 - 7, console.HEIGHT / 2 - 2); cout << "G A M E   O V E R";
        gotoxy(console.WIDTH / 2 - 5, console.HEIGHT / 2); cout << "Your score: " << BodyCoords.size() - STARTING_LENGTH - 1;
        console.setTextColor(FOREGROUND_DEFAULT);

        drawPlayAgainMenu();
    }
    void clearBoard()
    {
        gotoxy(1, 2);
        for (int i = 2; i < console.HEIGHT; i++)
        {
            cout << "$";
            for (int j = 2; j < console.WIDTH; j++)
                cout << " ";
            cout << endl;
        }
    }


    void chooseMenuOption(bool& again)
    {
        char option;
        bool correctOption = false;
        while (!correctOption)
        {
            cin >> option;
            switch (option)
            {
            case '1':
                //option_restart();
                correctOption = true;
                again = true;
                restart();
                break;
            case '2':
                system("CLS");
                gotoxy(console.WIDTH / 2 - 3, console.HEIGHT / 2);  cout << "Żegnaj! :)";
                Sleep(1500);
                exit(0);
                break;
            case '3':
                correctOption = true;
                clearBoard();

                gotoxy(console.WIDTH / 2 - 6, console.HEIGHT / 2);
                cout << "1. Scoreboard";
                gotoxy(console.WIDTH / 2 - 6, console.HEIGHT / 2 + 1);
                cout << "2. Last scores";
                gotoxy(console.WIDTH / 2 - 6, console.HEIGHT / 2 + 2);
                cout << "3. Settings (not ready)";
                gotoxy(console.WIDTH / 2 - 6, console.HEIGHT / 2 + 3);
                cout << "Choose option: ";


                bool correctMenuOption = false;
                char menuOption;
                while (!correctMenuOption)
                {
                    cin >> menuOption;
                    switch (menuOption)
                    {
                    case '1':
                        correctMenuOption = true;
                        loadScoresToScreen("Highscores");
                        break;
                    case '2':
                        correctMenuOption = true;
                        loadScoresToScreen("Last_Scores");
                        break;
                    }

                }

                //if ((width / 2 - 13) > 0)
                //{
                //    gotoxy(width / 2 - 15, height / 2 + 1);
                //    cout << "Press any key to go back to menu";
                //}
                //else //else for exeption where width is not big enough
                //{
                //    gotoxy(2, height / 2 + 1);
                //    cout << "Press any key";
                //}
                _getch();
                system("CLS");
                drawBoard();
                break;
            }
        }
    }

    void saveLastScore()
    {
        int scores[10];
        string dates[10];

        //saving scores in arrays to have the newest one on top
        ifstream lastScoreRead;
        _mkdir("Pliki");
        lastScoreRead.open("Pliki/Last_Scores.txt");

        int i = 0; string line;
        while (getline(lastScoreRead, line, '.')) //deleting number (ex. "1.")
        {
            getline(lastScoreRead, line, ':'); //deleting "Score:" word
            getline(lastScoreRead, line, '-'); //reading and converting score to a number
            scores[i] = stoi(line);
            getline(lastScoreRead, dates[i]); //reading date
            getline(lastScoreRead, line); //deleting empty line
            i++;
            if (i > 7) break;
        }

        lastScoreRead.close();


        //writing scores
        fstream lastScore;
        lastScore.open("Pliki/Last_Scores.txt", ios::out);

        int score = BodyCoords.size() - STARTING_LENGTH - 1;
        lastScore << "1. Score: " << score << " -----> " << "Date: " << date::getDate() << endl << endl;
        for (int j = 2; j <= i + 1; j++)
            lastScore << j << ". " << "Score: " << scores[j - 2] << " -" << dates[j - 2] << endl << endl;

        lastScore.close();
    }
    void saveHighscore()
    {
        int score = BodyCoords.size() - STARTING_LENGTH - 1;

        if (score > 0)
        {
            pair <int, string> scores[10];
            int n = sizeof(scores) / sizeof(scores[0]);

            //saving scores in array of pairs to have the highest one on top
            ifstream lastScoreRead;
            _mkdir("Pliki");
            lastScoreRead.open("Pliki/Highscores.txt");

            int i = 1; string line;
            while (getline(lastScoreRead, line, '.')) //deleting number (ex. "1.")
            {
                getline(lastScoreRead, line, ':'); //deleting "Score:" word
                getline(lastScoreRead, line, '>'); //reading and converting score to a number
                scores[i].first = stoi(line);
                getline(lastScoreRead, scores[i].second); //reading date
                getline(lastScoreRead, line); //deleting empty line
                i++;
                if (i > 9) break;
            }

            lastScoreRead.close();

            sort(scores, scores + n, comp);


            string date = " Date: " + date::getDate();
            if (scores[9].first < score)
            {
                scores[9].first = score;
                scores[9].second = date;
            }

            sort(scores, scores + n, comp);

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
        drawBoard();
        fillBodyCoords();
        gotoxy(console.WIDTH / 2 - 3, 1); 
        cout << "Score: " << 0;
    }

    void restart()
    {
        system("CLS");
        current = { console.WIDTH / 4, console.HEIGHT / 2 };
        //snake reset
        direction = 'D'; prevDirection = 'D';
        head = current;
        tail = { current.x - STARTING_LENGTH, current.y};

        drawBoard();
        BodyCoords.clear();
        fillBodyCoords();
        gotoxy(console.WIDTH / 2 - 3, 1); cout << "Score: " << BodyCoords.size() - STARTING_LENGTH - 1;
        spawnFruit();
    }


private:
    int prevTailY, prevTailX;
    int TfruitY, TfruitX;
    void fillBodyCoords()
    {
        BodyCoords.emplace_back(POINT{ head.x, head.y });
        for (int i = 1; i < STARTING_LENGTH; i++)
            BodyCoords.emplace_back(POINT{ head.x, head.y - i });
        BodyCoords.emplace_back(POINT{ tail.x, tail.y });

        /*for (const auto& [x, y] : BodyCoords)
            cout << x << ' ' << y << '\n';*/

    }

};


Object snake;


int main()
{


    //GAME PREPARATIONS
    snake.drawSnake();

    snake.spawnFruit();
    while (true)
    {
        while (_kbhit()) direction = _getch();

        //direction output /////DO DOPRACOWANIA!!!/////
            //gotoxy(10, 20); 
            //cout << "PD: " << prevDirection << " D: " << direction;

        snake.move();// direction, prevDirection);
        prevDirection = direction;
    }
}
