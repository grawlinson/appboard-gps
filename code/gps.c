// Includes
#include <stdio.h>	//sscanf
#include <stdlib.h>	//strtol/atoi/atof
#include <string.h>	//memset/memcpy/strcmp/strlen/strcat/strcpy
#include "gps.h"
#include "libGPSCalc.h" // external library used to calculate speed/etc

// Typedef: NMEA Data - Stores extracted values from recently received NMEA Data
typedef struct
{
	time time;	// Timestamp of received message (hh:mm:ss.sss)
	char latitude[BUF_FIELD_LENGTH];	// Latitude of received message (eg: "3731.28236393,S")
	char longitude[BUF_FIELD_LENGTH];	// Longitude of received message (eg: "17503.60096733,E")
	float altitude;	// Altitude (metres)
	float distance;	// Distance (metres)
} NMEAData;

// Variables: Buffer
// NOTE: All volatile as they're accessed/modified within an interrupt.
volatile char rxData[BUF_FIELDS][BUF_FIELD_LENGTH];	// Received data stored in this 2D array.
volatile int calculatedChecksum = 0;	// Calculated checksum on the received data.
volatile unsigned char fieldIndex = 0;	// Current field index.
volatile unsigned char charIndex = 0;	// Current char position within string.

// Variables: Global
volatile char flags = F_FIRST_PKT;	// Used to determine actions.
NMEAData previous, current;

// Internal Functions
void parseTime(char(*bufferData)[BUF_FIELDS][BUF_FIELD_LENGTH], NMEAData *nmeaData);
void parseAltitude(char(*bufferData)[BUF_FIELDS][BUF_FIELD_LENGTH], NMEAData *nmeaData);
void parseLatLong(char(*bufferData)[BUF_FIELDS][BUF_FIELD_LENGTH], NMEAData *nmeaData);
void parseNMEA(char(*bufferData)[BUF_FIELDS][BUF_FIELD_LENGTH], NMEAData *nmeaData);
unsigned int calcAndUpdateGPS(NMEAData *currentData, NMEAData *previousData, gpsData *gData);

// Decodes GPS (NMEA 0183) message, byte by byte.
unsigned int gpsDecode(char c)
{
	// Prevent buffer overflow due to bad message.
	if((fieldIndex == (BUF_FIELDS - 1)) || (charIndex == (BUF_FIELD_LENGTH - 1)))
	{
		memset((void*)rxData, 0, BUF_FIELDS*BUF_FIELD_LENGTH*sizeof(rxData[0][0]));	// Clear memory

		if(c != '$')	// Force function to wait for beginning of next message.
		{
			return GPS_MSG_OVERFLOW;	// Return value: Message overflow.
		}
	}

	switch(c)
	{
		case '$':	// Beginning of message.
			flags &= ~F_CKSM;	// Start checksum.

			calculatedChecksum = 0;	// Reset checksum & 2D array indices.
			fieldIndex = 0;
			charIndex = 0;
		break;
		case '\n':	// End of message. Note: What about \r?
			rxData[fieldIndex][charIndex] = 0;	// Signify end of string.

			return GPS_MSG_RDY;	// Return value: Message ready for parsing.
		break;
		case ',':	// Separator.
			calculatedChecksum ^= c;	// Checksum generation.
			rxData[fieldIndex][charIndex] = 0;	// Signify end of string.
			fieldIndex++;	// Jump to next field.
			charIndex = 0;	// Reset character index.
		break;
		case '*':	// End of message + checksum generation.
			flags |= F_CKSM;	// End checksum generation.

			rxData[fieldIndex][charIndex] = 0;	// Signify end of string.
			fieldIndex++;	// Jump to next field (checksum!)
			charIndex = 0;	// Reset character index.
		break;
		default:
			if(!(flags & F_CKSM))	// Checksum generation
			{
				calculatedChecksum ^= c;
			}

			rxData[fieldIndex][charIndex] = c;	// Fill up 2D array with data.
			charIndex++;	// Jump to next character.
		break;
	}

	return GPS_MSG_NONE;	// Return value: No complete message received.
}

// Parse complete GPS message & stores data.
unsigned int gpsParse(gpsData *gData)
{
	// Temporary variables.
	char data[BUF_FIELDS][BUF_FIELD_LENGTH];
	unsigned int calcStatus;

	memcpy(data, (const void*)rxData, sizeof(data));	// Create 'safe' copy of data to operate on.

	if (strcmp(data[0], "GPGGA") == 0)	// We're only looking for GPGGA messages.
	{
		if (calculatedChecksum == strtol(data[15], NULL, 16))	// Compare received & calculated checksums.
		{
			memcpy(&previous, &current, sizeof(NMEAData));	// Update previous values.

			parseNMEA(&data, &current);	// Update current data with received data.

			// Comparing can't start until we have a second message.
			if (flags & F_FIRST_PKT)
			{
				flags &= ~F_FIRST_PKT;
			}
			else
			{
				calcStatus = calcAndUpdateGPS(&current, &previous, gData);

				if (calcStatus == GPS_LARGE_DIFF)
				{
					return GPS_LARGE_DIFF;
				}

				return GPS_MSG_RDY;
			}

			return GPS_INSUFFICIENT_DATA;
		}
		else
		{
			return GPS_FAIL_CHECKSUM;
		}
	}

	return GPS_INVALID_MSG;
}

// Parse UTC time from received buffer.
void parseTime(char(*bufferData)[BUF_FIELDS][BUF_FIELD_LENGTH], NMEAData *nmeaData)
{
	char millisecondString[3];

	sscanf(
		bufferData[0][1],
		"%2u%2u%2u.%s",
		&nmeaData->time.hour,
		&nmeaData->time.minute,
		&nmeaData->time.second,
		millisecondString);

	nmeaData->time.millisecond = atoi(millisecondString);

	if (strlen(millisecondString) == 2)	// 2 digit = mS in centiseconds.
	{
		nmeaData->time.millisecond = nmeaData->time.millisecond * 10;
	}
}
// Parse altitude from received buffer.
void parseAltitude(char(*bufferData)[BUF_FIELDS][BUF_FIELD_LENGTH], NMEAData *nmeaData)
{
	nmeaData->altitude = atof(bufferData[0][9]);	//C4244 double to float -> loss of data.
}
// Parse latitude & longitude from received buffer.
void parseLatLong(char(*bufferData)[BUF_FIELDS][BUF_FIELD_LENGTH], NMEAData *nmeaData)
{
	// could be done with sprintf! sprintf(nData->latitude, "%s,%s", bufferData[0][2], bufferData[0][3]);
	strcpy(nmeaData->latitude, bufferData[0][2]);
	strcat(nmeaData->latitude, ",");
	strcat(nmeaData->latitude, bufferData[0][3]);

	strcpy(nmeaData->longitude, bufferData[0][4]);
	strcat(nmeaData->longitude, ",");
	strcat(nmeaData->longitude, bufferData[0][5]);
}
// Parse NMEA message for time, altitude & lat/long.
void parseNMEA(char(*bufferData)[BUF_FIELDS][BUF_FIELD_LENGTH], NMEAData *nmeaData)
{
	parseTime(bufferData, nmeaData);
	parseAltitude(bufferData, nmeaData);
	parseLatLong(bufferData, nmeaData);
}
// Calculate a variety of GPS related things & update GPS data.
unsigned int calcAndUpdateGPS(NMEAData *currentData, NMEAData *previousData, gpsData *gData)
{
	time diff;
	gps_coords tempGPS;

	// Calculate time difference between current & previous time.
	diff = calcTimeDifference(&previous.time, &current.time);

	if (diff.second > 0)	// Time difference over a second, not ideal.
	{
		return GPS_LARGE_DIFF;
	}

	// Copy previous & current GPS coords into temporary gps_coords variable.
	strcpy(tempGPS.lat1, previous.latitude);
	strcpy(tempGPS.lat2, current.latitude);
	strcpy(tempGPS.lng1, previous.longitude);
	strcpy(tempGPS.lng2, current.longitude);

	calcGPSParams(&tempGPS);	// EXTERNAL LIB: Calculate GPS parameters.

	// Update variables
	current.distance = tempGPS.distance;
	gData->elapsedDistance += (current.distance / 1000.0); // Add current distance to elapsed distance.
	updateElapsedTime(&gData->elapsedTime, &diff); // Add time difference to elapsed time.
	gData->speed = (current.distance / calcTotalTime_ms(&diff)) * 3600.0;	// (m/mS)*(60 seconds * 60 minutes) [km/hr]
	strcpy(gData->heading, tempGPS.compass_bearing);	// Heading
	gData->currentTime = current.time;
	gData->altitude = current.altitude;

	return GPS_MSG_RDY;
}
