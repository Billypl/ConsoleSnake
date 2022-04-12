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

using namespace std;

namespace date
{
    string f_date(int date)
    {
        string F_date = to_string(date);
        if (date < 10)
        {
            F_date = "0" + to_string(date);
        }
        return F_date;
    }

    string get_date()
    {
        time_t now = time(0);
        tm* ltm = localtime(&now);

        int day = ltm->tm_mday;
        int month = 1 + ltm->tm_mon;
        int year = 1900 + ltm->tm_year;

        int hour = ltm->tm_hour;// << ":";
        int min = ltm->tm_min; //<< ":";
        int sec = ltm->tm_sec; //<< endl;

        string dt = f_date(day) + "-" + f_date(month) + "-" + f_date(year);
        dt = dt + "   " + f_date(hour) + ":" + f_date(min) + ":" + f_date(sec);
        //dt = ctime(&now);

        return dt;
    }
}

void gotoxy(int x, int y)
{
    COORD c;
    c.X = x - 1;
    c.Y = y - 1;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);


const int width = 40, height = 15;
const int tabWidth = width + 1, tabHeight = height + 1;

const int h_speed = 150;
const int v_speed = h_speed * 1.4;
const bool centering = true;

void CenterWindow(int width, int height)
{
    if (centering != true)
        return;

    width = width * 13 + 36; height = height * 24 + 60;
    //pobieranie rozmiaru całego ekranu
    int s_h = 0, s_v = 0;

    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    s_h = desktop.right;
    s_v = desktop.bottom;

    //przygotowanie do przesunięcia okna 
    HWND console = GetConsoleWindow();

    //CENTROWANIE OKNA
    MoveWindow(console, (s_h / 2) - (width / 2), (s_v / 2) - (height / 2), width, height, TRUE);
}

void change_font()
{
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;                   // Width of each character in the font
    cfi.dwFontSize.Y = 24;                  // Height
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_BOLD;
    wcscpy_s(cfi.FaceName, L"Liberation Mono"); // Choose your font
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}


int currX = width / 4, currY = height / 2;

char direction = 'D';
char prevDirection = direction;

bool comp(pair <int, string> i, pair <int, string> j)
{
    return (i.first > j.first);
}

class Object
{
public:
    int length = 4;
    const int startingLength = length;
    vector <pair<int, int>> BodyCoords;

    bool gameOver = false;
    int gameOverTicks = 2; //ticks when player bumps into the wall

    int fruitY, fruitX;
    int headY = currY, headX = currX;
    int tailY = currY, tailX = currX - length; //tail is actually 1 block behind the visible tail


    //TODO: BodyCoords[0].first is Y and vice versa - change that

    void check_if_eaten()
    {
        if (headX == fruitX && headY == fruitY)//prevTailX == TfruitX && prevTailY == TfruitY)
        {
            //creating new tail
            BodyCoords.emplace_back(prevTailY, prevTailX);
            length++;
            //"deleting" fruit
            TfruitX = 0; TfruitY = 0;
            ///} 

                //if to avoid spawning multiple fruits at the same time
                //and to not make infinite snake 
                //(fruit coords staying it the same place/dissapearing immidiately and not counting as snake segments)
            ///if (headX == fruitX && headY == fruitY)
            ///{
            TfruitX = fruitX;
            TfruitY = fruitY;

            ///centred SCORE text///
            int centred = -(int)log10((BodyCoords.size() - 1 - 2)) - 3;
            string word = "Score: ";
            if ((-centred) - 3)
                word += " ";
            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            gotoxy(width / 2 + centred, 1); cout << word << BodyCoords.size() - startingLength - 1;
            SetConsoleTextAttribute(hConsole, 7);
            spawn_fruit();
        }
    }
    void spawn_fruit()
    {
        srand(time(NULL));
        int fruitX = width, fruitY;

        while (true) //if fruit coords are the same as any of snake segments, draw coords again
        {
            fruitX = rand() % (width - 2) + 2;
            fruitY = rand() % (height - 2) + 2;
            bool out = false;

            for (int i = 0; i < BodyCoords.size(); i++)
            {
                if (fruitX != BodyCoords[i].second && fruitY != BodyCoords[i].first)
                {
                    out = true;
                    break;
                }
            }
            if (out) break;
        }
        this->fruitX = fruitX;
        this->fruitY = fruitY;

        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
        gotoxy(fruitX, fruitY);
        cout << "*";
        SetConsoleTextAttribute(hConsole, 7);
    }


    void check_colision()
    {
        for (int i = 1; i < BodyCoords.size(); i++)
        {
            if (headX == BodyCoords[i].second && headY == BodyCoords[i].first)
            {
                bool again = false;
                while (!again)
                {
                    draw_game_over();
                    choose_menu_option(again);
                }
                //gameOver = true;
                break;
            }
        }
    }
    void check_game_over()
    {
        if (gameOverTicks > 2)
        {
            save_last_score();
            save_highscore();
            bool again = false;
            while (!again)
            {
                draw_game_over();
                choose_menu_option(again);
            }
        }
        else
            gameOverTicks++;
    }


    void move_coords()
    {
        prevTailY = tailY; prevTailX = tailX;
        pair <int, int> prevPos1 = BodyCoords[0];
        pair <int, int> prevPos2;
        if (BodyCoords[0].first == currY && BodyCoords[0].second == currX)
        {
        }
        else
        {
            BodyCoords[0] = { currY, currX };
            for (int i = 1; i < BodyCoords.size(); i++)
            {
                prevPos2 = BodyCoords[i];
                BodyCoords[i] = prevPos1;
                prevPos1 = prevPos2;
            }
            headY = currY, headX = currX;
            check_if_eaten();
            check_colision();
            tailY = BodyCoords[BodyCoords.size() - 1].first;
            tailX = BodyCoords[BodyCoords.size() - 1].second;

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
            if (currY > 2)
            {
                currY -= 1;
                gameOverTicks = 0;
            }
            else
                check_game_over();

            Sleep(v_speed);
            break;

        case 'A':
            if (currX > 2)
            {
                currX -= 1;
                gameOverTicks = 0;
            }
            else
                check_game_over();

            Sleep(h_speed);
            break;

        case 'S':
            if (currY < height - 1)
            {
                currY += 1;
                gameOverTicks = 0;
            }
            else
                check_game_over();

            Sleep(v_speed);
            break;

        case 'D':
            if (currX < width - 1)
            {
                currX += 1;
                gameOverTicks = 0;
            }
            else
                check_game_over();

            Sleep(h_speed);
            break;
        }

        move_coords();
        draw_snake();
    }


    void draw_board()
    {
        //bottom border
        gotoxy(1, 1);
        for (int i = 1; i <= width; i++)
            cout << "$";
        //left border
        for (int i = 1; i <= height; i++)
        {
            gotoxy(1, i);
            cout << "$";
        }
        //right border
        for (int i = 1; i <= height; i++)
        {
            gotoxy(width, i);
            cout << "$";
        }
        //top border
        gotoxy(1, height);
        for (int i = 1; i <= width; i++)
            cout << "$";
    }
    void draw_snake()
    {
        //clearing space after tail
        gotoxy(tailX, tailY);
        cout << " ";

        //drawing snake
        gotoxy(BodyCoords[0].second, BodyCoords[0].first);
        cout << "@";
        for (int i = 1; i < BodyCoords.size() - 1; i++)
        {
            gotoxy(BodyCoords[i].second, BodyCoords[i].first);
            cout << "#";
        }
        gotoxy(1, 1);
    }
    void draw_play_again()
    {
        gotoxy(width / 2 - 5, height / 2 + 2);
        cout << "1. Play again";
        gotoxy(width / 2 - 5, height / 2 + 3);
        cout << "2. Exit";
        gotoxy(width / 2 - 5, height / 2 + 4);
        cout << "3. Menu";

        gotoxy(width / 2 - 5, height / 2 + 5);
        cout << "Choose option: ";
        CenterWindow(width, height);

        COORD c; c.X = width + 10; c.Y = 9000;
        SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), c);

    }
    void draw_game_over()
    {
        //clearing board
        clear_board();
        //outputing "GAME OVER"
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
        gotoxy(width / 2 - 7, height / 2 - 2); cout << "G A M E   O V E R";
        gotoxy(width / 2 - 5, height / 2); cout << "Your score: " << BodyCoords.size() - startingLength - 1;
        SetConsoleTextAttribute(hConsole, 7);

        draw_play_again();
    }
    void clear_board()
    {
        gotoxy(1, 2);
        for (int i = 2; i < height; i++)
        {
            cout << "$";
            for (int j = 2; j < width; j++)
                cout << " ";
            cout << endl;
        }
    }


    void choose_menu_option(bool& again)
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
                option_restart();
                break;
            case '2':
                system("CLS");
                gotoxy(width / 2 - 3, height / 2);  cout << "Żegnaj! :)";
                Sleep(1500);
                exit(0);
                break;
            case '3':
                correctOption = true;
                clear_board();

                gotoxy(width / 2 - 6, height / 2);
                cout << "1. Scoreboard";
                gotoxy(width / 2 - 6, height / 2 + 1);
                cout << "2. Last scores";
                gotoxy(width / 2 - 6, height / 2 + 2);
                cout << "3. Settings (not ready)";
                gotoxy(width / 2 - 6, height / 2 + 3);
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
                        load_scores_to_screen("Highscores");
                        break;
                    case '2':
                        correctMenuOption = true;
                        load_scores_to_screen("Last_Scores");
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
                draw_board();
                break;
            }
        }
    }

    void save_last_score()
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

        int score = BodyCoords.size() - startingLength - 1;
        lastScore << "1. Score: " << score << " -----> " << "Date: " << date::get_date() << endl << endl;
        for (int j = 2; j <= i + 1; j++)
            lastScore << j << ". " << "Score: " << scores[j - 2] << " -" << dates[j - 2] << endl << endl;

        lastScore.close();
    }
    void save_highscore()
    {
        int score = BodyCoords.size() - startingLength - 1;

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


            string date = " Date: " + date::get_date();
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

    void load_scores_to_screen(string plik)
    {
        system("CLS");

        ifstream fileScoreRead;
        fileScoreRead.open("Pliki/" + plik + ".txt");
        float p = 1.5;
        int width = 40 * p, height = 15 * p - 1;
        CenterWindow(width, height);

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
        draw_board();
        fill_BodyCoords();
        gotoxy(width / 2 - 3, 1); 
        cout << "Score: " << 0;
    }

    void option_restart()
    {
        system("CLS");
        currX = width / 4; currY = height / 2;
        //snake reset
        gameOver = false;
        direction = 'D'; prevDirection = 'D';
        headY = currY, headX = currX;
        tailY = currY, tailX = currX - startingLength;

        draw_board();
        BodyCoords.clear();
        fill_BodyCoords();
        gotoxy(width / 2 - 3, 1); cout << "Score: " << BodyCoords.size() - startingLength - 1;
        spawn_fruit();
    }


private:
    int prevTailY, prevTailX;
    int TfruitY, TfruitX;
    void fill_BodyCoords()
    {
        BodyCoords.emplace_back(headY, headX);
        for (int i = 1; i < startingLength; i++)
            BodyCoords.emplace_back(headY, headX - i);
        BodyCoords.emplace_back(tailY, tailX);

        /*for (const auto& [x, y] : BodyCoords)
            cout << x << ' ' << y << '\n';*/

    }

};


Object snake;


int main()
{
    change_font();
    //lines to appear scroll, for better look
    COORD c; c.X = width + 100; c.Y = 9000;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), c);


    //GAME PREPARATIONS
    CenterWindow(width, height);
    snake.draw_snake();

    snake.spawn_fruit();
    while (true)
    {
        while (_kbhit()) direction = _getch();

        //direction output /////DO DOPRACOWANIA!!!/////
            //gotoxy(10, 20); 
            //cout << "PD: " << prevDirection << " D: " << direction;

        snake.move();// direction, prevDirection);
        if (snake.gameOver)
        {
            //draw_game_over();
        }
        prevDirection = direction;
    }
}
