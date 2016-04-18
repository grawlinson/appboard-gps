#ifndef _LCD_H_
#define _LCD_H_
// Defines
#define MAX_LENGTH 20	// Maximum length of char string for SLCD line

// Includes
#include"gps.h"	// Required for function declarations below

// Struct: LCD - Holds info about lines
typedef struct
{
	char Line1[MAX_LENGTH];
	char Line2[MAX_LENGTH];
	char Line3[MAX_LENGTH];
	char Line4[MAX_LENGTH];
}LCD;

// Function Declarations
void LCDInit(void);
void LCDDisplayCurrentTime(gpsData *gps);
void LCDDisplayElapsedTime(gpsData *gps);
void LCDDisplayCurrentAltitude(gpsData *gps);
void LCDDisplayElapsedDistance(gpsData *gps);
void LCDDisplayCurrentSpeed(gpsData *gps);
void LCDDisplayCurrentHeading(gpsData *gps);
#endif /* _LCD_H_ */
