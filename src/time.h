#include <stdint.h>
#define SECSINYEAR 31536000
#define SECSINLEAPYEAR 31622400

uint32_t getEpochTime(void);
int numLeapYears(uint16_t start, uint16_t end);
int isLeapYear(uint16_t year);
