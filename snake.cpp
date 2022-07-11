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
#include <array>
#pragma warning(disable : 4996)
#define FOREGROUND_DEFAULT 7

using namespace std;

inline bool operator== (POINT a, POINT b) { return ((a.x == b.x) && (a.y == b.y)); }
inline bool operator!= (POINT a, POINT b) { return !(a == b); }

void gotoxy(int x, int y)
{
    COORD c;
    c.X = x - 1;
    c.Y = y - 1;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void gotoxy(int x, int y, char character)
{
    gotoxy(x, y);
    cout << character;
}

int random(int rangeStart, int rangeEnd)
{
    return (rand() % rangeEnd) + rangeStart;
}

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
        SetConsoleTextAttribute(hConsole, color | FOREGROUND_INTENSITY);
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

namespace draw
{
    const int STARTING_LENGTH = 4; // fix to Object::STARTING_LENGTH later

    namespace border
    {
        void topBorder()
        {
            gotoxy(1, 1);
            for (int i = 1; i <= console.WIDTH; i++)
                cout << "$";
        }

        void leftBorder()
        {
            for (int i = 1; i <= console.HEIGHT; i++)
                gotoxy(1, i, '$');
        }

        void rightBorder()
        {
            for (int i = 1; i <= console.HEIGHT; i++)
                gotoxy(console.WIDTH, i, '$');
        }

        void bottomBorder()
        {
            gotoxy(1, console.HEIGHT);
            for (int i = 1; i <= console.WIDTH; i++)
                cout << "$";
        }
    }

    void board()
    {
        border::topBorder();
        border::leftBorder();
        border::rightBorder();
        border::bottomBorder();
    }
    
    void snake(vector <POINT> BodyCoords, POINT tail)
    {
        gotoxy(BodyCoords[0].x, BodyCoords[0].y, '@');
        for (int i = 1; i < BodyCoords.size() - 1; i++)
            gotoxy(BodyCoords[i].x, BodyCoords[i].y, '#');
        
        gotoxy(tail.x, tail.y, ' ');
        gotoxy(1, 1);
    }
    
    void playAgainMenu()
    {
        const pair<int, int> MIDDLE = { console.WIDTH / 2 - 5, console.HEIGHT / 2 };
        gotoxy(MIDDLE.first , MIDDLE.second + 2);
        cout << "1. Play again";
        gotoxy(MIDDLE.first, MIDDLE.second + 3);
        cout << "2. Exit";
        gotoxy(MIDDLE.first, MIDDLE.second + 4);
        cout << "3. Menu";

        gotoxy(MIDDLE.first, MIDDLE.second + 5);
        cout << "Choose option: ";
        console.centerWindow(console.WIDTH, console.HEIGHT);
    }
    
    void scoresMenu()
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

    void gameOverScreen(vector <POINT> BodyCoords)
    {
        system("CLS");
        console.setTextColor(FOREGROUND_RED );
        gotoxy(console.WIDTH / 2 - 7, console.HEIGHT / 2 - 2); 
        cout << "G A M E   O V E R";
        gotoxy(console.WIDTH / 2 - 5, console.HEIGHT / 2); 
        cout << "Your score: " << BodyCoords.size() - STARTING_LENGTH - 1;
        console.setTextColor(FOREGROUND_DEFAULT);

        playAgainMenu();
    }
    
}

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

    void readingScores(string filename, array<pair<int,string>, 10>& scores, int& i)
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
        tail = { currPos.x - STARTING_LENGTH, currPos.y};

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
