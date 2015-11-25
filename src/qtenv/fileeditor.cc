//==========================================================================
//  FILEEDITOR.CC - part of
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

#include "fileeditor.h"
#include "ui_fileeditor.h"
#include <QToolBar>
#include <QBoxLayout>
#include <QMessageBox>
#include <QMenu>
#include "textviewerwidget.h"
#include "textviewerproviders.h"
#include "logfinddialog.h"

#include <QDebug>

namespace omnetpp {
namespace qtenv {

FileEditor::FileEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fileEditor),
    contextMenu(nullptr),
    searchString(""),
    findOptions(0)
{
    ui->setupUi(this);
    ui->plainTextEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->plainTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
    connect(ui->plainTextEdit, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onCustomContextMenuRequested(const QPoint&)));

    addToolBar();
}

FileEditor::~FileEditor()
{
    delete ui;
}

void FileEditor::onCustomContextMenuRequested(const QPoint &pos)
{
    if(contextMenu == nullptr)
    {
        contextMenu = new QMenu();

        contextMenu->addAction("&Copy", ui->plainTextEdit, SLOT(copy()))->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
        contextMenu->addSeparator();
        contextMenu->addAction("&Find...", this, SLOT(find()))->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
        contextMenu->addAction("Find &Next", this, SLOT(findNext()))->setShortcut(QKeySequence(Qt::Key_F3));
        contextMenu->addSeparator();

        wrapLinesAction = contextMenu->addAction("&Wrap Lines", this, SLOT(wrapLines()));
        wrapLinesAction->setCheckable(true);
        contextMenu->addSeparator();

        contextMenu->addAction("&Select All", ui->plainTextEdit, SLOT(selectAll()))->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
    }

    contextMenu->exec(mapToGlobal(pos) + ui->plainTextEdit->pos());
}

void FileEditor::addToolBar()
{
    QToolBar *toolbar = new QToolBar();

    toolbar->addAction(QIcon(":/tools/icons/tools/copy.png"), "Copy selected text to clipboard",
                       ui->plainTextEdit, SLOT(copy()))->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    toolbar->addAction(QIcon(":/tools/icons/tools/find.png"), "Find string in window",
                       this, SLOT(find()))->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
    toolbar->addSeparator();
    toolbar->addAction(QIcon(":/tools/icons/tools/save.png"), "Save window contents to file", this, SLOT(save()));

    // Insert toolbar to first place in vertical layout.
    static_cast<QBoxLayout*>(this->layout())->insertWidget(0, toolbar);
}

void FileEditor::findNext()
{
    QTextDocument::FindFlags flags = 0;

    if(findOptions & TextViewerWidget::FIND_CASE_SENSITIVE)
        flags |= QTextDocument::FindCaseSensitively;
    if(findOptions & TextViewerWidget::FIND_WHOLE_WORDS)
        flags |= QTextDocument::FindWholeWords;
    if(findOptions & TextViewerWidget::FIND_BACKWARDS)
        flags |= QTextDocument::FindBackward;

    QTextCursor textCursor;
    if(findOptions & TextViewerWidget::FIND_REGULAR_EXPRESSION)
    {
        Qt::CaseSensitivity regExpFlag = flags & QTextDocument::FindCaseSensitively ?
                    Qt::CaseSensitive : Qt::CaseInsensitive;
        textCursor = ui->plainTextEdit->document()->find(QRegExp(searchString, regExpFlag),
                                            ui->plainTextEdit->textCursor(), flags);
    }
    else
        textCursor = ui->plainTextEdit->document()->find(searchString, ui->plainTextEdit->textCursor(), flags);

    if(textCursor.isNull())
        QMessageBox::warning(this, tr("Find"), tr("'") + searchString + tr("' not found."), QMessageBox::Ok);
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
    if(getQtenv()->getPref("editor-case-sensitive", false).value<bool>())
        findOptions |= TextViewerWidget::FIND_CASE_SENSITIVE;
    if(getQtenv()->getPref("editor-whole-words", false).value<bool>())
        findOptions |= TextViewerWidget::FIND_WHOLE_WORDS;
    if(getQtenv()->getPref("editor-regexp", false).value<bool>())
        findOptions |= TextViewerWidget::FIND_REGULAR_EXPRESSION;
    if(getQtenv()->getPref("editor-backwards", false).value<bool>())
        findOptions |= TextViewerWidget::FIND_BACKWARDS;

    QString lastText = ui->plainTextEdit->textCursor().selectedText().isEmpty() ?
                getQtenv()->getPref("editor-last-text").value<QString>() : ui->plainTextEdit->textCursor().selectedText();

    LogFindDialog findDialog(this, lastText, static_cast<TextViewerWidget::FindOptions>(findOptions));
    findDialog.exec();

    findOptions = findDialog.getOptions();
    getQtenv()->setPref("editor-case-sensitive", bool(findOptions & TextViewerWidget::FIND_CASE_SENSITIVE));
    getQtenv()->setPref("editor-whole-words", bool(findOptions & TextViewerWidget::FIND_WHOLE_WORDS));
    getQtenv()->setPref("editor-regexp", bool(findOptions & TextViewerWidget::FIND_REGULAR_EXPRESSION));
    getQtenv()->setPref("editor-backwards", bool(findOptions & TextViewerWidget::FIND_BACKWARDS));
    getQtenv()->setPref("editor-last-text", findDialog.getText());

    if(findDialog.result() == QDialog::Rejected || findDialog.getText().isEmpty())
        return;

    searchString = findDialog.getText();
    findNext();
}

void FileEditor::save()
{
    file.resize(0);
    QTextStream out(&file);
    out << ui->plainTextEdit->toPlainText();
}

void FileEditor::setFile(QString fileName)
{
    file.close();
    file.setFileName(fileName);
}

void FileEditor::show()
{
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    if(file.error() != QFile::NoError)
    {
        QMessageBox::warning(this, tr("Info"), tr("Error: ") + "The file could not be opened.", QMessageBox::Ok);
        file.close();
        return;
    }

    ui->plainTextEdit->setPlainText(QString(file.readAll()));
    setWindowTitle(file.fileName());
    QDialog::show();
}

void FileEditor::reject()
{
    file.close();
    QDialog::reject();
}

} // namespace qtenv
} // namespace omnetpp
