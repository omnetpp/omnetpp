//==========================================================================
//  TEXTVIEWERWIDGET.CC - part of
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

#include "textviewerwidget.h"
#include "textviewerproviders.h"
#include <QPainter>
#include <QStringList>
#include <QDebug>
#include <QGridLayout>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QApplication>
#include <QToolBar>
#include <QClipboard>
#include <QAction>
#include <QMessageBox>

namespace omnetpp {
namespace qtenv {

TextViewerWidget::TextViewerWidget(QWidget *parent)
    : QAbstractScrollArea(parent)
{
    backgroundColor = parent->palette().color(QPalette::Active, QPalette::Base);
    foregroundColor = parent->palette().color(QPalette::Active, QPalette::Text);

    selectionBackgroundColor = parent->palette().color(QPalette::Active, QPalette::Highlight);
    selectionForegroundColor = parent->palette().color(QPalette::Active, QPalette::HighlightedText);
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrolledHorizontally(int)));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrolledVertically(int)));

    connect(&autoScrollTimer, SIGNAL(timeout()), this, SLOT(onAutoScrollTimer()));
    connect(&caretBlinkTimer, SIGNAL(timeout()), this, SLOT(onCaretBlinkTimer()));

    caretBlinkTimer.setInterval(CARET_BLINK_DELAY);
    caretBlinkTimer.setSingleShot(false);
    caretBlinkTimer.start();

    // so the first single clicks behave normally
    timeSinceLastDoubleClick.invalidate();

    auto layout = new QVBoxLayout(this);
    setLayout(layout);
    headerModel = new QStandardItemModel(this);

    header = new QHeaderView(Qt::Horizontal);
    header->setStretchLastSection(true);
    header->setDefaultAlignment(Qt::Alignment(Qt::AlignLeft | Qt::AlignVCenter));
    header->setModel(headerModel);

    setFont(getMonospaceFont());

    layout->setMargin(0);
    layout->addWidget(header);
    layout->addWidget(viewport());

    connect(header, SIGNAL(sectionResized(int, int, int)), this, SLOT(onHeaderSectionResized(int, int, int)));
}

TextViewerWidget::~TextViewerWidget()
{
    delete content;
}

// from http://stackoverflow.com/a/18897568
QFont TextViewerWidget::getMonospaceFont()
{
    QFont font("monospace");
    if (QFontInfo(font).fixedPitch())
        return font;
    font.setStyleHint(QFont::Monospace);
    if (QFontInfo(font).fixedPitch())
        return font;
    font.setStyleHint(QFont::TypeWriter);
    if (QFontInfo(font).fixedPitch())
        return font;
    font.setFamily("courier");
    if (QFontInfo(font).fixedPitch())
        return font;
    return font;
}

void TextViewerWidget::setFont(QFont font)
{
    this->font = font;
    auto metrics = QFontMetrics(font, viewport());

    baseline = metrics.leading() + metrics.ascent();
    lineSpacing = metrics.lineSpacing();
    averageCharWidth = metrics.averageCharWidth();

    isMonospaceFont = QFontInfo(font).fixedPitch();

    viewport()->update();
}

void TextViewerWidget::setToolBar(QToolBar *toolBar)
{
    this->toolBar = toolBar;
    if (toolBar) {
        delete viewport()->layout();
        QGridLayout *layout = new QGridLayout(viewport());
        viewport()->setLayout(layout);

        layout->addWidget(toolBar, 0, 0, Qt::Alignment(Qt::AlignRight | Qt::AlignTop));
        layout->setMargin(toolbarSpacing);

        toolBar->setAutoFillBackground(true);
    }
}

void TextViewerWidget::setCaretPosition(int lineIndex, int column)
{
    caretLineIndex = clip(0, lineIndex, content->getLineCount()-1);
    caretColumn = std::max(0, column);  // do NOT clip to line length! content provider may be O(n)
    clearSelection();
    viewport()->update();
    emit caretMoved(caretLineIndex, caretColumn);
}

Pos TextViewerWidget::getCaretPosition()
{
    int clippedCaretColumn = clip(0, content->getLineText(caretLineIndex).length(), caretColumn);
    return Pos(caretLineIndex, clippedCaretColumn);
}

void TextViewerWidget::setSelection(int startLineIndex, int startColumn, int endLineIndex, int endColumn)
{
    // XXX assert ranges!
    selectionAnchorLineIndex = startLineIndex;
    selectionAnchorColumn = startColumn;
    caretLineIndex = endLineIndex;
    caretColumn = endColumn;
    viewport()->update();
    emit caretMoved(caretLineIndex, caretColumn);
}

Pos TextViewerWidget::getSelectionAnchor()
{
    int clippedSelectionAnchorColumn = clip(0, content->getLineText(selectionAnchorLineIndex).length(), selectionAnchorColumn);
    return Pos(selectionAnchorLineIndex, clippedSelectionAnchorColumn);
}

void TextViewerWidget::setSelectionAnchor(int lineIndex, int column)
{
    selectionAnchorLineIndex = lineIndex;  // XXX assert range!
    selectionAnchorColumn = column;  // XXX assert range!
    viewport()->update();
}

Pos TextViewerWidget::getSelectionStart()
{
    Pos anchor = getSelectionAnchor();
    Pos caret = getCaretPosition();
    return anchor < caret ? anchor : caret;
}

Pos TextViewerWidget::getSelectionEnd()
{
    Pos anchor = getSelectionAnchor();
    Pos caret = getCaretPosition();
    return anchor < caret ? caret : anchor;
}

void TextViewerWidget::find(QString text, FindOptions options)
{
    QString originalText = text;  // for the dialog if not found

    // just for convenience
    bool regExp = options.testFlag(FIND_REGULAR_EXPRESSION);
    bool wholeWords = options.testFlag(FIND_WHOLE_WORDS);
    bool backwards = options.testFlag(FIND_BACKWARDS);
    bool caseSensitive = options.testFlag(FIND_CASE_SENSITIVE);

    bool found = false;  // sticky!

    if (!regExp) {  // yes, we cheat, but it's way simpler like this
        text = QRegExp::escape(text);
    }

    if (wholeWords) {  // \b matches a "word boundary"
        text = "\\b" + text + "\\b";
    }

    QRegExp re(text, caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);

    // the -1 is to actually find the previous match, not the currently selected (using F3)
    int offset = backwards ? getSelectionStart().column - 1 : getSelectionEnd().column;
    int line = backwards ? getSelectionStart().line : getSelectionEnd().line;

    for (  /* nothing */; (line >= 0) && (line < content->getLineCount()); line += (backwards ? -1 : 1)) {
        int index = -1;

        if (backwards) {
            index = re.lastIndexIn(content->getLineText(line), offset);
            offset = -1;  // was needed only for the first searched line
        }
        else {
            index = re.indexIn(content->getLineText(line), offset);
            offset = 0;  // was needed only for the first searched line
        }

        if (index >= 0) {
            setSelection(line, index, line, index + re.matchedLength());
            found = true;  // yay!
            break;  // ouch.
        }
    }

    if (found) {
        revealCaret();
    }
    else {
        clearSelection();
        QMessageBox::information(this, "Not found", "No match for \"" + originalText + "\".");
    }

    viewport()->update();
}

int TextViewerWidget::getMaxVisibleLineWidth()
{
    auto metrics = QFontMetrics(font, viewport());
    int maxLength = 0;
    // can't use getNumVisibleLines here, that would potentially cause an infinite loop
    int n = (maximumViewportSize().height() + lineSpacing - 1) / lineSpacing;
    for (int lineIndex = topLineIndex; lineIndex < content->getLineCount() && lineIndex < topLineIndex + n; ++lineIndex) {
        auto line = content->getLineText(lineIndex);
        auto tabStops = content->getTabStops(lineIndex);
        maxLength = std::max(maxLength, getLineColumnOffset(metrics, line, tabStops, line.length()) + horizontalScrollOffset);
    }
    return maxLength;
}

int TextViewerWidget::getNumVisibleLines()
{
    // Counts partially visible lines as well.
    return (viewport()->height() + lineSpacing - 1) / lineSpacing;
}

int TextViewerWidget::getNumVisibleColumns()
{
    // Counts partially visible columns as well.
    return (viewport()->width() + averageCharWidth - 1) / averageCharWidth;
}

int TextViewerWidget::getLinePartOffset(const QFontMetrics& metrics, const QString& line, const QList<TabStop>& tabStops, int partIndex)
{
    int offset = leftMargin - horizontalScrollOffset;
    for (int i = 0; i < tabStops.size(); ++i) {
        bool last = (i == tabStops.size() - 1);
        QString part = line.mid(tabStops[i].atCharacter, last ? -1 : (tabStops[i+1].atCharacter - tabStops[i].atCharacter));
        // the + lineHeight is needed because in this function the y coordinate is the baseline
        int partX = std::max(offset, header->sectionPosition(i) - horizontalScrollOffset);
        if (partIndex == i) {
            return partX;
        }
        offset = partX + metrics.width(part);
    }

    throw std::runtime_error("partIndex out of bounds");
}

int TextViewerWidget::getLineColumnOffset(const QFontMetrics& metrics, const QString& line, const QList<TextViewerWidget::TabStop>& tabStops, int columnIndex)
{
    int tabIndex = tabStops.size()-1;
    while (tabIndex > 0 && tabStops[tabIndex].atCharacter > columnIndex) {
        --tabIndex;
    }
    tabIndex = clip(0, tabStops.size()-1, tabIndex);

    int offset = getLinePartOffset(metrics, line, tabStops, tabIndex)
                  + metrics.width(line.mid(tabStops[tabIndex].atCharacter,
                                           columnIndex - tabStops[tabIndex].atCharacter));
    return offset;
}

Pos TextViewerWidget::getLineColumnAt(int x, int y)
{
    int lineIndex = topLineIndex + (y-topLineY) / lineSpacing;
    lineIndex = clip(0, content->getLineCount()-1, lineIndex);

    QString line = content->getLineText(lineIndex);
    auto tabStops = content->getTabStops(lineIndex);

    auto metrics = QFontMetrics(font, viewport());

    int tabIndex;
    int offset = 0;

    for (tabIndex = tabStops.size() - 1; tabIndex > 0; --tabIndex) {  // not checking the 0., if we got there, we must certainly stop
        offset = getLinePartOffset(metrics, line, tabStops, tabIndex);

        if (offset < x) {
            break;
        }
    }
    // just to be safe
    tabIndex = clip(0, tabStops.size()-1, tabIndex);

    offset = getLinePartOffset(metrics, line, tabStops, tabIndex);

    QString part = line.mid(tabStops[tabIndex].atCharacter, (tabIndex == tabStops.size() -1) ? -1 : (tabStops[tabIndex+1].atCharacter - tabStops[tabIndex].atCharacter));
    int column = part.length();

    for (int i = 0; i < part.length(); i++) {
        if (offset + metrics.width(part, i+1) >= x) {
            column = i;
            break;
        }
    }

    return Pos(lineIndex, column + tabStops[tabIndex].atCharacter);
}

void TextViewerWidget::setContentProvider(TextViewerContentProvider *newContent)
{
    delete content;  // this will disconnect the signals

    content = newContent;

    if (content) {
        connect(content, SIGNAL(textChanged()), this, SLOT(onContentChanged()));
        connect(content, SIGNAL(linesDiscarded(int)), this, SLOT(onLinesDiscarded(int)));
    }

    headerModel->clear();
    auto headers = content->getHeaders();
    headerModel->setHorizontalHeaderLabels(headers);

    if (content->showHeaders()) {
        header->show();
        // no other way of setting a fixed height worked correctly for me
        header->setFixedHeight(header->sizeHint().height());
        setViewportMargins(0, header->height(), 0, 0);
    }
    else {
        header->hide();
        setViewportMargins(0, 0, 0, 0);
        for (int i = 0; i < headers.size(); ++i) {
            // needed to make all of them 0 width (except the last one, which will stretch)
            header->setSectionHidden(i, true);
        }
    }

    onContentChanged();

    doContentEnd(false);
    revealCaret();
}

TextViewerContentProvider *TextViewerWidget::getContentProvider()
{
    return content;
}

QList<QVariant> TextViewerWidget::getColumnWidths()
{
    QList<QVariant> widths;

    for (int i = 0; i < headerModel->columnCount(); ++i) {
        widths.append(header->sectionSize(i));
    }

    return widths;
}

void TextViewerWidget::setColumnWidths(const QList<QVariant>& widths)
{
    for (int i = 0; i < std::min(headerModel->columnCount(), widths.length()); ++i) {
        header->resizeSection(i, widths[i].toInt());
    }
}

void TextViewerWidget::doLineUp(bool select)
{
    caretLineIndex = std::max(0, caretLineIndex-1);
    if (!select)
        clearSelection();
    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::doLineDown(bool select)
{
    caretLineIndex = clip(0, content->getLineCount()-1, caretLineIndex+1);
    if (!select)
        clearSelection();
    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::doCursorPrevious(bool select)
{
    caretColumn = std::min(caretColumn, content->getLineText(caretLineIndex).length());
    if (caretColumn > 0)
        caretColumn--;
    else if (caretLineIndex > 0) {
        caretLineIndex--;
        caretColumn = content->getLineText(caretLineIndex).length();
    }
    if (!select)
        clearSelection();
    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::doCursorNext(bool select)
{
    if (caretColumn < content->getLineText(caretLineIndex).length())
        caretColumn++;
    else if (caretLineIndex < content->getLineCount()-1) {
        caretLineIndex++;
        caretColumn = 0;
    }
    if (!select)
        clearSelection();
    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::doPageUp(bool select)
{
    int pageLines = std::max(1, getNumVisibleLines()-1);
    caretLineIndex = std::max(0, caretLineIndex - pageLines);
    topLineIndex = clip(0, std::max(0, content->getLineCount()-getNumVisibleLines()), topLineIndex - pageLines);
    if (!select)
        clearSelection();
    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::doPageDown(bool select)
{
    int pageLines = std::max(1, getNumVisibleLines()-1);
    int lastLineIndex = content->getLineCount()-1;
    caretLineIndex = std::min(lastLineIndex, caretLineIndex + pageLines);
    topLineIndex = clip(0, std::max(0, content->getLineCount()-getNumVisibleLines()), topLineIndex + pageLines);
    if (!select)
        clearSelection();
    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::doLineStart(bool select)
{
    caretColumn = 0;
    if (!select)
        clearSelection();
    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::doLineEnd(bool select)
{
    caretColumn = content->getLineText(caretLineIndex).length();
    if (!select)
        clearSelection();
    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::doWordPrevious(bool select)
{
    QString line = content->getLineText(caretLineIndex);
    int pos = caretColumn;
    if (pos == 0) {
        // go to end of previous line
        if (caretLineIndex > 0) {
            caretLineIndex--;
            caretColumn = content->getLineText(caretLineIndex).length();
        }
    }
    else {
        // go to start of current or previous word
        while (pos > 0 && (pos >= line.length() || !isWordChar(line.at(pos))))
            pos--;
        while (pos > 0 && isWordChar(line.at(pos-1)))
            pos--;
        caretColumn = pos;
    }
    if (!select)
        clearSelection();
    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::doWordNext(bool select)
{
    QString line = content->getLineText(caretLineIndex);
    int pos = caretColumn;
    if (pos == line.length()) {
        // go to start of next line
        if (caretLineIndex < content->getLineCount()-1) {
            caretLineIndex++;
            caretColumn = 0;
        }
    }
    else {
        // go to end of current or next word
        pos++;  // move at least one character
        while (pos < line.length() && isWordChar(line.at(pos)))
            pos++;
        while (pos < line.length() && !isWordChar(line.at(pos)))
            pos++;
        caretColumn = pos;
    }
    if (!select)
        clearSelection();
    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::doPageStart(bool select)
{
    caretLineIndex = topLineIndex;
    caretColumn = 0;
    if (!select)
        clearSelection();
    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::doPageEnd(bool select)
{
    caretLineIndex = std::min(content->getLineCount()-1, topLineIndex + getNumVisibleLines());
    caretColumn = content->getLineText(caretLineIndex).length();
    if (!select)
        clearSelection();
    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::doContentStart(bool select)
{
    caretLineIndex = 0;
    caretColumn = 0;
    if (!select)
        clearSelection();
    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::doContentEnd(bool select)
{
    caretLineIndex = content->getLineCount()-1;
    caretColumn = content->getLineText(caretLineIndex).length();
    if (!select)
        clearSelection();
    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::selectAll()
{
    selectionAnchorLineIndex = 0;
    selectionAnchorColumn = 0;
    caretLineIndex = content->getLineCount()-1;
    caretColumn = content->getLineText(caretLineIndex).length();
    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::clearSelection()
{
    selectionAnchorLineIndex = caretLineIndex;
    selectionAnchorColumn = caretColumn;
}

int TextViewerWidget::clip(int lower, int upper, int x)
{
    if (x < lower)
        x = lower;
    if (x > upper)
        x = upper;
    return x;
}

bool TextViewerWidget::isWordChar(QChar ch)
{
    return ch.isLetterOrNumber() || ch == '_' || ch == '@';
}

void TextViewerWidget::resizeEvent(QResizeEvent *event)
{
    updateScrollbars();
    viewport()->update();
}

void TextViewerWidget::paintEvent(QPaintEvent *event)
{
    if (contentChangedFlag) {
        handleContentChange();
    }

    QPainter painter(viewport());

    painter.setBackground(backgroundColor);
    painter.setBrush(foregroundColor);

    QRect r = contentsRect();
    painter.fillRect(r, backgroundColor);

    painter.setFont(font);
    if (font != painter.font()) {
        // This is basically a HACK.
        // We can't create a QPainter equivalent to this (using the same QPaintDevice, etc...)
        // outside the paintEvent function (Qt gives a warning).
        // And the QPainter might not use exactly the font we give it,
        // rather it will find an appropriate "close match".
        // But we need the exact font used to calculate all kinds of caret positions and such, so
        // we have no other choice, but to save the actually used font for ourselves here.
        setFont(painter.font());
    }

    // this has to be after the font has been updated, because then the lineSpacing changes, and we need the actual value here
    updateScrollbars();

    int numLines = content->getLineCount();
    int numVisibleLines = getNumVisibleLines();

    // note: if the following asserts are triggered, odds are that content has changed without a call to our contentChanged() method
    Q_ASSERT(topLineIndex >= 0 && topLineIndex <= std::max(0, numLines-numVisibleLines));
    Q_ASSERT(numLines == 0 || (caretLineIndex >= 0 && caretLineIndex < numLines));

    int x = leftMargin - horizontalScrollOffset;
    int lineIndex = topLineIndex;
    int startY = topLineY;

    // Drawing the gray highlight under the line in which the caret is-
    // Have to do this before any text painting, otherwise
    // it might paint over some parts of a few letters.
    int highlightY = startY + (caretLineIndex-topLineIndex) * lineSpacing;
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor("lightgray"));
    painter.drawRect(0, highlightY, contentsRect().width(), lineSpacing);

    // draw the lines
    for (int y = startY; y < r.y()+r.height() && lineIndex < numLines; y += lineSpacing) {
        drawLine(painter, lineIndex++, x, y);
    }
}

void TextViewerWidget::drawLine(QPainter& painter, int lineIndex, int x, int y)
{
    // draw the line in the specified color
    QString line = content->getLineText(lineIndex).trimmed();  // removing the new line character
    auto tabStops = content->getTabStops(lineIndex);
    if (tabStops.empty()) {
        // so the model can give an empty list, but we will still have something to work with
        tabStops.append(TabStop(0, foregroundColor));
    }

    painter.setFont(font);
    QFontMetrics metrics = painter.fontMetrics();

    for (int partIndex = 0; partIndex < tabStops.size(); ++partIndex) {
        drawLinePart(painter, metrics, line, tabStops, lineIndex, partIndex, y);
    }
    painter.setPen(foregroundColor);
}

void TextViewerWidget::drawLinePart(QPainter& painter, const QFontMetrics& metrics, const QString& line, const QList<TabStop>& tabStops, int lineIndex, int partIndex, int y)
{
    bool last = (partIndex == tabStops.size() - 1);
    const TabStop& tabStop = tabStops[partIndex];
    int nextTabStopAt = last ? line.length() : tabStops[partIndex + 1].atCharacter;
    QString part = line.mid(tabStop.atCharacter, nextTabStopAt - tabStop.atCharacter);

    painter.setPen(tabStop.color);
    painter.drawText(QPoint(getLinePartOffset(metrics, line, tabStops, partIndex), y + baseline), part);

    // if there is selection, draw it
    if (selectionAnchorLineIndex != caretLineIndex || selectionAnchorColumn != caretColumn) {
        Pos selStart = getSelectionStart();
        Pos selEnd = getSelectionEnd();

        // if lineIndex is in the selected region, redraw the selected part
        if (lineIndex >= selStart.line && lineIndex <= selEnd.line) {
            int startColumn = (lineIndex == selStart.line) ? clip(tabStop.atCharacter, nextTabStopAt, selStart.column) : tabStop.atCharacter;
            int endColumn = (lineIndex == selEnd.line) ? clip(tabStop.atCharacter, nextTabStopAt, selEnd.column) : nextTabStopAt;

            if (startColumn != endColumn) {
                int startColOffset = getLineColumnOffset(metrics, line, tabStops, startColumn);
                int endColOffset = getLineColumnOffset(metrics, line, tabStops, endColumn);
                QString selection = line.mid(startColumn, endColumn - startColumn);

                painter.setBrush(selectionBackgroundColor);
                painter.setPen(selectionForegroundColor);
                painter.fillRect(startColOffset, y, endColOffset - startColOffset, lineSpacing, selectionBackgroundColor);
                painter.drawText(QPoint(getLineColumnOffset(metrics, line, tabStops, startColumn), y + baseline), selection);
                painter.setBackground(backgroundColor);
                painter.setPen(foregroundColor);
            }
        }
    }

    if (lineIndex == caretLineIndex && caretShown && hasFocus()) {
        // draw caret
        painter.setPen(foregroundColor);
        int caretX = getLineColumnOffset(metrics, line, tabStops, caretColumn);
        painter.drawLine(caretX, y, caretX, y + lineSpacing-1);
    }
}

void TextViewerWidget::keyPressEvent(QKeyEvent *event)
{
    bool shiftPressed = event->modifiers() & Qt::ShiftModifier;
    bool controlPressed = event->modifiers() & Qt::ControlModifier;

    switch (event->key()) {
        case Qt::Key_Right:
            doCursorNext(shiftPressed);
            break;
        case Qt::Key_Left:
            doCursorPrevious(shiftPressed);
            break;
        case Qt::Key_Up:
            doLineUp(shiftPressed);
            break;
        case Qt::Key_Down:
            doLineDown(shiftPressed);
            break;
        case Qt::Key_PageUp:
            doPageUp(shiftPressed);
            break;
        case Qt::Key_PageDown:
            doPageDown(shiftPressed);
            break;
        case Qt::Key_Home:
            if (controlPressed)
                doContentStart(shiftPressed);
            else
                doLineStart(shiftPressed);
            break;
        case Qt::Key_End:
            if (controlPressed)
                doContentEnd(shiftPressed);
            else
                doLineEnd(shiftPressed);
            break;
        case Qt::Key_A:
            if (controlPressed)
                selectAll();
            break;
    }

    caretShown = true;
    caretBlinkTimer.start();
    revealCaret();
    viewport()->update();
}

void TextViewerWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (clickCount > 0) {
        Pos lineColumn = getLineColumnAt(event->x(), event->y());
        caretLineIndex = clip(0, content->getLineCount()-1, lineColumn.line);
        caretColumn = clip(0, content->getLineText(caretLineIndex).length(), lineColumn.column);
        viewport()->update();
        doAutoScroll(event);  // start/stop autoscrolling as needed
        emit caretMoved(caretLineIndex, caretColumn);
    }
}

void TextViewerWidget::mouseReleaseEvent(QMouseEvent *event)
{
    clickCount = 0;
    stopAutoScroll();
}

void TextViewerWidget::mousePressEvent(QMouseEvent *event)
{
    Pos lineColumn = getLineColumnAt(event->x(), event->y());

    caretShown = true;
    caretBlinkTimer.start();

    if (event->buttons() & Qt::LeftButton) {
        if (timeSinceLastDoubleClick.isValid() // triple click event emulation
                && timeSinceLastDoubleClick.elapsed() < TRIPLE_CLICK_THRESHOLD_MS) {
            // selecting the whole line
            doLineStart(false);
            doLineEnd(true);

            clickCount = 3;
            timeSinceLastDoubleClick.invalidate();
        }
        else {  // it is really a single click
            caretLineIndex = clip(0, content->getLineCount()-1, lineColumn.line);
            caretColumn = clip(0, content->getLineText(caretLineIndex).length(), lineColumn.column);
            clickCount = 1;

            bool select = event->modifiers() & Qt::ShiftModifier;
            if (!select)
                clearSelection();
            emit caretMoved(caretLineIndex, caretColumn);
        }

        revealCaret();
        viewport()->update();
    }
    else if (event->buttons() & Qt::RightButton) {
        caretLineIndex = clip(0, content->getLineCount()-1, lineColumn.line);
        caretColumn = clip(0, content->getLineText(caretLineIndex).length(), lineColumn.column);
        clearSelection();

        revealCaret();
        viewport()->update();

        emit rightClicked(event->globalPos(), caretLineIndex, caretColumn);
    }
}

void TextViewerWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    Pos lineColumn = getLineColumnAt(event->x(), event->y());
    caretLineIndex = clip(0, content->getLineCount()-1, lineColumn.line);
    caretColumn = clip(0, content->getLineText(caretLineIndex).length(), lineColumn.column);

    // double click - select word
    doWordPrevious(false);
    doWordNext(true);

    revealCaret();
    viewport()->update();
    clickCount = 2;

    timeSinceLastDoubleClick.restart();
    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::doAutoScroll(QMouseEvent *event)
{
    QRect clientArea = contentsRect();
    if (event->y() > clientArea.height())
        startOrRefineAutoScroll(SCROLL_DOWN, event->y() - clientArea.height());
    else if (event->y() < 0)
        startOrRefineAutoScroll(SCROLL_UP, -event->y());
    else if (event->x() < 0)
        startOrRefineAutoScroll(SCROLL_LEFT, -event->x());
    else if (event->x() > clientArea.width())
        startOrRefineAutoScroll(SCROLL_RIGHT, event->x() - clientArea.width());
    else
        stopAutoScroll();
}

void TextViewerWidget::startOrRefineAutoScroll(ScrollDirection direction, int distance)
{
    autoScrollDistance = distance;

    if (autoScrollDirection == direction)
        return;  // already autoscrolling in the given direction, do nothing

    // restart timer
    autoScrollDirection = direction;
    int rate = autoScrollDirection == SCROLL_UP || autoScrollDirection == SCROLL_DOWN ? V_SCROLL_RATE : H_SCROLL_RATE;

    autoScrollTimer.stop();
    autoScrollTimer.setInterval(rate);
    autoScrollTimer.setSingleShot(false);
    autoScrollTimer.start();
}

void TextViewerWidget::stopAutoScroll()
{
    autoScrollDirection = SCROLL_NONE;
    autoScrollTimer.stop();
}

void TextViewerWidget::updateScrollbars(bool allowStickingToBottom)
{
    auto vsb = verticalScrollBar();
    auto hsb = horizontalScrollBar();

    // for following scroll at the bottom
    bool atBottom = vsb->value() == vsb->maximum();

    // configure
    int visibleLines = getNumVisibleLines();
    vsb->setMaximum(std::max(0, content->getLineCount() - visibleLines));
    vsb->setMinimum(0);
    vsb->setSingleStep(1);
    vsb->setPageStep(visibleLines);

    int viewportWidth = viewport()->width();
    hsb->setMinimum(0);
    hsb->setMaximum(std::max(0, getMaxVisibleLineWidth() - viewportWidth));
    hsb->setPageStep(viewportWidth);
    hsb->setSingleStep(4);

    // adjust
    topLineIndex = clip(0, std::max(0, content->getLineCount() - getNumVisibleLines()), topLineIndex);
    verticalScrollBar()->setValue(topLineIndex);
    horizontalScrollBar()->setValue(horizontalScrollOffset);

    if (vsb->maximum() > vsb->minimum()) {  // we can scroll at all
        // following output growth
        if (atBottom && allowStickingToBottom) {
            vsb->setValue(vsb->maximum());
            alignBottomLine();
        }
    }
    else {  // content fits on the viewport entirely
        alignTopLine();
    }
}

void TextViewerWidget::handleContentChange()
{
    Q_ASSERT(content->getLineCount() > 0);  // , "content must be at least one line");

    // adjust caret and selection line index
    int oldCaretLineIndex = caretLineIndex;
    caretLineIndex = clip(0, content->getLineCount()-1, caretLineIndex);
    selectionAnchorLineIndex = clip(0, content->getLineCount()-1, selectionAnchorLineIndex);

    // only emitting the signal if it actually moved
    if (oldCaretLineIndex != caretLineIndex)
        emit caretMoved(caretLineIndex, caretColumn);

    // NOTE: for performance reasons, DO NOT ADJUST COLUMN INDICES!!! An out-of-bounds
    // column index causes no problem, but to clip it to the line length would require
    // us to ask the content provide for the line -- at that can be very COSTLY if
    // the line index is far from the currently displayed line range. (The underlying
    // content provider was not designed for random access, it takes O(n) time to
    // return a random line!!!)
    //
    // So, do NOT do this:
    // caretColumn = clip(0, content.getLine(caretLineIndex).length(), caretColumn);
    // selectionAnchorColumn = clip(0, content.getLine(caretLineIndex).length(), selectionAnchorColumn);

    updateScrollbars();

    viewport()->update();

    contentChangedFlag = false;
}

void TextViewerWidget::revealCaret()
{
    if (caretLineIndex < topLineIndex)
        topLineIndex = caretLineIndex;
    if (caretLineIndex >= topLineIndex + getNumVisibleLines())
        topLineIndex = caretLineIndex - getNumVisibleLines() + 1;
    topLineIndex = clip(0, std::max(0, content->getLineCount()-getNumVisibleLines()), topLineIndex);

    // if caret is in the top or bottom line, view that line fully
    if (caretLineIndex == topLineIndex)
        alignTopLine();
    else if (caretLineIndex == topLineIndex+getNumVisibleLines()-1)
        alignBottomLine();

    // TODO with columns too
    //        if (caretColumn < )
    //            topLineIndex = caretLineIndex;
    //        if (caretLineIndex >= topLineIndex + getNumVisibleLines())
    //            topLineIndex = caretLineIndex - getNumVisibleLines() + 1;

    // we want to break free from the bottom
    updateScrollbars(false);
}

void TextViewerWidget::alignTopLine()
{
    topLineY = 0;
}

void TextViewerWidget::alignBottomLine()
{
    topLineY = viewport()->height() % lineSpacing;
    if (topLineY > 0)
        topLineY -= lineSpacing;
}

void TextViewerWidget::onAutoScrollTimer()
{
    QScrollBar *hsb = horizontalScrollBar();
    QScrollBar *vsb = verticalScrollBar();

    switch (autoScrollDirection) {
        case SCROLL_UP: // vertical scroll is in lines, hence the division
            vsb->setValue(vsb->value() - autoScrollDistance / 10);
            break;
        case SCROLL_DOWN:
            vsb->setValue(vsb->value() + autoScrollDistance / 10);
            break;
        case SCROLL_LEFT: // horizontal is in pixels
            hsb->setValue(hsb->value() - autoScrollDistance);
            break;
        case SCROLL_RIGHT:
            hsb->setValue(hsb->value() + autoScrollDistance);
            break;
        default:
            break;
    }
}

void TextViewerWidget::onCaretBlinkTimer()
{
    if (hasFocus()) {
        caretShown = !caretShown;
        viewport()->update();
    }
}

void TextViewerWidget::onHeaderSectionResized(int logicalIndex, int oldSize, int newSize)
{
    viewport()->update();
}

void TextViewerWidget::copySelection()
{
    QString text;

    Pos start = getSelectionStart();
    Pos end = getSelectionEnd();

    if (start.line == end.line) {
        text = content->getLineText(start.line).mid(end.column, end.column - start.column);
    }
    else {
        text = content->getLineText(start.line).mid(start.column).trimmed() + "\n";
        for (int l = start.line + 1; l < end.line; ++l) {
            text += content->getLineText(l).trimmed() + "\n";
        }
        text += content->getLineText(end.line).left(end.column).trimmed();
    }

    QApplication::clipboard()->setText(text);
}

void TextViewerWidget::onContentChanged()
{
    contentChangedFlag = true;
}

void TextViewerWidget::onLinesDiscarded(int numLinesDiscarded)
{
    caretLineIndex -= numLinesDiscarded;
    if (caretLineIndex < 0) {
        caretLineIndex = 0;
        caretColumn = 0;
    }
    selectionAnchorLineIndex -= numLinesDiscarded;
    if (selectionAnchorLineIndex < 0) {
        selectionAnchorLineIndex = 0;
        selectionAnchorColumn = 0;
    }

    contentChangedFlag = true;
}

void TextViewerWidget::scrolledHorizontally(int value)
{
    horizontalScrollOffset = value;
    viewport()->update();
    header->setOffset(value);
}

void TextViewerWidget::scrolledVertically(int value)
{
    int diff = value - topLineIndex;

    // aesthetics
    if (diff > 0) {
        alignBottomLine();
    }
    else if (diff < 0) {
        alignTopLine();
    }

    topLineIndex = value;

    viewport()->update();
}

Pos::Pos(int line, int column) : line(line), column(column)
{
}

bool Pos::operator<(const Pos& other)
{
    if (line != other.line)
        return line < other.line;
    return column < other.column;
}

bool Pos::operator==(const Pos& other)
{
    return (line == other.line) && (column == other.column);
}

bool Pos::operator!=(const Pos& other)
{
    return (line != other.line) || (column != other.column);
}

TextViewerWidget::TabStop::TabStop(int at, const QColor& col)
    : atCharacter(at), color(col)
{
}

}  // namespace qtenv
}  // namespace omnetpp

