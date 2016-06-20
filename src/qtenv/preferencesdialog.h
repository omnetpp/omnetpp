//==========================================================================
//  PREFERENCESDIALOG.H - part of
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

#ifndef __OMNETPP_QTENV_PREFERENCESDIALOG_H
#define __OMNETPP_QTENV_PREFERENCESDIALOG_H

#include <QDialog>

namespace Ui {
class PreferencesDialog;
}

namespace omnetpp {
namespace qtenv {

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(int defaultPage = -1, QWidget *parent = 0);
    ~PreferencesDialog();

private slots:
    void restoreDefaultFonts();

protected slots:
    // the slider was dragged by the user, setting the speed appropriately
    void onAnimationSliderMoved(int value);
    // the speed changed from some other source, making the slider follow it
    void onAnimationSpeedChanged(float speed);

public slots:
    void accept();

private:
    Ui::PreferencesDialog *ui;

    void init();
    void setFontsTabFonts(const QFont &interfaceFont, const QFont &timelineFont,
                          const QFont &canvasFont, const QFont &logBoxFont, const QFont &timeFont);
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_PREFERENCESDIALOG_H
