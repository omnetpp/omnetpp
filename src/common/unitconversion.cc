//=========================================================================
//  UNITCONVERSION.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _UNITCONVERSION_H_
#define _UNITCONVERSION_H_

#include "exception.h"

/**
 * XXX
 */
class UnitConversion
{
  public:
    enum Unit {UNIT_UNKNOWN, TIME_SECONDS, DATARATE_BPS, DATA_BYTES, DISTANCE_METERS, UNIT_OTHER };
    struct UnitDesc { const char *name; double mult; Unit type; };

  protected:
    UnitDesc unitTable[];
    double parseQuantity(const char *str);  //XXX error handling? return unit type?

};

#endif

//XXX find correct place for this stuff
UnitConversion::UnitDesc unitTable[] = {
    { "d",   86400, UnitConversion::TIME_SECONDS },
    { "h",    3600, UnitConversion::TIME_SECONDS },
    { "mi",     60, UnitConversion::TIME_SECONDS },
    { "s",       1, UnitConversion::TIME_SECONDS },
    { "ms",   1e-3, UnitConversion::TIME_SECONDS },
    { "us",   1e-9, UnitConversion::TIME_SECONDS },
    { "ns",  1e-12, UnitConversion::TIME_SECONDS },
    { "Tbps", 1e12, UnitConversion::DATARATE_BPS },
    { "Gbps",  1e9, UnitConversion::DATARATE_BPS },
    { "Mbps",  1e6, UnitConversion::DATARATE_BPS },
    { "Kbps",  1e3, UnitConversion::DATARATE_BPS },
    { "bps",     1, UnitConversion::DATARATE_BPS },
    { "TB",   1e12, UnitConversion::DATA_BYTES },
    { "GB",    1e9, UnitConversion::DATA_BYTES },
    { "MB",    1e6, UnitConversion::DATA_BYTES },
    { "KB",    1e3, UnitConversion::DATA_BYTES },
    { "B",       1, UnitConversion::DATA_BYTES },
    { "km",    1e3, UnitConversion::DISTANCE_METERS },
    { "m",       1, UnitConversion::DISTANCE_METERS },
    { "cm",   1e-2, UnitConversion::DISTANCE_METERS },
    { "mm",   1e-3, UnitConversion::DISTANCE_METERS },
    //XXX stop here? or we have to do watts, Hz, m/s, bauds, 1/s, dB, ...
    { NULL,      0, UnitConversion::UNIT_UNKNOWN }
};

double UnitConversion::parseQuantity(const char *str)  //XXX error handling? return unit type?
{
    double d = 0;
    Unit type = UNIT_UNKNOWN;
    while (*str!='\0')
    {
        // read number into num and skip it
        double num;
        int len;
        while (isspace(*str)) str++;
        if (0==sscanf(str, "%lf%n", &num, &len))
            break; // break if error
        str+=len;

        // is there a unit coming?
        while (isspace(*str)) str++;
        if (!isalpha(*str))
        {
            d += num;
            break; // nothing can come after a plain number  XXX revise
        }

        // extract unit
        char unit[16];
        int i;
        for (i=0; i<15 && isalpha(str[i]); i++)
            unit[i] = str[i];
        if (i==16)
            break; // error: unit name too long XXX revise
        unit[i] = '\0';

        // look up and apply unit
        for (i=0; unitTable[i].name; i++)
            if (!strcmp(unitTable[i].name, unit))
                break;
        if (!unitTable[i].name)
            break; // error: unit not in table  XXX revise
        if (type!=UNIT_UNKNOWN && unitTable[i].type!=type)
            break; // error: unit mismatch   XXX revise
        type = unitTable[i].type;

        d += unitTable[i].mult * num;
    }
    return d;
}


