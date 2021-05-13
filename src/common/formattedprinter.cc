//=========================================================================
//  FORMATTEDPRINTER.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "stringutil.h"
#include "formattedprinter.h"

using namespace std;

namespace omnetpp {
namespace common {

void FormattedPrinter::line(const string& line)
{
    out << line << endl;
}

void FormattedPrinter::para(const string& paragraph)
{
    out << opp_breaklines(paragraph, margin) << endl << endl;
}

void FormattedPrinter::indentPara(const string& paragraph)
{
    string indent = "  ";
    out << indent << opp_trim(opp_indentlines(opp_breaklines(paragraph, margin-indent.length()), indent)) << endl << endl;
}

void FormattedPrinter::option(const string& option, const string& description)
{
    string indent(optionsWidth, ' ');
    out << "  " << option;
    int pos = 2 + option.length();
    if (pos < optionsWidth)
        out << setw(optionsWidth - pos) << "";
    else
        out << endl << indent;
    out << opp_trim(opp_indentlines(opp_breaklines(description, margin-optionsWidth), indent)) << endl;
}

void FormattedPrinter::table(const std::string& text, const std::vector<int>& userColumnWidths)
{
    out << opp_format_table(text, columnSpacing, userColumnWidths);
}

}  // namespace common
}  // namespace omnetpp

