//==========================================================================
//  TEXTVIEWERPROVIDERS.H - part of
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

#ifndef __OMNETPP_QTENV_TEXTVIEWERPROVIDERS_H
#define __OMNETPP_QTENV_TEXTVIEWERPROVIDERS_H

#include <unordered_map>
#include <QtCore/QObject>
#include <QtGui/QColor>
#include <QtCore/QDebug>

// cRuntimeException is needed somewhere deep in an ASSERT in
// that logbuffer.h include (for the circularbuffer)
#include "omnetpp/cexception.h"

#include "omnetpp/cmessage.h"

#include "logbuffer.h"
#include "textviewerwidget.h"
#include "loginspector.h"

namespace omnetpp {
namespace qtenv {

class Qtenv;

class QTENV_API TextViewerContentProvider : public QObject {
    Q_OBJECT

public:

    /**
     * Return the number of lines. It should never return zero. It is recommended
     * that this provided adds an empty line at the end of all content (so that
     * the caret can be moved after the last line).
     */
    virtual int getLineCount() = 0;

    /**
     * Return the line at the given line index without delimiters.
     * <p>
     *
     * @param lineIndex index of the line to return. Does not include
     *  delimiters of preceding lines. Index 0 is the first line of the
     *  content.
     * @return the line text without delimiters
     */
    virtual const char *getLineText(int lineIndex) = 0;

    virtual bool showHeaders() = 0;
    virtual QStringList getHeaders() = 0;

    // empty means no status text
    virtual std::string getStatusText() = 0;

    // Only used in messages mode, in which
    // it returns a valid cMessage*, it is
    // a void* here only for the sake of genericity.
    // Or it can be a nullptr, prepare for it!
    virtual void *getUserData(int lineIndex) { return nullptr; }

    virtual eventnumber_t getEventNumberAtLine(int lineIndex) { return -1; }
    virtual int getLineAtEvent(eventnumber_t eventNumber) { return -1; }

    virtual simtime_t getSimTimeAtLine(int lineIndex) { return -1; }
    virtual int getLineAtSimTime(simtime_t simTime) { return -1; }

    // These are useful for implementing user-facing functionality.
    virtual int getBookmarkedLineIndex() { return -1; }
    virtual void bookmarkLine(int lineIndex) { }
    virtual void clearBookmark() { }

Q_SIGNALS:
    void textChanged();
    // This is used to move the cursor and anchor up in the viewer
    // so the content will not "slide up underneath them"
    // when lines are disappearing from the top.
    void linesDiscarded(int numDiscardedLines);
    void linesAdded(); // TODO - maybe numAddedLines as a parameter?
    void statusTextChanged(); // TODO - maybe newStatusText as a parameter?
};

class LineIndexMapping {
    // Indexed by the output line index, values are the source line indices.
    // The last (empty) line must _not_ be added here, to allow quick appending!
    circular_buffer<int> filteredLineIndices;
    // Every value in filteredLineIndices is this much bigger
    // in the internal buffer than shown to the outside.
    // This is to allow apparently decreasing all the values at once
    // without actually iterating through them, which would take longer.
    int numDiscardedSourceLines = 0;

public:
    bool empty() { return filteredLineIndices.empty(); }
    int last() { return filteredLineIndices.back() - numDiscardedSourceLines; }
    int get(int index) { return filteredLineIndices[index] - numDiscardedSourceLines;}

    int discardSourceLines(int numDiscardedLines);
    void normalize();
    void clear();
    void push(int sourceLineIndex);
    int lookup(int sourceLineIndex);
    int size() { return filteredLineIndices.size(); }
};

class QTENV_API LineFilteringContentProvider : public TextViewerContentProvider {
    Q_OBJECT

    TextViewerContentProvider *sourceModel;
    std::string filter;
    bool regex;
    LineIndexMapping indexMapping;
    int lastMatchedSourceLineIndex = -1;
    bool caseSensitive;
    QRegularExpression re;

    bool matchesFilter(const std::string& line, std::string& buffer, QString& qbuffer);
    void updateLineIndices();
    int appendLineIndices();
    int findOutputLineForSourceLine(int sourceLineIndex);

public:
    // sourceModel is taken, filter is copied
    LineFilteringContentProvider(TextViewerContentProvider *sourceModel, const std::string& filter, bool regex, bool caseSensitive);
    ~LineFilteringContentProvider() { delete sourceModel; }

    int getLineCount() override { return indexMapping.size() + 1; } // +1 is the last, empty line
    bool showHeaders() override { return sourceModel->showHeaders(); }
    QStringList getHeaders() override { return sourceModel->getHeaders(); }
    std::string getStatusText() override;
    const char *getLineText(int lineIndex) override;

    virtual void *getUserData(int lineIndex) override;
    virtual eventnumber_t getEventNumberAtLine(int lineIndex) override;
    virtual int getLineAtEvent(eventnumber_t eventNumber) override;
    virtual simtime_t getSimTimeAtLine(int lineIndex) override;
    virtual int getLineAtSimTime(simtime_t simTime) override;

    virtual void clearBookmark() { sourceModel->clearBookmark(); }
    virtual int getBookmarkedLineIndex() override;
    virtual void bookmarkLine(int lineIndex) override;

    void setFiltering(const std::string& filter, bool regex, bool caseSensitive);
};

/**
 * An implementation of ITextViewerContent that uses a string as input.
 *
 * @author Andras
 */
class QTENV_API StringTextViewerContentProvider: public TextViewerContentProvider {
    Q_OBJECT

    std::vector<std::string> lines;  // text, split to lines

public:
    StringTextViewerContentProvider(const std::string& text);

    int getLineCount() override;
    const char *getLineText(int lineIndex) override;
    bool showHeaders() override;
    QStringList getHeaders() override;
};


/**
 *
 * @author Andras
 */
class QTENV_API AbstractEventEntryLinesProvider {
protected:
    int inspectedComponentId;
    const std::set<int>& excludedComponents;
    ComponentHistory *componentHistory;

public:
    AbstractEventEntryLinesProvider(int inspectedComponentId, const std::set<int>& excludedComponents, ComponentHistory *componentHistory)
        : inspectedComponentId(inspectedComponentId), excludedComponents(excludedComponents), componentHistory(componentHistory) { }

    virtual ~AbstractEventEntryLinesProvider() { }

    virtual int getNumLines(LogBuffer::Entry *entry) = 0;
    virtual std::string getLineText(LogBuffer::Entry *entry, int lineIndex) = 0;

    virtual int textLineToBufferIndex(LogBuffer::Entry *entry, int lineIndex) = 0;
    virtual int bufferIndexToTextLine(LogBuffer::Entry *entry, int bufferIndex) = 0;

    // Optional.
    virtual cMessage *getMessageForLine(LogBuffer::Entry *entry, int lineIndex) { ASSERT2(false, "Unimplemented."); return nullptr; };
};



class QTENV_API EventEntryLinesProvider : public AbstractEventEntryLinesProvider {
    // helpers
    bool isAncestorModule(int componentId, int potentialAncestorModuleId);
    bool isMatchingComponent(int componentId);

    bool shouldShowBanner(LogBuffer::Entry *entry, bool shouldShowAnyLine);
    bool shouldShowLine(LogBuffer::Entry *entry, size_t lineIndex);
    bool shouldShowAnyLine(LogBuffer::Entry *entry);

public:
    using AbstractEventEntryLinesProvider::AbstractEventEntryLinesProvider;

    int getNumLines(LogBuffer::Entry *entry) override;
    std::string getLineText(LogBuffer::Entry *entry, int lineIndex) override;

    int textLineToBufferIndex(LogBuffer::Entry *entry, int lineIndex) override;
    int bufferIndexToTextLine(LogBuffer::Entry *entry, int bufferIndex) override;
};

class QTENV_API EventEntryMessageLinesProvider : public AbstractEventEntryLinesProvider {
protected:

    static cMessagePrinter *chooseMessagePrinter(cMessage *msg);

    const cMessagePrinter::Options *messagePrinterOptions;
    static SimTime referenceTime;

    bool isMatchingMessageSend(const LogBuffer::MessageSend& msgSend);
    std::vector<int> findRelevantHopModuleIds(const LogBuffer::MessageSend& msgSend, bool *lastHopIncluded = nullptr);
    LogBuffer::MessageSend& messageSendForLineIndex(LogBuffer::Entry *entry, int lineIndex);

    std::string getRelevantHopsString(const LogBuffer::MessageSend &msg);

public:

    EventEntryMessageLinesProvider(int inspectedComponentId, const std::set<int>& excludedComponents, ComponentHistory *componentHistory, const cMessagePrinter::Options *messagePrinterOptions)
        : AbstractEventEntryLinesProvider(inspectedComponentId, excludedComponents, componentHistory), messagePrinterOptions(messagePrinterOptions) { }

    int getNumLines(LogBuffer::Entry *entry) override;
    std::string getLineText(LogBuffer::Entry *entry, int lineIndex) override;

    int textLineToBufferIndex(LogBuffer::Entry *entry, int lineIndex) override;
    int bufferIndexToTextLine(LogBuffer::Entry *entry, int bufferIndex) override;

    cMessage *getMessageForLine(LogBuffer::Entry *entry, int lineIndex) override;

    static void setReferenceTime(SimTime rt);
    static SimTime getReferenceTime() { return referenceTime; }
};

// A lazily expanding cache for the text of content lines in ModuleOutputContentProvider.
// Encapsulates a circular buffer to allow efficiently discarding lines from the front.
class LineCache {
private:
    // expanded on-demand, only by put, not by find
    circular_buffer<const char *> cachedLines;

public:
    LineCache(): cachedLines(1024) { }
    ~LineCache() { clear(); }

    // Deletes all contents.
    void clear();
    // Drops the first `numLines` lines from the cache.
    // `numLines` is allowed to be >= than the number of lines in the cache.
    void drop(int numLines);
    // Puts a line into the cache at the given index.
    void put(int index, const std::string& line);
    // Returns the line at the given index, or nullptr if it is not in the cache.
    const char *const find(int index) const;
};

/**
 * A data structure that efficiently maps between LogBuffer entries and their
 * corresponding line numbers in the text display. It maintains an index of
 * starting line numbers for each entry in the LogBuffer, calculated using
 * an AbstractEventEntryLinesProvider to determine how many lines each entry occupies.
 * The implementation uses a circular buffer for efficient removal of entries from
 * the beginning without requiring a complete rebuild of the index.
 */
class EntryIndex {
    // This buffer stores the starting line number for each entry in the LogBuffer.
    // To allow efficient removal of entries from the beginning, we use a relative
    // indexing approach: all stored values are offset by the value of the first element.
    // When the first entry is removed, we don't need to adjust all remaining values -
    // we just remove the first value from the buffer, and the offset is implicitly updated.
    // This means externally, the first entry always appears to start at line 0,
    // regardless of its actual internal value.
    circular_buffer<int> entryStartLineNumbers;

    // Adjusts all stored internal values to zero out the implicit offset.
    // Should have no observable effect.
    void normalize();

public:
    bool empty() { return entryStartLineNumbers.empty(); }
    int size() { return entryStartLineNumbers.size(); }
    int get(int i) { return entryStartLineNumbers[i] - entryStartLineNumbers.front(); }
    int last() { return entryStartLineNumbers.back() - entryStartLineNumbers.front(); }
    void clear() { entryStartLineNumbers.clear(); }
    int rebuild(LogBuffer *logBuffer, AbstractEventEntryLinesProvider *linesProvider);
    // Approximately the inverse of `get`. Returns the index of the entry that contains the given line.
    int lookupLine(int lineIndex);
    void discardFirst();
    void push(int startLine);
};

/**
 * The main content provider for LogInspector that displays either textual log,
 * or the sent messages, which are relevant for a given component.
 *
 * The output text looks like this:
 *
 *  <lines for LogBuffer entries>*     <- 0 or more
 *     - Lines for each entry provided by an AbstractEventEntryLinesProvider, a separate
 *       implementation for each mode (LOG, MESSAGES), 0 or more for each.
 *  <empty last line>                  <- mandatory
 *
 * The last empty line ensures that the content is never empty.
 * The status text contains a message about the number of discarded entries
 * and whether module filtering is active.
 */
// FIXME: The "last empty line must always exist" being an implicit part of the API contract sucks...
class QTENV_API ModuleOutputContentProvider: public TextViewerContentProvider {
    Q_OBJECT

public:

    // Instead of storing the bookmark as a simple line index, it is stored
    // as a semantic marker, matching the LogBuffer structure. This way,
    // it can even be preserved across mode and target object changes.
    struct Bookmark {
        // -1 is invalid, meaning the bookmark is "not set".
        eventnumber_t eventNumber = -1;
        // in which mode `bufferIndex` is valid for
        LogInspector::Mode mode = LogInspector::LOG;
        // Index into the `LogBuffer::Entry` for the given event
        // (the `lines` or the `msgs` vector, depending on `lineMode`), not in the filtered view.
        // In LOG mode, it can be -1 to refer to the event banner line.
        int bufferIndex = -1;

        bool isValid() const { return eventNumber != -1; }
    };

private:
    LogBuffer *logBuffer;
    LogInspector::Mode mode;
    Bookmark bookmark;

    ComponentHistory *componentHistory;
    cComponent *inspectedComponent;
    AbstractEventEntryLinesProvider *linesProvider;

    std::set<int> excludedModuleIds;
    const cMessagePrinter::Options *messagePrinterOptions;

    QStringList gatherEnabledColumnNames();

    int lineCount = 1; // for the last empty line
    EntryIndex entryIndex;
    LineCache lineCache;

public:
    ModuleOutputContentProvider(Qtenv *qtenv, cComponent *inspectedComponent, LogInspector::Mode mode, const cMessagePrinter::Options *messagePrinterOptions);
    ~ModuleOutputContentProvider();

    LogBuffer *getLogBuffer() { return logBuffer; }
    ComponentHistory *getComponentHistory() { return componentHistory; }
    cComponent *getInspectedComponent() { return inspectedComponent; }
    AbstractEventEntryLinesProvider *getLinesProvider() { return linesProvider; }

    void setExcludedModuleIds(std::set<int> excludedModuleIds);

    void refresh();

    std::string getStatusText() override;
    int getLineCount() override;
    const char *getLineText(int lineIndex) override;
    bool showHeaders() override;
    QStringList getHeaders() override;
    void *getUserData(int lineIndex) override;

    eventnumber_t getEventNumberAtLine(int lineIndex) override;
    int getLineAtEvent(eventnumber_t eventNumber) override;

    simtime_t getSimTimeAtLine(int lineIndex) override;
    int getLineAtSimTime(simtime_t eventNumber) override;

    int getBookmarkedLineIndex() override;
    void bookmarkLine(int lineIndex) override;
    void clearBookmark() override { setBookmark(Bookmark()); }

    // These are useful for carrying over a semantic bookmark
    // (not a plain line index) to a new content provider instance.
    Bookmark getBookmark() const { return bookmark; }
    void setBookmark(const Bookmark& bookmark);

    // These can be used to convert between semantic bookmarks and line indices.
    Bookmark createBookmarkForLine(int lineIndex);
    int getLineForBookmark(const Bookmark& bookmark);

protected:
    int getIndexOfEntryAt(int lineIndex);

    void invalidateIndex();
    bool isIndexValid();
    void rebuildIndex();

protected Q_SLOTS:
    // See corresponding `LogBuffer` signals for details.
    void onLogEntryAdded(LogBuffer::Entry *entry);
    void onLogLineAdded(LogBuffer::Entry *entry);
    void onMessageSendAdded(LogBuffer::Entry *entry);
    void onEntryDiscarded(LogBuffer::Entry *discardedEntry); // TODO (rename to onFirstEntryDiscarded to convey implicit contract about which entry it is?)
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_TEXTVIEWERPROVIDERS_H
