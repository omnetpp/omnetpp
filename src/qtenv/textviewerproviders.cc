//==========================================================================
//  TEXTVIEWERPROVIDERS.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "textviewerproviders.h"
#include <QDebug>

namespace omnetpp {
namespace qtenv {

ModuleOutputContentProvider::ModuleOutputContentProvider(Qtenv *qtenv, cComponent *inspectedComponent, LogInspector::Mode mode)
    : logBuffer(qtenv->getLogBuffer()),
      mode(mode),
      componentHistory(qtenv->getComponentHistory()),
      inspectedComponent(inspectedComponent)
{
    if (mode == LogInspector::MESSAGES) {
        linesProvider = new EventEntryMessageLinesProvider(inspectedComponent, componentHistory);
    } else {
        linesProvider = new EventEntryLinesProvider(componentHistory);
    }

    connect(logBuffer, SIGNAL(logEntryAdded()), this, SLOT(onContentAdded()));
    connect(logBuffer, SIGNAL(logLineAdded()), this, SLOT(onContentAdded()));
    connect(logBuffer, SIGNAL(messageSendAdded()), this, SLOT(onContentAdded()));
    connect(logBuffer, SIGNAL(entryDiscarded(LogBuffer::Entry*)), this, SLOT(onEntryDiscarded(LogBuffer::Entry*)));
}

LogBuffer *ModuleOutputContentProvider::getLogBuffer() {
    return logBuffer;
}

AbstractEventEntryLinesProvider *ModuleOutputContentProvider::getLinesProvider() {
    return linesProvider;
}

void ModuleOutputContentProvider::setFilter(AbstractEventEntryFilter *filter) {
    if (filter != this->filter) { // TODO: equality by content?
        this->filter = filter;
        invalidateIndex();
        emit textChanged();
    }
}

AbstractEventEntryFilter *ModuleOutputContentProvider::getFilter() {
    return filter;
}

void ModuleOutputContentProvider::setExcludedModuleIds(std::set<int> excludedModuleIds) {
    this->excludedModuleIds = excludedModuleIds;
    invalidateIndex();
    emit textChanged();
}

void ModuleOutputContentProvider::refresh() {
    invalidateIndex();
    emit textChanged();
}

void ModuleOutputContentProvider::invalidateIndex() {
    lineCount = -1;
    entryStartLineNumbers.clear();
}

bool ModuleOutputContentProvider::isIndexValid() {
    return lineCount > 0 && !entryStartLineNumbers.empty();
}

int ModuleOutputContentProvider::getLineCount() {
    if (!isIndexValid())
        rebuildIndex();
    return lineCount;
}

QString ModuleOutputContentProvider::getLineText(int lineIndex) {
    Q_ASSERT(lineIndex >= 0 && lineIndex < lineCount);
    if (lineIndex == lineCount-1)  // empty last line
        return "";

    int entryIndex = getIndexOfEntryAt(lineIndex);
    LogBuffer::Entry *eventEntry = logBuffer->getEntries()[entryIndex];
    Q_ASSERT(!filter || filter->matches(eventEntry));

    return linesProvider->getLineText(eventEntry,  lineIndex - entryStartLineNumbers[entryIndex]);
}

QList<ModuleOutputContentProvider::TabStop> ModuleOutputContentProvider::getTabStops(int lineIndex) {
    QList<TabStop> tabStops;
    Q_ASSERT(lineIndex >= 0 && lineIndex < lineCount);
    if (lineIndex == lineCount-1) {  // empty last line
        tabStops.append(TabStop(0, QColor()));
        return tabStops;
    }

    int entryIndex = getIndexOfEntryAt(lineIndex);
    LogBuffer::Entry *eventEntry = logBuffer->getEntries()[entryIndex];
    Q_ASSERT(!filter || filter->matches(eventEntry));
    return linesProvider->getTabStops(eventEntry, lineIndex - entryStartLineNumbers[entryIndex]);
}

bool ModuleOutputContentProvider::showHeaders() {
    return mode == LogInspector::MESSAGES;
}

QStringList ModuleOutputContentProvider::getHeaders() {
    return mode == LogInspector::MESSAGES
            ? QStringList("Event#") << "Time" << "Src/Dest" << "Name" << "Info"
            : QStringList("prefix") << "line";
}

void *ModuleOutputContentProvider::getUserData(int lineIndex)
{
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

int ModuleOutputContentProvider::getIndexOfEntryAt(int lineIndex) {
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

void ModuleOutputContentProvider::rebuildIndex() {
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

        if ((!filter || filter->matches(entry)) && isMatchingComponent(entry->componentId))
            currentLineNumber += linesProvider->getNumLines(entry);
    }

    lineCount = currentLineNumber + 1;  // note: +1 is for empty last line (content cannot be zero lines!)
}

// Merged the 3 on*Added handlers into this, since they were all the same.
void ModuleOutputContentProvider::onContentAdded() {
    invalidateIndex();
    emit textChanged();
}

void ModuleOutputContentProvider::onEntryDiscarded(LogBuffer::Entry *entry) {
    invalidateIndex();
    emit linesDiscarded(linesProvider->getNumLines(entry));
    emit textChanged();
}

StringTextViewerContentProvider::StringTextViewerContentProvider(QString text) {
    lines = text.split("\n");  // XXX split() discards trailing blank lines
    if (lines.length() == 0)
        lines.append("");
}

int StringTextViewerContentProvider::getLineCount() {
    return lines.length();
}

QString StringTextViewerContentProvider::getLineText(int lineIndex) {
    return lines[lineIndex];
}

QList<StringTextViewerContentProvider::TabStop> StringTextViewerContentProvider::getTabStops(int lineIndex) {
    static QColor colors[] = {QColor("red"), QColor("green"), QColor("blue"), QColor("black")};
    QList<TabStop> tabStops;
    for (int i = 0; i < 5; ++i)
        tabStops.append(TabStop(5*i, colors[(lineIndex + i) % 4]));

    return tabStops;
}

bool StringTextViewerContentProvider::showHeaders() {
    return true;
}

QStringList StringTextViewerContentProvider::getHeaders() {
    return QStringList("part1") << "part2" << "part3" << "part4" << "part5";
}



EventEntryLinesProvider::EventEntryLinesProvider(ComponentHistory *componentHistory)
    : componentHistory(componentHistory) {

}

int EventEntryLinesProvider::getNumLines(LogBuffer::Entry *entry) {
    int count = entry->isEvent() ? 1 /* the banner line */ : 0;
    count += entry->lines.size();
    return count;
}

QString EventEntryLinesProvider::getLineText(LogBuffer::Entry *entry, int lineIndex) {
    if (entry->isEvent()) {
        if (lineIndex == 0)
            return entry->banner;
        else
            lineIndex--;
    }

    if (lineIndex < (int)entry->lines.size()) {
        LogBuffer::Line &line = entry->lines[lineIndex];

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

    throw std::runtime_error("log entry line index out of bounds");
}

QList<EventEntryLinesProvider::TabStop> EventEntryLinesProvider::getTabStops(LogBuffer::Entry *entry, int lineIndex) {
    QList<TabStop> tabStops;
    if (entry->isEvent()) {
        if (lineIndex == 0) {
            tabStops.append(TabStop(0, QColor("blue")));
            return tabStops;
        } else {
            lineIndex--;
        }
    }

    if (lineIndex < (int)entry->lines.size()) {
        tabStops.append(TabStop(0, QColor("dimgray")));
        LogBuffer::Line &line = entry->lines[lineIndex];

        int prefixLength = line.prefix ? strlen(line.prefix) : 0;

        if (entry->componentId != line.contextComponentId && line.contextComponentId != 0)
            prefixLength += (componentHistory->getComponentFullPath(line.contextComponentId)+": ").length();

        tabStops.append(TabStop(prefixLength, QColor("black")));

        return tabStops;
    }

    throw std::runtime_error("log entry line index out of bounds");
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

int EventEntryMessageLinesProvider::findFirstRelevantHop(const LogBuffer::MessageSend &msgsend, int fromHop) {

    // A hop is relevant (visible in the inspected compound module) when both
    // src and dest are either this module or a direct submodule of it.
    // Normally there is only one relevant hop in a msgsend, although degenerate modules
    // (such as a compound module wired inside like this: in-->out) may result in several hops.

    int inspectedComponentId = inspectedObject->getId();
    int srcModuleId = msgsend.hopModuleIds[fromHop];
    bool isSrcOk = srcModuleId == inspectedComponentId || componentHistory->getParentModuleId(srcModuleId) == inspectedComponentId;
    int n = msgsend.hopModuleIds.size();
    for (int i = fromHop; i < n-1; i++) {
        int destModuleId = msgsend.hopModuleIds[i+1];
        bool isDestOk = destModuleId == inspectedComponentId || componentHistory->getParentModuleId(destModuleId) == inspectedComponentId;
        if (isSrcOk && isDestOk)
            return i;
        isSrcOk = isDestOk;
    }
    return -1;

}

bool ModuleOutputContentProvider::isMatchingComponent(int componentId)
{
    if (componentId == 0 || !inspectedComponent)
        return false;
    int inspectedComponentId = inspectedComponent->getId();
    return (componentId == inspectedComponentId || isAncestorModule(componentId, inspectedComponentId))
                && (excludedModuleIds.count(componentId) == 0);
}

bool ModuleOutputContentProvider::isAncestorModule(int componentId, int potentialAncestorModuleId)
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

QString EventEntryMessageLinesProvider::getMessageSrcDest(const LogBuffer::MessageSend &msgsend) {
    int hopIndex = findFirstRelevantHop(msgsend, 0);
    QString result;

    int inspectedComponentId = inspectedObject->getId();
    if (hopIndex != -1) {
        int srcModuleId = msgsend.hopModuleIds[hopIndex];
        int destModuleId = msgsend.hopModuleIds[hopIndex+1];
        bool hasSrc = srcModuleId != inspectedComponentId;
        bool hasDest = destModuleId != inspectedComponentId;
        if (hasSrc)
            result += componentHistory->getComponentFullName(srcModuleId);
        if (hasSrc && hasDest)
            result += " --> ";
        else if (hasSrc)
            result += " --->";
        else if (hasDest)
            result += "---> ";
        else
            result += "---->";
        if (hasDest)
            result += componentHistory->getComponentFullName(destModuleId);
    }
    else if (msgsend.hopModuleIds.front() == inspectedComponentId) {
        result += componentHistory->getComponentFullName(inspectedComponentId);
        result += " --->";
    }
    else if (msgsend.hopModuleIds.back() == inspectedComponentId) {
        result += "---> ";
        result += componentHistory->getComponentFullName(inspectedComponentId);
    }
    else {
        ASSERT(false);
    }

    return result;
}

EventEntryMessageLinesProvider::EventEntryMessageLinesProvider(cComponent *inspectedObject, ComponentHistory *componentHistory)
    : inspectedObject(inspectedObject), componentHistory(componentHistory)
{

}

int EventEntryMessageLinesProvider::getNumLines(LogBuffer::Entry *entry) {
    int inspectedModuleId = inspectedObject->getId();
    int n = 0;
    for (auto &msgSend : entry->msgs) {
        int hopIndex = findFirstRelevantHop(msgSend, 0);
        if (hopIndex != -1 || msgSend.hopModuleIds.front() == inspectedModuleId || msgSend.hopModuleIds.back() == inspectedModuleId) {
            ++n;
        }
    }
    return n;
}

QString EventEntryMessageLinesProvider::getLineText(LogBuffer::Entry *entry, int lineIndex) {
    int inspectedModuleId = inspectedObject->getId();
    int i = 0;
    for (auto &msgSend : entry->msgs) {
        int hopIndex = findFirstRelevantHop(msgSend, 0);
        // this message should be displayed
        if (hopIndex != -1 || msgSend.hopModuleIds.front() == inspectedModuleId || msgSend.hopModuleIds.back() == inspectedModuleId) {
            if (lineIndex == i) {
                break;
            } else {
                ++i;
            }
        }
    }

    if (lineIndex < (int)entry->msgs.size()) {
        cMessage *msg = entry->msgs[lineIndex].msg;

        QString eventNumber = QString::number(entry->eventNumber);

        QString text = "#" + eventNumber + " ";
        text += entry->simtime.str().c_str();
        text += " ";
        text += getMessageSrcDest(entry->msgs[lineIndex]);
        text += " ";
        text += msg->getName();
        text += " ";

        cMessagePrinter *printer = chooseMessagePrinter(msg);
        std::stringstream os;
        if (printer)
            printer->printMessage(os, msg);
        else
            os << "[no message printer for this object]";

        text += os.str().c_str();

        return text;
    }

    throw std::runtime_error("log entry line index out of bounds");
}

QList<EventEntryMessageLinesProvider::TabStop> EventEntryMessageLinesProvider::getTabStops(LogBuffer::Entry *entry, int lineIndex) {
    QList<TabStop> tabStops;

    if (lineIndex < (int)entry->msgs.size()) {
        int column = 0;

        tabStops.append(TabStop(column, QColor("dimgray")));
        column += 1 + QString::number(entry->eventNumber).length() + 1;

        tabStops.append(TabStop(column, QColor("dimgray")));
        column += entry->simtime.str().length() + 1;

        tabStops.append(TabStop(column, QColor("green")));
        column += getMessageSrcDest(entry->msgs[lineIndex]).length() + 1;

        tabStops.append(TabStop(column, QColor("brown")));
        column += strlen(entry->msgs[lineIndex].msg->getName()) + 1;


        tabStops.append(TabStop(column, QColor("black")));

        return tabStops;
    }

    throw std::runtime_error("log entry line index out of bounds");
}

ModulePathsEventEntryFilter::ModulePathsEventEntryFilter(const QStringList &moduleFullPaths, ComponentHistory *componentHistory)
 : moduleFullPaths(moduleFullPaths), componentHistory(componentHistory) {

}

bool ModulePathsEventEntryFilter::matches(LogBuffer::Entry *entry) {
    QString moduleFullPath = componentHistory->getComponentFullPath(entry->componentId).c_str();

    // check if moduleFullPath is the same (or prefix of) one of the filter module paths
    for (QString &filterModule : moduleFullPaths) {
        // try to avoid calling String.startsWith(), so first check last-but-one char, as that is
        // likely to differ (it's the last digit of the index if filter ends in a vector submodule)
        int filterModuleLength = filterModule.length();
        if (moduleFullPath.length() >= filterModuleLength)
            if (filterModuleLength < 2 || moduleFullPath.at(filterModuleLength-2) == filterModule.at(filterModuleLength-2))
                if (moduleFullPath.startsWith(filterModule))  // this is the most expensive operation
                    if (moduleFullPath.length() == filterModuleLength || moduleFullPath.at(filterModuleLength)=='.')
                        return true;
    }
    return false;
}

} // namespace qtenv
} // namespace omnetpp

