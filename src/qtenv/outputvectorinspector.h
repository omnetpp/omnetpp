//==========================================================================
//  OUTPUTVECTORINSPECTOR.H - part of
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

#ifndef __OMNETPP_QTENV_OUTPUTVECTORINSPECTOR_H
#define __OMNETPP_QTENV_OUTPUTVECTORINSPECTOR_H

#include "envir/envirbase.h"
#include "inspector.h"

class QStatusBar;

namespace omnetpp {
namespace qtenv {

class OutputVectorView;
class OutputVectorInspectorConfigDialog;

// TODO: use existing circular_buffer<T> instead
class QTENV_API CircBuffer
{
  public:
    struct CBEntry {
        simtime_t t;
        double value;
    };

  protected:
    CBEntry *buf;
    int siz, n, head;

  public:
    CircBuffer(int size);
    ~CircBuffer();
    void add(simtime_t t, double value);
    int headPos() const { return head; }
    int tailPos() const { return (head-n+1+siz)%siz; }
    int items() const { return n; }
    int size() const { return siz; }
    CBEntry& entry(int k) const { return buf[k]; }
};

class QTENV_API OutputVectorInspector : public Inspector
{
    Q_OBJECT

  public:
    CircBuffer circbuf;

  protected:
    OutputVectorView *view;
    QStatusBar *statusBar;
    OutputVectorInspectorConfigDialog *configDialog;

    // configuration
    bool isMinYAutoscaled = true;
    bool isMaxYAutoscaled = true;
    bool isTimeScaleAutoscaled = true;
    double minY, maxY, timeScale; // will be computed upon refresh

    void resizeEvent(QResizeEvent *event) override;

    // return textual information in general or about a value/value pair
    QString generalInfo();
    void yRangeCorrection();
    void setConfig();

  protected slots:
    void onOptionsDialogTriggerd();
    void onCustomContextMenuRequested(QPoint pos);
    void onClickApply();

  public:
    OutputVectorInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f);
    ~OutputVectorInspector();
    virtual void doSetObject(cObject *obj) override;
    virtual void refresh() override;
};

}  // namespace qtenv
}  // namespace omnetpp

#endif

