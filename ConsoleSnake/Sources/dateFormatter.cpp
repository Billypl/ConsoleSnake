#include <string>
#include "../Headers/dateFormatter.h"
#include <ctime>
#pragma warning(disable : 4996)

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
        localtime_s(&ltm, &now);

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
