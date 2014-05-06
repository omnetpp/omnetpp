//==========================================================================
//  COMPONENTHISTORY.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __COMPONENTHISTORY_H
#define __COMPONENTHISTORY_H

#include <string>
#include <map>
#include "tkdefs.h"
#include "simutil.h"

NAMESPACE_BEGIN

class cComponentType;

class TKENV_API ComponentHistory
{
    private:
        struct Data {
            int parentModuleId;
            opp_string fullName; //TODO stringpool it!
            cComponentType *componentType;
        };
        std::map<int,Data> components; // indexed by componentId

    private:
        const Data& lookup(int componentId) const {
            std::map<int,Data>::const_iterator it = components.find(componentId);
            ASSERT(it!=components.end());
            return it->second;
        }

    public:
        ComponentHistory() {}
        ~ComponentHistory() {}
        void componentDeleted(cComponent *component);
        //void componentReparented(cComponent *component, cModule *oldParent, int oldId);
        int getParentModuleId(int componentId) const;
        const char *getComponentFullName(int componentId) const;
        std::string getComponentFullPath(int componentId) const;
        std::string getComponentRelativePath(int componentId, int rootComponentId) const;
        cComponentType *getComponentType(int componentId) const;
        std::string getComponentTypeName(int componentId) const;
        void clear();
};

NAMESPACE_END

#endif

