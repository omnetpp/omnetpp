//==========================================================================
//  SERIALIZER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CMDENV_SERIALIZER_H
#define __CMDENV_SERIALIZER_H

#ifdef SIMFRONTEND_SUPPORT

#include <map>
#include "cmddefs.h"
#include "cobject.h"
#include "json.h"

NAMESPACE_BEGIN

/**
 * TODO
 */
class CMDENV_API Serializer
{
  private:
    // object<->id mapping (we don't want to return pointer values to our HTTP client)
    std::map<cObject*,long> objectToIdMap;
    std::map<long,cObject*> idToObjectMap;
    long lastObjectId;

  public:
    Serializer();

    cObject *getObjectById(long id);
    long getIdForObject(cObject *obj);
    std::string getIdStringForObject(cObject *obj);
    void objectDeleted(cObject *obj);

    JsonObject *serializeObject(cObject *obj, JsonObject *jObject, int64 lastRefreshSerial);
    JsonObject *serializeObjectChildren(cObject *obj, JsonObject *jObject, int64 lastRefreshSerial);
    JsonObject *serializeObjectFields(cObject *obj, JsonObject *jObject, int64 lastRefreshSerial);
    const char *getKnownBaseClass(cObject *object);
};

NAMESPACE_END

#endif

#endif

