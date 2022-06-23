//==========================================================================
//  TEXTVIEWERPROVIDERS.CC - part of
//
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

#include "textviewerproviders.h"
#include "common/stlutil.h"
#include "common/sgr_macro.h"
#include "qtutil.h"
#include "qtenv.h"
#include <QtCore/QDebug>

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {
namespace qtenv {

// must use the default ctor because it is static
SimTime EventEntryMessageLinesProvider::referenceTime;


QStringList ModuleOutputContentProvider::gatherEnabledColumnNames()
{
    // TODO: only collect headers from message printers actually in use
    // - maybe even to the extent of updating it while scrolling vertically,
    // so only the headers of message printers selected for messages that
    // are actually on the screen in any given moment are shown
    std::vector<std::string> columns;

    for (auto mp : messagePrinters) {
        cMessagePrinter *printer = static_cast<cMessagePrinter *>(mp);

        auto curColumns = printer->getColumnNames(messagePrinterOptions);

        if (columns.size() < curColumns.size())
            columns.resize(curColumns.size());

        for (size_t i = 0; i < curColumns.size(); ++i) {
            if (!columns[i].empty())
                columns[i] += " / ";
            columns[i] += curColumns[i];
        }
    }

    QStringList result;
    for (auto c : columns)
        result << c.c_str();
    return result;
}

ModuleOutputContentProvider::ModuleOutputContentProvider(Qtenv *qtenv, cComponent *inspectedComponent, LogInspector::Mode mode, const cMessagePrinter::Options *messagePrinterOptions)
    : logBuffer(qtenv->getLogBuffer()),
      mode(mode),
      componentHistory(qtenv->getComponentHistory()),
      inspectedComponent(inspectedComponent),
      messagePrinterOptions(messagePrinterOptions)
{
    int componentId = inspectedComponent ? inspectedComponent->getId() : -1;

    if (mode == LogInspector::MESSAGES)
        linesProvider = new EventEntryMessageLinesProvider(componentId, excludedModuleIds, componentHistory, messagePrinterOptions);
    else
        linesProvider = new EventEntryLinesProvider(componentId, excludedModuleIds, componentHistory);

    connect(logBuffer, SIGNAL(logEntryAdded()), this, SLOT(onContentAdded()));
    connect(logBuffer, SIGNAL(logLineAdded()), this, SLOT(onContentAdded()));
    connect(logBuffer, SIGNAL(messageSendAdded()), this, SLOT(onContentAdded()));
    connect(logBuffer, SIGNAL(entryDiscarded(LogBuffer::Entry *)), this, SLOT(onEntryDiscarded(LogBuffer::Entry *)));
}

ModuleOutputContentProvider::~ModuleOutputContentProvider()
{
    delete linesProvider;
}

void ModuleOutputContentProvider::setExcludedModuleIds(std::set<int> excludedModuleIds)
{
    this->excludedModuleIds = excludedModuleIds;
    invalidateIndex();
    emit textChanged();
}

void ModuleOutputContentProvider::refresh()
{
    invalidateIndex();
    emit textChanged();
}

void ModuleOutputContentProvider::invalidateIndex()
{
    lineCache.clear();
    lineCount = -1;
    entryStartLineNumbers.clear();
}

bool ModuleOutputContentProvider::isIndexValid()
{
    return lineCount > 0 && !entryStartLineNumbers.empty();
}

int ModuleOutputContentProvider::getLineCount()
{
    if (!isIndexValid())
        rebuildIndex();
    int numDiscarded = logBuffer->getNumEntriesDiscarded();
    return lineCount + (numDiscarded > 0 ? 1 : 0);
}

QString ModuleOutputContentProvider::getLineText(int lineIndex)
{
    if (!isIndexValid())
        rebuildIndex();

    int numDiscarded = logBuffer->getNumEntriesDiscarded();
    if (numDiscarded > 0) {
        if (lineIndex == 0)
            return QString(SGR(FG_RED) "[Partial history, %1 earlier entries already discarded]" SGR(RESET)).arg(numDiscarded);
        --lineIndex;
    }

    Q_ASSERT(lineIndex >= 0 && lineIndex < lineCount);
    if (lineIndex == lineCount-1)  // empty last line
        return "";

    if (lineCache.count(lineIndex) > 0)
        return lineCache[lineIndex];

    int entryIndex = getIndexOfEntryAt(lineIndex);
    LogBuffer::Entry *eventEntry = logBuffer->getEntries()[entryIndex];

    auto lineText = linesProvider->getLineText(eventEntry, lineIndex - entryStartLineNumbers[entryIndex]);
    while (lineText.endsWith('\n'))
        lineText.chop(1);
    return lineCache[lineIndex] = lineText;
}

bool ModuleOutputContentProvider::showHeaders()
{
    return mode == LogInspector::MESSAGES;
}

QStringList ModuleOutputContentProvider::getHeaders()
{
    QStringList result;

    switch (mode) {
        case LogInspector::LOG:
            result << "prefix" << "line";
            break;
        case LogInspector::MESSAGES:
            result << "Event#" << "Time" << "Relevant Hops" << "Name";

            result += gatherEnabledColumnNames();

            for (int i = 1; i < 10; ++i)
                result << "";
            break;
    }

    return result;
}

void *ModuleOutputContentProvider::getUserData(int lineIndex)
{
    int numDiscarded = logBuffer->getNumEntriesDiscarded();
    if (numDiscarded > 0) {
        if (lineIndex == 0)
            return nullptr;
        --lineIndex;
    }

    if (mode != LogInspector::MESSAGES)
        return nullptr;

    Q_ASSERT(lineIndex >= 0 && lineIndex < lineCount);
    if (lineIndex == lineCount-1)  // empty last line
        return nullptr;

    int entryIndex = getIndexOfEntryAt(lineIndex);
    LogBuffer::Entry *eventEntry = logBuffer->getEntries()[entryIndex];
    return linesProvider->getMessageForLine(eventEntry, lineIndex - entryStartLineNumbers[entryIndex]);
}

eventnumber_t ModuleOutputContentProvider::getEventNumberAtLine(int lineIndex)
{
    int entryIndex = getIndexOfEntryAt(lineIndex);
    if (entryIndex < 0 || entryIndex >= logBuffer->getNumEntries())
        return -1;
    LogBuffer::Entry *eventEntry = logBuffer->getEntries()[entryIndex];
    return eventEntry->eventNumber;
};

int ModuleOutputContentProvider::getLineAtEvent(eventnumber_t eventNumber)
{
    int entryIndex = logBuffer->findEntryByEventNumber(eventNumber);
    if (!isIndexValid())
        rebuildIndex();
    if (entryIndex < 0 || entryIndex >= entryStartLineNumbers.size())
        return -1;
    return entryStartLineNumbers[entryIndex];
};

int ModuleOutputContentProvider::getIndexOfEntryAt(int lineIndex)
{
    // The lineIndex parameter here is already corrected for the single
    // line offset caused by the "incomplete history" notification.
    if (!isIndexValid())
        rebuildIndex();

    int entryIndex = std::upper_bound(
                entryStartLineNumbers.begin(),
                entryStartLineNumbers.end(),
                lineIndex) - entryStartLineNumbers.begin() - 1;

    // entryStartLineNumber[] contains one slot for ALL event entries, even those that
    // contribute zero lines, so we have to find the LAST slot with the same line number,
    // and that will be the matching entry
    int baseLineNumber = entryStartLineNumbers[entryIndex];
    while (entryIndex+1 < (int)entryStartLineNumbers.size() && entryStartLineNumbers[entryIndex+1] == baseLineNumber)
        entryIndex++;

    return entryIndex;
}

void ModuleOutputContentProvider::rebuildIndex()
{
    /*if (!inspectedComponent) { // this caused the nasty assertion failures in the viewer.cc,
        return;                  // but it seems like this is not even needed
    }*/

    // recompute line numbers. note: entryStartLineNumber[] contains one slot
    // for ALL event entries, even those that contribute zero lines!
    int n = logBuffer->getNumEntries();
    entryStartLineNumbers.resize(n);

    int currentLineNumber = 0;
    for (int i = 0; i < n; i++) {
        entryStartLineNumbers[i] = currentLineNumber;
        LogBuffer::Entry *entry = logBuffer->getEntries()[i];
        currentLineNumber += linesProvider->getNumLines(entry);
    }

    lineCount = currentLineNumber + 1;  // note: +1 is for empty last line (content cannot be zero lines!)
}

// Merged the 3 on*Added handlers into this, since they were all the same.
void ModuleOutputContentProvider::onContentAdded()
{
    invalidateIndex();
    emit textChanged();
}

void ModuleOutputContentProvider::onEntryDiscarded(LogBuffer::Entry *entry)
{
    invalidateIndex();
    emit linesDiscarded(linesProvider->getNumLines(entry));
    emit textChanged();
}

StringTextViewerContentProvider::StringTextViewerContentProvider(QString text)
{
    lines = text.split("\n");  // XXX split() discards trailing blank lines
    if (lines.length() == 0)
        lines.append("");
}

int StringTextViewerContentProvider::getLineCount()
{
    return lines.length();
}

QString StringTextViewerContentProvider::getLineText(int lineIndex)
{
    return lines[lineIndex];
}

bool StringTextViewerContentProvider::showHeaders()
{
    return true;
}

QStringList StringTextViewerContentProvider::getHeaders()
{
    return QStringList("part1") << "part2" << "part3" << "part4" << "part5";
}

bool EventEntryLinesProvider::isAncestorModule(int componentId, int potentialAncestorModuleId)
{
    cComponent *component = getSimulation()->getComponent(componentId);
    if (component) {
        // more efficient version for live modules
        while (component) {
            if (component->getId() == potentialAncestorModuleId)
                return true;
            component = component->getParentModule();
        }
    }
    else {
        while (componentId != -1 && componentId != 0) {
            if (componentId == potentialAncestorModuleId)
                return true;
            componentId = componentHistory->getParentModuleId(componentId);
        }
    }
    return false;
}

bool EventEntryLinesProvider::isMatchingComponent(int componentId)
{
    if (componentId <= 0)
        return false;

    return (componentId > 0)
            && (componentId == inspectedComponentId
                || isAncestorModule(componentId, inspectedComponentId));
}

int EventEntryLinesProvider::getNumLines(LogBuffer::Entry *entry)
{
    int count = 0;

    for (size_t i = 0; i < entry->lines.size(); ++i)
        if (shouldShowLine(entry, i))
            count++;

    bool shouldShowAnyLine = count > 0;

    // the line below might iterate on the lines again, but can break early
    if (shouldShowBanner(entry, shouldShowAnyLine))
        count++; // the banner line

    return count;
}

bool EventEntryLinesProvider::shouldShowBanner(LogBuffer::Entry *entry, bool shouldShowAnyLine)
{
    // if there is no banner, we can't show it
    if (entry->banner == nullptr)
        return false;

    // a green "info" line should always be shown
    if (entry->isSystemMessage())
        return true;

    // shouldn't spam banners for "empty" initialization stages (or the genesis entry)
    if (entry->isInitializationStage() && entry->lines.empty())
        return false;

    // if the inspected component is an ancestor of the component of the event, the banner should be shown
    if (!entry->isInitializationStage() && isAncestorModule(entry->componentId, inspectedComponentId))
        return true;

    // otherwise, if any line in this (unrelated) entry should be shown regardless, make sure the banner is there too
    // (for example, when the log was eimtted in a methodcall into the inspected component from an event processed somewhere else)
    return shouldShowAnyLine;
}

 bool EventEntryLinesProvider::shouldShowLine(LogBuffer::Entry *entry, size_t lineIndex)
 {
    LogBuffer::Line &line = entry->lines[lineIndex];
    return !contains(excludedComponents, line.contextComponentId)
        && (entry->isSystemMessage() || line.contextComponentId <= 0 ||
            isAncestorModule(line.contextComponentId, inspectedComponentId)
            || isAncestorModule(entry->componentId, inspectedComponentId));
}

bool EventEntryLinesProvider::shouldShowAnyLine(LogBuffer::Entry *entry)
{
    for (int i = 0; i < entry->lines.size(); i++)
        if (shouldShowLine(entry, i))
            return true;
    return false;
}

QString EventEntryLinesProvider::getLineText(LogBuffer::Entry *entry, int lineIndex)
{
    if (shouldShowBanner(entry, shouldShowAnyLine(entry))) {
        if (lineIndex == 0) // it's an event banner, or if no component, an info line
            return (entry->isSystemMessage() ? SGR(FG_GREEN) : SGR(FG_BRIGHT_BLUE)) + QString(entry->banner) + SGR(RESET);
        else
            lineIndex--; // skipping the banner
    }

    size_t entryLineIndex = 0;

    // looking up which actually stored line is the one that should be shown as the given lineIndex,
    // according to the line-level filtering
    for (; entryLineIndex < entry->lines.size(); ++entryLineIndex)
        if (shouldShowLine(entry, entryLineIndex)) {
            if (lineIndex == 0)
                break;
            --lineIndex;
        }

    if (entryLineIndex >= entry->lines.size())
        throw std::runtime_error("Log entry line index out of bounds");

    LogBuffer::Line& line = entry->lines[entryLineIndex];

    QString text;
    if (line.prefix) {
        switch (entry->lines[entryLineIndex].logLevel) {
            case LOGLEVEL_WARN: text += SGR(FG_YELLOW); break;
            case LOGLEVEL_ERROR: text += SGR(FG_RED); break;
            case LOGLEVEL_FATAL: text += SGR(FG_BRIGHT_RED); break;
            default: text += SGR(FG_WHITE); break;
        }

        text += line.prefix;
        text += SGR(RESET);
    }

    // TODO: This still prints the context component path twice for some reason,
    // like in the initialization phaseof samples/routing. There is a mismatch
    // between the stored [context]Component IDs and the stored/formatted texts.

    if (entry->lines[entryLineIndex].line)
        text += entry->lines[entryLineIndex].line;

    return text;
}

cMessagePrinter *EventEntryMessageLinesProvider::chooseMessagePrinter(cMessage *msg)
{
    cRegistrationList *instance = messagePrinters.getInstance();
    cMessagePrinter *bestPrinter = nullptr;
    int bestScore = 0;

    for (int i = 0; i < (int)instance->size(); i++) {
        cMessagePrinter *printer = (cMessagePrinter *)instance->get(i);
        int score = printer->getScoreFor(msg);
        if (score > bestScore) {
            bestPrinter = printer;
            bestScore = score;
        }
    }

    return bestPrinter;
}

bool EventEntryMessageLinesProvider::isMatchingMessageSend(const LogBuffer::MessageSend &msgSend)
{
    auto relevantHops = findRelevantHopModuleIds(msgSend);
    return (!relevantHops.empty()
            && excludedComponents.count(relevantHops.front()) == 0
            && excludedComponents.count(relevantHops.back()) == 0);
}

std::vector<int> EventEntryMessageLinesProvider::findRelevantHopModuleIds(const LogBuffer::MessageSend& msgSend, bool *lastHopIncluded)
{
    if (lastHopIncluded)
        *lastHopIncluded = false;

    std::vector<int> relevantModuleIds;
    auto& hops = msgSend.hopModuleIds;

    for (int i = 0; i < (int)hops.size(); ++i) {
        int hop = hops[i];
        if (hop == inspectedComponentId ||
                componentHistory->getParentModuleId(hop) == inspectedComponentId) {
            relevantModuleIds.push_back(hop);
            if (lastHopIncluded && (i == (int)hops.size() - 1))
                *lastHopIncluded = true;
        }
    }

    // If we only found a single relevant module, and it happens to be
    // the inspected component itself, adding the previous and next hops
    // into the result, so we can show something useful.
    if (relevantModuleIds.size() == 1 && relevantModuleIds.front() == inspectedComponentId) {
        auto iter = std::find(hops.begin(), hops.end(), inspectedComponentId);

        if (iter != hops.begin())
            relevantModuleIds.insert(relevantModuleIds.begin(), *(iter - 1));

        if (iter + 1 != hops.end())
            relevantModuleIds.push_back(*(iter + 1));

        if (lastHopIncluded && (iter + 2 == hops.end()))
             *lastHopIncluded = true;
    }

    return relevantModuleIds;
}

LogBuffer::MessageSend &EventEntryMessageLinesProvider::messageSendForLineIndex(LogBuffer::Entry *entry, int lineIndex)
{
    int i = 0;
    for (auto& msgSend : entry->msgs) {
        if (isMatchingMessageSend(msgSend)) {
            if (lineIndex == i)
                return msgSend;
            else
                ++i;
        }
    }
    ASSERT(false);
    return entry->msgs.front(); // meh.
}

QString EventEntryMessageLinesProvider::getRelevantHopsString(const LogBuffer::MessageSend& msgsend)
{
    bool lastHopIncluded;
    std::vector<int> hops = findRelevantHopModuleIds(msgsend, &lastHopIncluded);
    bool lastIsDiscard = lastHopIncluded && msgsend.discarded;

    bool incoming = !hops.empty() && hops.front() == inspectedComponentId;
    bool outgoing = !hops.empty() && hops.back() == inspectedComponentId;

    // don't show left arrows for incoming or outgoing messages, or if they are disabled
    bool reversed = (!hops.empty() && !incoming && !outgoing && getQtenv()->opt->allowBackwardArrowsForHops)
                        ? (hops.front() > hops.back())
                        : false;

    if (reversed)
        std::reverse(hops.begin(), hops.end());

    QString result;
    if (hops.size() == 2 && incoming && outgoing)
        // simple passthrough
        result = QString("^.") + componentHistory->getComponentFullName(hops[0])
            + (reversed ? " <-- " : " --> ")
            + QString("^.") + componentHistory->getComponentFullName(hops[1]);
    else {

        int parentId = componentHistory->getParentModuleId(inspectedComponentId);

        bool first = true;
        for (size_t i = 0; i < hops.size(); ++i) {
            QString hopName = componentHistory->getComponentFullName(hops[i]);
            int hopParentId = componentHistory->getParentModuleId(hops[i]);

            if (hopParentId != inspectedComponentId) {
                // if not a submodule, must be the parent, or a sibling
                ASSERT(hops[i] == parentId || hopParentId == parentId);
                hopName = (hops[i] == parentId) ? "^" : "^." + hopName;
            }

            if (hops[i] == inspectedComponentId) {
                if (i == 0) {
                    result += reversed ? ". <-- " : ". --> ";
                    continue;
                } else if (i == hops.size() - 1) {
                    result += reversed ? " <-- ." : " --> .";
                    continue;
                }
            }

            if (!first)
                result += reversed ? " <-- " : " --> ";
            result += hopName;
            first = false;
        }
    }

    if (lastIsDiscard) {
        if (reversed) {
            int firstIndex = result.indexOf("<-");
            result.replace(firstIndex, 2, "<-X");
        }
        else {
            int lastIndex = result.lastIndexOf("->");
            result.replace(lastIndex, 2, "X->");
        }
    }

    /* // Loads of debug output
    qDebug() << "----";
    qDebug() << "inspected:" << inspectedComponentId << componentHistory->getComponentFullPath(inspectedComponentId).c_str();
    qDebug() << "messagehops:";
    for (auto id : msgsend.hopModuleIds) {
        qDebug() << id << componentHistory->getComponentFullPath(id).c_str();
    }
    qDebug() << "relevant hops";
    for (auto id : hops) {
        qDebug() << id << componentHistory->getComponentFullPath(id).c_str();
    }
    qDebug() << "lastHopIncluded:" << lastHopIncluded << "discarded:" << msgsend.discarded << "lastIsDiscard:" << lastIsDiscard;
    qDebug() << "result:" << result;
    */

    return result;
}

int EventEntryMessageLinesProvider::getNumLines(LogBuffer::Entry *entry)
{
    int n = 0;
    for (auto& msgSend : entry->msgs)
        if (isMatchingMessageSend(msgSend))
            ++n;
    return n;
}

static inline int getNumWholeDigits(double x)
{
    // even 0 is one digit
    return std::max(1, (int)std::ceil(std::log10(std::floor(std::abs(x)) + 1)));
}

static inline int getLengthWithSeparators(int digits)
{
    int numGroups = std::ceil(digits / 3.0);
    return digits + std::max(0, numGroups - 1);
}

QString EventEntryMessageLinesProvider::getLineText(LogBuffer::Entry *entry, int lineIndex)
{
    // ---- formatting the Event# column ----
    int eventNumberLength = getNumWholeDigits(cSimulation::getActiveSimulation()->getEventNumber()) + 1; // + 1 is for the #
    QString eventNumberText = ("#" + QString::number(entry->eventNumber)).rightJustified(eventNumberLength);

    // ---- formatting the Time column ----
    SimTime refTime = getReferenceTime();
    SimTime timeToPrint = entry->simtime;
    bool timeIsReference = false;

    if (refTime > 0) {
        if (timeToPrint == refTime)
            timeIsReference = true;
        else
            timeToPrint -= refTime;
    }

    const bool digitGrouping = getQtenv()->opt->messageLogDigitGrouping;

    // between groups of 3
    const char *digitSeparator = digitGrouping
        ? SGR(FG_WHITE) "'" SGR(FG_DEFAULT) // FG_WHITE is actually gray
        : "";

    QString simTimeText = timeToPrint.format(SimTime::getScaleExp(), ".", digitSeparator).c_str();

    if (refTime > 0) {
        if (timeIsReference)
            // timeToPrint was not modified
            simTimeText = "=" + simTimeText;
        else if (timeToPrint > 0) // already relative
            simTimeText = "+" + simTimeText;
        // else nothing, "-" is already there if needed
    }

    // make it decimal-justified
    int maxNumWholeDigits = (refTime > 0)
        // In case of relative display, get the max. of the numDigits for:
        //    refTime: for the "=" line
        //    simTime()-refTime: for the "+" lines
        //    (the length of "-" lines is covered by the "=" line...)
        ? std::max(getNumWholeDigits(refTime.dbl()), getNumWholeDigits((simTime() - refTime).dbl()))
        // we assume that the current simTime() is a maximum for the absolute times that can occur
        : getNumWholeDigits(simTime().dbl());

    int numWholeDigits = getNumWholeDigits(std::abs(timeToPrint.dbl()));

    if (digitGrouping) {
        int maxNumDigitsWithSep = getLengthWithSeparators(maxNumWholeDigits);
        int numDigitsWithSep = getLengthWithSeparators(numWholeDigits);
        ASSERT(maxNumDigitsWithSep >= numDigitsWithSep);
        simTimeText = QString(" ").repeated(maxNumDigitsWithSep - numDigitsWithSep) + simTimeText;

        // remove all trailing '000 groups (incl. escape sequences)
        QString suffix = QString(digitSeparator) + "000";
        simTimeText = stripSuffixes(simTimeText, suffix);
    }
    else {
        ASSERT(maxNumWholeDigits >= numWholeDigits);
        simTimeText = QString(" ").repeated(maxNumWholeDigits - numWholeDigits) + simTimeText;

        // remove all trailing zeroes
        simTimeText = stripSuffixes(simTimeText, "0");
        // we might strip all the fractional digits, so no need for a . either
        // XXX: alternative: add back one zero if we stripped all of them...
        simTimeText = stripSuffixes(simTimeText, ".");
    }

    // highlight the reference time
    if (timeIsReference)
        simTimeText = SGR(BOLD) + simTimeText + SGR(REGULAR);

    // ---- putting the content of the fixed columns together ----
    LogBuffer::MessageSend& messageSend = messageSendForLineIndex(entry, lineIndex);
    cMessage *msg = messageSend.msg;

    QString text = QString(SGR(FG_WHITE) "%1\t" SGR(FG_DEFAULT) "%2\t" SGR(FG_GREEN) "%3\t"                  SGR(FG_RED) "%4\t" SGR(FG_DEFAULT))
                      .arg(               eventNumberText,       simTimeText,         getRelevantHopsString(messageSend), msg ? msg->getFullName() : "<nullptr>");


    cMessagePrinter *printer = msg ? chooseMessagePrinter(msg) : nullptr;
    std::stringstream os;

    if (printer)
        try {
            DisableDebugOnErrors dummy;
            printer->printMessage(os, msg, messagePrinterOptions);
        }
        catch (cRuntimeError &e) {
            os << SGR(FG_BRIGHT_RED)"ERROR: " << e.getFormattedMessage() << SGR(RESET);
        }
    else
        os << SGR(FG_RED) "[no message printer for this object]" SGR(FG_DEFAULT);

    text += QString(os.str().c_str());

    return text;
}

cMessage *EventEntryMessageLinesProvider::getMessageForLine(LogBuffer::Entry *entry, int lineIndex)
{
    return messageSendForLineIndex(entry, lineIndex).msg;
}

void EventEntryMessageLinesProvider::setReferenceTime(SimTime rt)
{
    referenceTime = rt;
    getQtenv()->refreshInspectors();
}

}  // namespace qtenv
}  // namespace omnetpp

