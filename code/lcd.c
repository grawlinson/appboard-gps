// Includes
#include <stdio.h>	// sprintf
#include "lcd.h"
#include "LabBoard.h"

// Variables
LCD display;	// Holds info about lines

// Internal Functions
void updateDisplay(void);

// Functions
// Initialise LCD
void LCDInit(void)
{
	SLCDInit();	// Initialise SLCD
	SLCDDisplayOn();	// Turn on Display
	SLCDClearScreen();	// Clear screen
}
// Display: Current Time
void LCDDisplayCurrentTime(gpsData *gps)
{
	sprintf(display.Line1, "cTime: %02u:%02u:%02u.%03u", gps->currentTime.hour, gps->currentTime.minute, gps->currentTime.second, gps->currentTime.millisecond);
	updateDisplay();
}
// Display: Elapsed Time
void LCDDisplayElapsedTime(gpsData *gps)
{
	sprintf(display.Line1, "eTime: %02u:%02u:%02u.%03u", gps->elapsedTime.hour, gps->elapsedTime.minute, gps->elapsedTime.second, gps->elapsedTime.millisecond);
	updateDisplay();
}
// Display: Current Altitude
void LCDDisplayCurrentAltitude(gpsData *gps)
{
	sprintf(display.Line1, "Alt: %.1f m", gps->altitude);
	updateDisplay();
}
// Display: Elapsed Distance
void LCDDisplayElapsedDistance(gpsData *gps)
{
	sprintf(display.Line1, "eDist: %.3f km", gps->elapsedDistance);
	updateDisplay();
}
// Display: Current Speed
void LCDDisplayCurrentSpeed(gpsData *gps)
{
	sprintf(display.Line1, "cSpeed: %.2f km/hr", gps->speed);
	updateDisplay();
}
// Display: Current Heading
void LCDDisplayCurrentHeading(gpsData *gps)
{
	sprintf(display.Line1, "Heading: %s", gps->heading);
	updateDisplay();
}
// Refresh SLCD
void updateDisplay(void)
{
		sprintf(display.Line1, "%-20s", display.Line1);	// Clear the first line
		SLCDSetCursorPosition(firstLine, 0);	// Set cursor position to beginning of first line
		SLCDWriteString(display.Line1);	// Write string to SLCD
}
