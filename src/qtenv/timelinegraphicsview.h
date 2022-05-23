//==========================================================================
//  TIMELINEGRAPHICSVIEW.H - part of
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

#ifndef __OMNETPP_QTENV_TIMELINEGRAPHICSVIEW_H
#define __OMNETPP_QTENV_TIMELINEGRAPHICSVIEW_H

#include <QtWidgets/QGraphicsView>
#include "omnetpp/simtime_t.h"
#include "qtenvdefs.h"

namespace omnetpp {

class cMessage;
class cModule;
class cObject;

namespace qtenv {

class QTENV_API TimeLineGraphicsView : public QGraphicsView
{
    Q_OBJECT
private:
    bool drawMinorTicks = true;
    bool adaptiveMinExponent = true;
    bool adaptiveMaxExponent = true;
    int minExponent = -1;
    int maxExponent = 0;
    bool enableTickLabels = true;
    bool enableMessageLabels = true;
    QFont tickLabelFont;  // null for default font
    QFont messageLabelFont;  // null for default font
    int defaultNumMessageLabelRows = 2;
    //TODO Selected modules in modulinspector
    QVector<cModule*> modules;

    //ISelection selection = new StructuredSelection();

    // drawing parameters (recomputed in each repaint call)
    int zeroStartX;
    int zeroEndX;
    int arrowStartX;
    int arrowEndX;
    int minExponentX;
    int maxExponentX;
    int axisY;
    QVector<int> messageXCoords;

    QVector<cMessage *> getMessages();
    bool matches(cMessage *object);
    void drawMessageSymbol(cMessage *message, bool active, int x, int y);
    QVector<cObject*> getObjectsUnderCursor(QPointF pos);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

    void updateMinExponent(simtime_t now, QVector<cMessage*> messages);
    void updateMaxExponent(simtime_t now, QVector<cMessage*> messages);
    int getXForExponent(double exp);

     //Format the given number (time in seconds) in an easy-to-digest form, such as
     //"1ms", "10ms", "100ms", "1s", "10s", "100s", "1000s", "1E4s", etc.
    QString getTickLabel(int mantissa, int exp);
    int getXForTimeDelta(simtime_t delta);
    QVector<int> computeMessageCoordinates(simtime_t now, QVector<cMessage*> messages);

Q_SIGNALS:
    void contextMenuRequested(QVector<cObject*> objects, QPoint globalPos);
    void click(cObject *object);
    void doubleClick(cObject *object);

public:
    TimeLineGraphicsView();

    int getMinExponent();
    void setMinExponent(int minExponent);
    int getMaxExponent();
    void setMaxExponent(int maxExponent);
    void setAdaptiveExponents(bool value);
    bool isAdaptiveMinExponent();
    void setAdaptiveMinExponent(bool adaptiveMinExponent);
    bool isAdaptiveMaxExponent();
    void setAdaptiveMaxExponent(bool adaptiveMaxExponent);
    bool getDrawMinorTicks();
    void setDrawMinorTicks(bool drawMinorTicks);
    bool getShowTickLabels();
    void setShowTickLabels(bool enableTickLabels);
    bool getShowMessageLabels();
    void setShowMessageLabels(bool enableMessageLabels);
    QFont getTickLabelFont();
    void setTickLabelFont(QFont tickLabelFont);
    QFont getMessageLabelFont();
    void setMessageLabelFont(QFont messageLabelFont);
    int getDefaultNumMessageLabelRows();
    QVector<cModule*> getModulePathsMessageFilter();
    void setModulePathsMessageFilter(QVector<cModule*> modules);
    double getInitHeight();

     //This only affects the behavior of computeSize().
    void setDefaultNumMessageLabelRows(int defaultNumMessageLabelRows);
    void paintMessages(QVector<cMessage*> messages, bool wantHighlighted, int messageLabelHeight, int numRows);

    void rebuildScene();
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_TIMELINEGRAPHICSVIEW_H
