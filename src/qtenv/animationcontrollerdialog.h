//==========================================================================
//  animationcontrollerdialog.h - part of
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

#ifndef __OMNETPP_QTENV_ANIMATIONCONTROLLERDIALOG_H
#define __OMNETPP_QTENV_ANIMATIONCONTROLLERDIALOG_H

#include <QtWidgets/QDialog>
#include "qtenvdefs.h"

namespace Ui {
class AnimationControllerDialog;
}

namespace omnetpp {
namespace qtenv {

class DisplayUpdateController;
struct RunModeProfile;

class QTENV_API AnimationControllerDialog : public QDialog
{
    Q_OBJECT

    Ui::AnimationControllerDialog *ui;
    DisplayUpdateController *duc;

    RunModeProfile *getSelectedProfile(); // based on the combobox
    double getMinAnimSpeed(); // from the spinbox, NAN if "none"
    double getMaxAnimSpeed(); // from the spinbox, NAN if "none"

public:
    explicit AnimationControllerDialog(QWidget *parent = nullptr);
    ~AnimationControllerDialog();

public Q_SLOTS:
    void switchToRunMode(RunMode mode);
    void displayMetrics();
    void displayControlValues();
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_ANIMATIONCONTROLLERDIALOG_H
