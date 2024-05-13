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
    virtual std::string getLineText(int lineIndex) = 0;

    virtual bool showHeaders() = 0;
    virtual QStringList getHeaders() = 0;

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
    StringTextViewerContentProvider(std::string text);

    int getLineCount() override;
    std::string getLineText(int lineIndex) override;
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

/**
 * The main content provider for LogInspector that displays either textual log,
 * or the sent messages, which are relevant for a given component.
 *
 * The output text looks like this:
 *
 *  [<preface line>]                   <- optional, based on `LogBuffer::getNumEntriesDiscarded()`
 *  <lines for LogBuffer entries>*     <- 0 or more
 *     - Lines for each entry provided by an AbstractEventEntryLinesProvider, a separate
 *       implementation for each mode (LOG, MESSAGES), 0 or more for each.
 *  <empty last line>                  <- mandatory
 *
 * Depending on whether there's a preface present or not, there may or may not
 * be an offset of 1 between the line indices of the provided text, and the
 * lines gathered from the LogBuffer entries. Pay attention to this!
 * This preface line shows a message about the number of discarded entries.
 * The last empty line ensures that the content is never empty.
 * See also: isPrefacePresent(), adjustLineIndexForPrefaceIn(), and adjustLineIndexForPrefaceOut()
 */
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

    // Cached data - NOTE that NONE of these include the line index offset caused by the preface, if present!
    // Use adjustLineIndexForPrefaceIn() and adjustLineIndexForPrefaceOut() to correct for this!
    int lineCount = 1; // for the last empty line
    std::vector<int> entryStartLineNumbers; // indexed by the entry's index in logBuffer
    std::unordered_map<int, std::string> lineCache;

public:
    ModuleOutputContentProvider(Qtenv *qtenv, cComponent *inspectedComponent, LogInspector::Mode mode, const cMessagePrinter::Options *messagePrinterOptions);
    ~ModuleOutputContentProvider();

    LogBuffer *getLogBuffer() { return logBuffer; }
    ComponentHistory *getComponentHistory() { return componentHistory; }
    cComponent *getInspectedComponent() { return inspectedComponent; }
    AbstractEventEntryLinesProvider *getLinesProvider() { return linesProvider; }

    void setExcludedModuleIds(std::set<int> excludedModuleIds);

    void refresh();

    int getLineCount() override;
    std::string getLineText(int lineIndex) override;
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
    // NOTE: The lineIndex parameter here should be corrected for the preface offset!
    int getIndexOfEntryAt(int lineIndex);

    bool isPrefacePresent() const { return logBuffer->getNumEntriesDiscarded() > 0; }
    int adjustLineIndexForPrefaceIn(int lineIndex) const { return isPrefacePresent() ? lineIndex - 1 : lineIndex; }
    int adjustLineIndexForPrefaceOut(int lineIndex) const { return isPrefacePresent() ? lineIndex + 1 : lineIndex; }

    void invalidateIndex();
    bool isIndexValid();
    void rebuildIndex();

protected Q_SLOTS:
    void onContentAdded();
    void onEntryDiscarded(LogBuffer::Entry *discardedEntry);
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_TEXTVIEWERPROVIDERS_H
