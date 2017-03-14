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
#include <QMessageBox>
#include <QKeyEvent>

#include <QDebug>

namespace omnetpp {

using namespace common;

namespace qtenv {

QString FindObjectsDialog::classNamePatternHelp =
    "Generic filter expression which matches class name by default.\n"
    "\n"
    "        Wildcards (\"?\", \"*\"), AND, OR, NOT and field matchers are accepted;\n"
    "        see Object Filter help for a more complete list.\n"
    "\n"
    "        Examples:\n"
    "          cQueue\n"
    "                    matches cQueue objects\n"
    "          TCP* or (IP* and not IPDatagram)\n"
    "                    matches objects whose class name begins with TCP or IP,\n"
    "                    excluding IPDatagrams\n"
    "          cMessage and kind(3)\n"
    "                    matches objects of class cMessage and message kind 3.";

QString FindObjectsDialog::namePatternHelp =
    "Generic filter expression which matches the object full path by default.\n"
    "\n"
    "Wildcards (\"?\", \"*\") are allowed. \"{a-exz}\" matches any character in the\n"
    "range \"a\"..\"e\", plus \"x\" and \"z\". You can match numbers: \"*.job{128..191}\"\n"
    "will match objects named \"job128\", \"job129\", ..., \"job191\". \"job{128..}\"\n"
    "and \"job{..191}\" are also understood. You can combine patterns with AND, OR\n"
    "and NOT and parentheses (lowercase and, or, not are also OK). You can match\n"
    "against other object fields such as queue length, message kind, etc., with\n"
    "the syntax \"fieldname(pattern)\". Put quotation marks around a pattern if it\n"
    "contains parentheses.\n"
    "\n"
    "HINT: You'll want to start the pattern with \"*.\" in most cases, to match\n"
    "objects anywhere in the network!\n"
    "\n"
    "Examples:\n"
    " *.destAddr\n"
    "            matches all objects whose name is \"destAddr\" (likely module\n"
    "            parameters)\n"
    " *.subnet2.*.destAddr\n"
    "            matches objects named \"destAddr\" inside \"subnet2\"\n"
    " *.node[8..10].*\n"
    "            matches anything inside module node[8], node[9] and node[10]\n"
    " className(cQueue) and not length(0)\n"
    "            matches non-empty queue objects\n"
    " className(cQueue) and length({10..})\n"
    "            matches queue objects with length>=10\n"
    " kind(3) or kind({7..9})\n"
    "            matches messages with message kind equal to 3, 7, 8 or 9\n"
    "            (Only messages have a \"kind\" attribute.)\n"
    " className(IP*) and *.data-*\n"
    "            matches objects whose class name begins with \"IP\" and\n"
    "            name begins with \"data-\"\n"
    " not className(cMessage) and byteLength({1500..})\n"
    "            matches messages whose class is not cMessage, and byteLength is\n"
    "            at least 1500. (Only messages have a \"byteLength\" attribute.)\n"
    " \"*(*\" or \"*.msg(ACK)\"\n"
    "            quotation marks needed when pattern is a reserved word or contains\n"
    "            parentheses. (Note: *.msg(ACK) without parens would be interpreted\n"
    "            as some object having a \"*.msg\" attribute with the value \"ACK\"!)";

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
    visitor.process(getSimulation());
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
        className = "*";
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

