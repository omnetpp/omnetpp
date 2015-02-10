//==========================================================================
//  SERIALIZER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
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

#ifdef SIMFRONTEND_SUPPORT

#include "serializer.h"
#include "cproperties.h"
#include "cproperty.h"
#include "cenum.h"
#include "cscheduler.h"
#include "visitor.h"
#include "cclassdescriptor.h"
#include "cpacket.h"
#include "cqueue.h"
#include "cchannel.h"
#include "coutvector.h"
#include "csimplemodule.h"
#include "cstatistic.h"
#include "cdensityestbase.h"
#include "cwatch.h"
#include "cdisplaystring.h"


USING_NAMESPACE

Serializer::Serializer()
{
    lastObjectId = 0;
}

cObject *Serializer::getObjectById(long id)
{
    if (id == 0)
        return NULL;
    std::map<long,cObject*>::iterator it = idToObjectMap.find(id);
    return it == idToObjectMap.end() ? NULL : it->second;
}

long Serializer::getIdForObject(cObject *obj)
{
    if (obj == NULL)
        return 0;

    std::map<cObject*,long>::iterator it = objectToIdMap.find(obj);
    if (it == objectToIdMap.end())
    {
        int id = ++lastObjectId;
        //TODO debug("[objcache] assigning id: %d --> (%s)%s\n", id, obj->getClassName(), obj->getFullPath().c_str());
        objectToIdMap[obj] = id;
        idToObjectMap[id] = obj;
        return id;
    }
    else
        return it->second;
}

std::string Serializer::getIdStringForObject(cObject *obj)
{
    if (!obj)
        return "0";
    std::stringstream out;
    out << getIdForObject(obj) << ":" << getKnownBaseClass(obj);
    return out.str();
}

void Serializer::objectDeleted(cObject *obj)
{
    if (!objectToIdMap.empty()) // this method has to have minimal overhead if there are no object queries (maps are empty)
    {
        std::map<cObject*,long>::iterator it = objectToIdMap.find(obj);
        if (it != objectToIdMap.end())
        {
            long id = it->second;
            objectToIdMap.erase(it);
            idToObjectMap.erase(idToObjectMap.find(id)); //XXX this lookup could be eliminated if objectToIdMap contained idToObjectMap::iterator as value!
        }
    }
}

JsonObject *Serializer::serializeObject(cObject *obj, JsonObject *jObject, int64 lastRefreshSerial)
{
    if (!obj->hasChangedSince(lastRefreshSerial))
        return jObject;

    if (!jObject)
        jObject = new JsonObject();

    // cObject
    jObject->put("name", jsonWrap(obj->getName()));
    jObject->put("fullName", jsonWrap(obj->getFullName()));
    jObject->put("fullPath", jsonWrap(obj->getFullPath()));
    jObject->put("className", jsonWrap(obj->getClassName()));
    jObject->put("knownBaseClass", jsonWrap(getKnownBaseClass(obj)));
    jObject->put("info", jsonWrap(obj->info()));
    jObject->put("owner", jsonWrap(getIdStringForObject(obj->getOwner())));
    jObject->put("descriptor", jsonWrap(getIdStringForObject(obj->getDescriptor())));
    cClassDescriptor *desc = obj->getDescriptor();
    const char *icon = desc ? desc->getProperty("icon") : NULL;
    jObject->put("icon", jsonWrap(icon ? icon : ""));

    // cComponent
    if (dynamic_cast<cComponent *>(obj)) {
        cComponent *component = (cComponent *)obj;

        jObject->put("parentModule", jsonWrap(getIdStringForObject(component->getParentModule())));
        jObject->put("componentType", jsonWrap(getIdStringForObject(component->getComponentType())));
        jObject->put("displayString", jsonWrap(component->parametersFinalized() ? component->getDisplayString().str() : ""));

        JsonArray *jparameters = new JsonArray();
        for (int i = 0; i < component->getNumParams(); i++) {
            cPar *par = &component->par(i);
            jparameters->push_back(jsonWrap(getIdStringForObject(par)));
        }
        jObject->put("parameters", jparameters);
    }

    // cModule
    if (dynamic_cast<cModule *>(obj)) {
        cModule *mod = (cModule *)obj;
        jObject->put("moduleId", jsonWrap(mod->getId()));
        jObject->put("index", jsonWrap(mod->getIndex()));
        jObject->put("vectorSize", jsonWrap(mod->isVector() ? mod->getVectorSize() : -1));

        JsonArray *jgates = new JsonArray();
        for (cModule::GateIterator i(mod); !i.end(); i++) {
            cGate *gate = i();
            jgates->push_back(jsonWrap(getIdStringForObject(gate)));
        }
        jObject->put("gates", jgates);

        JsonArray *jsubmodules = new JsonArray();
        for (cModule::SubmoduleIterator i(mod); !i.end(); i++) {
            cModule *submod = i();
            jsubmodules->push_back(jsonWrap(getIdStringForObject(submod)));
        }
        jObject->put("submodules", jsubmodules);
    }

    // cPar
    if (dynamic_cast<cPar *>(obj)) {
        cPar *par = (cPar *)obj;
        jObject->put("value", jsonWrap(par->str()));
        jObject->put("type", jsonWrap(cPar::getTypeName(par->getType())));
        jObject->put("isVolatile", jsonWrap(par->isVolatile()));
        jObject->put("unit", jsonWrap(par->getUnit()));
        jObject->put("properties", jsonWrap(getIdStringForObject(par->getProperties())));
    }

    // cGate
    if (dynamic_cast<cGate *>(obj)) {
        cGate *gate = (cGate *)obj;
        jObject->put("type", jsonWrap(cGate::getTypeName(gate->getType())));
        jObject->put("gateId", jsonWrap(gate->getId()));
        jObject->put("index", jsonWrap(gate->getIndex()));
        jObject->put("vectorSize", jsonWrap(gate->isVector() ? gate->getVectorSize() : -1));
        jObject->put("ownerModule", jsonWrap(getIdStringForObject(gate->getOwnerModule())));
        jObject->put("nextGate", jsonWrap(getIdStringForObject(gate->getNextGate())));
        jObject->put("previousGate", jsonWrap(getIdStringForObject(gate->getPreviousGate())));
        jObject->put("channel", jsonWrap(getIdStringForObject(gate->getChannel())));
    }

    // cChannel
    if (dynamic_cast<cChannel *>(obj)) {
        cChannel *channel = (cChannel *)obj;
        jObject->put("isTransmissionChannel", jsonWrap(channel->isTransmissionChannel()));
        jObject->put("sourceGate", jsonWrap(getIdStringForObject(channel->getSourceGate())));
    }

    // cMessage
    if (dynamic_cast<cMessage *>(obj)) {
        cMessage *msg = (cMessage *)obj;
        jObject->put("kind", jsonWrap(msg->getKind()));
        jObject->put("priority", jsonWrap(msg->getSchedulingPriority()));
        jObject->put("id", jsonWrap(msg->getId()));
        jObject->put("treeId", jsonWrap(msg->getTreeId()));
        jObject->put("senderModule", jsonWrap(getIdStringForObject(msg->getSenderModule())));
        jObject->put("senderGate", jsonWrap(getIdStringForObject(msg->getSenderGate())));
        jObject->put("arrivalModule", jsonWrap(getIdStringForObject(msg->getArrivalModule())));
        jObject->put("arrivalGate", jsonWrap(getIdStringForObject(msg->getArrivalGate())));
        jObject->put("sendingTime", jsonWrap(msg->getSendingTime()));
        jObject->put("arrivalTime", jsonWrap(msg->getArrivalTime()));
    }

    // cPacket
    if (dynamic_cast<cPacket *>(obj)) {
        cPacket *msg = (cPacket *)obj;
        jObject->put("bitLength", jsonWrap(msg->getBitLength()));
        jObject->put("bitError", jsonWrap(msg->hasBitError()));
        jObject->put("duration", jsonWrap(msg->getDuration()));
        jObject->put("isReceptionStart", jsonWrap(msg->isReceptionStart()));
        jObject->put("encapsulatedPacket", jsonWrap(getIdStringForObject(msg->_getEncapMsg())));
        jObject->put("encapsulationId", jsonWrap(msg->getEncapsulationId()));
        jObject->put("encapsulationTreeId", jsonWrap(msg->getEncapsulationTreeId()));
    }

    // cSimulation
    if (dynamic_cast<cSimulation *>(obj)) {
        cSimulation *sim = (cSimulation *)obj;
        jObject->put("systemModule", jsonWrap(getIdStringForObject(sim->getSystemModule())));
        jObject->put("scheduler", jsonWrap(getIdStringForObject(sim->getScheduler())));
        jObject->put("messageQueue", jsonWrap(getIdStringForObject(&sim->getMessageQueue())));
        JsonArray *jmodules = new JsonArray();
        for (int id = 0; id <= sim->getLastComponentId(); id++)
            jmodules->push_back(jsonWrap(getIdStringForObject(sim->getModule(id)))); //FIXME rather: getComponent() !!!!!
        jObject->put("modules", jmodules);
    }

    // cMessageHeap
    if (dynamic_cast<cMessageHeap *>(obj)) {
        cMessageHeap *fes = (cMessageHeap *)obj;
        fes->sort(); // extremely handy for the UI
        JsonArray *msgs = new JsonArray();
        for (int i = 0; i < fes->length(); i++)
            msgs->push_back(jsonWrap(getIdStringForObject(fes->peek(i))));
        jObject->put("messages", msgs);
    }

    // cComponentType: nothing worth serializing

    // cClassDescriptor
    if (dynamic_cast<cClassDescriptor*>(obj)) {
        cClassDescriptor *desc = (cClassDescriptor *)obj;
        jObject->put("baseClassDescriptor", jsonWrap(getIdStringForObject(desc->getBaseClassDescriptor())));
        jObject->put("extendsCObject", jsonWrap(desc->extendsCObject()));
        JsonObject *jProperties = new JsonObject();
        const char **propertyNames = desc->getPropertyNames();
        if (propertyNames) {
            for (const char **pp = propertyNames; *pp; pp++) {
                const char *name = *pp;
                const char *value = desc->getProperty(name);
                jProperties->put(name, jsonWrap(value));
            }
        }
        jObject->put("properties", jProperties);
        JsonArray *jFields = new JsonArray();
        for (int fieldId = 0; fieldId < desc->getFieldCount(); fieldId++)
        {
            JsonObject *jField = new JsonObject();
            jField->put("name", jsonWrap(desc->getFieldName(fieldId)));
            jField->put("type", jsonWrap(desc->getFieldTypeString(fieldId)));
            jField->put("declaredOn", jsonWrap(desc->getFieldDeclaredOn(fieldId)));
            JsonObject *jFieldProperties = new JsonObject();
            const char **propertyNames = desc->getFieldPropertyNames(fieldId);
            if (propertyNames) {
                for (const char **pp = propertyNames; *pp; pp++) {
                    const char *name = *pp;
                    const char *value = desc->getFieldProperty(fieldId, name);
                    jFieldProperties->put(name, jsonWrap(value));
                }
            }
            jField->put("properties", jFieldProperties);
            //TODO: merge this into a single "flags" field!
            if (desc->getFieldIsArray(fieldId))
                jField->put("isArray", jsonWrap(true));
            if (desc->getFieldIsCompound(fieldId))
                jField->put("isCompound", jsonWrap(true));
            if (desc->getFieldIsPointer(fieldId))
                jField->put("isPointer", jsonWrap(true));
            if (desc->getFieldIsCObject(fieldId))
                jField->put("isCObject", jsonWrap(true));
            if (desc->getFieldIsCOwnedObject(fieldId))
                jField->put("isCOwnedObject", jsonWrap(true));
            if (desc->getFieldIsEditable(fieldId))
                jField->put("isEditable", jsonWrap(true));
            if (desc->getFieldStructName(fieldId))
                jField->put("structName", jsonWrap(desc->getFieldStructName(fieldId)));
            jFields->push_back(jField);
        }
        jObject->put("fields", jFields);
    }
    return jObject;
}

JsonObject *Serializer::serializeObjectChildren(cObject *obj, JsonObject *jObject, int64 lastRefreshSerial)
{
    if (!obj->hasChangedSince(lastRefreshSerial))
        return jObject;

    if (!jObject)
        jObject = new JsonObject();

    cCollectChildrenVisitor visitor(obj);
    visitor.process(obj);

    JsonArray *jchildren = new JsonArray();
    for (int i = 0; i < visitor.getArraySize(); i++)
        jchildren->push_back(jsonWrap(getIdStringForObject(visitor.getArray()[i])));
    jObject->put("children", jchildren);
    return jObject;
}

JsonObject *Serializer::serializeObjectFields(cObject *obj, JsonObject *jObject, int64 lastRefreshSerial)
{
    if (!obj->hasChangedSince(lastRefreshSerial))
        return jObject;

    if (!jObject)
        jObject = new JsonObject();

    cClassDescriptor *desc = obj->getDescriptor();
    if (!desc)
        return jObject;

    JsonArray *jFields = new JsonArray();
    for (int fieldId = 0; fieldId < desc->getFieldCount(); fieldId++)
    {
        // Note: we only need to serialize the field's value (or values if it's an array)
        // because all other info about fields (type, declaring class, properties, etc)
        // is available via the class descriptor, cClassDescriptor.
        JsonObject *jField = new JsonObject();
        jField->put("name", jsonWrap(desc->getFieldName(fieldId))); // strictly speaking this is redundant (index already identifies the field) but provides more safety and readability
        if (desc->getFieldIsCObject(fieldId))
            jField->put("isObjRef", jsonWrap(true));  // value(s) are object references (produced with getIdStringForObject())
        if (!desc->getFieldIsArray(fieldId)) {
            if (!desc->getFieldIsCompound(fieldId))
                jField->put("value", jsonWrap(desc->getFieldValueAsString(obj, fieldId, 0)));
            else {
                void *ptr = desc->getFieldStructValuePointer(obj, fieldId, 0);
                if (desc->getFieldIsCObject(fieldId)) {
                    cObject *o = (cObject *)ptr;
                    jField->put("value", jsonWrap(getIdStringForObject(o)));
                }
                else {
                    jField->put("value", jsonWrap("...some struct...")); //XXX look up using getFieldStructName() -- recursive!
                }
            }
        } else {
            int n = desc->getFieldArraySize(obj, fieldId);
            JsonArray *jValues = new JsonArray();
            for (int i = 0; i < n; i++) {
                if (!desc->getFieldIsCompound(fieldId))
                    jValues->push_back(jsonWrap(desc->getFieldValueAsString(obj, fieldId, i)));
                else {
                    void *ptr = desc->getFieldStructValuePointer(obj, fieldId, i);
                    if (desc->getFieldIsCObject(fieldId)) {
                        cObject *o = (cObject *)ptr;
                        jValues->push_back(jsonWrap(getIdStringForObject(o)));
                    }
                    else {
                        jValues->push_back(jsonWrap("...some struct...")); //XXX look up using getFieldStructName() -- recursive!
                    }
                }
            }
            jField->put("values", jValues);
        }
        jFields->push_back(jField);
    }
    jObject->put("fields", jFields);
    return jObject;
}

const char *Serializer::getKnownBaseClass(cObject *object)
{
    if (dynamic_cast<cModule *>(object) && ((cModule *)object)->isPlaceholder())
        return"cPlaceholderModule";
    else if (dynamic_cast<cSimpleModule *>(object))
        return "cSimpleModule";
    else if (dynamic_cast<cModule *>(object))
        return "cModule";
    else if (dynamic_cast<cPacket *>(object))
        return "cPacket";
    else if (dynamic_cast<cMessage *>(object))
        return "cMessage";
    else if (dynamic_cast<cArray *>(object))
        return "cArray";
    else if (dynamic_cast<cQueue *>(object))
        return "cQueue";
    else if (dynamic_cast<cGate *>(object))
        return "cGate";
    else if (dynamic_cast<cPar *>(object))
        return "cPar";
    else if (dynamic_cast<cChannel *>(object))
        return "cChannel";
    else if (dynamic_cast<cOutVector *>(object))
        return "cOutVector";
    else if (dynamic_cast<cDensityEstBase *>(object))
        return "cDensityEstBase";
    else if (dynamic_cast<cStatistic *>(object))
        return "cStatistic";
    else if (dynamic_cast<cSimulation*>(object))
        return "cSimulation";
    else if (dynamic_cast<cMessageHeap *>(object))
        return "cMessageHeap";
    else if (dynamic_cast<cWatchBase *>(object))
        return "cWatchBase";
    else if (dynamic_cast<cComponentType*>(object))
        return "cComponentType";
    else if (dynamic_cast<cRegistrationList*>(object))
        return "cRegistrationList";
    else if (dynamic_cast<cClassDescriptor*>(object))
        return "cClassDescriptor";
    else
        return "cObject"; // note: cOwnedObject and cNamedObject are not interesting
}

#endif
