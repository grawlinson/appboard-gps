# AppBoard GPS

## Introduction

Develop a GPS interpreter for a new embedded product. The purpose of the
interpreter is to extract common navigation parameters for an embedded
rear-vision mirror.

The parameters that can be displayed by the rear-view mirror include:

* Current time (24hr)
* Current speed (km/hr)
* Current compass heading (e.g. N, NE, E, etc)
* Current altitude (as measured by the GPS in m)
* Total distance driven this trip (km)
* Total elapsed driving time this trip (hrs:mins)

In order to prototype this software, the AT90USB1287 AppBoard will be used
as target hardware including the on-board UART to receive the GPS position
information, and the LCD to display real-time information as per above spec.

## Required Functionality

* Only one parameter (e.g. time or speed) is displayed at a time, with units, on the top line of the LCD. This emulates the single line LCD in the final product.
* The parameter being displayed is updated in real-time while valid GPS coordinates are being received.
* If the user wishes to view the other parameters, two push buttons are provided (UP/DOWN) which will cycle through the parameters in the order described in the introduction.
* The total distance driven & elapsed time are measured from the first valid GPS coordinate. If the power is reset, these two counters are reset as well.

## GPS Simulation

[SatGen NMEA][sg-nmea] allows pre-made GPS datasets to be streamed via Serial Port, this will be used to stream mock data to the AppBoard.

[sg-nmea]: http://www.labsat.co.uk/index.php/en/free-gps-nmea-simulator-software

# LICENSE

The MIT License (MIT)

Copyright (c) 2015 George Rawlinson <mailto:george@rawlinson.net.nz>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
