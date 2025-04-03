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

void LineCache::clear()
{
    for (const char *line : cachedLines)
        delete[] line;
    cachedLines.clear();
}

void LineCache::drop(int numLines)
{
    for (int i = 0; i < std::min(cachedLines.size(), numLines); i++)
        delete[] cachedLines[i];
    if (numLines >= cachedLines.size())
        cachedLines.clear();
    else
        cachedLines.pop_front(numLines);
}

void LineCache::put(int index, const std::string& line)
{
    if (index <(int)cachedLines.size()) {
        ASSERT(cachedLines[index] == nullptr);
    }
    else {
        int oldSize = cachedLines.size();
        cachedLines.resize(index + 1);
        for (int i = oldSize; i < index; i++)
            cachedLines[i] = nullptr;
    }
    const char *newLine = new char[line.size() + 1];
    strcpy(const_cast<char *>(newLine), line.c_str());
    cachedLines[index] = newLine;
}

const char *const LineCache::find(int index) const
{
    if (index >= (int)cachedLines.size())
        return nullptr;
    return cachedLines[index];
}

void EntryIndex::normalize()
{
    int numDiscardedLines = entryStartLineNumbers.front();
    for (int i = 0; i < entryStartLineNumbers.size(); i++)
        entryStartLineNumbers[i] -= numDiscardedLines;
    ASSERT(entryStartLineNumbers[0] == 0);
}

int EntryIndex::rebuild(LogBuffer *logBuffer, AbstractEventEntryLinesProvider *linesProvider)
{
    int n = logBuffer->getNumEntries();
    int *buf = entryStartLineNumbers.prepare_fill(n);

    int currentLineNumber = 0;
    for (int i = 0; i < n; i++) {
        *buf++ = currentLineNumber;
        LogBuffer::Entry *entry = logBuffer->getEntries()[i];
        currentLineNumber += linesProvider->getNumLines(entry);
    }

    return currentLineNumber;
}

int EntryIndex::lookupLine(int lineIndex)
{
    // Adjust lineIndex to internal coordinates
    lineIndex += entryStartLineNumbers.front();

    // this is a proper index into the internal vector
    int index = std::upper_bound(
        entryStartLineNumbers.begin(),
        entryStartLineNumbers.end(),
        lineIndex) - entryStartLineNumbers.begin() - 1;

    // Handle zero-line entries by finding the last slot with same line number
    int baseLineNumber = entryStartLineNumbers[index];
    while (index+1 < (int)entryStartLineNumbers.size() && entryStartLineNumbers[index+1] == baseLineNumber)
        index++;

    return index;
}

void EntryIndex::discardFirst()
{
    ASSERT(entryStartLineNumbers.size() > 1);
    entryStartLineNumbers.pop_front();
    // This looks strange, but if we just discarded the last entry
    // (which is unlikely, but still) we can drop everything internally.
    if (empty())
        clear();
    else
        // just to avoid any numerical overflow mishaps
        if (entryStartLineNumbers.front() > 1'000'000'000)
            normalize();
}

void EntryIndex::push(int startLine)
{
    if (entryStartLineNumbers.empty())
        ASSERT(startLine == 0);
    else {
        startLine += entryStartLineNumbers.front();
        ASSERT(startLine >= entryStartLineNumbers.back());
    }
    entryStartLineNumbers.push_back(startLine);
}

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

    connect(logBuffer, SIGNAL(logEntryAdded(LogBuffer::Entry *)), this, SLOT(onLogEntryAdded(LogBuffer::Entry *)));
    connect(logBuffer, SIGNAL(logLineAdded(LogBuffer::Entry *)), this, SLOT(onLogLineAdded(LogBuffer::Entry *)));
    connect(logBuffer, SIGNAL(messageSendAdded(LogBuffer::Entry *)), this, SLOT(onMessageSendAdded(LogBuffer::Entry *)));
    connect(logBuffer, SIGNAL(entryDiscarded(LogBuffer::Entry *)), this, SLOT(onEntryDiscarded(LogBuffer::Entry *)));
}

ModuleOutputContentProvider::~ModuleOutputContentProvider()
{
    delete linesProvider;
}

void ModuleOutputContentProvider::setExcludedModuleIds(std::set<int> excludedModuleIds)
{
    // NOTE: equality check is properly defined by the standard for std::set, but not std::unordered_set!
    if (this->excludedModuleIds != excludedModuleIds) {
        this->excludedModuleIds = excludedModuleIds;
        invalidateIndex();
        Q_EMIT textChanged();
        Q_EMIT statusTextChanged();
    }
}

void ModuleOutputContentProvider::refresh()
{
    invalidateIndex();
    Q_EMIT textChanged();
}

void ModuleOutputContentProvider::invalidateIndex()
{
    lineCache.clear();
    lineCount = -1;
    entryIndex.clear();
}

bool ModuleOutputContentProvider::isIndexValid()
{
    return lineCount > 0 && !entryIndex.empty();
}

std::string ModuleOutputContentProvider::getStatusText()
{
    int numDiscarded = logBuffer->getNumEntriesDiscarded();
    std::string status;

    if (!excludedModuleIds.empty())
        status += "Filtering by modules active. ";

    if (numDiscarded > 0)
        status += std::string("Truncated due to history limit. ");

    return status;
}

int ModuleOutputContentProvider::getLineCount()
{
    if (!isIndexValid())
        rebuildIndex();
    return lineCount;
}

const char *ModuleOutputContentProvider::getLineText(int lineIndex)
{
    if (!isIndexValid())
        rebuildIndex();

    Q_ASSERT(lineIndex >= 0 && lineIndex < lineCount);
    if (lineIndex == lineCount-1)  // empty last line
        return "";

    auto found = lineCache.find(lineIndex);
    if (found)
        return found;

    int entryIdx = getIndexOfEntryAt(lineIndex);
    LogBuffer::Entry *eventEntry = logBuffer->getEntries()[entryIdx];
    std::string lineText = linesProvider->getLineText(eventEntry, lineIndex - entryIndex.get(entryIdx));
    lineCache.put(lineIndex, lineText);
    return lineCache.find(lineIndex);
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
    if (lineIndex < 0)
        return nullptr;

    if (mode != LogInspector::MESSAGES)
        return nullptr;

    Q_ASSERT(lineIndex >= 0 && lineIndex < lineCount);
    if (lineIndex == lineCount-1)  // empty last line
        return nullptr;

    int entryIdx = getIndexOfEntryAt(lineIndex);
    LogBuffer::Entry *eventEntry = logBuffer->getEntries()[entryIdx];
    return linesProvider->getMessageForLine(eventEntry, lineIndex - entryIndex.get(entryIdx));
}

eventnumber_t ModuleOutputContentProvider::getEventNumberAtLine(int lineIndex)
{
    if (lineIndex < 0)
        return -1;

    int entryIndex = getIndexOfEntryAt(lineIndex);
    if (entryIndex < 0 || entryIndex >= logBuffer->getNumEntries())
        return -1;
    LogBuffer::Entry *eventEntry = logBuffer->getEntries()[entryIndex];
    return eventEntry->eventNumber;
}

int ModuleOutputContentProvider::getLineAtEvent(eventnumber_t eventNumber)
{
    int entryIdx = logBuffer->findEntryByEventNumber(eventNumber);
    if (!isIndexValid())
        rebuildIndex();
    if (entryIdx < 0 || entryIdx >= (int)entryIndex.size())
        return -1;
    return entryIndex.get(entryIdx);
}

simtime_t ModuleOutputContentProvider::getSimTimeAtLine(int lineIndex)
{
    if (lineIndex < 0)
        return -1;

    int entryIndex = getIndexOfEntryAt(lineIndex);
    if (entryIndex < 0 || entryIndex >= logBuffer->getNumEntries())
        return -1;
    LogBuffer::Entry *eventEntry = logBuffer->getEntries()[entryIndex];
    return eventEntry->simtime;
}

int ModuleOutputContentProvider::getLineAtSimTime(simtime_t simTime)
{
    int entryIdx = logBuffer->findEntryBySimTime(simTime);
    if (!isIndexValid())
        rebuildIndex();
    if (entryIdx < 0 || entryIdx >= (int)entryIndex.size())
        return -1;
    return entryIndex.get(entryIdx);
}

using Bookmark = ModuleOutputContentProvider::Bookmark;

void ModuleOutputContentProvider::setBookmark(const Bookmark &bookmark)
{
    this->bookmark = bookmark;
    Q_EMIT textChanged();
}

int ModuleOutputContentProvider::getBookmarkedLineIndex()
{
    return bookmark.isValid() ? getLineForBookmark(bookmark) : -1;
}

void ModuleOutputContentProvider::bookmarkLine(int lineIndex)
{
    setBookmark(createBookmarkForLine(lineIndex));
}

Bookmark ModuleOutputContentProvider::createBookmarkForLine(int lineIndex)
{
    if (lineIndex < 0)
        return Bookmark();

    int entryIndex = getIndexOfEntryAt(lineIndex);
    if (entryIndex < 0 || entryIndex >= logBuffer->getNumEntries())
        return Bookmark();
    LogBuffer::Entry *eventEntry = logBuffer->getEntries()[entryIndex];

    eventnumber_t eventNumber = eventEntry->eventNumber;
    int eventStartLine = getLineAtEvent(eventNumber);
    int textLine = linesProvider->textLineToBufferIndex(eventEntry, lineIndex - eventStartLine);

    return { eventNumber, mode, textLine };
}

int ModuleOutputContentProvider::getLineForBookmark(const Bookmark& bookmark)
{
    if (bookmark.isValid()) {
        LogBuffer::Entry *entry = logBuffer->getEntryByEventNumber(bookmark.eventNumber);
        if (!entry)
            return -1;
        int li = linesProvider->bufferIndexToTextLine(entry, bookmark.bufferIndex);

        int line = getLineAtEvent(bookmark.eventNumber);
        if (bookmark.mode == mode)
            line += li;
        return line;
    }
    return -1;
}

int ModuleOutputContentProvider::getIndexOfEntryAt(int lineIndex)
{
    if (!isIndexValid())
        rebuildIndex();

    return entryIndex.lookupLine(lineIndex);
}

void ModuleOutputContentProvider::rebuildIndex()
{
    lineCount = entryIndex.rebuild(logBuffer, linesProvider) + 1;  // note: +1 is for empty last line (content cannot be zero lines!)
}

void ModuleOutputContentProvider::onLogEntryAdded(LogBuffer::Entry *entry)
{
    if (isIndexValid()) {
        entryIndex.push(lineCount-1); // correction for last empty line
        int numLines = linesProvider->getNumLines(entry);
        if (numLines > 0) {
            lineCount += numLines;
            Q_EMIT linesAdded();
        }
    }
}

void ModuleOutputContentProvider::onLogLineAdded(LogBuffer::Entry *entry)
{
    if (mode == LogInspector::LOG) {
        if (isIndexValid()) {
            int newLineCount = entryIndex.last() + linesProvider->getNumLines(entry);
            int properOldLineCount = lineCount - 1; // exclude the empty last line
            // Either:
            //  - The newly added line is filtered out (by component, not text),
            //    so the linecount stays the same.
            //  - The newly added line matches the component filter, so linecount
            //    increases by one.
            //  - On top of the previous, this could be the first line that is not
            //    filtered out by component, so it could also make the entry banner
            //    become visible at the same time, adding two lines to the content.
            ASSERT(newLineCount >= properOldLineCount);
            ASSERT(newLineCount <= properOldLineCount + 2);
            if (newLineCount > properOldLineCount) {
                lineCount = newLineCount + 1; // empty last line
                Q_EMIT linesAdded();
            }
        }
    }
}

void ModuleOutputContentProvider::onMessageSendAdded(LogBuffer::Entry *entry)
{
    if (mode == LogInspector::MESSAGES) {
        if (isIndexValid()) {
            int newLineCount = entryIndex.last() + linesProvider->getNumLines(entry);
            int properOldLineCount = lineCount - 1; // exclude the empty last line
            ASSERT(newLineCount == properOldLineCount || newLineCount == properOldLineCount + 1);
            if (newLineCount > properOldLineCount) {
                lineCount = newLineCount + 1; // empty last line
                Q_EMIT linesAdded();
            }
        }
    }
}

void ModuleOutputContentProvider::onEntryDiscarded(LogBuffer::Entry *entry)
{
    // (implicitly known) It's always the first entry, so we can use entryStartLineNumbers[1], but sanity-check nevertheless
    int numDiscardedLines;
    if (isIndexValid() && entryIndex.size() > 1) {
        numDiscardedLines = entryIndex.get(1);
        ASSERT(numDiscardedLines == linesProvider->getNumLines(entry));
    }
    else
        numDiscardedLines = linesProvider->getNumLines(entry);

    if (isIndexValid()) {
        entryIndex.discardFirst();
        lineCount -= numDiscardedLines;
        ASSERT(lineCount > 0); // the empty line must remain
    }

    if (numDiscardedLines > 0) {
        lineCache.drop(numDiscardedLines);
        Q_EMIT linesDiscarded(numDiscardedLines);
    }

    Q_EMIT statusTextChanged();
}

StringTextViewerContentProvider::StringTextViewerContentProvider(const std::string& text)
{
    std::istringstream f(text);
    std::string temp;
    while (std::getline(f, temp))
        lines.push_back(temp);
    lines.push_back("");
}

int StringTextViewerContentProvider::getLineCount()
{
    return lines.size();
}

const char *StringTextViewerContentProvider::getLineText(int lineIndex)
{
    return lines[lineIndex].c_str();
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
    // system messages are always shown
    if (entry->isSystemMessage())
        return true;

    const LogBuffer::Line& line = entry->lines[lineIndex];

    // if the context component is excluded, don't show the line
    if (contains(excludedComponents, line.contextComponentId))
        return false;

    // only show lines without a context component at the network module
    if (line.contextComponentId <= 0)
        return componentHistory->getParentModuleId(inspectedComponentId) < 0;

    // otherwise, show lines with context components in the subtree of the inspected component,
    // and all lines printed during events being processed anywhere in said subtree
    if (line.contextComponentId == inspectedComponentId || entry->componentId == inspectedComponentId)
        return true; // (this is just a fast path for the trivial case of the check below)
    return isAncestorModule(line.contextComponentId, inspectedComponentId)
            || isAncestorModule(entry->componentId, inspectedComponentId);
}

bool EventEntryLinesProvider::shouldShowAnyLine(LogBuffer::Entry *entry)
{
    for (int i = 0; i < (int)entry->lines.size(); i++)
        if (shouldShowLine(entry, i))
            return true;
    return false;
}

std::string EventEntryLinesProvider::getLineText(LogBuffer::Entry *entry, int lineIndex)
{
    int entryLineIndex = textLineToBufferIndex(entry, lineIndex);
    if (entryLineIndex == -1)
        return (entry->isSystemMessage() || entry->isScheduler() ? SGR(FG_GREEN) : SGR(FG_BRIGHT_BLUE)) + std::string(entry->banner) + SGR(RESET);

    if (entryLineIndex >= entry->lines.size())
        throw std::runtime_error("Log entry line index out of bounds");

    LogBuffer::Line& line = entry->lines[entryLineIndex];

    std::string text;
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

int EventEntryLinesProvider::textLineToBufferIndex(LogBuffer::Entry *entry, int lineIndex)
{
    if (shouldShowBanner(entry, shouldShowAnyLine(entry))) {
        if (lineIndex == 0) // it's an event banner, or if no component, an info line
            return -1;
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

    return entryLineIndex;
}

int EventEntryLinesProvider::bufferIndexToTextLine(LogBuffer::Entry *entry, int bufferIndex)
{
    int lineIndex = 0;
    if (shouldShowBanner(entry, shouldShowAnyLine(entry))) {
        if (bufferIndex == -1)
            return 0;
        lineIndex++; // skipping the banner
    }

    if (bufferIndex == -1)
        return -1;

    for (size_t entryLineIndex = 0; entryLineIndex < entry->lines.size(); ++entryLineIndex)
        if (shouldShowLine(entry, entryLineIndex)) {
            if (entryLineIndex == bufferIndex)
                return lineIndex;
            ++lineIndex;
        }

    return lineIndex;
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
    int msgSendIndex = textLineToBufferIndex(entry, lineIndex);
    ASSERT(msgSendIndex >= 0 && msgSendIndex < (int)entry->msgs.size());
    return entry->msgs[msgSendIndex];
}

std::string EventEntryMessageLinesProvider::getRelevantHopsString(const LogBuffer::MessageSend& msgsend)
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

    std::string result;
    if (hops.size() == 2 && incoming && outgoing)
        // simple passthrough
        result = std::string("^.") + componentHistory->getComponentFullName(hops[0])
            + (reversed ? " <-- " : " --> ")
            + std::string("^.") + componentHistory->getComponentFullName(hops[1]);
    else {

        int parentId = componentHistory->getParentModuleId(inspectedComponentId);

        bool first = true;
        for (size_t i = 0; i < hops.size(); ++i) {
            std::string hopName = componentHistory->getComponentFullName(hops[i]);
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
            size_t firstIndex = result.find("<-");
            result.replace(firstIndex, 2, "<-X");
        }
        else {
            size_t lastIndex = result.rfind("->");
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
std::string EventEntryMessageLinesProvider::getLineText(LogBuffer::Entry *entry, int lineIndex)
{
    // ---- formatting the Event# column ----
    int eventNumberLength = getNumWholeDigits(cSimulation::getActiveSimulation()->getEventNumber()) + 1; // + 1 is for the #
    std::string eventNumberText = "#" + std::to_string(entry->eventNumber);
    eventNumberText = std::string(eventNumberLength - eventNumberText.length(), ' ') + eventNumberText;

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

    std::string simTimeText = timeToPrint.format(SimTime::getScaleExp(), ".", digitSeparator);

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
        simTimeText = std::string(maxNumDigitsWithSep - numDigitsWithSep, ' ') + simTimeText;

        // remove all trailing '000 groups (incl. escape sequences)
        std::string suffix = std::string(digitSeparator) + "000";
        simTimeText = stripSuffixes(simTimeText, suffix);
    }
    else {
        ASSERT(maxNumWholeDigits >= numWholeDigits);
        simTimeText = std::string(maxNumWholeDigits - numWholeDigits, ' ') + simTimeText;

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

    std::ostringstream os;
    os << SGR(FG_WHITE) << eventNumberText << "\t"
          SGR(FG_DEFAULT) << simTimeText << "\t"
          SGR(FG_GREEN) << getRelevantHopsString(messageSend) << "\t"
          SGR(FG_RED) << (msg ? msg->getFullName() : "<nullptr>") << "\t"
          SGR(FG_DEFAULT);

    cMessagePrinter *printer = msg ? chooseMessagePrinter(msg) : nullptr;

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

    return os.str();
}

int EventEntryMessageLinesProvider::textLineToBufferIndex(LogBuffer::Entry *entry, int lineIndex)
{
    int msgSendIndex = 0;
    for (auto& msgSend : entry->msgs) {
        if (isMatchingMessageSend(msgSend)) {
            if (lineIndex == 0)
                return msgSendIndex;
            else
                --lineIndex;
        }
        ++msgSendIndex;
    }
    return -1;
}

int EventEntryMessageLinesProvider::bufferIndexToTextLine(LogBuffer::Entry *entry, int bufferIndex)
{
    int textLine = 0;
    for (auto& msgSend : entry->msgs) {
        if (bufferIndex == 0)
            return textLine;
        if (isMatchingMessageSend(msgSend))
            ++textLine;
        --bufferIndex;
    }
    return -1;
}

cMessage *EventEntryMessageLinesProvider::getMessageForLine(LogBuffer::Entry *entry, int lineIndex)
{
    return messageSendForLineIndex(entry, lineIndex).msg;
}

void EventEntryMessageLinesProvider::setReferenceTime(SimTime rt)
{
    referenceTime = rt;
}

int LineIndexMapping::discardSourceLines(int numDiscardedLines)
{
    int discardedNow = 0;
    for (size_t i = 0; i < filteredLineIndices.size(); ++i) {
        int effectiveValue = filteredLineIndices[i] - numDiscardedSourceLines - numDiscardedLines;
        if (effectiveValue < 0)
            ++discardedNow;
        else
            break;
    }

    numDiscardedSourceLines += numDiscardedLines;
    filteredLineIndices.pop_front(discardedNow);

    // just to avoid any numerical overflow mishaps
    if (numDiscardedSourceLines > 1'000'000'000)
        normalize();

    return discardedNow;
}

void LineIndexMapping::normalize()
{
    for (size_t i = 0; i < filteredLineIndices.size(); ++i)
        filteredLineIndices[i] -= numDiscardedSourceLines;
    numDiscardedSourceLines = 0;
}

void LineIndexMapping::clear()
{
    filteredLineIndices.clear();
    numDiscardedSourceLines = 0;
}

int LineIndexMapping::lookup(int sourceLineIndex)
{
    sourceLineIndex += numDiscardedSourceLines;

    auto foundIter = std::lower_bound(filteredLineIndices.begin(), filteredLineIndices.end(), sourceLineIndex);
    // if the source line is filtered out, we may not get an exact match
    //assert(foundIter != filteredLineIndices.end());
    //assert(*foundIter == sourceLineIndex);
    return foundIter - filteredLineIndices.begin();
}

void LineIndexMapping::push(int sourceLineIndex)
{
    filteredLineIndices.push_back(sourceLineIndex + numDiscardedSourceLines);
}

// ---- LineFilteringContentProvider implementation ----

bool LineFilteringContentProvider::matchesFilter(const std::string& line, std::string& buffer, QString& qbuffer)
{
    const std::string& stripped = stripFormatting(line, buffer);

    if (regex) {
        // TODO in Qt6 - QLatin1StringView instead of copying into qbuffer?
        qbuffer = stripped.c_str();
        return re.match(qbuffer).hasMatch();
    }
    else
        return caseSensitive
            ? strstr(stripped.c_str(), filter.c_str())
            : strcasestr(stripped.c_str(), filter.c_str());
}

void LineFilteringContentProvider::updateLineIndices()
{
    std::string buffer;
    QString qbuffer;
    indexMapping.clear();
    int sourceLineCount = sourceModel->getLineCount();
    // the -1 is to not look at the last empy line (we supply our own)
    for (int i = 0; i < sourceLineCount-1; i++)
        if (matchesFilter(sourceModel->getLineText(i), buffer, qbuffer))
            indexMapping.push(i);
    lastMatchedSourceLineIndex = sourceLineCount - 2;
    ASSERT(lastMatchedSourceLineIndex >= -1);
}

int LineFilteringContentProvider::appendLineIndices()
{
    std::string buffer;
    QString qbuffer;

    int sourceLineCount = sourceModel->getLineCount();
    int addedFilteredLines = 0;
    // +1 is to not re-add the currently added last matching line, -1 is the last empty line (we supply our own)
    for (int i = lastMatchedSourceLineIndex + 1; i < sourceLineCount-1; i++) {
        if (matchesFilter(sourceModel->getLineText(i), buffer, qbuffer)) {
            ASSERT(indexMapping.empty() || i > indexMapping.last());
            indexMapping.push(i);
            ++addedFilteredLines;
        }
    }
    lastMatchedSourceLineIndex = sourceLineCount - 2;
    ASSERT(lastMatchedSourceLineIndex >= -1);
    return addedFilteredLines;
}

int LineFilteringContentProvider::findOutputLineForSourceLine(int sourceLineIndex)
{
    if (sourceLineIndex < 0)
        return -1;
    assert(sourceLineIndex < sourceModel->getLineCount());
    return indexMapping.lookup(sourceLineIndex);
}

LineFilteringContentProvider::LineFilteringContentProvider(TextViewerContentProvider *sourceModel, const std::string& filter, bool regex, bool caseSensitive):
    sourceModel(sourceModel), filter(filter), regex(regex), caseSensitive(caseSensitive)
{
    assert(sourceModel);

    connect(sourceModel, &TextViewerContentProvider::statusTextChanged, [this] {
        Q_EMIT statusTextChanged();
    });

    connect(sourceModel, &TextViewerContentProvider::textChanged, [this] {
        updateLineIndices();
        Q_EMIT textChanged();
    });

    connect(sourceModel, &TextViewerContentProvider::linesDiscarded, [this](int numDiscardedLines) {
        int numDiscardedFilteredLines = indexMapping.discardSourceLines(numDiscardedLines);
        if (numDiscardedFilteredLines > 0)
            Q_EMIT linesDiscarded(numDiscardedFilteredLines);
        lastMatchedSourceLineIndex -= numDiscardedLines;
        ASSERT(lastMatchedSourceLineIndex >= -1);
        ASSERT(lastMatchedSourceLineIndex < this->sourceModel->getLineCount() - 1);
    });

    connect(sourceModel, &TextViewerContentProvider::linesAdded, [this]() {
        int numAddedFilteredLines = appendLineIndices();
        if (numAddedFilteredLines > 0)
            Q_EMIT linesAdded();
    });

    // this compiles the regex (if in use)
    setFiltering(filter, regex, caseSensitive);
}

std::string LineFilteringContentProvider::getStatusText()
{
    std::string status;

    if (!filter.empty())
        status += "Line filter: \"" + filter + "\". ";

    status += sourceModel->getStatusText();

    return status;
}

const char *LineFilteringContentProvider::getLineText(int lineIndex)
{
    if (lineIndex == indexMapping.size()) // the last, empty line
        return "";
    return sourceModel->getLineText(indexMapping.get(lineIndex));
}

void *LineFilteringContentProvider::getUserData(int lineIndex)
{
    if (lineIndex == indexMapping.size()) // the last, empty line
        return nullptr;
    return sourceModel->getUserData(indexMapping.get(lineIndex));
}

eventnumber_t LineFilteringContentProvider::getEventNumberAtLine(int lineIndex)
{
    if (lineIndex == indexMapping.size()) // the last, empty line
        return -1;
    return sourceModel->getEventNumberAtLine(indexMapping.get(lineIndex));
}

int LineFilteringContentProvider::getLineAtEvent(eventnumber_t eventNumber)
{
    return findOutputLineForSourceLine(sourceModel->getLineAtEvent(eventNumber));
}

simtime_t LineFilteringContentProvider::getSimTimeAtLine(int lineIndex)
{
    if (lineIndex == indexMapping.size()) // the last, empty line
        return -1;
    return sourceModel->getSimTimeAtLine(indexMapping.get(lineIndex));
}

int LineFilteringContentProvider::getLineAtSimTime(simtime_t simTime)
{
    return findOutputLineForSourceLine(sourceModel->getLineAtSimTime(simTime));
}

int LineFilteringContentProvider::getBookmarkedLineIndex()
{
    return findOutputLineForSourceLine(sourceModel->getBookmarkedLineIndex());
}

void LineFilteringContentProvider::bookmarkLine(int lineIndex)
{
    if (lineIndex == indexMapping.size()) // the last, empty line
        return; // no-op
    sourceModel->bookmarkLine(indexMapping.get(lineIndex));
}

void LineFilteringContentProvider::setFiltering(const std::string& filter, bool regex, bool caseSensitive)
{
    ASSERT(!filter.empty());
    this->filter = filter;
    this->regex = regex;
    this->caseSensitive = caseSensitive;
    if (regex) {
        re = QRegularExpression(QString::fromStdString(filter),
            caseSensitive ? QRegularExpression::NoPatternOption : QRegularExpression::CaseInsensitiveOption);
        re.optimize();
    }
    updateLineIndices();
    Q_EMIT textChanged();
    Q_EMIT statusTextChanged();
}

}  // namespace qtenv
}  // namespace omnetpp
