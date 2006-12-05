//=========================================================================
//  UNITCONVERSION.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "unitconversion.h"

UnitConversion::UnitDesc unitTable[] = {
    { "d",   86400, "s",    "day" },
    { "h",    3600, "s",    "hour" },
    { "mi",     60, "s",    "minute" },
    { "s",       1, "s",    "second" },
    { "ms",   1e-3, "s",    "millisecond" },
    { "us",   1e-9, "s",    "microsecond" },
    { "ns",  1e-12, "s",    "nanosecond" },
    { "Tbps", 1e12, "bps",  "terabit/s" },
    { "Gbps",  1e9, "bps",  "gigabit/s" },
    { "Mbps",  1e6, "bps",  "megabit/s" },
    { "Kbps",  1e3, "bps",  "kilobit/s" },
    { "bps",     1, "bps",  "bit/s" },
    { "TB",   1e12, "B",    "terabyte" },
    { "GB",    1e9, "B",    "gigabyte" },
    { "MB",    1e6, "B",    "megabyte" },
    { "KB",    1e3, "B",    "kilobyte" },
    { "B",       1, "B",    "byte" },
    { "km",    1e3, "m",    "kilometer" },
    { "m",       1, "m",    "meter" },
    { "cm",   1e-2, "m",    "centimeter" },
    { "mm",   1e-3, "m",    "millimeter" },
    { "mW",   1e-3, "W",    "milliwatt" },
    { "W",       1, "W",    "watt" },
    { "GHz",   1e9, "Hz",   "gigaherz" },
    { "MHz",   1e6, "Hz",   "megaherz" },
    { "kHz",   1e3, "Hz",   "kiloherz" },
    { "Hz",      1, "Hz",   "herz" },
    { NULL,      0, NULL,   NULL }
};

UnitConversion::UnitDesc *UnitConversion::lookupUnit(const char *unit)
{
    for (int i=0; unitTable[i].unit; i++)
        if (!strcmp(unitTable[i].unit, unit))
            return unitTable+i;
    return NULL;
}

double UnitConversion::parseQuantity(const char *str, const char *unit)  //XXX error handling? return unit type?
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


