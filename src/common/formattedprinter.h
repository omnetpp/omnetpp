//=========================================================================
//  FORMATTEDPRINTER.H - part of
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

#ifndef __OMNETPP_COMMON_FORMATTEDPRINTER_H
#define __OMNETPP_COMMON_FORMATTEDPRINTER_H

#include "commondefs.h"

namespace omnetpp {
namespace common {

/**
 * Formatted printing to a stream. Primarily intended for
 * printing help in command-line tools.
 */
class COMMON_API FormattedPrinter
{
private:
    std::ostream& out;
    int margin = 80;
    int optionsWidth = 22;
    int columnSpacing = 2; // for tables
public:
    FormattedPrinter(std::ostream& out) : out(out) {}
    void setMargin(int n) {margin = n;}
    void setOptionsWidth(int n) {optionsWidth = n;}
    void line(const std::string& line = "");  // not wrapped, no blank line after
    void para(const std::string& paragraph); // wrapped, blank line after
    void indentPara(const std::string& paragraph); // indented, wrapped, blank line after
    void option(const std::string& option, const std::string& description);
    void table(const std::string& text, const std::vector<int>& userColumnWidths=std::vector<int>());
};

}  // namespace common
}  // namespace omnetpp

#endif
