/*
 *
 * NMEA library
 * URL: http://nmea.sourceforge.net
 * Author: Tim (xtimor@gmail.com)
 * Licence: http://www.gnu.org/licenses/lgpl.html
 * $Id: info.c 17 2008-03-11 11:56:11Z xtimor $
 *
 */

#include <string.h>

#include "info.h"

void nmea_zero_INFO(nmeaINFO *info)
{
	/*This function copies the value of c (converted to an unsigned char) into each of the first size bytes of
 the object beginning at block. It returns the value of block.
 Prototype: void * memset (void *block, int c, size_t size)

 */
    memset(info, 0, sizeof(nmeaINFO));
    nmea_time_now(&info->utc);
    info->sig = NMEA_SIG_BAD;
    info->fix = NMEA_FIX_BAD;
}
