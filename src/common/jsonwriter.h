//=========================================================================
//  JSONWRITER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_JSONWRITER_H
#define __OMNETPP_COMMON_JSONWRITER_H

#include <fstream>
#include <string>
#include <stack>
#include <cstdint>
#include "commondefs.h"

namespace omnetpp {
namespace common {

class BigDecimal;

/**
 * Utility class for writing JSON files. It can also be used to
 * write Python structured data, because it has almost the
 * same syntax.
 */
class COMMON_API JsonWriter
{
private:
    int prec = 14;  // number of significant digits when writing doubles
    int indentSize = 4;  // number of spaces to indent with
    std::string trueStr = "true", falseStr = "false";  // representation of bool constants
    std::string nanStr = "null", infStr = "\"inf\"", negInfStr = "\"-inf\"";  // representation of special numeric values (note: such values are not covered in the JSON spec)

    std::ostream *outp = nullptr;
    std::ofstream fileStream;

    enum Type {NONE, OBJECT, ARRAY};
    struct Container { Type type; bool isOneliner; };
    std::stack<Container> stack;
    bool isCurrentContainerEmpty = true;

private:
    void reset();

public:
    // creation, opening
    JsonWriter() {}
    ~JsonWriter();
    JsonWriter(const char *filename, std::ios::openmode mode=std::ios::out) {open(filename, mode);}
    JsonWriter(std::ostream& out) {setOut(out);}
    void open(const char* filename, std::ios::openmode mode=std::ios::out);
    void setOut(std::ostream& out) {outp = &out; reset();}
    void close(); // only needed when using file output
    std::ostream& out();

    // configuration
    void setPrecision(int p) {prec = p;}
    int getPrecision() const {return prec;}
    void setIndentSize(int n) {indentSize = n;}
    int getIndentSize() const {return indentSize;}

    void setTrueString(const char *s) {trueStr = s;}
    const char *getTrueString() const {return trueStr.c_str();}
    void setFalseString(const char *s) {falseStr = s;}
    const char *getFalseString() const {return falseStr.c_str();}
    void setNanString(const char *s) {nanStr = s;}
    const char *getNanString() const {return nanStr.c_str();}
    void setInfString(const char *s) {infStr = s;}
    const char *getInfString() const {return infStr.c_str();}
    void setNegInfString(const char *s) {negInfStr = s;}
    const char *getNegInfString() const {return negInfStr.c_str();}

    // objects
    void openObject(bool isOneliner=false); // in array
    void openObject(const std::string& key, bool isOneliner=false); // in object
    void openObject(const char *key, bool isOneliner=false) {openObject(std::string(key), isOneliner);}  // DO NOT REMOVE, otherwise the bool overload will kick in
    void closeObject();

    // arrays
    void openArray(bool isOneliner=false); // in array
    void openArray(const std::string& key, bool isOneliner=false);  // in object
    void openArray(const char *key, bool isOneliner=false) {openArray(std::string(key), isOneliner);}  // DO NOT REMOVE, otherwise the bool overload will kick in
    void closeArray();

    // write object fields
    void writeBool(const std::string& key, bool value);
    void writeInt(const std::string& key, int64_t value);
    void writeDouble(const std::string& key, double value);
    void writeBigDecimal(const std::string& key, const BigDecimal& value);
    void writeString(const std::string& key, const std::string& value);
    void writeRaw(const std::string& key, const std::string& value);
    void startRawValue(const std::string& key); // after this, raw value can be written to out(), and lasts until next writeXXX() or closeObject() call

    // write array elements
    void writeBool(bool value);
    void writeInt(int64_t value);
    void writeDouble(double value);
    void writeBigDecimal(const BigDecimal& value);
    void writeString(const std::string& value);
    void writeRaw(const std::string& raw);
    void startRawValue(); // after this, raw value can be written to out(), and lasts until next writeXXX() or closeArray() call

    // low-level file writing -- careful, bypasses structural tests!
    void doWriteBool(bool b);
    void doWriteInt(int64_t value);
    void doWriteDouble(double value);
    void doWriteBigDecimal(const BigDecimal& value);
    void doWriteString(const std::string& value);
    void doWriteRaw(const std::string& value);
    void doWriteKeyEtc(const std::string& value);
    void doWriteArraySep();
    void doWriteNewLine(int relDepth=0);
};

}  // namespace common
}  // namespace omnetpp


#endif
