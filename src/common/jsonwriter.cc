//=========================================================================
//  JSONWRITER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "commonutil.h"
#include "bigdecimal.h"
#include "stringutil.h"
#include "jsonwriter.h"

namespace omnetpp {
namespace common {


JsonWriter::~JsonWriter()
{
    // note: no close() because it could throw! and std::ofstream closes automatically anyway
}

void JsonWriter::reset()
{
    stack = std::stack<Container>(); // clear()
    isCurrentContainerEmpty = true;
}

void JsonWriter::open(const char *filename, std::ios::openmode mode)
{
    fileStream.open(filename, mode);
    if (fileStream.fail())
        throw opp_runtime_error("Cannot open '%s' for write", filename);
    outp = &fileStream;
    reset();
}

void JsonWriter::close()
{
    Assert(outp == &fileStream);
    fileStream.close();
    if (!fileStream)
        throw opp_runtime_error("Error writing JSON file");
}

std::ostream& JsonWriter::out()
{
    Assert(outp);
    return *outp;
}

void JsonWriter::doWriteBool(bool b)
{
    out() << (b ? trueStr :falseStr);
}

void JsonWriter::doWriteInt(int64_t value)
{
    out() << value;
}

void JsonWriter::doWriteDouble(double value)
{
    if (std::isfinite(value))
        out() << std::setprecision(prec) << value;
    else if (isPositiveInfinity(value))
        out() << infStr;
    else if (isNegativeInfinity(value))
        out() << negInfStr;
    else
        out() << nanStr;
}

void JsonWriter::doWriteBigDecimal(const BigDecimal& value)
{
    if (!value.isSpecial())
        out() << value.str();
    else if (value.isPositiveInfinity())
        out() << infStr;
    else if (value.isNegativeInfinity())
        out() << negInfStr;
    else
        out() << nanStr;
}

void JsonWriter::doWriteString(const std::string& value)
{
    out() << opp_quotestr(value);
}

void JsonWriter::doWriteRaw(const std::string& value)
{
    out() << value;
}

void JsonWriter::writeBool(const std::string& key, bool value)
{
    doWriteKeyEtc(key);
    doWriteBool(value);
}

void JsonWriter::writeInt(const std::string& key, int64_t value)
{
    doWriteKeyEtc(key);
    doWriteInt(value);
}

void JsonWriter::writeDouble(const std::string& key, double value)
{
    doWriteKeyEtc(key);
    doWriteDouble(value);
}

void JsonWriter::writeBigDecimal(const std::string& key, const BigDecimal& value)
{
    doWriteKeyEtc(key);
    doWriteBigDecimal(value);
}

void JsonWriter::writeString(const std::string& key, const std::string& value)
{
    doWriteKeyEtc(key);
    doWriteString(value);
}

void JsonWriter::writeRaw(const std::string& key, const std::string& raw)
{
    doWriteKeyEtc(key);
    doWriteRaw(raw);
}

void JsonWriter::startRawValue(const std::string& key)
{
    doWriteKeyEtc(key);
}

void JsonWriter::writeBool(bool value)
{
    doWriteArraySep();
    doWriteBool(value);
}

void JsonWriter::writeInt(int64_t value)
{
    doWriteArraySep();
    doWriteInt(value);
}

void JsonWriter::writeDouble(double value)
{
    doWriteArraySep();
    doWriteDouble(value);
}

void JsonWriter::writeBigDecimal(const BigDecimal& value)
{
    doWriteArraySep();
    doWriteBigDecimal(value);
}

void JsonWriter::writeString(const std::string& value)
{
    doWriteArraySep();
    doWriteString(value);
}

void JsonWriter::writeRaw(const std::string& raw)
{
    doWriteArraySep();
    doWriteRaw(raw);
}

void JsonWriter::startRawValue()
{
    doWriteArraySep();
}

void JsonWriter::doWriteKeyEtc(const std::string& key)
{
    if (stack.empty() || stack.top().type != OBJECT)
        throw opp_runtime_error("JSON: cannot write key, not inside an object");
    if (!isCurrentContainerEmpty)
        out() << ",";
    else
        isCurrentContainerEmpty = false;
    doWriteNewLine();
    doWriteString(key);
    out() << " : ";
}

void JsonWriter::doWriteArraySep()
{
    if (stack.empty() || stack.top().type != ARRAY)
        throw opp_runtime_error("JSON: cannot write array item, not inside an array");
    if (!isCurrentContainerEmpty)
        out() << ",";
    else
        isCurrentContainerEmpty = false;
    doWriteNewLine();
}

void JsonWriter::doWriteNewLine(int relDepth)
{
    if (stack.top().isOneliner)
        out() << " ";
    else
        out() << "\n" << std::setw(indentSize*(stack.size()+relDepth)) << "";
}

void JsonWriter::openObject(bool isOneliner)
{
    if (!stack.empty()) {
        doWriteArraySep();
        isOneliner = isOneliner || stack.top().isOneliner; // make whole subtree one line
    }
    stack.push(Container{OBJECT, isOneliner});
    out() << "{";
    isCurrentContainerEmpty = true;
}

void JsonWriter::openObject(const std::string& key, bool isOneliner)
{
    doWriteKeyEtc(key);
    isOneliner = isOneliner || stack.top().isOneliner; // make whole subtree one line
    stack.push(Container{OBJECT, isOneliner});
    out() << "{";
    isCurrentContainerEmpty = true;
}

void JsonWriter::closeObject()
{
    if (stack.empty() || stack.top().type != OBJECT)
        throw opp_runtime_error("JSON: closeObject: not inside an object");
    doWriteNewLine(-1);
    out() << "}";
    stack.pop();
    isCurrentContainerEmpty = false;
    if (stack.empty())
        out() << "\n";
}

void JsonWriter::openArray(bool isOneliner)
{
    if (!stack.empty()) {
        doWriteArraySep();
        isOneliner = isOneliner || stack.top().isOneliner; // make whole subtree one line
    }
    stack.push(Container{ARRAY, isOneliner});
    out() << "[";
    isCurrentContainerEmpty = true;
}

void JsonWriter::openArray(const std::string& key, bool isOneliner)
{
    doWriteKeyEtc(key);
    isOneliner = isOneliner || stack.top().isOneliner; // make whole subtree one line
    stack.push(Container{ARRAY, isOneliner});
    out() << "[";
    isCurrentContainerEmpty = true;
}

void JsonWriter::closeArray()
{
    if (stack.empty() || stack.top().type != ARRAY)
        throw opp_runtime_error("JSON: closeArray, not inside an array");
    doWriteNewLine(-1);
    out() << "]";
    stack.pop();
    isCurrentContainerEmpty = false;
    if (stack.empty())
        out() << "\n";
}

}  // namespace common
}  // namespace omnetpp
