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

// the logbuffer.h file included in the header indirectly includes
// simkerneldefs.h which undefs emit, but we need it here, for Qt reasons
#define emit

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
    virtual QString getLineText(int lineIndex) = 0;

    virtual bool showHeaders() = 0;
    virtual QStringList getHeaders() = 0;

    // Only used in messages mode, in which
    // it returns a valid cMessage*, it is
    // a void* here only for the sake of genericity.
    // Or it can be a nullptr, prepare for it!
    virtual void *getUserData(int lineIndex) { return nullptr; }

    virtual eventnumber_t getEventNumberAtLine(int lineIndex) { return -1; };
    virtual int getLineAtEvent(eventnumber_t eventNumber) { return -1; };

signals:
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

    QStringList lines;  // text, split to lines

public:
    StringTextViewerContentProvider(QString text);

    int getLineCount() override;
    QString getLineText(int lineIndex) override;
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
    virtual QString getLineText(LogBuffer::Entry *entry, int lineIndex) = 0;

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
    QString getLineText(LogBuffer::Entry *entry, int lineIndex) override;
};

class QTENV_API EventEntryMessageLinesProvider : public AbstractEventEntryLinesProvider {
protected:

    static cMessagePrinter *chooseMessagePrinter(cMessage *msg);

    const cMessagePrinter::Options *messagePrinterOptions;
    static SimTime referenceTime;

    bool isMatchingMessageSend(const LogBuffer::MessageSend& msgSend);
    std::vector<int> findRelevantHopModuleIds(const LogBuffer::MessageSend& msgSend, bool *lastHopIncluded = nullptr);
    LogBuffer::MessageSend& messageSendForLineIndex(LogBuffer::Entry *entry, int lineIndex);

    QString getRelevantHopsString(const LogBuffer::MessageSend &msg);

public:

    EventEntryMessageLinesProvider(int inspectedComponentId, const std::set<int>& excludedComponents, ComponentHistory *componentHistory, const cMessagePrinter::Options *messagePrinterOptions)
        : AbstractEventEntryLinesProvider(inspectedComponentId, excludedComponents, componentHistory), messagePrinterOptions(messagePrinterOptions) { }

    int getNumLines(LogBuffer::Entry *entry) override;
    QString getLineText(LogBuffer::Entry *entry, int lineIndex) override;

    cMessage *getMessageForLine(LogBuffer::Entry *entry, int lineIndex) override;

    static void setReferenceTime(SimTime rt);
    static SimTime getReferenceTime() { return referenceTime; }
};

class QTENV_API ModuleOutputContentProvider: public TextViewerContentProvider {
    Q_OBJECT

    LogBuffer *logBuffer;
    LogInspector::Mode mode;

    ComponentHistory *componentHistory;
    cComponent *inspectedComponent;
    AbstractEventEntryLinesProvider *linesProvider;

    std::set<int> excludedModuleIds;
    const cMessagePrinter::Options *messagePrinterOptions;

    QStringList gatherEnabledColumnNames();

    // cached data
    int lineCount = 1; // the empty line, the "earlier history discarded" is added over this
    std::vector<int> entryStartLineNumbers; // indexed by the entry's index in logBuffer
    std::map<int, QString> lineCache;

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
    QString getLineText(int lineIndex) override;
    bool showHeaders() override;
    QStringList getHeaders() override;
    void *getUserData(int lineIndex) override;

    eventnumber_t getEventNumberAtLine(int lineIndex) override;
    int getLineAtEvent(eventnumber_t eventNumber) override;

protected:
    int getIndexOfEntryAt(int lineIndex);

    void invalidateIndex();
    bool isIndexValid();
    void rebuildIndex();

protected slots:
    void onContentAdded();
    void onEntryDiscarded(LogBuffer::Entry *discardedEntry);
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_TEXTVIEWERPROVIDERS_H
