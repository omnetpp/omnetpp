//=========================================================================
//  CSVWRITER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_CSVWRITER_H
#define __OMNETPP_COMMON_CSVWRITER_H

#include <fstream>
#include <string>
#include <cstdint>
#include "commondefs.h"

namespace omnetpp {
namespace common {

class BigDecimal;

/**
 * Utility class for writing CSV files. With the default settings the format
 * complies to RFC 4180, with the exception of line terminators. (This class
 * uses the platform's default line terminator, not always CRLF.)
 */
class COMMON_API CsvWriter
{
public:
    enum QuoteEscapingMethod {DOUBLING, BACKSLASH};

private:
    int prec = 14;       // number of significant digits when writing doubles
    char separator = ',';
    char quoteChar = '"';
    QuoteEscapingMethod quoteEscapingMethod = DOUBLING;

    std::ostream *outp = nullptr;
    std::ofstream fileStream;
    int lineNumber = 0, columnNumber = 0;
    bool insideRaw = false;

private:
    bool needsQuote(const std::string& value);
    void writeChar(char ch);
    void writeSep();
    void doWriteDouble(double value);

public:
    // creation, opening
    CsvWriter() {}
    ~CsvWriter();
    CsvWriter(const char *filename, std::ios::openmode mode=std::ios::out) {open(filename, mode);}
    CsvWriter(std::ostream& out) {setOut(out);}
    void open(const char* filename, std::ios::openmode mode=std::ios::out);
    void setOut(std::ostream& out) {outp = &out;}
    void close(); // only needed when using file output
    std::ostream& out();

    // configuration
    void setPrecision(int p) {prec = p;}
    int getPrecision() const {return prec;}
    void setSeparator(char sep) {separator = sep;}
    char getSeparator() const {return separator;}
    void setQuoteChar(char quoteCh) {quoteChar = quoteCh;}
    char getQuoteChar() const {return quoteChar;}
    void setQuoteEscapingMethod(QuoteEscapingMethod q) {this->quoteEscapingMethod = q;}
    QuoteEscapingMethod getQuoteEscapingMethod() const {return quoteEscapingMethod;}

    // writer methods
    void writeInt(int64_t value);
    void writeDouble(double value);
    void writeBigDecimal(const BigDecimal& value);
    void writeString(const std::string& value);
    void writeBlank();
    void writeNewLine();
    void beginRaw();
    void endRaw();
    void writeRawDouble(double value); // omits separator
    void writeRawQuotedStringBody(const std::string& value);  // omits separator and quote chars

    int getLine() const {return lineNumber;}
    int getColumn() const {return columnNumber;}
};

}  // namespace common
}  // namespace omnetpp


#endif
