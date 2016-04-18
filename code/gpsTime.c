// Includes
#include <stdlib.h>	//abs
#include "gpsTime.h"

// Functions
// Updates an elapsed time variable with new time (usually diff from calcTimeDifference)
void updateElapsedTime(time *elapsedTime, time *diffTime)
{
	elapsedTime->millisecond += diffTime->millisecond;
	if (elapsedTime->millisecond > 999)
	{
		elapsedTime->millisecond -= 1000;
		elapsedTime->second++;
	}
	elapsedTime->second += diffTime->second;
	if (elapsedTime->second > 59)
	{
		elapsedTime->second -= 60;
		elapsedTime->minute++;
	}
	elapsedTime->minute += diffTime->minute;
	if (elapsedTime->minute > 59)
	{
		elapsedTime->minute -= 60;
		elapsedTime->hour++;
	}
	elapsedTime->hour += diffTime->hour;
}

// Calculates difference between two times & returns difference.
time calcTimeDifference(time *time1, time *time2)
{
	// NOTE: Only tested within 24 hours apart, although longer should work. Up until an integer overflow in total1/total2 occurs...
	time difference;
	unsigned int total1, total2;
	unsigned long diff_ms;	// prevent int overflow

	total1 = calcTotalTime_ms(time1);
	total2 = calcTotalTime_ms(time2);
	diff_ms = abs(total2 - total1) % (24UL * 60UL * 60UL * 1000UL);	// possible int overflow when dealing with big numbers

	difference.hour = diff_ms / 1000 / 3600;
	difference.minute = ((diff_ms / 1000) / 60) % 60;
	difference.second = (diff_ms / 1000) % 60;
	difference.millisecond = diff_ms % 1000;

	return difference;
}

// Calculate total time in milliseconds.
unsigned int calcTotalTime_ms(time *value)
{
	return ((value->hour * 3600000) + (value->minute * 60000) + (value->second * 1000) + value->millisecond);
}