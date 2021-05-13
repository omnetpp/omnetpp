//==========================================================================
//  AREASELECTORDIALOG.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2018 Andras Varga
  Copyright (C) 2006-2018 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_QTENV_AREASELECTORDIALOG_H
#define __OMNETPP_QTENV_AREASELECTORDIALOG_H

#include <QtWidgets/QDialog>
#include "qtenvdefs.h"

namespace Ui {
class AreaSelectorDialog;
}

namespace omnetpp {
namespace qtenv {

class ModuleCanvasViewer;

class QTENV_API AreaSelectorDialog : public QDialog
{
    Q_OBJECT

    const QString PREF_KEY_AREA = "area-selector-area";
    const QString PREF_KEY_MARGIN = "area-selector-margin";

    const QString PREF_VALUE_AREA_BOUNDING_BOX = "bounding-box";
    const QString PREF_VALUE_AREA_MODULE_RECTANGLE = "module-rectangle";
    const QString PREF_VALUE_AREA_VIEWPORT = "viewport";

public:
    enum Area {
        BOUNDING_BOX,
        MODULE_RECTANGLE,
        VIEWPORT
    };

    explicit AreaSelectorDialog(QWidget *parent = nullptr);
    ~AreaSelectorDialog();

    Area getArea();
    int getMargin();

    void accept() override;

private:
    Ui::AreaSelectorDialog *ui;
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_AREASELECTORDIALOG_H
