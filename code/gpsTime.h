#ifndef _GPSTIME_H_
#define _GPSTIME_H_

// Typedef: Time - hh:mm:ss.sss
typedef struct
{
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
	unsigned int millisecond;
}time;

// Function Declarations
void updateElapsedTime(time *mainTime, time *newTime);
time calcTimeDifference(time *time1, time *time2);
unsigned int calcTotalTime_ms(time *value);


#endif /* _GPSTIME_H_ */