//==========================================================================
//  PREFERENCESDIALOG.H - part of
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

#ifndef __OMNETPP_QTENV_PREFERENCESDIALOG_H
#define __OMNETPP_QTENV_PREFERENCESDIALOG_H

#include <QtWidgets/QDialog>
#include "qtenvdefs.h"
#include "inspectorutiltypes.h"

namespace Ui {
class PreferencesDialog;
}

namespace omnetpp {
namespace qtenv {

class QTENV_API PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(eTab defaultPage = TAB_NOTDEFINED, QWidget *parent = nullptr);
    ~PreferencesDialog();

private Q_SLOTS:
    void restoreDefaultFonts();

public Q_SLOTS:
    void accept();

private:
    Ui::PreferencesDialog *ui;

    void init();
    void setFontsTabFonts(const QFont &interfaceFont, const QFont &timelineFont,
                          const QFont &canvasFont, const QFont &logBoxFont, const QFont &timeFont);
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_PREFERENCESDIALOG_H
