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
#include "qtenv.h"
#include <QDebug>

namespace omnetpp {
namespace qtenv {

ModuleOutputContentProvider::ModuleOutputContentProvider(Qtenv *qtenv, cComponent *inspectedComponent, LogInspector::Mode mode)
    : logBuffer(qtenv->getLogBuffer()),
    mode(mode),
    componentHistory(qtenv->getComponentHistory()),
    inspectedComponent(inspectedComponent)
{
    int componentId = inspectedComponent ? inspectedComponent->getId() : -1;

    if (mode == LogInspector::MESSAGES)
        linesProvider = new EventEntryMessageLinesProvider(componentId, excludedModuleIds, componentHistory);
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
    tabStopCache.clear();
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
            return QString("[Partial history, %1 earlier entries already discarded]").arg(numDiscarded);
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

    return lineCache[lineIndex] = linesProvider->getLineText(eventEntry, lineIndex - entryStartLineNumbers[entryIndex]);
}

QList<ModuleOutputContentProvider::TabStop> ModuleOutputContentProvider::getTabStops(int lineIndex)
{
    if (!isIndexValid())
        rebuildIndex();

    QList<TabStop> tabStops;

    int numDiscarded = logBuffer->getNumEntriesDiscarded();
    if (numDiscarded > 0) {
        if (lineIndex == 0) {
            tabStops.append(TabStop(0, QColor("red")));
            return tabStops;
        }
        --lineIndex;
    }

    Q_ASSERT(lineIndex >= 0 && lineIndex < lineCount);
    if (lineIndex == lineCount-1) {  // empty last line
        tabStops.append(TabStop(0, QColor()));
        return tabStops;
    }

    if (tabStopCache.count(lineIndex) > 0)
        return tabStopCache[lineIndex];

    int entryIndex = getIndexOfEntryAt(lineIndex);
    LogBuffer::Entry *eventEntry = logBuffer->getEntries()[entryIndex];
    Q_ASSERT(!filter || filter->matches(eventEntry));
    return tabStopCache[lineIndex] = linesProvider->getTabStops(eventEntry, lineIndex - entryStartLineNumbers[entryIndex]);
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
            result << "Event#" << "Time" << "Relevant Hops" << "Name" << "Info";
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

QList<StringTextViewerContentProvider::TabStop> StringTextViewerContentProvider::getTabStops(int lineIndex)
{
    static QColor colors[] = {
        QColor("red"), QColor("green"), QColor("blue"), QColor("black")
    };
    QList<TabStop> tabStops;
    for (int i = 0; i < 5; ++i)
        tabStops.append(TabStop(5*i, colors[(lineIndex + i) % 4]));

    return tabStops;
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
        if (lineIndex == 0)
            return entry->banner;
        else
            lineIndex--;
    }

    if (lineIndex < (int)entry->lines.size()) {
        LogBuffer::Line& line = entry->lines[lineIndex];

        QString text;
        if (line.prefix) {
            text += line.prefix;
        }

        if (entry->componentId != line.contextComponentId && line.contextComponentId != 0)
            text += (componentHistory->getComponentFullPath(line.contextComponentId)+": ").c_str();

        if (entry->lines[lineIndex].line) {
            text += entry->lines[lineIndex].line;
        }
        return text;
    }

    throw std::runtime_error("Log entry line index out of bounds");
}

QList<EventEntryLinesProvider::TabStop> EventEntryLinesProvider::getTabStops(LogBuffer::Entry *entry, int lineIndex)
{
    QList<TabStop> tabStops;
    if (entry->banner) {
        if (lineIndex == 0) {  // it's a banner, or if no component, an info line
            tabStops.append(TabStop(0, QColor(entry->componentId <= 0 ? "green" : "blue")));
            return tabStops;
        }
        lineIndex--;
    }

    if (lineIndex < (int)entry->lines.size()) {
        tabStops.append(TabStop(0, QColor("dimgray")));
        LogBuffer::Line& line = entry->lines[lineIndex];

        int prefixLength = line.prefix ? strlen(line.prefix) : 0;

        if (entry->componentId != line.contextComponentId && line.contextComponentId != 0)
            prefixLength += (componentHistory->getComponentFullPath(line.contextComponentId)+": ").length();

        tabStops.append(TabStop(prefixLength, QColor("black")));

        return tabStops;
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
        for (int i = 0; i < (int)hops.size(); ++i) {
            QString hopName = componentHistory->getComponentFullName(hops[i]);

            if (hops[i] == inspectedComponentId) {
                if (i == 0) {
                    result += "---> ";
                    continue;
                } else if (i == (int)hops.size() - 1) {
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
    for (auto& msgSend : entry->msgs) {
        if (isMatchingMessageSend(msgSend)) {
            ++n;
        }
    }
    return n;
}

QString EventEntryMessageLinesProvider::getLineText(LogBuffer::Entry *entry, int lineIndex)
{
    LogBuffer::MessageSend& messageSend = messageSendForLineIndex(entry, lineIndex);
    cMessage *msg = messageSend.msg;

    QString eventNumber = QString::number(entry->eventNumber);

    QString text = "#" + eventNumber + " ";
    text += entry->simtime.str().c_str();
    text += " ";
    text += getRelevantHopsString(messageSend);
    text += " ";
    text += msg->getName();
    text += " ";

    cMessagePrinter *printer = chooseMessagePrinter(msg);
    std::stringstream os;
    if (printer)
        printer->printMessage(os, msg);
    else
        os << "[no message printer for this object]";

    text += QString(os.str().c_str()).replace('\t', ' ');

    return text;
}

QList<EventEntryMessageLinesProvider::TabStop> EventEntryMessageLinesProvider::getTabStops(LogBuffer::Entry *entry, int lineIndex)
{
    LogBuffer::MessageSend& messageSend = messageSendForLineIndex(entry, lineIndex);

    QList<TabStop> tabStops;
    int column = 0;

    tabStops.append(TabStop(column, QColor("dimgray")));
    column += 1 + QString::number(entry->eventNumber).length() + 1;

    tabStops.append(TabStop(column, QColor("dimgray")));
    column += entry->simtime.str().length() + 1;

    tabStops.append(TabStop(column, QColor("green")));
    column += getRelevantHopsString(messageSend).length() + 1;

    tabStops.append(TabStop(column, QColor("brown")));
    column += strlen(messageSend.msg->getName()) + 1;

    cMessagePrinter *printer = chooseMessagePrinter(messageSend.msg);
    std::stringstream os;
    if (printer)
        printer->printMessage(os, messageSend.msg);
    else
        os << "[no message printer for this object]";

    QString info = os.str().c_str();

    int tabIndex = 0;
    while (true) {
        tabStops.append(TabStop(column + tabIndex, QColor("black")));
        tabIndex = info.indexOf('\t', tabIndex) + 1;
        if (tabIndex <= 0)
            break;
    }

    return tabStops;
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

