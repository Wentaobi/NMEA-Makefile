/*!
 *
 * NMEA library
 * URL: http://nmea.sourceforge.net
 * Author: Tim (xtimor@gmail.com)
 * Licence: http://www.gnu.org/licenses/lgpl.html
 * $Id: nmea.h 17 2008-03-11 11:56:11Z xtimor $
 *
 * \mainpage  NMEA library

Disclaimer

The National Marine Electronics Association (NMEA) has developed a specification that defines the interface between various pieces of marine electronic equipment. The standard permits marine electronics to send information to computers and to other marine equipment.
Most computer programs that provide real time position information understand and expect data to be in NMEA format. This data includes the complete PVT (position, velocity, time) solution computed by the GPS receiver. The idea of NMEA is to send a line of data called a sentence that is totally self contained and independent from other sentences. All NMEA sentences is sequences of ACSII symbols begins with a '$' and ends with a carriage return/line feed sequence and can be no longer than 80 characters of visible text (plus the line terminators).

Introduction

We present library in 'C' programming language for work with NMEA protocol. Small and easy to use. The library build on different compilers under different platforms (see below). The code was tested in real projects. Just download and try...

UPDATE: This version has been updated to use GNU AUTOTOOLS for configuration and build and now creates distribution libnmea-$VERSION.so  DRE 2015

Features

- Analysis NMEA sentences and granting GPS data in C structures
- Generate NMEA sentences
- Supported sentences: GPGGA, GPGSA, GPGSV, GPRMC, GPVTG
- Multilevel architecture of algorithms
- Additional functions of geographical mathematics and work with navigation data

Supported (tested) platforms

- Microsoft Windows (MS Visual Studio 8.0, GCC)
- Windows Mobile, Windows CE (MS Visual Studio 8.0)
- UNIX (GCC)

Licence: LGPL
* 
*  For more details visit:  http://nmea.sourceforge.net/#intro
 * 
 * 
 * 
 * 
 * RMC - Recommended Minimum Specific GNSS Data. Time, date, position, course and speed data.

$GPRMC,hhmmss.dd,S,xxmm.dddd,<N|S>,yyymm.dddd,<E|W>,s.s,h.h,ddmmyy,d.d, <E|W>,M*hh<CR><LF>

hhmmss.dd

	UTC time of the fix.
hh = hours
mm = minutes
ss = seconds
dd = decimal part of seconds

S

	Status indicator
A = valid
V = invalid

xxmm.dddd 

	Latitude coordinate.
xx = degrees
mm = minutes
dddd = decimal part of minutes

<N|S>

	Character denoting either N = North or S = South.

yyymm.dddd

	Longitude coordinate
yyy = degrees
mm = minutes
dddd = decimal part of minutes

<E|W>

	Character denoting either E = East or W = West.

s.s

      

	Speed in knots.

h.h

      

	Heading

ddmmyy 

	UTC Date of the fix.
dd = day of month
mm = month
yy = year

d.d 

	Magnetic variation in degrees, i.e. difference between geometrical and magnetic north direction.

<E|W>

	Letter denoting direction of magnetic variation. Either E = East or W = West.

M

	Mode indicator
A=autonomous
N=data not valid

Example:

$GPRMC,095035.91,A,6016.3066,N,02458.3832,E,1.08,210.6,131204,6.1,E,A*0A
 */

#ifndef __NMEA_H__
#define __NMEA_H__

#include "./config.h"
#include "./units.h"
#include "./gmath.h"
#include "./info.h"
#include "./sentence.h"
#include "./generate.h"
#include "./generator.h"
#include "./parse.h"
#include "./parser.h"
#include "./context.h"

#endif /* __NMEA_H__ */
