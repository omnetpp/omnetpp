//=========================================================================
//  CSVWRITER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "commonutil.h"
#include "bigdecimal.h"
#include "opp_ctype.h"
#include "csvwriter.h"

namespace omnetpp {
namespace common {


CsvWriter::~CsvWriter()
{
    if (outp == &fileStream)
        close();
}

void CsvWriter::open(const char *filename, std::ios::openmode mode)
{
    fileStream.open(filename, mode);
    if (!fileStream.is_open())
        throw opp_runtime_error("cannot open '%s' for write", filename);
    outp = &fileStream;
    atLineStart = true;
}

void CsvWriter::close()
{
    Assert(outp == &fileStream);
    fileStream.close();
}

std::ostream& CsvWriter::out()
{
    Assert(outp);
    return *outp;
}

void CsvWriter::writeInt(int64_t value)
{
    writeSep();
    out() << value;
}

void CsvWriter::writeDouble(double value)
{
    writeSep();
    if (isNaN(value))
        out() << "NaN";
    else if (isPositiveInfinity(value))
        out() << "Inf";
    else if (isNegativeInfinity(value))
        out() << "-Inf";
    else
        out() << std::setprecision(prec) << value;
}

void CsvWriter::writeBigDecimal(const BigDecimal& value)
{
    writeSep();
    out() << value.str();
}

void CsvWriter::writeString(const std::string& value)
{
    writeSep();
    if (needsQuote(value)) {
        out() << quoteChar;
        for (char c : value)
            writeChar(c);
        out() << quoteChar;
    }
    else {
        out() << value;
    }
}

void CsvWriter::writeBlank()
{
    writeSep();
}

void CsvWriter::writeNewLine()
{
    out() << std::endl;
    atLineStart = true;
}

void CsvWriter::writeSep()
{
    if (!atLineStart)
        out() << separator;
    atLineStart = false;
}

bool CsvWriter::needsQuote(const std::string& value)
{
    // RFC4180: "Fields containing line breaks (CRLF), double quotes, and commas should be enclosed in double-quotes."
    switch (quoteEscapingMethod) {
        case BACKSLASH:
            for (char c : value)
                if (c == separator || c == quoteChar || c == '\\' || c == '\n')
                    return true;
            return false;

        case DOUBLING:
            for (char c : value)
                if (c == separator || c == quoteChar || c == '\n')
                    return true;
            return false;

        default:
            throw opp_runtime_error("Unknown quote method");
    }
}

void CsvWriter::writeChar(char ch)
{
    switch (quoteEscapingMethod) {
        case BACKSLASH:
            if (ch == '\\' || ch == quoteChar)
                out() << '\\';
            out() << ch;
            break;

        case DOUBLING:
            if (ch == quoteChar)
                out() << ch;
            out() << ch;
            break;
    }
}

} // namespace common
}  // namespace omnetpp
