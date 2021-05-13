//==========================================================================
//  LOGFINDDIALOG.H - part of
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

#ifndef __OMNETPP_QTENV_LOGFINDDIALOG_H
#define __OMNETPP_QTENV_LOGFINDDIALOG_H

#include <QtWidgets/QDialog>
#include "textviewerwidget.h"
#include "qtenvdefs.h"

namespace Ui {
class LogFindDialog;
}

namespace omnetpp {

class cObject;

namespace qtenv {

class QTENV_API LogFindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogFindDialog(QWidget *parent = 0, QString lastText = "", TextViewerWidget::FindOptions options = 0);
    ~LogFindDialog();

    QString getText();
    TextViewerWidget::FindOptions getOptions();

private:
    Ui::LogFindDialog *ui;
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_LOGFINDDIALOG_H
