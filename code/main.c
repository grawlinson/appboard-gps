// Defines
#define F_CPU 8000000UL
#define USART_BAUDRATE 38400
#define BAUD_PRESCALE ((( F_CPU / ( USART_BAUDRATE * 16UL ))) - 1)

// Includes
#include <avr/io.h>	// I/O
#include <avr/interrupt.h>	// UART
#include "gps.h" // gps stuff
#include "lcd.h" // lcd display

// Function Declarations
void setup(void);
char getInput(void);
void refreshDisplay(void);

// Enums
enum inputs { NONE, UP, DOWN };
enum states { CURRENT_TIME, ELAPSED_TIME, CURRENT_ALTITUDE, ELAPSED_DISTANCE, CURRENT_SPEED, CURRENT_HEADING };

// Variables
enum states state;
enum inputs input;

// Variables
volatile gpsData gps;
volatile unsigned int decodeStatus, parseStatus;

// Main
int main(void)
{
	setup();	// Setup. Keeps main function relatively clean.

	while(1)
	{
		input = getInput();

		/*
		State machine that determines what is currently displayed on the screen.

		MENUs: See state enums.

		Upon user input, state is changed & display refreshed to reflect the change of state.
		*/
		switch(state)
		{
		case CURRENT_TIME:
			switch(input)
			{
			case UP:
				state = CURRENT_HEADING;
				refreshDisplay();
				break;
			case DOWN:
				state = ELAPSED_TIME;
				refreshDisplay();
				break;
			}
			break;
		case ELAPSED_TIME:
			switch(input)
			{
			case UP:
				state = CURRENT_TIME;
				refreshDisplay();
				break;
			case DOWN:
				state = CURRENT_ALTITUDE;
				refreshDisplay();
				break;
			}
			break;
		case CURRENT_ALTITUDE:
			switch(input)
			{
			case UP:
				state = ELAPSED_TIME;
				refreshDisplay();
				break;
			case DOWN:
				state = ELAPSED_DISTANCE;
				refreshDisplay();
				break;
			}
			break;
		case ELAPSED_DISTANCE:
			switch(input)
			{
			case UP:
				state = CURRENT_ALTITUDE;
				refreshDisplay();
				break;
			case DOWN:
				state = CURRENT_SPEED;
				refreshDisplay();
				break;
			}
			break;
		case CURRENT_SPEED:
			switch(input)
			{
			case UP:
				state = ELAPSED_DISTANCE;
				refreshDisplay();
				break;
			case DOWN:
				state = CURRENT_HEADING;
				refreshDisplay();
				break;
			}
			break;
		case CURRENT_HEADING:
			switch (input)
			{
			case UP:
				state = CURRENT_SPEED;
				refreshDisplay();
				break;
			case DOWN:
				state = CURRENT_TIME;
				refreshDisplay();
				break;
			}
			break;
		}

		// GPS message parsing.
		if (decodeStatus == GPS_MSG_RDY)	// Wait until GPS message has been received before parsing GPS message.
		{
			decodeStatus = GPS_MSG_NONE;	// Reset decode flag (useful when there's no GPS TX or SatNMEA.exe is paused.)

			parseStatus = gpsParse(&gps);	// Pass gps struct to gpsParse for up-to-date information, also update parseStatus.

			if (parseStatus == GPS_MSG_RDY)	// Refresh display on successful parse.
			{
				refreshDisplay();
			}
		}
	}
}
// Grabs input & returns button pressed.
char getInput(void)
{
	char input = NONE;

	if (!(PINA & _BV(PINA3)))
	{
		input = UP;
	}
	else if (!(PINA & _BV(PINA5)))
	{
		input = DOWN;
	}

	return input;
}
// Refreshes display based on current state.
void refreshDisplay(void)
{
	switch (state)
	{
	case CURRENT_TIME:
		LCDDisplayCurrentTime(&gps);
		break;
	case ELAPSED_TIME:
		LCDDisplayElapsedTime(&gps);
		break;
	case CURRENT_ALTITUDE:
		LCDDisplayCurrentAltitude(&gps);
		break;
	case ELAPSED_DISTANCE:
		LCDDisplayElapsedDistance(&gps);
		break;
	case CURRENT_SPEED:
		LCDDisplayCurrentSpeed(&gps);
		break;
	case CURRENT_HEADING:
		LCDDisplayCurrentHeading(&gps);
		break;
	}
}
// Setup Microcontroller
void setup(void)
{
	// Setup LCD & Display
	LCDInit();
	refreshDisplay();

	// Setup IO
	DDRE = 0x03;	// Multiplexer: Buttons
	PORTE = 0x01;
	DDRA = 0x00;	// Buttons

	// USART
	UCSR1A = 0x00;
	UCSR1B |= (_BV(RXEN1) | _BV(TXEN1));	// Enable TX & RX.
	UCSR1C |= (_BV(UCSZ10) | _BV(UCSZ11));	// Async UART, No Parity, 1 Stop Bit, 8-bit DATA.
	UBRR1H = (BAUD_PRESCALE >> 8);
	UBRR1L = BAUD_PRESCALE;

	// INTERRUPTS
	UCSR1B |= _BV(RXCIE1);	// UART RX complete
	sei();	// Global Interrupts enabled

	// Variables
	decodeStatus = GPS_MSG_NONE;
	parseStatus = GPS_MSG_NONE;
}

// Interrupt: UART Receive
ISR(USART1_RX_vect)
{
	// Save received byte to signal to the microcontroller that we have successfully received it.
	char rxByte;
	rxByte = UDR1;

	// Pass the received byte to gpsDecode, also update the decode status.
	decodeStatus = gpsDecode(rxByte);
}
