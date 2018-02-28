#include "nmea.h" /* nmea library from nmea offical website*/
#include <string.h> /* eclipse c/c++ library itself*/
#include <stdio.h>

#define NUM_POINTS 7 /* NUM_POINTS = 7*/

int main()
{	/* char* type array*/
    const char *buff[] = {
        "$GPRMC,173843,A,6649.896,N,11808.521,W,000.0,360.0,230108,013.4,E*69\r\n",
        "$GPGGA,111609.14,7001.27,N,3613.06,E,3,08,0.0,10.2,M,0.0,M,0.0,0000*70\r\n",
        "$GPGSV,2,1,08,01,05,005,80,02,05,050,80,03,05,095,80,04,05,140,80*7f\r\n",
        "$GPGSV,2,2,08,05,05,185,80,06,05,230,80,07,05,275,80,08,05,320,80*71\r\n",
        "$GPGSA,A,3,01,02,03,04,05,06,07,08,00,00,00,00,0.0,0.0,0.0*3a\r\n",
        "$GPRMC,111609.14,A,5001.27,N,3613.06,E,11.2,0.0,261206,0.0,E*50\r\n",
        "$GPVTG,217.5,T,208.8,M,000.00,N,000.01,K*4C\r\n"
    };

    int it;/*define it as how many char data strings*/

    nmeaPOS pos[NUM_POINTS];/*nmeaPOS pos[7], it means structure pos array has 7 structures
     	 	 	 	 	 	 nmeapos structure has two elements: latitude & lonitude
     	 	 	 	 	 	 this is for getting position data specially outputing*/

    nmeaINFO info;/* define almost all information that normal GPS has:
    mask type/ time structure/ GPS quality indicator/ navigation mode/ *DOP/ lat/longiture
    antenna altitude above sea level/ ground speed/ track angle/ magnetic degrees satelite info structure*/
    nmeaPARSER parser;
    /*
     *void* top_node, end_node
     *char* buffer
     *int buff_size, buff_use
     */

    nmea_zero_INFO(&info);
    /*
     * function for time and GPS mode initialization
     * input:structure GPS data info
     * sizeof(nmeaINFO);
    	nmea_time_now(&info->utc);// get now GMT time
    	info->sig = NMEA_SIG_BAD;// singal indicator
    	info->fix = NMEA_FIX_BAD;// operation mode
     */
    nmea_parser_init(&parser);
    /*
     * function input: nmeaPARSER* parser,
     * parse_buff_size 1024
     * allocate memory and for sure, if else
     * otherwise, printf nmea memory error
     * return 0/1
     */

    for(it = 0; it < 7; ++it)
    	/*
    	 * it is iteration you want parse
    	 */
    {
	
    	/*
    	 * function in parser.c
    	 * input para:parser structure, which buffer array, length of array, gps
    	 * GPS data structure, general info
    	 */
    	/**
    	 * \brief Analysis of buffer and put results to information structure
    	 * @return Number of packets wos parsed
    	 */
        nmea_parse(&parser, buff[it], (int)strlen(buff[it]), &info);
        /**
         * \brief Convert position from INFO to radians position
         * *pi/180 degree to radian
         */
        /*
         * I think some data type no pos will use the previous pos.
         */
        nmea_info2pos(&info, &pos[it]);
        /*
         * print position, lat/lan
         */
        printf("*********   time %d: %d-%d-%d-%d-%d-%d.%d \n", it,info.utc.year,info.utc.mon,info.utc.day,info.utc.hour,info.utc.min,info.utc.sec,info.utc.hsec);
        printf(
            "P%d in radians: lat:%9.6lf lon:%9.6lf  \tin degree: lat:%+010.6lf° lon:%+011.6lf°\n", 
            it, pos[it].lat, pos[it].lon, nmea_radian2degree(pos[it].lat), nmea_radian2degree(pos[it].lon)
            );



	}
    /*
     * release/free memory
     */
    nmea_parser_destroy(&parser);

    return (0);
}
