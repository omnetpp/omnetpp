//==========================================================================
//  CCONFIGURATIONREADER.H - part of
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

#ifndef __OMNETPP_CCONFIGURATIONREADER_H
#define __OMNETPP_CCONFIGURATIONREADER_H

#include "simkerneldefs.h"
#include "fileline.h"

namespace omnetpp {

class cConfiguration;

/**
 * @brief Abstract base class for configuration readers.
 *
 * This class presents configuration contents as key-value pairs grouped
 * into sections. This class does not try to make sense of section/key names.
 *
 * The default implementation is InifileReader, but other variants (e.g. those
 * that read the configuration from a database) can also be implemented.
 *
 * @ingroup EnvirAndExtensions
 */
class SIM_API cConfigurationReader
{
  public:
    class SIM_API Callback {
      public:
        virtual ~Callback() {}
        virtual void sectionHeader(const char *sectionName, const char *fileName, int lineNumber) = 0; // note: without the "Config " prefix
        virtual void keyValue(const char *key, const char *value, const char *baseDir, const FileLine& fileLine) = 0;
    };
  protected:
    Callback *callback = nullptr;
  public:
    cConfigurationReader() {}
    cConfigurationReader(Callback *callback) : callback(callback) {}
    virtual ~cConfigurationReader();
    virtual void setCallback(Callback *callback) {this->callback = callback;}
    virtual Callback *getCallback() const {return callback;}
    virtual void read(cConfiguration *spec) = 0;
};

}  // namespace omnetpp

#endif


