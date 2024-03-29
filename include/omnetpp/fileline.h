//==========================================================================
//  FILELINE.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_FILELINE_H
#define __OMNETPP_FILELINE_H

#include "simkerneldefs.h"
#include "opp_pooledstring.h"

namespace omnetpp {

/**
 * Stores location in a file in filename:line form.
 */
class SIM_API FileLine
{
  private:
    opp_staticpooledstring file; // file name (or something like <input> or <cmdline>)
    int line = -1;  // line number
    opp_staticpooledstring note;  // in which section, module, etc.
  public:
    FileLine() {}
    FileLine(const char *file, int line=-1, const char *note=nullptr) : file(file), line(line), note(note) {}
    FileLine(const opp_staticpooledstring& file, int line=-1, const opp_staticpooledstring& note=nullptr) : file(file), line(line), note(note) {}
    bool empty() const {return file.empty() && note.empty();}
    const char *getFilename() const {return file.c_str();}
    int getLineNumber() const {return line;}
    std::string str() const {return empty() ? "" : (line == -1 ? file.str() : file.str() + ":" + std::to_string(line)) + (note.empty() ? "" : ", " + note.str());}
};

}  // namespace omnetpp

#endif

