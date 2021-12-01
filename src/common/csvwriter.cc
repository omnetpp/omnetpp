//=========================================================================
//  CSVWRITER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cmath>
#include "commonutil.h"
#include "bigdecimal.h"
#include "opp_ctype.h"
#include "csvwriter.h"

namespace omnetpp {
namespace common {


CsvWriter::~CsvWriter()
{
    // note: no close() because it could throw! and std::ofstream closes automatically anyway
}

void CsvWriter::open(const char *filename, std::ios::openmode mode)
{
    fileStream.open(filename, mode);
    if (fileStream.fail())
        throw opp_runtime_error("Cannot open '%s' for write", filename);
    outp = &fileStream;
    lineNumber = columnNumber = 0;
}

void CsvWriter::close()
{
    Assert(outp == &fileStream);
    fileStream.close();
    if (!fileStream)
        throw opp_runtime_error("Error writing CSV file");
}

std::ostream& CsvWriter::out()
{
    Assert(outp);
    return *outp;
}

void CsvWriter::writeInt(int64_t value)
{
    Assert(!insideRaw);
    writeSep();
    out() << value;
    columnNumber++;
}

void CsvWriter::writeDouble(double value)
{
    Assert(!insideRaw);
    writeSep();
    doWriteDouble(value);
    columnNumber++;
}

void CsvWriter::writeBigDecimal(const BigDecimal& value)
{
    Assert(!insideRaw);
    writeSep();
    out() << value.str();
    columnNumber++;
}

void CsvWriter::writeString(const std::string& value)
{
    Assert(!insideRaw);
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
    columnNumber++;
}

void CsvWriter::writeBlank()
{
    Assert(!insideRaw);
    writeSep();
    columnNumber++;
}

void CsvWriter::writeNewLine()
{
    Assert(!insideRaw);
    out() << std::endl;
    lineNumber++;
    columnNumber = 0;
}

void CsvWriter::beginRaw()
{
    Assert(!insideRaw); // cannot nest beginRaw() calls
    writeSep();
    insideRaw = true;
}

void CsvWriter::endRaw()
{
    Assert(insideRaw);  // missing beginRaw()
    insideRaw = false;
    columnNumber++;
}

void CsvWriter::writeRawDouble(double value)
{
    Assert(insideRaw);
    doWriteDouble(value);
}

void CsvWriter::writeRawQuotedStringBody(const std::string& value)
{
    Assert(insideRaw);
    for (char c : value)
        writeChar(c);
}

void CsvWriter::writeSep()
{
    if (columnNumber != 0)
        out() << separator;
}

void CsvWriter::doWriteDouble(double value)
{
    if (std::isfinite(value))
        out() << std::setprecision(prec) << value;
    else if (isPositiveInfinity(value))
        out() << "Inf";
    else if (isNegativeInfinity(value))
        out() << "-Inf";
    else
        out() << "NaN";
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

}  // namespace common
}  // namespace omnetpp
