//==========================================================================
//   CFINGERPRINT.CC  - part of
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

#include "omnetpp/cfingerprint.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cpacket.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/cclassdescriptor.h"
#include "omnetpp/crng.h"
#include "omnetpp/cstatistic.h"
#include "omnetpp/cstringtokenizer.h"
#include "parsim/cmemcommbuffer.h"

namespace omnetpp {

Register_Class(cSingleFingerprint);

const char *cSingleFingerprint::MatchableObject::getAsString() const
{
    return object->getFullPath().c_str();
}

const char *cSingleFingerprint::MatchableObject::getAsString(const char *attribute) const
{
    cClassDescriptor *descriptor = const_cast<cObject *>(object)->getDescriptor();
    int fieldId = descriptor->findField(attribute);
    if (fieldId == -1)
        return nullptr;
    else {
        attributeValue = descriptor->getFieldValueAsString(const_cast<cObject *>(object), fieldId, 0);
        return attributeValue.c_str();
    }
}

cSingleFingerprint::cSingleFingerprint()
{
    eventMatcher = nullptr;
    moduleMatcher = nullptr;
    resultMatcher = nullptr;
    hasher = nullptr;
    addEvents = false;
    addScalarResults = false;
    addStatisticResults = false;
    addVectorResults = false;
    addExtraData_ = false;
}

cSingleFingerprint::~cSingleFingerprint()
{
    delete eventMatcher;
    delete moduleMatcher;
    delete resultMatcher;
    delete hasher;
}

void cSingleFingerprint::initialize(const char *expectedFingerprints, const char *categories, const char *eventMatcher, const char *moduleMatcher, const char *resultMatcher)
{
    this->expectedFingerprints = expectedFingerprints;
    hasher = new cHasher();
    parseCategories(categories);
    parseEventMatcher(eventMatcher);
    parseModuleMatcher(moduleMatcher);
    parseResultMatcher(resultMatcher);
}

cSingleFingerprint::FingerprintCategory cSingleFingerprint::getCategory(char ch)
{
    if (strchr("etncklodimparszvx0", ch) == nullptr)
        throw cRuntimeError("Unknown fingerprint category character '%c'", ch);
    return (cSingleFingerprint::FingerprintCategory) ch;
}

void cSingleFingerprint::parseCategories(const char *s)
{
    categories.clear();
    char *current = const_cast<char *>(s);
    while (true)
    {
        char ch = *current;
        if (ch == '\0')
            break;
        else if (ch != ' ') {
            FingerprintCategory category = getCategory(ch);
            if (category == RESULT_SCALAR)
                addScalarResults = true;
            else if (category == RESULT_STATISTIC)
                addStatisticResults = true;
            else if (category == RESULT_VECTOR)
                addVectorResults = true;
            else if (category == EXTRA_DATA)
                addExtraData_ = true;
            else {
                addEvents = true;
                categories.push_back(category);
            }
        }
        current++;
    }
}

void cSingleFingerprint::parseEventMatcher(const char *s)
{
    if (strcmp("*", s)) {
        eventMatcher = new cMatchExpression();
        eventMatcher->setPattern(s, true, false, false);
    }
}

void cSingleFingerprint::parseModuleMatcher(const char *s)
{
    if (strcmp("*", s)) {
        moduleMatcher = new cMatchExpression();
        moduleMatcher->setPattern(s, true, true, true);
    }
}

void cSingleFingerprint::parseResultMatcher(const char *s)
{
    if (strcmp("*", s)) {
        resultMatcher = new cMatchExpression();
        resultMatcher->setPattern(s, true, false, false);
    }
}

void cSingleFingerprint::addEvent(cEvent *event)
{
    if (addEvents) {
        const MatchableObject matchableEvent(event);
        if (eventMatcher == nullptr || eventMatcher->matches(&matchableEvent)) {
            cMessage *message = nullptr;
            cPacket *packet = nullptr;
            cObject *controlInfo = nullptr;
            cModule *module = nullptr;
            if (event->isMessage()) {
                message = static_cast<cMessage *>(event);
                if (message->isPacket())
                    packet = static_cast<cPacket *>(message);
                controlInfo = message->getControlInfo();
                module = message->getArrivalModule();
            }

            MatchableObject matchableModule(module);
            if (module == nullptr || moduleMatcher == nullptr || moduleMatcher->matches(&matchableModule)) {
                //for (auto category : categories) {
                for (std::vector<FingerprintCategory>::iterator it = categories.begin(); it != categories.end(); ++it) {
                    FingerprintCategory category = *it;
                    if (!addEventCategory(event, category)) {
                        switch (category) {
                            case EVENT_NUMBER:
                                hasher->add(getSimulation()->getEventNumber()); break;
                            case SIMULATION_TIME:
                                hasher->add(simTime().raw()); break;
                            case MESSAGE_FULL_NAME:
                                hasher->add(event->getFullName()); break;
                            case MESSAGE_CLASS_NAME:
                                hasher->add(event->getClassName()); break;
                            case MESSAGE_KIND:
                                if (message != nullptr)
                                    hasher->add(message->getKind());
                                break;
                            case MESSAGE_BIT_LENGTH:
                                if (packet != nullptr)
                                    hasher->add(packet->getBitLength());
                                break;
                            case MESSAGE_CONTROL_INFO_CLASS_NAME:
                                if (controlInfo != nullptr)
                                    hasher->add(controlInfo->getClassName());
                                break;
                            case MESSAGE_DATA:
                                if (message != nullptr) {
                                    // NOTE: workaround for control info and context pointer which cannot be packed
                                    // TODO: we should rather use a network byte order serialization API
                                    cMemCommBuffer buffer;
                                    cMessage *copy = message->dup();
                                    copy->parsimPack(&buffer);
                                    hasher->add(buffer.getBuffer(), buffer.getMessageSize());
                                    delete copy;
                                }
                                break;
                            case MODULE_ID:
                                if (module != nullptr)
#ifdef USE_OMNETPP4x_FINGERPRINTS
                                    hasher->add(module->getVersion4ModuleId());
#else
                                    hasher->add(module->getId());
#endif
                                break;
                            case MODULE_FULL_NAME:
                                if (module != nullptr)
                                    hasher->add(module->getFullName());
                                break;
                            case MODULE_FULL_PATH:
                                if (module != nullptr)
                                    hasher->add(module->getFullPath().c_str());
                                break;
                            case MODULE_CLASS_NAME:
                                if (module != nullptr)
                                    hasher->add(module->getComponentType()->getClassName());
                                break;
                            case RANDOM_NUMBERS_DRAWN:
                                for (int i = 0; i < getEnvir()->getNumRNGs(); i++)
                                    hasher->add(getEnvir()->getRNG(i)->getNumbersDrawn());
                                break;
                            case CLEAN_HASHER:
                                hasher->reset();
                                break;
                            default:
                                throw cRuntimeError("Unknown fingerprint category '%d'", category);
                        }
                    }
                }
            }
        }
    }
}

bool cSingleFingerprint::addEventCategory(cEvent *event, FingerprintCategory category)
{
    return false;
}

void cSingleFingerprint::addScalarResult(const cComponent *component, const char *name, double value)
{
    if (addScalarResults) {
        MatchableObject matchableComponent(component);
        if (moduleMatcher == nullptr || moduleMatcher->matches(&matchableComponent)) {
            cNamedObject object(name);
            MatchableObject matchableResult(&object);
            if (resultMatcher == nullptr || resultMatcher->matches(&matchableResult))
                hasher->add(value);
        }
    }
}

void cSingleFingerprint::addStatisticResult(const cComponent *component, const char *name, const cStatistic *value)
{
    if (addStatisticResults) {
        MatchableObject matchableComponent(component);
        if (moduleMatcher == nullptr || moduleMatcher->matches(&matchableComponent)) {
            MatchableObject matchableResult(value);
            if (resultMatcher == nullptr || resultMatcher->matches(&matchableResult)) {
                hasher->add(value->getCount());
                hasher->add(value->getSum());
                hasher->add(value->getMin());
                hasher->add(value->getMax());
                hasher->add(value->getMean());
                hasher->add(value->getStddev());
            }
        }
    }
}

void cSingleFingerprint::addVectorResult(const cComponent *component, const char *name, const simtime_t& t, double value)
{
    if (addVectorResults) {
        MatchableObject matchableComponent(component);
        // TODO: remove workaround for unknown component
        if (moduleMatcher == nullptr || component == nullptr || moduleMatcher->matches(&matchableComponent)) {
            cNamedObject object(name);
            MatchableObject matchableResult(&object);
            if (resultMatcher == nullptr || resultMatcher->matches(&matchableResult)) {
                hasher->add(t.raw());
                hasher->add(value);
            }
        }
    }
}

bool cSingleFingerprint::checkFingerprint() const
{
    cStringTokenizer tokenizer(expectedFingerprints.c_str());
    while (tokenizer.hasMoreTokens()) {
        const char *fingerprint = tokenizer.nextToken();
        if (hasher->equals(fingerprint))
            return true;
    }
    return false;
}

//----

//XXX This is basically equivalent to "for (auto & element : elements)", but we don't want to rely on C++11 yet...
#define for_each_element(CODE) for (std::vector<cFingerprint *>::iterator it = elements.begin(); it != elements.end(); ++it) { cFingerprint *element = *it; CODE; }
#define for_each_element_const(CODE) for (std::vector<cFingerprint *>::const_iterator it = elements.begin(); it != elements.end(); ++it) { cFingerprint *element = *it; CODE; }

cMultiFingerprint::cMultiFingerprint(cFingerprint *prototype) :
    prototype(prototype)
{
}

cMultiFingerprint::~cMultiFingerprint()
{
    delete prototype;
    for_each_element(
        delete element;
    )
}

void cMultiFingerprint::initialize(const char *expectedFingerprintsList, const char *categoriesList, const char *eventMatcherList, const char *moduleMatcherList, const char *resultMatcherList)
{
    cStringTokenizer expectedFingerprintsTokenizer(expectedFingerprintsList, ",");
    cStringTokenizer categoriesTokenizer(categoriesList, ",");
    cStringTokenizer eventMatcherTokenizer(eventMatcherList, ",");
    cStringTokenizer moduleMatcherTokenizer(moduleMatcherList, ",");
    cStringTokenizer resultMatcherTokenizer(resultMatcherList, ",");
    const char *categories = nullptr;
    const char *eventMatcher = nullptr;
    const char *moduleMatcher = nullptr;
    const char *resultMatcher = nullptr;
    while (expectedFingerprintsTokenizer.hasMoreTokens()) {
        const char *expectedFingerprint = expectedFingerprintsTokenizer.nextToken();
        if (categoriesTokenizer.hasMoreTokens())
            categories = categoriesTokenizer.nextToken();
        if (eventMatcherTokenizer.hasMoreTokens())
            eventMatcher = eventMatcherTokenizer.nextToken();
        if (moduleMatcherTokenizer.hasMoreTokens())
            moduleMatcher = moduleMatcherTokenizer.nextToken();
        if (resultMatcherTokenizer.hasMoreTokens())
            resultMatcher = resultMatcherTokenizer.nextToken();
        cFingerprint *fingerprint = static_cast<cFingerprint *>(prototype->dup());
        fingerprint->initialize(expectedFingerprint, categories, eventMatcher, moduleMatcher, resultMatcher);
        elements.push_back(fingerprint);
    }
}

void cMultiFingerprint::addEvent(cEvent *event)
{
    for_each_element(
        element->addEvent(event);
    )
}

void cMultiFingerprint::addScalarResult(const cComponent *component, const char *name, double value)
{
    for_each_element(
        element->addScalarResult(component, name, value);
    )
}

void cMultiFingerprint::addStatisticResult(const cComponent *component, const char *name, const cStatistic *value)
{
    for_each_element(
        element->addStatisticResult(component, name, value);
    )
}

void cMultiFingerprint::addVectorResult(const cComponent *component, const char *name, const simtime_t& t, double value)
{
    for_each_element(
        element->addVectorResult(component, name, t, value);
    )
}

bool cMultiFingerprint::checkFingerprint() const
{
    for_each_element_const(
        if (!element->checkFingerprint())
            return false;
    )
    return true;
}

std::string cMultiFingerprint::info() const
{
    std::stringstream stream;
    for_each_element_const(
        stream << ", " << element->info();
    );
    return stream.str().substr(2);
}

#undef for_each_element

} // namespace omnetpp


