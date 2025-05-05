//==========================================================================
//  FILEEDITOR.CC - part of
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

#include "fileeditor.h"
#include "ui_fileeditor.h"
#include <QtWidgets/QToolBar>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenu>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include "textviewerwidget.h"
#include "textviewerproviders.h"
#include "logfinddialog.h"
#include "qtenv.h"

#include <QtCore/QDebug>

namespace omnetpp {
namespace qtenv {

FileEditor::FileEditor(QWidget *parent) : QDialog(parent), ui(new Ui::fileEditor)
{
    ui->setupUi(this);

    ui->plainTextEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->plainTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);

    // Configure the filename edit to look like a label but allow selection
    ui->filenameEdit->setReadOnly(true);
    ui->filenameEdit->setFrame(false);
    ui->filenameEdit->setCursor(Qt::IBeamCursor); // Text cursor for selection

    connect(getQtenv(), SIGNAL(fontChanged()), this, SLOT(updateFont()));

    connect(ui->plainTextEdit, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(onCustomContextMenuRequested(const QPoint &)));

    copySelectionAction = new QAction(QIcon(":/tools/copy"), "&Copy", this);
    copySelectionAction->setToolTip("Copy selected text to clipboard");
    // we do this opposite of the usual: Hold shift to copy _formatted_ text
    copySelectionAction->setShortcut(QKeySequence((int)Qt::CTRL | (int)Qt::SHIFT | Qt::Key_C));
    connect(copySelectionAction, SIGNAL(triggered(bool)), ui->plainTextEdit, SLOT(copy()));
    addAction(copySelectionAction);

    findAction = new QAction(QIcon(":/tools/find"), "&Find...", this);
    findAction->setToolTip("Find string in window");
    findAction->setShortcut(QKeySequence((int)Qt::CTRL | Qt::Key_F));
    connect(findAction, SIGNAL(triggered(bool)), this, SLOT(find()));
    addAction(findAction);

    findNextAction = new QAction("Find &Next", this);
    findNextAction->setShortcut(QKeySequence((int)Qt::Key_F3));
    connect(findNextAction, SIGNAL(triggered(bool)), this, SLOT(findNext()));
    addAction(findNextAction);

    saveAction = new QAction(QIcon(":/tools/save"), "&Save", this);
    saveAction->setToolTip("Save window contents to file");
    saveAction->setShortcut(QKeySequence((int)Qt::CTRL | Qt::Key_S));
    connect(saveAction, SIGNAL(triggered(bool)), this, SLOT(save()));
    addAction(saveAction);

    addToolBar();
    updateSaveActionState();
}

FileEditor::~FileEditor()
{
    delete ui;
}

void FileEditor::onCustomContextMenuRequested(const QPoint& pos)
{
    if (contextMenu == nullptr) {
        contextMenu = new QMenu();

        contextMenu->addAction(copySelectionAction);
        contextMenu->addSeparator();
        contextMenu->addAction(findAction);
        contextMenu->addAction(findNextAction);
        contextMenu->addSeparator();

        wrapLinesAction = contextMenu->addAction("&Wrap Lines", this, SLOT(wrapLines()));
        wrapLinesAction->setCheckable(true);
        contextMenu->addSeparator();

        contextMenu->addAction("&Select All", ui->plainTextEdit, SLOT(selectAll()))->setShortcut(QKeySequence((int)Qt::CTRL | Qt::Key_A));
    }

    contextMenu->exec(mapToGlobal(pos) + ui->plainTextEdit->pos());
}

void FileEditor::addToolBar()
{
    QToolBar *toolbar = new QToolBar();

    toolbar->addAction(copySelectionAction);
    toolbar->addAction(findAction);
    toolbar->addSeparator();
    toolbar->addAction(saveAction);

    // Insert toolbar to first place in vertical layout.
    static_cast<QBoxLayout *>(this->layout())->insertWidget(0, toolbar);
}

void FileEditor::updateFont()
{
    QFont logFont = getQtenv()->getLogFont();
    // ui->plainTextEdit->setFont(logFont); -- takes no effect
    ui->plainTextEdit->document()->setDefaultFont(logFont);
}

void FileEditor::findNext()
{
    QTextDocument::FindFlags flags = 0;

    if (findOptions & FIND_CASE_SENSITIVE)
        flags |= QTextDocument::FindCaseSensitively;
    if (findOptions & FIND_WHOLE_WORDS)
        flags |= QTextDocument::FindWholeWords;
    if (findOptions & FIND_BACKWARDS)
        flags |= QTextDocument::FindBackward;

    QTextCursor textCursor;
    if (findOptions & FIND_REGULAR_EXPRESSION) {
        Qt::CaseSensitivity regExpFlag = flags & QTextDocument::FindCaseSensitively ?
                    Qt::CaseSensitive : Qt::CaseInsensitive;
        textCursor = ui->plainTextEdit->document()->find(QRegExp(searchString, regExpFlag),
                                            ui->plainTextEdit->textCursor(), flags);
    }
    else
        textCursor = ui->plainTextEdit->document()->find(searchString, ui->plainTextEdit->textCursor(), flags);

    if (textCursor.isNull())
        QMessageBox::information(this, "Find", "'" + searchString + "' not found.", QMessageBox::Ok);
    else
        ui->plainTextEdit->setTextCursor(textCursor);
}

void FileEditor::wrapLines()
{
    qDebug() << wrapLinesAction->isChecked();
    ui->plainTextEdit->setLineWrapMode(wrapLinesAction->isChecked() ? QPlainTextEdit::WidgetWidth
                                                                    : QPlainTextEdit::NoWrap);
}

void FileEditor::find()
{
    findOptions = 0;
    if (getQtenv()->getPref("editor-case-sensitive", false).value<bool>())
        findOptions |= FIND_CASE_SENSITIVE;
    if (getQtenv()->getPref("editor-whole-words", false).value<bool>())
        findOptions |= FIND_WHOLE_WORDS;
    if (getQtenv()->getPref("editor-regexp", false).value<bool>())
        findOptions |= FIND_REGULAR_EXPRESSION;
    if (getQtenv()->getPref("editor-backwards", false).value<bool>())
        findOptions |= FIND_BACKWARDS;

    QString lastText = ui->plainTextEdit->textCursor().selectedText().isEmpty() ?
        getQtenv()->getPref("editor-last-text").value<QString>() : ui->plainTextEdit->textCursor().selectedText();

    LogFindDialog findDialog(this, lastText, static_cast<SearchFlags>(findOptions));
    findDialog.exec();

    findOptions = findDialog.getOptions();
    getQtenv()->setPref("editor-case-sensitive", bool(findOptions & FIND_CASE_SENSITIVE));
    getQtenv()->setPref("editor-whole-words", bool(findOptions & FIND_WHOLE_WORDS));
    getQtenv()->setPref("editor-regexp", bool(findOptions & FIND_REGULAR_EXPRESSION));
    getQtenv()->setPref("editor-backwards", bool(findOptions & FIND_BACKWARDS));
    getQtenv()->setPref("editor-last-text", findDialog.getText());

    if (findDialog.result() == QDialog::Rejected || findDialog.getText().isEmpty())
        return;

    searchString = findDialog.getText();
    findNext();
}

void FileEditor::save()
{
    if (!hasFile || readOnly)
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not save the file.", QMessageBox::Ok);
        return;
    }

    QTextStream out(&file);
    out << ui->plainTextEdit->toPlainText();
    file.close();
}

void FileEditor::setFile(const QString &fileName)
{
    this->fileName = fileName;
    this->hasFile = !fileName.isEmpty();
    ui->filenameEdit->setText(fileName);

    // actual file reading deferred to show()

    updateSaveActionState();
}

void FileEditor::setContent(const QString &content)
{
    ui->plainTextEdit->setPlainText(content);

    // No file associated with this content
    fileName = "";
    hasFile = false;

    updateSaveActionState();
}

void FileEditor::setDisplayFilename(const QString &title)
{
    ui->filenameEdit->setText(title);
}

void FileEditor::updateSaveActionState()
{
    saveAction->setEnabled(hasFile && !readOnly);
    ui->plainTextEdit->setReadOnly(readOnly);
}

bool FileEditor::readFile()
{
    QFile file(fileName);
    QIODevice::OpenMode mode = readOnly ? QIODevice::ReadOnly : QIODevice::ReadWrite;

    if (!file.open(mode | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "The file could not be opened.", QMessageBox::Ok);
        return false;
    }

    ui->plainTextEdit->setPlainText(QString(file.readAll()));
    file.close();

    return true;
}

void FileEditor::show()
{
    if (hasFile) {
        bool success = readFile();
        if (!success)
            return;
    }

    // Restore window geometry if available
    QByteArray geometry = getQtenv()->getPref("fileeditor-geom").toByteArray();
    if (!geometry.isEmpty())
        restoreGeometry(geometry);

    QDialog::show();

    updateFont(); // apparently this only takes effect if it is issued after show()
}

void FileEditor::reject()
{
    // Save window geometry when closing
    getQtenv()->setPref("fileeditor-geom", saveGeometry());
    QDialog::reject();
}

}  // namespace qtenv
}  // namespace omnetpp
