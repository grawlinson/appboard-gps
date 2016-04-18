#ifndef _GPS_H_
#define _GPS_H_
// Includes
#include "gpsTime.h"	// needs to be in here because typedef struct time used within header

// Defines: Flags
#define F_CKSM 0x01	// CHECKSUM
#define F_RDY 0x02	// DATA READY
#define F_FIRST_PKT	0x04	// FIRST PACKET

// Defines: Parser/Decode statuses
#define GPS_MSG_RDY 0
#define GPS_MSG_NONE 1
#define GPS_MSG_OVERFLOW 2
#define GPS_INVALID_MSG 3
#define GPS_FAIL_CHECKSUM 4
#define GPS_INSUFFICIENT_DATA 5
#define GPS_LARGE_DIFF 6

// Defines: 2D array size limits
#define BUF_FIELDS 20	// Max # of fields
#define BUF_FIELD_LENGTH 30	// Max field length

// Typedef: GPS Data - Processed data from a NMEA-0183 compatible GPS unit
typedef struct
{
	time currentTime;	// Current timestamp (hh:mm:ss.sss)
	time elapsedTime;	// Elapsed time (hh:mm:ss.sss)
	float altitude;	// Altitude (metres)
	float elapsedDistance;	// Elapsed distance (km)
	float speed;	// Speed (km/hr)
	char heading[3];	// GPS Bearing(NESW)
} gpsData;


// Function Declarations
unsigned int gpsDecode(char c);
unsigned int gpsParse(gpsData *gData);

#endif /* _GPS_H_ */
