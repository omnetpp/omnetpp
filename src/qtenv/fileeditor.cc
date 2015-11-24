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
#include "textviewerwidget.h"
#include "textviewerproviders.h"
#include "logfinddialog.h"

#include <QDebug>

namespace omnetpp {
namespace qtenv {

FileEditor::FileEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fileEditor)
{
    ui->setupUi(this);

    addToolBar();
}

FileEditor::~FileEditor()
{
    delete ui;
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
    static_cast<QBoxLayout*>(this->layout())->insertWidget(0, toolbar);
}

void FileEditor::find()
{
    TextViewerWidget::FindOptions options = 0;
    if(getQtenv()->getPref("editor-case-sensitive", false).value<bool>())
        options |= TextViewerWidget::FIND_CASE_SENSITIVE;
    if(getQtenv()->getPref("editor-whole-words", false).value<bool>())
        options |= TextViewerWidget::FIND_WHOLE_WORDS;
    if(getQtenv()->getPref("editor-regexp", false).value<bool>())
        options |= TextViewerWidget::FIND_REGULAR_EXPRESSION;
    if(getQtenv()->getPref("editor-backwards", false).value<bool>())
        options |= TextViewerWidget::FIND_BACKWARDS;

    LogFindDialog findDialog(this, ui->plainTextEdit->textCursor().selectedText(), options);
    findDialog.exec();

    QTextDocument::FindFlags flags = 0;
    options = findDialog.getOptions();

    if(options & TextViewerWidget::FIND_CASE_SENSITIVE)
        flags |= QTextDocument::FindCaseSensitively;
    if(options & TextViewerWidget::FIND_WHOLE_WORDS)
        flags |= QTextDocument::FindWholeWords;
    if(options & TextViewerWidget::FIND_BACKWARDS)
        flags |= QTextDocument::FindBackward;

    QTextCursor textCursor;
    if(options & TextViewerWidget::FIND_REGULAR_EXPRESSION)
    {
        Qt::CaseSensitivity regExpFlag = flags & QTextDocument::FindCaseSensitively ?
                    Qt::CaseSensitive : Qt::CaseInsensitive;
        textCursor = ui->plainTextEdit->document()->find(QRegExp(findDialog.getText(), regExpFlag),
                                            ui->plainTextEdit->textCursor(), flags);
    }
    else
        textCursor = ui->plainTextEdit->document()->find(findDialog.getText(), ui->plainTextEdit->textCursor(), flags);

    if(textCursor.isNull())
        QMessageBox::warning(this, tr("Find"), tr("'") + findDialog.getText() + tr("' not found."), QMessageBox::Ok);
    else
        ui->plainTextEdit->setTextCursor(textCursor);

    getQtenv()->setPref("editor-case-sensitive", bool(options & TextViewerWidget::FIND_CASE_SENSITIVE));
    getQtenv()->setPref("editor-whole-words", bool(options & TextViewerWidget::FIND_WHOLE_WORDS));
    getQtenv()->setPref("editor-regexp", bool(options & TextViewerWidget::FIND_REGULAR_EXPRESSION));
    getQtenv()->setPref("editor-backwards", bool(options & TextViewerWidget::FIND_BACKWARDS));
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
    QDialog::show();
}

void FileEditor::reject()
{
    file.close();
    QDialog::reject();
}

} // namespace qtenv
} // namespace omnetpp
