/*
 *
 * NMEA library
 * URL: http://nmea.sourceforge.net
 * Author: Tim (xtimor@gmail.com)
 * Licence: http://www.gnu.org/licenses/lgpl.html
 * $Id: time.c 4 2007-08-27 13:11:03Z xtimor $
 *
 */

/*! \file time.h */

#include "nmea_time.h"

#ifdef NMEA_WIN
#   pragma warning(disable: 4201)
#   pragma warning(disable: 4214)
#   pragma warning(disable: 4115)
#   include <windows.h>
#   pragma warning(default: 4201)
#   pragma warning(default: 4214)
#   pragma warning(default: 4115)
#else
#
#endif

#ifdef NMEA_WIN

void nmea_time_now(nmeaTIME *stm)
{
    SYSTEMTIME st;

    GetSystemTime(&st);

    stm->year = st.wYear - 1900;
    stm->mon = st.wMonth - 1;
    stm->day = st.wDay;
    stm->hour = st.wHour;
    stm->min = st.wMinute;
    stm->sec = st.wSecond;
    stm->hsec = st.wMilliseconds / 10;
}

#else /* NMEA_WIN */
#include <time.h>

/**
 * #ifdef
 * #else
 * #endif
 * used to check #define or #include
 * sometimes, coder define something again and forget it.
 * sometimes, this is to check to use different #include for
 * different platform, but the main code did not need to change
 *
 */


void nmea_time_now(nmeaTIME *stm)
/*
 * nmeaTIME* variable stm, all about address
 */
{
    time_t lt;/*
    long int type
    seconds since the Epoch
    */
    struct tm *tt;
    /*
     * tm structure include: tm_xxx
     * sec/ min/ hour/ mday/ mon/ year/ wday/ yday/ isdst
     */

    time(&lt);
    /* method 1
     * time_t time (time_t *result)
     * The time function returns the current calendar time as a value
     *  of type time_t. If the argument result
 	 is not a null pointer, the calendar time value is also
 	 stored in *result. If the current calendar time is
 	 not available, the value (time_t)(-1) is returned.
     */
    tt = gmtime(&lt);
    /* method 2
      almost same as time but better
     * This function is similar to localtime, except that the
     * broken-down time is expressed as Coordinated
 	 Universal Time (UTC) (formerly called Greenwich Mean Time (GMT))
 	  rather than relative to a local time zone.
     */

    /*
     * get nmea now time save to strtcure stm year-mon-day-hour-min-sec-hsec
     */
    stm->year = tt->tm_year;
    stm->mon = tt->tm_mon;
    stm->day = tt->tm_mday;
    stm->hour = tt->tm_hour;
    stm->min = tt->tm_min;
    stm->sec = tt->tm_sec;
    stm->hsec = 0;
}

#endif
