//==========================================================================
//  TEXTVIEWERPROVIDERS.H - part of
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

#ifndef __OMNETPP_QTENV_TEXTVIEWERPROVIDERS_H
#define __OMNETPP_QTENV_TEXTVIEWERPROVIDERS_H

#include <QObject>
#include <QColor>
#include <QDebug>

// cRuntimeException is needed somewhere deep in an ASSERT in
// that logbuffer.h include (for the circularbuffer)
#include "omnetpp/cexception.h"

#include "omnetpp/cmessage.h"

#include "logbuffer.h"
#include "qtenv.h"
#include "textviewerwidget.h"
#include "loginspector.h"

// the logbuffer.h file included in the header indirectly includes
// simkerneldefs.h which undefs emit, but we need it here, for Qt reasons
#define emit

namespace omnetpp {
namespace qtenv {


class TextViewerContentProvider : public QObject {
    Q_OBJECT

public:
    using TabStop = TextViewerWidget::TabStop;

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

    virtual QList<TabStop> getTabStops(int lineIndex) = 0;

    virtual bool showHeaders() = 0;
    virtual QStringList getHeaders() = 0;

signals:
    void textChanged();
};




/**
 * An implementation of ITextViewerContent that uses a string as input.
 *
 * @author Andras
 */
class StringTextViewerContentProvider: public TextViewerContentProvider {
    Q_OBJECT

    QStringList lines;  // text, split to lines

public:
    StringTextViewerContentProvider(QString text);

    int getLineCount() override;
    QString getLineText(int lineIndex) override;
    QList<TabStop> getTabStops(int lineIndex) override;
    bool showHeaders() override;
    QStringList getHeaders() override;
};


/**
 *
 * @author Andras
 */
class AbstractEventEntryLinesProvider {
public:
    using TabStop = TextViewerWidget::TabStop;
    virtual int getNumLines(LogBuffer::Entry *entry) = 0;
    virtual QString getLineText(LogBuffer::Entry *entry, int lineIndex) = 0;
    virtual QList<TabStop> getTabStops(LogBuffer::Entry *entry, int lineIndex) = 0;
};



class EventEntryLinesProvider : public AbstractEventEntryLinesProvider {
    ComponentHistory *componentHistory;
public:
    EventEntryLinesProvider(ComponentHistory *componentHistory);
    int getNumLines(LogBuffer::Entry *entry) override;
    QString getLineText(LogBuffer::Entry *entry, int lineIndex) override;
    virtual QList<TabStop> getTabStops(LogBuffer::Entry *entry, int lineIndex) override;
};

class EventEntryMessageLinesProvider : public AbstractEventEntryLinesProvider {
protected:
    cComponent *inspectedObject;
    ComponentHistory *componentHistory;

    static cMessagePrinter *chooseMessagePrinter(cMessage *msg);

    int findFirstRelevantHop(const LogBuffer::MessageSend& msgsend, int fromHop);

    QString getMessageSrcDest(const LogBuffer::MessageSend &msg);
public:
    EventEntryMessageLinesProvider(cComponent *inspectedObject, ComponentHistory *componentHistory);

    int getNumLines(LogBuffer::Entry *entry) override;
    QString getLineText(LogBuffer::Entry *entry, int lineIndex) override;
    virtual QList<TabStop> getTabStops(LogBuffer::Entry *entry, int lineIndex) override;
};


class AbstractEventEntryFilter {
public:
    virtual bool matches(LogBuffer::Entry *entry) = 0;
};

class ModulePathsEventEntryFilter : public AbstractEventEntryFilter {
    QStringList moduleFullPaths;
    ComponentHistory *componentHistory;

public:
    ModulePathsEventEntryFilter(const QStringList &moduleFullPaths, ComponentHistory *componentHistory);

    bool matches(LogBuffer::Entry *entry) override;
};

class ModuleOutputContentProvider: public TextViewerContentProvider {
    Q_OBJECT

    LogBuffer *logBuffer;
    LogInspector::Mode mode;

    ComponentHistory *componentHistory;
    cComponent *inspectedComponent;
    AbstractEventEntryFilter *filter = nullptr; // currently not used
    AbstractEventEntryLinesProvider *linesProvider;

    std::set<int> excludedModuleIds;

    bool isMatchingComponent(int componentId);
    bool isAncestorModule(int componentId, int potentialAncestorModuleId);

    // cached data
    int lineCount = 1;
    std::vector<int> entryStartLineNumbers; // indexed by the entry's index in logBuffer

public:
    ModuleOutputContentProvider(Qtenv *qtenv, cComponent *inspectedComponent, LogInspector::Mode mode);

    LogBuffer *getLogBuffer();
    AbstractEventEntryLinesProvider *getLinesProvider();

    void setFilter(AbstractEventEntryFilter *filter);
    AbstractEventEntryFilter *getFilter();

    void setExcludedModuleIds(std::set<int> excludedModuleIds);

    void refresh();

    int getLineCount() override;
    QString getLineText(int lineIndex) override;
    QList<TabStop> getTabStops(int lineIndex) override;
    bool showHeaders() override;
    QStringList getHeaders() override;

protected:
    int getIndexOfEntryAt(int lineIndex);

    void invalidateIndex();
    bool isIndexValid();
    void rebuildIndex();

protected slots:
    void onLogEntryAdded();
    void onLogLineAdded();
    void onMessageSendAdded();
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_TEXTVIEWERPROVIDERS_H
