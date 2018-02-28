/*!
 *
 * NMEA library
 * URL: http://nmea.sourceforge.net
 * Author: Tim (xtimor@gmail.com)
 * Licence: http://www.gnu.org/licenses/lgpl.html
 * $Id: parser.c 17 2008-03-11 11:56:11Z xtimor $
 *
 *  * \mainpage  NMEA library

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
 */

/**
 * \file parser.h
 */

#include "tok.h"
#include "parse.h"
#include "parser.h"
#include "context.h"

#include <string.h>
#include <stdlib.h>

typedef struct _nmeaParserNODE
{
    int packType;
    void *pack;
    struct _nmeaParserNODE *next_node;

} nmeaParserNODE;

/*
 * high level
 */

/**
 * \brief Initialization of parser object
 * @return true (1) - success or false (0) - fail
 */
int nmea_parser_init(nmeaPARSER *parser)
{
    int resv = 0;
    /*
     * parse_buff_size = 1024
     */
    int buff_size = nmea_property()->parse_buff_size;
/*
 * if condition return false, code abort/terminated
 */
    NMEA_ASSERT(parser);

    /*
     * NMEA_MIN_PARSEBUFF = 256
     */
    if(buff_size < NMEA_MIN_PARSEBUFF)
        buff_size = NMEA_MIN_PARSEBUFF;
    /*
     * allocate memory
     */
    memset(parser, 0, sizeof(nmeaPARSER));

    /*
     * check buff_size is not empty
     * if error, print nmea_error function
     */
    if(0 == (parser->buffer = malloc(buff_size)))
        nmea_error("Insufficient memory!");
    else
    {
    	/*
    	 *  buff_size to  parser.buff_size
    	 */
        parser->buff_size = buff_size;
        resv = 1;
    }    

    return resv;
}

/**
 * \brief Destroy parser object
 */
void nmea_parser_destroy(nmeaPARSER *parser)
{
    NMEA_ASSERT(parser && parser->buffer);
    free(parser->buffer);
    nmea_parser_queue_clear(parser);
    memset(parser, 0, sizeof(nmeaPARSER));
}

/**
 * \brief Analysis of buffer and put results to information structure
 * @return Number of packets wos parsed
 */
int nmea_parse(    
    nmeaPARSER *parser,
    const char *buff, int buff_sz,
    nmeaINFO *info
    )
{
    int ptype, nread = 0;
    void *pack = 0;
/* if value is 0 or false,
 * there will be a error/ abort
 * check value: parser && parser->buffer
 */
    NMEA_ASSERT(parser && parser->buffer);
    /*
     * return nread (int)
     */
    /*
     * push
     */
    nmea_parser_push(parser, buff, buff_sz);
    /*int nmea_parser_push(nmeaPARSER *parser, const char *buff, int buff_sz)
     * pop
     */

    /**
     * \brief Withdraw top packet from parser
     * @return Received packet type
     * @see nmeaPACKTYPE
     */
    while(GPNON != (ptype = nmea_parser_pop(parser, &pack)))
    {
        nread++;

        switch(ptype)
        {
        /*
         *
         */
        case GPRMC:
            nmea_GPRMC2info((nmeaGPRMC *)pack, info);
            break;
        case GPGGA:
        	/**
        	 * \brief Fill nmeaINFO structure by GGA,GSA<GSV<RMC<VTG packet data.
        	 * @param pack a pointer of packet structure.
        	 * @param info a pointer of summary information structure.
        	 */
            nmea_GPGGA2info((nmeaGPGGA *)pack, info);
            break;
        case GPGSA:
            nmea_GPGSA2info((nmeaGPGSA *)pack, info);
            break;
        case GPGSV:
            nmea_GPGSV2info((nmeaGPGSV *)pack, info);
            break;
        case GPVTG:
            nmea_GPVTG2info((nmeaGPVTG *)pack, info);
            break;
        };

        free(pack);
    }

    return nread;
}

/*
 * low level
 */

int nmea_parser_real_push(nmeaPARSER *parser, const char *buff, int buff_sz)
{
    int nparsed = 0, crc, sen_sz, ptype;
    /*
     * initialize nmeaParserNode
     */
    nmeaParserNODE *node = 0;
    /*
     * parser && parser->buffer
     * if false, return error
     */

    NMEA_ASSERT(parser && parser->buffer);

    /* clear unuse buffer (for debug) */
    /*
    memset(
        parser->buffer + parser->buff_use, 0,
        parser->buff_size - parser->buff_use
        );
        */

    /* add */
    /*
     * clear cache of parser
     */
    if(parser->buff_use + buff_sz >= parser->buff_size)
        nmea_parser_buff_clear(parser);
/*
 * The memcpy function copies size bytes from the object beginning at from into the object beginning at
 to. The behavior of this function is undefined if the two arrays to and from overlap; use memmove
 instead if overlapping is possible.
  memcpy (new, old, arraysize * sizeof (struct foo));
 */
    memcpy(parser->buffer + parser->buff_use, buff, buff_sz);
    parser->buff_use += buff_sz;

    /* parse */
    for(;;node = 0)
    {
    	/**
    	 * \brief Find tail of packet ("\r\n") in buffer and check control sum (CRC).
    	 * @param buff a constant character pointer of packets buffer.
    	 * @param buff_sz buffer size.
    	 * @param res_crc a integer pointer for return CRC of packet (must be defined).
    	 * @return Number of bytes to packet tail.
    	 * nmea_find_tail(const char *buff, int buff_sz, int *res_crc)
    	 * return: nread
    	 */
        sen_sz = nmea_find_tail(
        		/*
        		 * para: buff, buff_sz, res_cec
        		 */
            (const char *)parser->buffer + nparsed,
            (int)parser->buff_use - nparsed, &crc);

        if(!sen_sz)
        {
            if(nparsed)
            	/*
            	 * copy memory
            	 */
                memcpy(
                /*
                 * format：memcpy (new, old, arraysize * sizeof (struct foo));
                 */
                parser->buffer,
                parser->buffer + nparsed,
                parser->buff_use -= nparsed);
            break;
        }
        else if(crc >= 0)
        {
        	/**
        	 * \brief Define packet type by header (nmeaPACKTYPE).
        	 * @param buff： a constant character pointer of packet buffer.
        	 * @param buff_sz： buffer size.
        	 * @return The defined packet type
        	 * @see nmeaPACKTYPE
        	 */
            ptype = nmea_pack_type(
                (const char *)parser->buffer + nparsed + 1,
                parser->buff_use - nparsed - 1);

            if(0 == (node = malloc(sizeof(nmeaParserNODE))))
                goto mem_fail;

            node->pack = 0;

            switch(ptype)
            {
            case GPGGA:
                if(0 == (node->pack = malloc(sizeof(nmeaGPGGA))))
                    goto mem_fail;
                node->packType = GPGGA;
                if(!nmea_parse_GPGGA(
                    (const char *)parser->buffer + nparsed,
                    sen_sz, (nmeaGPGGA *)node->pack))
                {
                    free(node);
                    node = 0;
                }
                break;
            case GPGSA:
                if(0 == (node->pack = malloc(sizeof(nmeaGPGSA))))
                    goto mem_fail;
                node->packType = GPGSA;
                if(!nmea_parse_GPGSA(
                    (const char *)parser->buffer + nparsed,
                    sen_sz, (nmeaGPGSA *)node->pack))
                {
                    free(node);
                    node = 0;
                }
                break;
            case GPGSV:
                if(0 == (node->pack = malloc(sizeof(nmeaGPGSV))))
                    goto mem_fail;
                node->packType = GPGSV;
                if(!nmea_parse_GPGSV(
                    (const char *)parser->buffer + nparsed,
                    sen_sz, (nmeaGPGSV *)node->pack))
                {
                    free(node);
                    node = 0;
                }
                break;
            case GPRMC:
                if(0 == (node->pack = malloc(sizeof(nmeaGPRMC))))
                    goto mem_fail;
                node->packType = GPRMC;
                if(!nmea_parse_GPRMC(
                    (const char *)parser->buffer + nparsed,
                    sen_sz, (nmeaGPRMC *)node->pack))
                {
                    free(node);
                    node = 0;
                }
                break;
            case GPVTG:
                if(0 == (node->pack = malloc(sizeof(nmeaGPVTG))))
                    goto mem_fail;
                node->packType = GPVTG;
                if(!nmea_parse_GPVTG(
                    (const char *)parser->buffer + nparsed,
                    sen_sz, (nmeaGPVTG *)node->pack))
                {
                    free(node);
                    node = 0;
                }
                break;
            default:
                free(node);
                node = 0;
                break;
            };

            if(node)
            {
                if(parser->end_node)
                    ((nmeaParserNODE *)parser->end_node)->next_node = node;
                parser->end_node = node;
                if(!parser->top_node)
                    parser->top_node = node;
                node->next_node = 0;
            }
        }

        nparsed += sen_sz;
    }

    return nparsed;

mem_fail:
    if(node)
        free(node);

    nmea_error("Insufficient memory!");

    return -1;
}

/**
 * \brief Analysis of buffer and keep results into parser
 * @return Number of bytes wos parsed from buffer
 */
int nmea_parser_push(nmeaPARSER *parser, const char *buff, int buff_sz)
{
    int nparse, nparsed = 0;

    do
    {
    	/*
    	 * if > 2014, = 1024
    	 * otherwise, = itself
    	 */
        if(buff_sz > parser->buff_size)
            nparse = parser->buff_size;
        else
            nparse = buff_sz;

        nparsed += nmea_parser_real_push(
            parser, buff, nparse);

        buff_sz -= nparse;

    } while(buff_sz);

    return nparsed;
}

/**
 * \brief Get type of top packet keeped into parser
 * @return Type of packet
 * @see nmeaPACKTYPE
 */
int nmea_parser_top(nmeaPARSER *parser)
{
    int retval = GPNON;
    nmeaParserNODE *node = (nmeaParserNODE *)parser->top_node;

    NMEA_ASSERT(parser && parser->buffer);

    if(node)
        retval = node->packType;

    return retval;
}

/**
 * \brief Withdraw top packet from parser
 * @return Received packet type
 * @see nmeaPACKTYPE
 */
int nmea_parser_pop(nmeaPARSER *parser, void **pack_ptr)
{
    int retval = GPNON;
    /*
     * typedef struct _nmeaParserNODE
		{
			int packType;
			void *pack;
			struct _nmeaParserNODE *next_node;

		} nmeaParserNODE;
     */
    nmeaParserNODE *node = (nmeaParserNODE *)parser->top_node;

    NMEA_ASSERT(parser && parser->buffer);

    if(node)
    {
        *pack_ptr = node->pack;
        retval = node->packType;
        parser->top_node = node->next_node;
        if(!parser->top_node)
            parser->end_node = 0;
        free(node);
    }

    return retval;
}

/**
 * \brief Get top packet from parser without withdraw
 * @return Received packet type
 * @see nmeaPACKTYPE
 */
int nmea_parser_peek(nmeaPARSER *parser, void **pack_ptr)
{
    int retval = GPNON;
    nmeaParserNODE *node = (nmeaParserNODE *)parser->top_node;

    NMEA_ASSERT(parser && parser->buffer);

    if(node)
    {
        *pack_ptr = node->pack;
        retval = node->packType;
    }

    return retval;
}

/**
 * \brief Delete top packet from parser
 * @return Deleted packet type
 * @see nmeaPACKTYPE
 */
int nmea_parser_drop(nmeaPARSER *parser)
{
    int retval = GPNON;
    nmeaParserNODE *node = (nmeaParserNODE *)parser->top_node;

    NMEA_ASSERT(parser && parser->buffer);

    if(node)
    {
/*
 * check and free
 */
        if(node->pack)
            free(node->pack);
        retval = node->packType;
        parser->top_node = node->next_node;
        if(!parser->top_node)
            parser->end_node = 0;
        free(node);
    }

    return retval;
}

/**
 * \brief Clear cache of parser
 * @return true (1) - success
 */
int nmea_parser_buff_clear(nmeaPARSER *parser)
{
    NMEA_ASSERT(parser && parser->buffer);
    parser->buff_use = 0;
    return 1;
}

/**
 * \brief Clear packets queue into parser
 * @return true (1) - success
 */
int nmea_parser_queue_clear(nmeaPARSER *parser)
{
    NMEA_ASSERT(parser);
    while(parser->top_node)
        nmea_parser_drop(parser);
    return 1;
}
