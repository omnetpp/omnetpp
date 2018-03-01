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
#include "qtenv.h"
#include <QDebug>

namespace omnetpp {
namespace qtenv {

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

void ModuleOutputContentProvider::setFilter(AbstractEventEntryFilter *filter)
{
    if (filter != this->filter) {  // TODO: equality by content?
        this->filter = filter;
        invalidateIndex();
        emit textChanged();
    }
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
            return QString("\x1b[31m[Partial history, %1 earlier entries already discarded]\x1b[0m").arg(numDiscarded);
        --lineIndex;
    }

    Q_ASSERT(lineIndex >= 0 && lineIndex < lineCount);
    if (lineIndex == lineCount-1)  // empty last line
        return "";

    if (lineCache.count(lineIndex) > 0)
        return lineCache[lineIndex];

    int entryIndex = getIndexOfEntryAt(lineIndex);
    LogBuffer::Entry *eventEntry = logBuffer->getEntries()[entryIndex];
    Q_ASSERT(!filter || filter->matches(eventEntry));

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
    Q_ASSERT(!filter || filter->matches(eventEntry));

    return eventEntry->msgs[lineIndex - entryStartLineNumbers[entryIndex]].msg;
}

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
    // don't match the filter; so we have to find the LAST slot with the same line number,
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
    // for ALL event entries, even those that don't match the filter!
    int n = logBuffer->getNumEntries();
    entryStartLineNumbers.resize(n);

    int currentLineNumber = 0;
    for (int i = 0; i < n; i++) {
        entryStartLineNumbers[i] = currentLineNumber;

        LogBuffer::Entry *entry = logBuffer->getEntries()[i];

        if (filter && !filter->matches(entry)) // currently not even necessary, filter is always null
            continue;

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
        while (componentId != -1) {
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
    if (entry->isEvent() && (
                !isMatchingComponent(entry->componentId)
                || excludedComponents.count(entry->componentId) > 0))
        return 0;

    int count = entry->banner ? 1  /* the banner line */ : 0;
    count += entry->lines.size();
    return count;
}

QString EventEntryLinesProvider::getLineText(LogBuffer::Entry *entry, int lineIndex)
{
    if (entry->banner) {
        if (lineIndex == 0) // it's an event banner, or if no component, an info line
            return (entry->componentId <= 0 ? "\x1b[32m" : "\x1b[94m") + QString(entry->banner) + "\x1b[0m";
        else
            lineIndex--;
    }

    if (lineIndex < (int)entry->lines.size()) {
        LogBuffer::Line& line = entry->lines[lineIndex];

        QString text;
        if (line.prefix) {
            text += "\x1b[37m";
            text += line.prefix;
            text += "\x1b[0m";
        }

        // TODO: This still prints the context component path twice for some reason,
        // like in the initialization phaseof samples/routing. There is a mismatch
        // between the stored [context]Component IDs and the stored/formatted texts.
        if (entry->componentId != line.contextComponentId && line.contextComponentId != 0)
            text += (componentHistory->getComponentFullPath(line.contextComponentId)+": ").c_str();

        if (entry->lines[lineIndex].line)
            text += entry->lines[lineIndex].line;

        return text;
    }

    throw std::runtime_error("Log entry line index out of bounds");
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

    QString result;

    if (hops.size() == 2 && hops.front() == inspectedComponentId && hops.back() == inspectedComponentId)
        result = "---->";
    else {
        bool first = true;
        for (size_t i = 0; i < hops.size(); ++i) {
            QString hopName = componentHistory->getComponentFullName(hops[i]);

            if (hops[i] == inspectedComponentId) {
                if (i == 0) {
                    result += "---> ";
                    continue;
                } else if (i == hops.size() - 1) {
                    result += " --->";
                    continue;
                }
            }

            if (!first)
                result += " --> ";
            result += hopName;
            first = false;
        }
    }

    if (lastIsDiscard) {
        int lastIndex = result.lastIndexOf("->");
        bool isLongArrow = lastIndex >= 2 && result[lastIndex-2] == '-';
        result.replace(lastIndex, 2, isLongArrow ? "X-->" : "X->");
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

QString EventEntryMessageLinesProvider::getLineText(LogBuffer::Entry *entry, int lineIndex)
{
    LogBuffer::MessageSend& messageSend = messageSendForLineIndex(entry, lineIndex);
    cMessage *msg = messageSend.msg;

    QString eventNumber = QString::number(entry->eventNumber);

    QString text = QString(
                "\x1b[37m" // gray
                "#%1\t%2\t" // first and second arg, and tabs
                "\x1b[32m" // green
                "%3\t" //
                "\x1b[31m" // red
                "%4\t"
                "\x1b[0m" // reset
                ).arg(
                eventNumber, entry->simtime.str().c_str(),
                getRelevantHopsString(messageSend), msg->getName());

    cMessagePrinter *printer = chooseMessagePrinter(msg);
    std::stringstream os;

    // TODO try-catch
    if (printer)
        printer->printMessage(os, msg, messagePrinterOptions);
    else
        os << "[no message printer for this object]";

    text += QString(os.str().c_str());

    return text;
}

ModulePathsEventEntryFilter::ModulePathsEventEntryFilter(const QStringList& moduleFullPaths, ComponentHistory *componentHistory)
    : moduleFullPaths(moduleFullPaths), componentHistory(componentHistory)
{
}

bool ModulePathsEventEntryFilter::matches(LogBuffer::Entry *entry)
{
    QString moduleFullPath = componentHistory->getComponentFullPath(entry->componentId).c_str();

    // check if moduleFullPath is the same (or prefix of) one of the filter module paths
    for (QString& filterModule : moduleFullPaths) {
        // try to avoid calling String.startsWith(), so first check last-but-one char, as that is
        // likely to differ (it's the last digit of the index if filter ends in a vector submodule)
        int filterModuleLength = filterModule.length();
        if (moduleFullPath.length() >= filterModuleLength)
            if (filterModuleLength < 2 || moduleFullPath.at(filterModuleLength-2) == filterModule.at(filterModuleLength-2))
                if (moduleFullPath.startsWith(filterModule))  // this is the most expensive operation
                    if (moduleFullPath.length() == filterModuleLength || moduleFullPath.at(filterModuleLength) == '.')
                        return true;

    }
    return false;
}

}  // namespace qtenv
}  // namespace omnetpp

