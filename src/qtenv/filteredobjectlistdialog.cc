//==========================================================================
//  FILTEREDOBJECTLISTDIALOG.CC - part of
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

#include "filteredobjectlistdialog.h"
#include "ui_filteredobjectlistdialog.h"
#include "omnetpp/cobject.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/globals.h"
#include "envir/visitor.h"
#include "common/matchexpression.h"
#include "qtenv.h"
#include "inspector.h"
#include "inspectorlistbox.h"
#include "inspectorlistboxview.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QKeyEvent>

#include <QDebug>

namespace omnetpp {
namespace qtenv {

QString FilteredObjectListDialog::classNamePattern =
"Generic filter expression which matches class name by default.\n\
\n\
        Wildcards (\"?\", \"*\"), AND, OR, NOT and field matchers are accepted;\n\
        see Object Filter help for a more complete list.\n\
\n\
        Examples:\n\
          cQueue\n\
                    matches cQueue objects\n\
          TCP* or (IP* and not IPDatagram)\n\
                    matches objects whose class name begins with TCP or IP,\n\
                    excluding IPDatagrams\n\
          cMessage and kind(3)\n\
                    matches objects of class cMessage and message kind 3.";

QString FilteredObjectListDialog::namePattern =
"Generic filter expression which matches the object full path by default.\n\
\n\
Wildcards (\"?\", \"*\") are allowed. \"{a-exz}\" matches any character in the\n\
range \"a\"..\"e\", plus \"x\" and \"z\". You can match numbers: \"*.job{128..191}\"\n\
will match objects named \"job128\", \"job129\", ..., \"job191\". \"job{128..}\"\n\
and \"job{..191}\" are also understood. You can combine patterns with AND, OR\n\
and NOT and parentheses (lowercase and, or, not are also OK). You can match\n\
against other object fields such as queue length, message kind, etc., with\n\
the syntax \"fieldname(pattern)\". Put quotation marks around a pattern if it\n\
contains parentheses.\n\
\n\
HINT: You'll want to start the pattern with \"*.\" in most cases, to match\n\
objects anywhere in the network!\n\
\n\
Examples:\n\
 *.destAddr\n\
            matches all objects whose name is \"destAddr\" (likely module\n\
            parameters)\n\
 *.subnet2.*.destAddr\n\
            matches objects named \"destAddr\" inside \"subnet2\"\n\
 *.node[8..10].*\n\
            matches anything inside module node[8], node[9] and node[10]\n\
 className(cQueue) and not length(0)\n\
            matches non-empty queue objects\n\
 className(cQueue) and length({10..})\n\
            matches queue objects with length>=10\n\
 kind(3) or kind({7..9})\n\
            matches messages with message kind equal to 3, 7, 8 or 9\n\
            (Only messages have a \"kind\" attribute.)\n\
 className(IP*) and *.data-*\n\
            matches objects whose class name begins with \"IP\" and\n\
            name begins with \"data-\"\n\
 not className(cMessage) and byteLength({1500..})\n\
            matches messages whose class is not cMessage, and byteLength is\n\
            at least 1500. (Only messages have a \"byteLength\" attribute.)\n\
 \"*(*\" or \"*.msg(ACK)\"\n\
            quotation marks needed when pattern is a reserved word or contains\n\
            parentheses. (Note: *.msg(ACK) without parens would be interpreted\n\
            as some object having a \"*.msg\" attribute with the value \"ACK\"!)";

FilteredObjectListDialog::FilteredObjectListDialog(cObject *ptr, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilteredObjectListDialog)
{
    ui->setupUi(this);

    if(ptr == nullptr)
        ptr = getSimulation();

    setWindowFlags(Qt::WindowStaysOnTopHint);
    ui->comboBox->setToolTip(classNamePattern);
    ui->comboLabel->setToolTip(classNamePattern);
    ui->fullPathEdit->setToolTip(namePattern);
    ui->fullPathLabel->setToolTip(namePattern);

    // vars
    QVariant variant = getQtenv()->getPref("filtobjlist-class");
    QString classText = variant.isValid() ? variant.value<QString>() : "";
    variant = getQtenv()->getPref("filtobjlist-name");
    QString name = variant.isValid() ? variant.value<QString>() : "";
    variant = getQtenv()->getPref("filtobjlist-width");
    if(variant.isValid())
    {
        int width = variant.value<int>();
        variant = getQtenv()->getPref("filtobjlist-height");
        if(variant.isValid())
        {
            int height = variant.value<int>();
            resize(width, height);
        }
    }

    // panel for filters
    ui->searchEdit->setText(ptr->getFullPath().c_str());

    QStringList classNames = getClassNames();
    classNames.push_front("");
    ui->comboBox->addItems(classNames);
    int index = ui->comboBox->findText(classText);
    ui->comboBox->setCurrentIndex(index);
    ui->fullPathEdit->setText(name);

    connect(ui->refresh, SIGNAL(clicked()), this, SLOT(refresh()));

    // category filters
    variant = getQtenv()->getPref("cat-m");
    ui->modulesCheckBox->setChecked(variant.isValid() ? variant.value<bool>() : false);
    variant = getQtenv()->getPref("cat-p");
    ui->paramCheckBox->setChecked(variant.isValid() ? variant.value<bool>() : false);
    variant = getQtenv()->getPref("cat-q");
    ui->queuesCheckBox->setChecked(variant.isValid() ? variant.value<bool>() : false);
    variant = getQtenv()->getPref("cat-s");
    ui->outVectorsCheckBox->setChecked(variant.isValid() ? variant.value<bool>() : false);
    variant = getQtenv()->getPref("cat-g");
    ui->messagesCheckBox->setChecked(variant.isValid() ? variant.value<bool>() : false);
    variant = getQtenv()->getPref("cat-c");
    ui->gatesCheckBox->setChecked(variant.isValid() ? variant.value<bool>() : false);
    variant = getQtenv()->getPref("cat-v");
    ui->fsmCheckBox->setChecked(variant.isValid() ? variant.value<bool>() : false);
    variant = getQtenv()->getPref("cat-o");
    ui->otherCheckBox->setChecked(variant.isValid() ? variant.value<bool>() : false);

    // Listbox
    InspectorListBox *inspectorListBox = new InspectorListBox();
    inspectorListBoxView = new InspectorListBoxView();
    inspectorListBoxView->setModel(inspectorListBox);

    QVBoxLayout *layout = new QVBoxLayout();
    ui->inspectorListView->setLayout(layout);
    layout->addWidget(inspectorListBoxView);

    connect(ui->fullPathEdit, SIGNAL(returnPressed()), this, SLOT(refresh()));
    connect(ui->comboBox->lineEdit(), SIGNAL(returnPressed()), this, SLOT(refresh()));
    connect(inspectorListBoxView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(inspect(QModelIndex)));
    connect(inspectorListBoxView, SIGNAL(pressEnter(QModelIndex)), this, SLOT(inspect(QModelIndex)));
}

void FilteredObjectListDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        return;

    MainWindow *mainWindow = getQtenv()->getMainWindow();

    if(event->modifiers() == Qt::ControlModifier)
    {
        if(event->key() == Qt::Key_F5)
            mainWindow->on_actionRunUntil_triggered();
        else if(event->key() == Qt::Key_F9)
            mainWindow->on_actionDebugNextEvent_triggered();
        else if(event->key() == Qt::Key_G)
            mainWindow->on_actionEventlogRecording_triggered();
        else if(event->key() == Qt::Key_D)
            mainWindow->on_actionStatusDetails_triggered();
        else if(event->key() == Qt::Key_T)
            mainWindow->on_actionTimeline_toggled();
        else if(event->key() == Qt::Key_Q)
            mainWindow->on_actionQuit_triggered();
    }
    else
    {
        if(event->key() == Qt::Key_F4)
            mainWindow->on_actionOneStep_triggered();
        else if(event->key() == Qt::Key_F5)
            mainWindow->on_actionRun_triggered();
        else if(event->key() == Qt::Key_F6)
            mainWindow->on_actionFastRun_triggered();
        else if(event->key() == Qt::Key_F7)
            mainWindow->on_actionExpressRun_triggered();
        else if(event->key() == Qt::Key_F8)
            mainWindow->on_actionStop_triggered();
    }
    QDialog::keyPressEvent(event);
}

void FilteredObjectListDialog::inspect(QModelIndex index)
{
    if(!index.isValid())
        return;

    QVariant variant = getQtenv()->getPref("outofdate");
    bool outOfDate = variant.isValid() ? variant.value<bool>() : true;
    bool isRunning = getQtenv()->getMainWindow()->isRunning();

    if(outOfDate || isRunning)
    {
        QString advice;
        if(isRunning)
            advice = "please stop the simulation and click Refresh first";
        else
            advice = "please click Refresh first";

        QMessageBox::warning(this, "Filtered object list",
                "Dialog contents might be out of date -- " + advice + ".",
                QMessageBox::StandardButton::Ok);
        return;
    }

    cObject* object = static_cast<cObject*>(index.internalPointer());
    getQtenv()->inspect(object, INSP_DEFAULT, true, "");
}

void FilteredObjectListDialog::setSearchEdit(cObject *obj)
{
    ui->searchEdit->setText(obj->getFullPath().c_str());
}

QStringList FilteredObjectListDialog::getClassNames()
{
    cRegistrationList *rList = classes.getInstance();
    envir::cCollectChildrenVisitor visitor(rList);
    visitor.process(rList);
    int objsSize = visitor.getArraySize();
    cObject **objs = visitor.getArray();

    // get the names
    QStringList classNames;
    for(int i = 0; i < objsSize; ++i)
        classNames.push_back(objs[i]->getFullName());

    classNames.sort();
    return classNames;
}

void FilteredObjectListDialog::done(int r)
{
    getQtenv()->setPref("filtobjlist-class", QVariant::fromValue(ui->comboBox->currentText()));
    getQtenv()->setPref("filtobjlist-name", QVariant::fromValue(ui->fullPathEdit->text()));

    // filtobjlist-category
    getQtenv()->setPref("cat-m", ui->modulesCheckBox->isChecked());
    getQtenv()->setPref("cat-p", ui->paramCheckBox->isChecked());
    getQtenv()->setPref("cat-q", ui->queuesCheckBox->isChecked());
    getQtenv()->setPref("cat-s", ui->outVectorsCheckBox->isChecked());
    getQtenv()->setPref("cat-g", ui->messagesCheckBox->isChecked());
    getQtenv()->setPref("cat-c", ui->gatesCheckBox->isChecked());
    getQtenv()->setPref("cat-v", ui->fsmCheckBox->isChecked());
    getQtenv()->setPref("cat-o", ui->otherCheckBox->isChecked());

    QDialog::done(r);
}

void FilteredObjectListDialog::refresh()
{
    // resolve root object
    const char *fullPath = ui->searchEdit->text().toStdString().c_str();

    cFindByPathVisitor visitor(fullPath, nullptr, -1);
    visitor.process(getSimulation());
    if(visitor.getArraySize() == 0)
    {
        QMessageBox::warning(this, "Error",
                QString("Object to search in (\"") + fullPath + "\") could not be resolved.",
                QMessageBox::StandardButton::Ok);
        return;
    }

    cObject *rootObject = visitor.getArray()[0];

    QString className = ui->comboBox->currentText();
    if(className.isEmpty())
        className = "*";

    try
    {
        checkPattern(className.toStdString().c_str());
    }
    catch(...)
    {
        QMessageBox::warning(this, "Qtenv",
                QString("Class filter pattern \"") + className + "\" has invalid syntax -- using \"*\" instead.",
                QMessageBox::StandardButton::Ok);
        className = "*";
    }

    QString name = ui->fullPathEdit->text();
    if(name.isEmpty())
        name = "*";

    try
    {
        checkPattern(name.toStdString().c_str());
    }
    catch(...)
    {
        QMessageBox::warning(this, "Qtenv",
                QString("Name filter pattern \"") + name + "\" has invalid syntax -- using \"*\" instead.",
                QMessageBox::StandardButton::Ok);
        className = "*";
    }

    // get list
    QVariant variant = getQtenv()->getPref("filtobjlist-maxcount");
    int maxCount = variant.isValid() ? variant.value<int>() : 1000;
    int num = 0;
    cObject **objList = getSubObjectsFilt(rootObject, className.toStdString().c_str(), name.toStdString().c_str(), maxCount, num);

    // ask user if too many...
    QMessageBox::StandardButton viewAll = QMessageBox::StandardButton::Ok;
    if(num == maxCount)
    {
        viewAll = QMessageBox::warning(this, "Too many objects",
                QString("Your query matched at least ") + num + " objects, click OK to display them.",
                QMessageBox::StandardButton::Ok | QMessageBox::StandardButton::Cancel);
    }

    // clear listbox
    inspectorListBoxView->model()->removeRow(0);

    // insert into listbox
    if(viewAll == QMessageBox::StandardButton::Ok)
    {
        if(num == maxCount)
            ui->label->setText("The first " + QString::number(num) + " objects found:");
        else
            ui->label->setText("Found " + QString::number(num) + " objects:");

        QVector<cObject*> objects;
        for(int i = 0; i < num; ++i)
            objects.push_back(objList[i]);

        static_cast<InspectorListBox*>(inspectorListBoxView->model())->setObjects(objects);
    }

    getQtenv()->setPref("outofdate", false);
}

cObject **FilteredObjectListDialog::getSubObjectsFilt(cObject *object, const char *classNamePattern, const char *objFullPathPattern,
                                                 int maxCount, int &num)
{
    // args: <ptr> <class> <fullpath> <maxcount>, where
    //    <class> and <fullpath> may contain wildcards

    unsigned int category = 0;

    if(ui->modulesCheckBox->isChecked())
        category |= CATEGORY_MODULES;
    if(ui->paramCheckBox->isChecked())
        category |= CATEGORY_MODPARAMS;
    if(ui->queuesCheckBox->isChecked())
        category |= CATEGORY_QUEUES;
    if(ui->outVectorsCheckBox->isChecked())
        category |= CATEGORY_STATISTICS;
    if(ui->messagesCheckBox->isChecked())
        category |= CATEGORY_MESSAGES;
    if(ui->gatesCheckBox->isChecked())
        category |= CATEGORY_CHANSGATES;
    if(ui->fsmCheckBox->isChecked())
        category |= CATEGORY_VARIABLES;
    if(ui->otherCheckBox->isChecked())
        category |= CATEGORY_OTHERS;

    // get filtered list
    cFilteredCollectObjectsVisitor visitor;
    visitor.setSizeLimit(maxCount);
    visitor.setFilterPars(category, classNamePattern, objFullPathPattern);
    visitor.process(object);
    num = visitor.getArraySize();

    cObject **array = visitor.getArray();
    cObject **objs = new cObject *[num];
    for(int i = 0; i < num; ++i)
        objs[i] = array[i];

    return objs;
}

void FilteredObjectListDialog::checkPattern(const char *pattern)
{
    // try parse className
    OPP::common::MatchExpression matcher(pattern, false, true, true);
    // let's see if MatchableObjectAdapter can parse field names inside
    // (unmatched "[", etc.), by trying to match some random object
    MatchableObjectAdapter objectAdapter(MatchableObjectAdapter::FULLNAME, getSimulation());
    matcher.matches(&objectAdapter);
}

FilteredObjectListDialog::~FilteredObjectListDialog()
{
    delete ui;
    getQtenv()->setPref("filtobjlist-width", width());
    getQtenv()->setPref("filtobjlist-height", height());
}

} // namespace qtenv
} // namespace omnetpp
