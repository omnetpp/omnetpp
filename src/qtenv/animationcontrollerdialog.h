//==========================================================================
//  animationcontrollerdialog.h - part of
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

#ifndef __OMNETPP_QTENV_ANIMATIONCONTROLLERDIALOG_H
#define __OMNETPP_QTENV_ANIMATIONCONTROLLERDIALOG_H

#include <QDialog>
#include "qtenvdefs.h"

namespace Ui {
class AnimationControllerDialog;
}

namespace omnetpp {
namespace qtenv {

class DisplayUpdateController;

class QTENV_API AnimationControllerDialog : public QDialog
{
    Q_OBJECT

    Ui::AnimationControllerDialog *ui;
    DisplayUpdateController *duc;

public:
    explicit AnimationControllerDialog(QWidget *parent = 0);
    ~AnimationControllerDialog();

public slots:
    void displayMetrics();
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_ANIMATIONCONTROLLERDIALOG_H
