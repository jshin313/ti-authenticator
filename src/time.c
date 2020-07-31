#include <stdint.h>
#include "time.h"
#include <tice.h>

uint32_t getEpochTime(void)
{
    uint32_t epochTime = 0;
    // Get time and date in the "normal" Gregorian calendar format
    uint8_t day, month, seconds, minutes, hours;
    uint16_t year;
    int leapYears, i;
    boot_GetDate(&day, &month, &year);
    boot_GetTime(&seconds, &minutes, &hours);

    leapYears = numLeapYears((uint16_t)1970, year);

    epochTime = leapYears * SECSINLEAPYEAR + (year - 1970 - leapYears) * SECSINYEAR;

    for (i = 1; i < month; i++)
    {
        switch (i)
        {
            case 9:
            case 4:
            case 6:
            case 11:
                epochTime += 30 * 3600 * 24;
                break;
            case 2:
                if (isLeapYear(year))
                    epochTime += 29 * 3600 * 24;
                else
                    epochTime += 28 * 3600 * 24;
                break;
            default:
                epochTime += 31 * 3600 * 24;
                break;
        }
    }

    epochTime += (day - 1) * 3600 * 24;
    epochTime += seconds + minutes * 60 + hours * 3600;

    return epochTime;
}

int numLeapYears(uint16_t start, uint16_t end)
{
    int i, count = 0;
    for (i = start; i < end; i++)
    {
        if (isLeapYear(i))
            count++;
    }
    return count;
}

int isLeapYear(uint16_t year)
{
    return ((year % 400 == 0) || (year % 100 != 0 && year % 4 == 0));
}
