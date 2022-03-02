//==========================================================================
//  FINDOBJECTSDIALOG.CC - part of
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

#include "findobjectsdialog.h"
#include "ui_findobjectsdialog.h"
#include "omnetpp/cobject.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/globals.h"
#include "envir/visitor.h"
#include "common/matchexpression.h"
#include "qtenv.h"
#include "inspector.h"
#include "objectlistmodel.h"
#include "objectlistview.h"
#include "mainwindow.h"
#include <QtWidgets/QMessageBox>
#include <QtGui/QKeyEvent>

#include <QtCore/QDebug>

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {
namespace qtenv {

QString FindObjectsDialog::classNamePatternHelp =
R"HELP(Generic filter expression which matches class name by default.

Wildcards ("?", "*"), AND, OR, NOT and field matchers are accepted;
see Object Filter help for a more complete list.

NOTE: Include the namespace in class names, or prefix classnames with "*::".

Examples:
    omnetpp::cQueue
        matches cQueue objects
    TCP* OR (IP* AND NOT IPDatagram)
        matches objects whose class name begins with TCP or IP,
        excluding IPDatagrams
    omnetpp::cMessage AND kind =~ 3
        matches objects of class cMessage and message kind 3)HELP";

QString FindObjectsDialog::namePatternHelp =
R"HELP(Generic filter expression which matches the object full path by default.

Wildcards ("?", "*") are allowed. "{a-exz}" matches any character in the
range "a".."e", plus "x" and "z". You can match numbers: "*.job{128..191}"
will match objects named "job128", "job129", ..., "job191". "job{128..}"
and "job{..191}" are also understood. You can combine patterns with AND, OR
and NOT and parentheses (lowercase and, or, not are also OK). You can match
against other object fields such as queue length, message kind, etc., with
the syntax "fieldname =~ pattern". Put quotation marks around a pattern if it
contains special characters.

HINT: You'll want to start the pattern with "*." in most cases, to match
objects anywhere in the network!

Examples:
    *.destAddr
        matches all objects whose name is "destAddr" (likely module
        parameters)
    *.subnet2.*.destAddr
        matches objects named "destAddr" inside "subnet2"
    *.node[8..10].*
        matches anything inside module node[8], node[9] and node[10]
    className =~ omnetpp::cQueue AND NOT length =~ 0
        matches non-empty queue objects
    className =~ omnetpp::cQueue AND length =~ {10..}
        matches queue objects with length>=10
    kind =~ 3 OR kind =~ {7..9}
        matches messages with message kind equal to 3, 7, 8 or 9
        (Objects with no "kind" field are not matched.)
    className =~ IP* AND *.data-*
        matches objects whose class name begins with "IP" and
        name begins with "data-"
    NOT className =~ omnetpp::cMessage AND byteLength =~ {1500..}
        matches messages whose class is not cMessage, and byteLength is
        at least 1500.
    "*(*" OR "*.msg(ACK)"
        quotation marks needed when pattern is a reserved word or contains
        special characters.)HELP";

FindObjectsDialog::FindObjectsDialog(cObject *ptr, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindObjectsDialog)
{
    ui->setupUi(this);
    setFont(getQtenv()->getBoldFont());

    if (ptr == nullptr) {
        cFindByPathVisitor visitor(getPref("rootobject").toByteArray());
        visitor.process(getSimulation());

        ptr = (visitor.getArraySize() > 0)
                ? visitor.getArray()[0]
                : getSimulation();
    }

    ui->comboBox->setToolTip(classNamePatternHelp);
    ui->comboLabel->setToolTip(classNamePatternHelp);
    ui->fullPathEdit->setToolTip(namePatternHelp);
    ui->fullPathLabel->setToolTip(namePatternHelp);

    // vars
    QString classText = getPref("class", "").toString();
    QString name = getPref("name", "").toString();
    QVariant variant = getPref("width");
    if (variant.isValid()) {
        int width = variant.toInt();
        variant = getPref("height");
        if (variant.isValid()) {
            int height = variant.toInt();
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
    variant = getPref("cat-m", true);
    ui->modulesCheckBox->setChecked(variant.toBool());
    variant = getPref("cat-p", true);
    ui->paramsCheckBox->setChecked(variant.toBool());
    variant = getPref("cat-q", true);
    ui->queuesCheckBox->setChecked(variant.toBool());
    variant = getPref("cat-s", true);
    ui->statisticsCheckBox->setChecked(variant.toBool());
    variant = getPref("cat-g", true);
    ui->messagesCheckBox->setChecked(variant.toBool());
    variant = getPref("cat-c", true);
    ui->gatesCheckBox->setChecked(variant.toBool());
    variant = getPref("cat-v", true);
    ui->watchesCheckBox->setChecked(variant.toBool());
    variant = getPref("cat-f", true);
    ui->figuresCheckBox->setChecked(variant.toBool());
    variant = getPref("cat-o", true);
    ui->otherCheckBox->setChecked(variant.toBool());

    // Listbox
    listModel = new ObjectListModel();
    listView = new ObjectListView();
    listView->setModel(listModel);
    connect(getQtenv(), SIGNAL(objectDeletedSignal(cObject*)), listModel, SLOT(removeObject(cObject*)));

    int sortColumn = getPref("sortcolumn", 0).toInt();
    Qt::SortOrder sortOrder = getPref("sortorder", 0).value<Qt::SortOrder>();
    listView->sortByColumn(sortColumn, sortOrder);

    variant = getPref("columnwidths");
    if (variant.isValid()) {
        QString widths = variant.toString();
        QStringList columnWidths = widths.split("#");
        for (int i = 0; i < columnWidths.size(); ++i)
            listView->setColumnWidth(i, columnWidths[i].toInt());
    }

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    ui->listViewArea->setLayout(layout);
    layout->addWidget(listView);

    connect(listView, SIGNAL(activated(QModelIndex)), this, SLOT(inspect(QModelIndex)));
    connect(listView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(onListBoxSelectionChanged(QItemSelection, QItemSelection)));
    connect(getQtenv(), SIGNAL(fontChanged()), this, SLOT(onFontChanged()));

    // geometry
    QByteArray geometry = getPref("geometry", QByteArray()).value<QByteArray>();
    if (!geometry.isEmpty())
        restoreGeometry(geometry);
}

void FindObjectsDialog::onFontChanged()
{
    setFont(getQtenv()->getBoldFont());
}

void FindObjectsDialog::setPref(const QString &key, const QVariant &value)
{
    getQtenv()->setPref("FindObjectsDialog/" + key, value);
}

QVariant FindObjectsDialog::getPref(const QString &key, const QVariant &defaultValue)
{
    return getQtenv()->getPref("FindObjectsDialog/" + key, defaultValue);
}

void FindObjectsDialog::inspect(QModelIndex index)
{
    if (!index.isValid())
        return;

    bool outOfDate = getPref("outofdate", true).toBool();
    bool isRunning = getQtenv()->getMainWindow()->isRunning();

    if (outOfDate || isRunning) {
        QString advice;
        if (isRunning)
            advice = "please stop the simulation and click Refresh first";
        else
            advice = "please click Refresh first";

        QMessageBox::warning(this, "Filtered object list",
                "Dialog contents might be out of date -- " + advice + ".",
                QMessageBox::StandardButton::Ok);
        return;
    }

    cObject *object = static_cast<cObject *>(index.internalPointer());
    // XXX should call the future objectPicked method
    getQtenv()->inspect(object, INSP_DEFAULT, true);
}

void FindObjectsDialog::onListBoxSelectionChanged(QItemSelection selected, QItemSelection  /*deselected*/)
{
    cObject *object = selected.indexes().front().data(Qt::UserRole).value<cObject *>();
    if (object)
        // XXX should emit signal here, directly calling the handler is not nice, but will work
        getQtenv()->onSelectionChanged(object);
}

QStringList FindObjectsDialog::getClassNames()
{
    cRegistrationList *rList = classes.getInstance();
    envir::cCollectChildrenVisitor visitor(rList);
    visitor.process(rList);
    int objsSize = visitor.getArraySize();
    cObject **objs = visitor.getArray();

    // get the names
    QStringList classNames;
    for (int i = 0; i < objsSize; ++i)
        classNames.push_back(objs[i]->getFullName());

    classNames.sort();
    return classNames;
}

void FindObjectsDialog::refresh()
{
    // resolve root object
    std::string fullPath = ui->searchEdit->text().toStdString();
    cFindByPathVisitor visitor(fullPath.c_str());

    std::vector<cObject*> roots {
        getSimulation(),
        componentTypes.getInstance(),
        nedFunctions.getInstance(),
        classes.getInstance(),
        enums.getInstance(),
        classDescriptors.getInstance(),
        configOptions.getInstance(),
        resultFilters.getInstance(),
        resultRecorders.getInstance(),
        messagePrinters.getInstance(),
    };

    for (cObject *root : roots)
        visitor.process(root);

    if (visitor.getArraySize() == 0) {
        QMessageBox::warning(this, "Error",
                QString("Object to search in (\"") + fullPath.c_str() + "\") could not be resolved.",
                QMessageBox::StandardButton::Ok);
        return;
    }

    cObject *rootObject = visitor.getArray()[0];

    QString className = ui->comboBox->currentText();
    if (className.isEmpty())
        className = "*";

    try {
        checkPattern(className.toStdString().c_str());
    }
    catch (std::exception& e) {
        QMessageBox::warning(this, "Qtenv",
                QString("Class filter pattern \"") + className + "\" has invalid syntax -- using \"*\" instead.",
                QMessageBox::StandardButton::Ok);
        className = "*";
    }
    QString name = ui->fullPathEdit->text();
    if (name.isEmpty())
        name = "*";

    try {
        checkPattern(name.toStdString().c_str());
    }
    catch (std::exception& e) {
        QMessageBox::warning(this, "Qtenv",
                QString("Name filter pattern \"") + name + "\" has invalid syntax -- using \"*\" instead.",
                QMessageBox::StandardButton::Ok);
        name = "*";
    }
    // get list
    int maxCount = getPref("maxcount", 1000).toInt();
    int num = 0;
    cObject **objList = getSubObjectsFilt(rootObject, className.toStdString().c_str(), name.toStdString().c_str(), maxCount, num);

    // ask user if too many...
    QMessageBox::StandardButton viewAll = QMessageBox::StandardButton::Ok;
    if (num == maxCount) {
        viewAll = QMessageBox::warning(this, "Too many objects",
                    QString("Your query matched at least %1 objects, click OK to display them.").arg(num),
                    QMessageBox::StandardButton::Ok | QMessageBox::StandardButton::Cancel);
    }

    // clear listbox
    listModel->removeRow(0);

    // insert into listbox
    if (viewAll == QMessageBox::StandardButton::Ok) {
        if (num == maxCount)
            ui->label->setText("The first " + QString::number(num) + " objects found:");
        else
            ui->label->setText("Found " + QString::number(num) + " objects:");

        QVector<cObject *> objects;
        for (int i = 0; i < num; ++i)
            objects.push_back(objList[i]);

        listModel->setObjects(objects);
    }

    setPref("outofdate", false);
}

cObject **FindObjectsDialog::getSubObjectsFilt(cObject *object, const char *classNamePattern, const char *objFullPathPattern,
                                                 int maxCount, int &num)
{
    // args: <ptr> <class> <fullpath> <maxcount>, where
    //    <class> and <fullpath> may contain wildcards

    unsigned int category = 0;

    if (ui->modulesCheckBox->isChecked())
        category |= CATEGORY_MODULES;
    if (ui->paramsCheckBox->isChecked())
        category |= CATEGORY_PARAMS;
    if (ui->queuesCheckBox->isChecked())
        category |= CATEGORY_QUEUES;
    if (ui->statisticsCheckBox->isChecked())
        category |= CATEGORY_STATISTICS;
    if (ui->messagesCheckBox->isChecked())
        category |= CATEGORY_MESSAGES;
    if (ui->gatesCheckBox->isChecked())
        category |= CATEGORY_CHANSGATES;
    if (ui->watchesCheckBox->isChecked())
        category |= CATEGORY_WATCHES;
    if (ui->figuresCheckBox->isChecked())
        category |= CATEGORY_FIGURES;
    if (ui->otherCheckBox->isChecked())
        category |= CATEGORY_OTHERS;

    // get filtered list
    cFilteredCollectObjectsVisitor visitor;
    visitor.setSizeLimit(maxCount);
    visitor.setFilterPars(category, classNamePattern, objFullPathPattern);
    visitor.process(object);
    num = visitor.getArraySize();

    cObject **array = visitor.getArray();
    cObject **objs = new cObject *[num];
    for (int i = 0; i < num; ++i)
        objs[i] = array[i];

    return objs;
}

void FindObjectsDialog::checkPattern(const char *pattern)
{
    // try parse className
    MatchExpression matcher(pattern, false, true, true);
    // let's see if MatchableObjectAdapter can parse field names inside
    // (unmatched "[", etc.), by trying to match some random object
    MatchableObjectAdapter objectAdapter(MatchableObjectAdapter::FULLNAME, getSimulation());
    matcher.matches(&objectAdapter);
}

FindObjectsDialog::~FindObjectsDialog()
{
    setPref("rootobject", ui->searchEdit->text());
    setPref("class", ui->comboBox->currentText());
    setPref("name", ui->fullPathEdit->text());

    setPref("cat-m", ui->modulesCheckBox->isChecked());
    setPref("cat-p", ui->paramsCheckBox->isChecked());
    setPref("cat-q", ui->queuesCheckBox->isChecked());
    setPref("cat-s", ui->statisticsCheckBox->isChecked());
    setPref("cat-g", ui->messagesCheckBox->isChecked());
    setPref("cat-c", ui->gatesCheckBox->isChecked());
    setPref("cat-v", ui->watchesCheckBox->isChecked());
    setPref("cat-f", ui->figuresCheckBox->isChecked());
    setPref("cat-o", ui->otherCheckBox->isChecked());

    setPref("width", width());
    setPref("height", height());

    setPref("sortcolumn", listModel->getLastSortColumn());
    setPref("sortorder", listModel->getLastSortOrder());

    QString widths = "";
    for (int i = 0; i < listModel->columnCount(); ++i)
        widths += QString::number(listView->columnWidth(i)) + "#";

    setPref("columnwidths", widths);
    setPref("geometry", saveGeometry());

    // When the main window is closed while this dialog is open, Qt called
    // the data function unecessarily, and used corrupted pointers, which
    // causes a segfault in a dynamic_cast, so we have to delete the model.
    delete listModel;

    delete ui;
}

void FindObjectsDialog::invalidate()
{
    listModel->setObjects(QVector<cObject*>(0));
}

}  // namespace qtenv
}  // namespace omnetpp

