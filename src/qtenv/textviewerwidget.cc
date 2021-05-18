//==========================================================================
//  TEXTVIEWERWIDGET.CC - part of
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

#include "textviewerwidget.h"
#include "textviewerproviders.h"
#include "qtutil.h"
#include <QtGui/QPainter>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QApplication>
#include <QtWidgets/QToolBar>
#include <QtGui/QClipboard>
#include <QtWidgets/QAction>
#include <QtWidgets/QMessageBox>

namespace omnetpp {
namespace qtenv {

/*
// custom generated:
static std::vector<QColor> terminalPalette = {
    // dark palette, 30-37 for FG, 40-47 for BG
    "#000000", "#730909", "#097309", "#737309", "#090973", "#730973", "#097373", "#bfbfbf",
    // bright palette, 90-97 for FG, 100-107 for BG
    "#404040", "#f47272", "#72f472", "#f4f472", "#7272f4", "#f472f4", "#72f4f4", "#ffffff"
};
*/


// VGA:
static std::vector<QColor> terminalPalette = {
    // dark palette, 30-37 for FG, 40-47 for BG
    "#000000", "#AA0000", "#00AA00", "#AA5500", "#0000AA", "#AA00AA", "#00AAAA", "#AAAAAA",
    // bright palette, 90-97 for FG, 100-107 for BG
    "#555555", "#FF5555", "#55FF55", "#FFFF55", "#5555FF", "#FF55FF", "#55FFFF", "#FFFFFF"
};


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

    setFont(getMonospaceFont());

    headerModel = new QStandardItemModel(this);
    header = new QHeaderView(Qt::Horizontal);
    header->setMinimumSectionSize(3);
    header->setStretchLastSection(true);
    header->setDefaultAlignment(Qt::Alignment(Qt::AlignLeft | Qt::AlignVCenter));
    header->setModel(headerModel);
    connect(header, SIGNAL(sectionResized(int, int, int)), this, SLOT(onHeaderSectionResized(int, int, int)));

    // finally setting up the layout
    QGridLayout *layout = new QGridLayout(this);

    setLayout(layout);

    /*
     * The layout is set up in a grid like this:
     *
     *                      stretch          contentsMargin
     *                0        1       2    /              \
     *            +--------+-------+-------+
     * optional 0 | headerview (colspan 3) |       ver
     *            +--------+-------+-------+       tic
     *          1 | empty row, for spacing |       al
     *            +--------+-------+-------+       scr
     *  stretch 2 |spacing |toolbar|spacing|       oll
     *            +--------+-------+-------+       bar
     *          3 | empty row, for spacing |
     *            +--------+-------+-------+
     *                                      \
     *               horizontal scrollbar    contentsMargin
     *                                      /
     *
     * The viewport (where the scrolled content gets drawn)
     * is below this layout, but occupies the same space as
     * rows 1 through 3, via viewportMargin.
     * The layout's own spacing is set to zero.
     * The scrollbars are not children of the layout.
     * The toolbar is only added in here if this widget is
     * in an embedded LogInspector, otherwise the inspector
     * handles it itself, and we can keep that cell empty.
     * The toolbar is aligned on the top right corner of
     * the big stretching cell in the center, respecting
     * the spacings we added around it as grid rows/columns.
     */

    layout->addWidget(header, 0, 0, 1, 3);

    layout->setRowStretch(0, 0); // the header is here
    layout->setRowStretch(1, 0); // the spacing is fixed size
    layout->setRowStretch(2, 1); // the toolbar lives here
    layout->setRowStretch(3, 0); // the spacing is fixed size

    layout->setRowMinimumHeight(1, toolbarSpacing); // spacing above the toolbar (and below the header)
    layout->setRowMinimumHeight(3, toolbarSpacing); // spacing below the toolbar

    layout->setColumnStretch(0, 0); // the spacing is fixed size
    layout->setColumnStretch(1, 1); // the toolbar lives here
    layout->setColumnStretch(2, 0); // the spacing is fixed size

    layout->setColumnMinimumWidth(0, toolbarSpacing); // spacing left of the toolbar
    layout->setColumnMinimumWidth(2, toolbarSpacing); // spacing right of the toolbar

    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    viewport()->setCursor(Qt::IBeamCursor);
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
    // This way the underline is not obscured when the line below is selected: (and the 'g' letters look better)
    lineSpacing = metrics.lineSpacing() + metrics.underlinePos();
    averageCharWidth = metrics.averageCharWidth();

    isMonospaceFont = QFontInfo(font).fixedPitch();

    updateScrollbars();

    viewport()->update();
}

void TextViewerWidget::setToolBar(QToolBar *toolBar)
{
    this->toolBar = toolBar;
    if (toolBar) {
        static_cast<QGridLayout*>(layout())->addWidget(toolBar, 2, 1, Qt::Alignment(Qt::AlignRight | Qt::AlignTop));
        toolBar->setAutoFillBackground(false);
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
    if (contentChangedFlag)
        handleContentChange();
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
    if (contentChangedFlag)
        handleContentChange();
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

    offset = mapColumnToFormatted(content->getLineText(line).data(), offset);

    for (  /* nothing */; (line >= 0) && (line < content->getLineCount()); line += (backwards ? -1 : 1)) {
        int index = -1;

        QString lineText = content->getLineText(line);
        QString strippedText = stripFormatting(lineText);

        if (backwards) {
            index = re.lastIndexIn(strippedText, offset);
            offset = -1;  // was needed only for the first searched line
        }
        else {
            index = re.indexIn(strippedText, offset);
            offset = 0;  // was needed only for the first searched line
        }

        if (index >= 0) {
            int endIndex = index + re.matchedLength();

            index = mapColumnToUnformatted(lineText.data(), index);
            endIndex = mapColumnToUnformatted(lineText.data(), endIndex);

            setSelection(line, index, line, endIndex);
            found = true;  // yay!
            break;  // ouch.
        }
    }

    if (found) {
        int horizontalMargin = viewport()->width() / 5;
        int verticalMargin = viewport()->height() / 5;
        // 20% on the top and left, 40% on the bottom and right
        revealCaret(QMargins(horizontalMargin, verticalMargin, horizontalMargin * 2, verticalMargin * 2));
    }
    else {
        clearSelection();
        QMessageBox::information(this, "Not found", "No match for \"" + originalText + "\".");
    }

    viewport()->update();
}

int TextViewerWidget::getMaxVisibleLineWidth()
{
    auto vsb = verticalScrollBar();
    return getMaxVisibleLineWidth(vsb->value(), vsb->value() + viewport()->height());
}

int TextViewerWidget::getMaxVisibleLineWidth(int contentPixelBegin, int contentPixelEnd)
{
    ASSERT(contentPixelBegin <= contentPixelEnd);

    if (contentPixelBegin == contentPixelEnd)
        return 0; // the viewport is 0 pixels tall

    auto metrics = QFontMetrics(font, viewport());
    int firstLine = (int)std::floor(contentPixelBegin / (float)lineSpacing);
    int lastLine = (int)std::floor((contentPixelEnd-1) / (float)lineSpacing); // -1 is to make it inclusive

    firstLine = clip(0, content->getLineCount() - 1, firstLine);
    lastLine = clip(0, content->getLineCount() - 1, lastLine);

    ASSERT(lastLine >= firstLine);

    int maxWidth = 0;
    for (int lineIndex = firstLine; lineIndex <= lastLine; ++lineIndex) {
        const QString& line = content->getLineText(lineIndex);
        // left margin is included in getLineColumnOffset, adding the right one here
        maxWidth = std::max(maxWidth, getLineColumnOffset(metrics, lineIndex, line.length()) + horizontalMargin);
    }
    return maxWidth;
}

int TextViewerWidget::getNumVisibleLines(int height)
{
    // Counts partially visible lines as well.
    return (height + lineSpacing - 1) / lineSpacing;
}

int TextViewerWidget::getLineColumnOffset(const QFontMetrics& metrics, int lineIndex, int columnIndex)
{
    auto line = content->getLineText(lineIndex);

    const QChar *const textStart = line.unicode();
    const QChar *textPointer = line.unicode();

    int inColumn = 0;
    int x = horizontalMargin;

    if (columnIndex == 0)
        return x;

    while (*textPointer != 0) {
        textPointer = skipEscapeSequences(textPointer);
        if (*textPointer == '\t') {
            // this is a tab, so let's jump forward to the next header segment position
            ++inColumn;
            int sectionPosition = header->sectionPosition(inColumn); // TODO range check

            x = std::max(sectionPosition, x + metrics.width(" "));

            ++textPointer;
        }
        else {
            // this is regular text, see how many characters until we hit a
            // tab, an escape sequence, or the end of the string, and print
            // the text before that

            const QChar *start = textPointer;

            while (*textPointer != '\t' && *textPointer != '\x1b' && *textPointer != 0)
                ++textPointer;

            if ((textPointer - textStart) >= columnIndex) {
                QString text(start, columnIndex - (start - textStart));
                x += metrics.width(text);
                break;
            }

            QString text(start, textPointer - start);

            x += metrics.width(text);
        }
    }

    return x;
}

Pos TextViewerWidget::getLineColumnAt(int x, int y)
{
    int lineIndex = (verticalScrollOffset + y) / lineSpacing;
    lineIndex = clip(0, content->getLineCount()-1, lineIndex);
    return getColumnInLineAt(x, lineIndex);
}

Pos TextViewerWidget::getColumnInLineAt(int x, int lineIndex)
{
    if (contentChangedFlag)
        handleContentChange();

    auto line = content->getLineText(lineIndex);
    auto metrics = QFontMetrics(font, viewport());

    const QChar *const textStart = line.unicode();
    const QChar *textPointer = line.unicode();
    int curX = horizontalMargin - horizontalScrollOffset + metrics.averageCharWidth()/2;

    int inColumn = 0;
    int numVisibleChars = 0;

    Pos result(-1, -1);

    while (*textPointer != 0) {
        textPointer = skipEscapeSequences(textPointer);
        if (*textPointer == '\t') {
            // this is a tab, so let's jump forward to the next header segment position
            ++inColumn;
            int sectionPosition = header->sectionPosition(inColumn) - horizontalScrollOffset; // TODO range check

            int gap = std::max(sectionPosition - curX, metrics.width(" "));
            if (gap > 0)
                curX += gap;

            ++textPointer;

            ++numVisibleChars;
        }
        else {
            // this is regular text, see how many characters until we hit a
            // tab, an escape sequence, or the end of the string, and print
            // the text before that

            const QChar *start = textPointer;

            while (*textPointer != '\t' && *textPointer != '\x1b' && *textPointer != 0)
                ++textPointer;

            int len = textPointer - start;
            QString text(start, len);

            int width = metrics.width(text);

            if (curX + width >= x) {
                for (int i = 0; i <= len; ++i) {
                    int subWidth = metrics.width(text, i);
                    if (curX + subWidth >= x) {
                        result = Pos(lineIndex, start - textStart + i);
                        break;
                    }
                }
                if (result == Pos(-1, -1))
                    result = Pos(lineIndex, start - textStart + len);
                break;
            }

            curX += width;

            numVisibleChars += len;
        }
    }

    if (result == Pos(-1, -1))
        result = Pos(lineIndex, line.length());
    return result;
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
        for (int i = 0; i < headers.size(); ++i) {
            header->setSectionHidden(i, false);
        }
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
    int x = getLineColumnOffset(QFontMetrics(font, viewport()), caretLineIndex, caretColumn);

    caretLineIndex = std::max(0, caretLineIndex-1);
    if (caretLineIndex <= getTopLineIndex())
        followContentEnd = false;
    caretColumn = getColumnInLineAt(x - horizontalScrollOffset, caretLineIndex).column;

    if (!select)
        clearSelection();

    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::doLineDown(bool select)
{
    int x = getLineColumnOffset(QFontMetrics(font, viewport()), caretLineIndex, caretColumn);

    caretLineIndex = clip(0, content->getLineCount()-1, caretLineIndex+1);
    if (caretLineIndex == content->getLineCount() - 1)
        followContentEnd = true;
    caretColumn = getColumnInLineAt(x - horizontalScrollOffset, caretLineIndex).column;

    if (!select)
        clearSelection();

    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::doCursorPrevious(bool select)
{
    auto text = content->getLineText(caretLineIndex);

    caretColumn = std::min(caretColumn, text.length());
    if (caretColumn > 0)
         caretColumn = mapColumnToUnformatted(text.unicode(), mapColumnToFormatted(text.unicode(), caretColumn)-1);
    else if (caretLineIndex > 0) {
        caretLineIndex--;
        caretColumn = text.length();
    }
    if (!select)
        clearSelection();
    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::doCursorNext(bool select)
{
    auto text = content->getLineText(caretLineIndex);

    if (caretColumn < text.length())
         caretColumn = mapColumnToUnformatted(text.unicode(), mapColumnToFormatted(text.unicode(), caretColumn)+1);
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
    auto vsb = verticalScrollBar();
    vsb->setValue(vsb->value() - vsb->pageStep());
    followContentEnd = false;
    if (!select)
        clearSelection();
    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::doPageDown(bool select)
{
    int pageLines = std::max(1, getNumVisibleLines()-1);
    int lastLineIndex = content->getLineCount()-1;
    caretLineIndex = std::min(lastLineIndex, caretLineIndex + pageLines);
    auto vsb = verticalScrollBar();
    vsb->setValue(vsb->value() + vsb->pageStep());
    if (caretLineIndex == lastLineIndex)
        followContentEnd = true;
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
    int pos = mapColumnToFormatted(line.unicode(), caretColumn);
    if (pos == 0) {
        // go to end of previous line
        if (caretLineIndex > 0) {
            caretLineIndex--;
            caretColumn = content->getLineText(caretLineIndex).length();
        }
    }
    else {
        QString unformattedLine = stripFormatting(line);

        // go to start of current or previous word
        while (pos > 0 && (pos >= unformattedLine.length() || !isWordChar(unformattedLine.at(pos))))
            pos--;
        while (pos > 0 && isWordChar(unformattedLine.at(pos-1)))
            pos--;
        caretColumn = mapColumnToUnformatted(line.unicode(), pos);
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
        caretColumn = pos;
    }
    if (!select)
        clearSelection();
    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::doPageStart(bool select)
{
    caretLineIndex = getTopLineIndex();
    caretColumn = 0;
    if (!select)
        clearSelection();
    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::doPageEnd(bool select)
{
    caretLineIndex = std::min(content->getLineCount()-1, getTopLineIndex() + getNumVisibleLines());
    caretColumn = content->getLineText(caretLineIndex).length();
    if (!select)
        clearSelection();
    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::doContentStart(bool select)
{
    caretLineIndex = 0;
    caretColumn = 0;
    followContentEnd = false;
    if (!select)
        clearSelection();
    emit caretMoved(caretLineIndex, caretColumn);
}

void TextViewerWidget::doContentEnd(bool select)
{
    caretLineIndex = content->getLineCount()-1;
    caretColumn = content->getLineText(caretLineIndex).length();
    followContentEnd = true;
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

QString TextViewerWidget::getSelectedText()
{
    QString text;

    Pos start = getSelectionStart();
    Pos end = getSelectionEnd();

    if (start.line == end.line) {
        text = content->getLineText(start.line).mid(start.column, end.column - start.column);
    }
    else {
        text = content->getLineText(start.line).mid(start.column).trimmed() + "\n";
        for (int l = start.line + 1; l < end.line; ++l)
            text += content->getLineText(l).trimmed() + "\n";
        text += content->getLineText(end.line).left(end.column).trimmed();
    }

    return text;
}

QString TextViewerWidget::getSelectedTextUnformatted()
{
    QString text;

    Pos start = getSelectionStart();
    Pos end = getSelectionEnd();

    if (start.line == end.line) {
        text = stripFormatting(content->getLineText(start.line).mid(start.column, end.column - start.column));
    }
    else {
        text = stripFormatting(content->getLineText(start.line).mid(start.column).trimmed()) + "\n";
        for (int l = start.line + 1; l < end.line; ++l)
            text += stripFormatting(content->getLineText(l).trimmed()) + "\n";
        text += stripFormatting(content->getLineText(end.line).left(end.column).trimmed());
    }

    return text;
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

int TextViewerWidget::mapColumnToFormatted(const QChar *textPointer, int unformattedColumn)
{
    if (unformattedColumn == 0)
        return 0;

    const QChar *const textStart = textPointer;

    int formattedColumn = 0;

    while (*textPointer != 0) {
        // '\t' is not a special case here

        textPointer = skipEscapeSequences(textPointer);

        // this is regular text, see how many characters until we hit an
        // escape sequence, or the end of the string

        while (*textPointer != '\x1b' && *textPointer != 0) {
            if ((textPointer - textStart) >= unformattedColumn)
                return formattedColumn;
            ++textPointer;
            ++formattedColumn;
        }
    }

    return formattedColumn;
}

int TextViewerWidget::mapColumnToUnformatted(const QChar *textPointer, int formattedColumn)
{
    if (formattedColumn == 0)
        return 0;

    const QChar *const textStart = textPointer;

    int formattedChars = 0;

    while (*textPointer != 0) {
        // '\t' is not a special case here

        textPointer = skipEscapeSequences(textPointer);

        // this is regular text, see how many characters until we hit an
        // escape sequence, or the end of the string

        while (*textPointer != '\x1b' && *textPointer != 0) {
            if (formattedChars >= formattedColumn)
                return textPointer - textStart;
            ++textPointer;
            ++formattedChars;
        }
    }

    return textPointer - textStart;
}

void TextViewerWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateScrollbars();
    viewport()->update();
}

void TextViewerWidget::paintEvent(QPaintEvent *event)
{
    if (contentChangedFlag)
        handleContentChange();

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

        // this has to be after the font has been updated, because then the lineSpacing changes, and we need the actual value here
        updateScrollbars();
    }

    int numLines = content->getLineCount();
    int numVisibleLines = getNumVisibleLines();

    // note: if the following asserts are triggered, odds are that content has changed without a call to our contentChanged() method
    Q_ASSERT(getTopLineIndex() >= 0 && getTopLineIndex() <= std::max(0, numLines-numVisibleLines));
    Q_ASSERT(numLines == 0 || (caretLineIndex >= 0 && caretLineIndex < numLines));

    int x = horizontalMargin - horizontalScrollOffset;
    int lineIndex = getTopLineIndex();
    int startY = getTopLineY();

    // Drawing the gray highlight under the line in which the caret is-
    // Have to do this before any text painting, otherwise
    // it might paint over some parts of a few letters.
    int highlightY = startY + (caretLineIndex-lineIndex) * lineSpacing;
    painter.setPen(Qt::NoPen);
    painter.setBrush(palette().color(QPalette::AlternateBase));
    painter.drawRect(0, highlightY, contentsRect().width(), lineSpacing);

    Pos selectionStart = getSelectionStart();
    Pos selectionEnd = getSelectionEnd();

    // draw the lines
    for (int y = startY; y < r.y() + r.height() && lineIndex < numLines; ++lineIndex, y += lineSpacing) {
        if (selectionStart.line > lineIndex || selectionEnd.line < lineIndex || selectionStart == selectionEnd)
            // this line is entirely unselected (the last term means that there is no selection at all)
            drawLine(painter, lineIndex, x, y, false);
        else if (selectionStart.line < lineIndex && selectionEnd.line > lineIndex)
            // this line is entirely selected
            drawLine(painter, lineIndex, x, y, true);
        else {
            // this line is partially selected; either just a part of it is selected,
            // or this is the first or last, partially selected line of a multi-line selection

            int left = 0;
            int right = viewport()->width() + horizontalScrollOffset;

            // TODO we could avoid these two getLineColumnOffset calls if we
            // returned the two positions from the drawLine method, where we
            // have to process the whole line anyway
            if (selectionStart.line == lineIndex)
                left = getLineColumnOffset(painter.fontMetrics(), lineIndex, selectionStart.column);

            if (selectionEnd.line == lineIndex)
                right = getLineColumnOffset(painter.fontMetrics(), lineIndex, selectionEnd.column);

            drawLine(painter, lineIndex, x, y, false);

            painter.save();
            auto metrics = painter.fontMetrics();

            painter.setClipRect(left - horizontalScrollOffset, y, right - left, lineSpacing);
            drawLine(painter, lineIndex, x, y, true);
            painter.restore();
        }
    }
}

static int readInt(const QChar *&textPointer)
{
    int result = 0;
    while (*textPointer >= '0' && *textPointer <= '9') {
        result = result * 10 + (textPointer->unicode() - '0');
        ++textPointer;
    }
    if (*textPointer == ';')
        ++textPointer;

    return result;
}

static void readColor(const QChar *&textPointer, QColor& color)
{
    int format = readInt(textPointer);

    if (format == 2) {
        int r = readInt(textPointer);
        int g = readInt(textPointer);
        int b = readInt(textPointer);

        color.setRgb(r, g, b);
    }
    else if (format == 5) {
        int n = readInt(textPointer);
        if (n < 16)
            // the 16 basic (normal and bright) terminal colors
            color = terminalPalette[n];
        else if (n < 232) {
            // A 6x6x6 RGB cube: 16 + 36 * r + 6 * g + b
            n -= 16;

            int r = n / 36;
            int g = (n / 6) % 6;
            int b = n % 6;

            // I don't know why, but this is used by xterm, on Wikipedia, etc...
            // instead of a simple linear scale. To be honest, it looks better too.
            if (r > 0) r = 95 + (r-1)*40;
            if (g > 0) g = 95 + (g-1)*40;
            if (b > 0) b = 95 + (b-1)*40;

            color.setRgb(r, g, b);
        }
        else {
            // 24 steps of grayscale from 232 to 255
            n -= 232;
            color.setRgbF(n / 23.0, n / 23.0, n / 23.0);
        }
    }
}

static void performSgrControlSequence(const QChar *&textPointer, const QFont &defaultFont, QColor &fgColor, QColor &bgColor, QFont &font, bool &faint)
{
    // "\x1b[m"
    // "\x1b[34m"
    // "\x1b[34;92m"
    // "\x1b[34;;92m"
    // "\x1b[34;92;0;;34;41;;;m"

    ASSERT(*textPointer == '\x1b');

    ++textPointer;

    if (*textPointer != '[')
        return; // invalid format, terminate processing

    ++textPointer;

    while (*textPointer != 0 && *textPointer != 'm') {

        if (!(*textPointer == ';' || *textPointer == 'm' || (*textPointer >= '0' && *textPointer <= '9')))
            break; // invalid sequence, act as if it ended here

        int action = readInt(textPointer);

        if (action >= 30 && action <= 37)
            fgColor = terminalPalette[action-30];
        else if (action == 38)
            readColor(textPointer, fgColor); // 5;n or 2;r;g;b
        else if (action == 39)
            fgColor = QColor();
        else if (action >= 40 && action <= 47)
            bgColor = terminalPalette[action-40];
        else if (action == 48)
            readColor(textPointer, bgColor); // 5;n or 2;r;g;b
        else if (action == 49)
            bgColor = QColor();
        else if (action >= 90 && action <= 97)
            fgColor = terminalPalette[action-90 + 8];
        else if (action >= 100 && action <= 107)
            bgColor = terminalPalette[action-100 + 8];
        else if (action == 0) {
            // reset
            bgColor = QColor();
            fgColor = QColor();
            font = defaultFont;
            faint = false;
        }
        else if (action == 1)
            // Here 1 only sets "bold", but not "high intensity" (or "light color")
            // because there is an other way to set "light color", with the
            // codes 90..97 and 100..107, but there is no other way to set "bold".
            // And we prefer to keep bold and light independent.
            font.setBold(true);
        else if (action == 2)
            // Here 2 only sets "faint", but not "low intensity" or "thin"
            faint = true;
        else if (action == 3)
            font.setItalic(true);
        else if (action == 4)
            font.setUnderline(true);
        else if (action == 22) { // "regular" - not bold, not faint
            // The "bold off" code would be more consistent if it was 21,
            // but xterm interprets it as underlined (supposed to be double
            // underline), but 22 works as bold off there as well.
            // To remain a bit more compatible, we follow xterm.
            font.setBold(false);
            faint = false;
        }
        else if (action == 23)
            font.setItalic(false);
        else if (action == 24)
            font.setUnderline(false);
    }

    if (*textPointer == 'm')
        ++textPointer;
}

int TextViewerWidget::paintText(const QString& text, QPainter& painter, const QFontMetrics& metrics, int x, int y, const QColor& fgColor, const QColor& bgColor, const QFont& font)
{
    painter.setFont(font);

    int width = metrics.width(text);

    // background if needed
    if (bgColor.isValid()) {
        QRect bgRect(x+1, y, width, lineSpacing);
        painter.fillRect(bgRect, bgColor);
    }

    // text
    painter.setPen(fgColor);
    painter.drawText(x, y + metrics.ascent(), text);

    return width;
}

void TextViewerWidget::drawLine(QPainter& painter, int lineIndex, int x, int y, bool asSelected)
{
    // draw the line in the specified color
    QFont curFont = font;

    painter.setFont(curFont);
    QFontMetrics metrics = painter.fontMetrics();

    QString lineText = content->getLineText(lineIndex);

    const QChar *textPointer = lineText.unicode();

    int inColumn = 0;

    QColor curBgColor = QColor(), curFgColor = foregroundColor;
    // This is the "opposite of bold" - although they can be enabled at the same time.
    // Not using the alpha component of fgColor because color and faintness are supposed to be independent.
    bool faint = false;

    // TODO: the data returned by unicode() is not necessarily 0-terminated!
    while (*textPointer != 0) {
        if (*textPointer == '\n') {
            break;
        }
        else if (*textPointer == '\t') {

            // this is a tab, so let's jump forward to the next header segment position
            ++inColumn;
            int sectionPosition = header->sectionPosition(inColumn) - horizontalScrollOffset; // TODO range check

            int width = std::max(sectionPosition - x, metrics.width(" ")); // treat the tab as at least a space wide

            // we might already be past the section position
            if (width > 0) {
                QRect bgRect(x+1, y, width, lineSpacing);

                // fill background if needed
                if (asSelected)
                    painter.fillRect(bgRect, selectionBackgroundColor);
                else if (curBgColor.isValid())
                    painter.fillRect(bgRect, curBgColor);
                // else: not painting

                x += width;
            }

            ++textPointer;
        }
        else if (*textPointer == '\x1b') {
            // this is an escape sequence, handled separately
            performSgrControlSequence(textPointer, font, curFgColor, curBgColor, curFont, faint);

            if (!curFgColor.isValid())
                curFgColor = foregroundColor;
        }
        else {
            // this is regular text, see how many characters until we hit a
            // tab, an escape sequence, or the end of the string, and print
            // the text before that

            const QChar *start = textPointer;

            while (*textPointer != '\t' && *textPointer != '\x1b' && *textPointer != 0)
                ++textPointer;

            // we ignore any coloring here, but keep the font as in the formatting
            if (asSelected) {
                curBgColor = selectionBackgroundColor;
                curFgColor = selectionForegroundColor;
            }

            QString text(start, textPointer - start);

            QColor actualTextColor = curFgColor;
            actualTextColor.setAlpha(faint ? 128 : 255);
            int width = paintText(text, painter, metrics, x, y, actualTextColor, curBgColor, curFont);

            x += width;
        }
    }

    if (lineIndex == caretLineIndex && caretShown && hasFocus()) {
        // draw caret
        painter.save();
        painter.setPen(Qt::white);
        int caretX = getLineColumnOffset(metrics, lineIndex, caretColumn) - horizontalScrollOffset;
        painter.setCompositionMode(QPainter::CompositionMode_Exclusion);
        painter.drawLine(caretX, y, caretX, y + lineSpacing-1);
        painter.restore();
    }
}

void TextViewerWidget::keyPressEvent(QKeyEvent *event)
{
    if (contentChangedFlag)
        handleContentChange();

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
        default:
            return;
    }

    if (shiftPressed)
        QApplication::clipboard()->setText(getSelectedTextUnformatted(), QClipboard::Selection);

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

    if (getSelectionStart() != getSelectionEnd())
        QApplication::clipboard()->setText(getSelectedTextUnformatted(), QClipboard::Selection);
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

void TextViewerWidget::updateScrollbars()
{
    // This function should be completely idempotent (with pixel accuracy)
    // to avoid any stack overflows / infinite event floods / flickering.
    // So with any combination of followContentEnd's value, both scrollbars
    // being visible or not, etc, invoking this function more than once
    // with no other state changing should give the exact same result.
    // Note that it can happen that we are showing a scrollbar that is
    // not scrollable at all, but this can not be avoided, deal with it.

    // Counts how many times this function is on the stack.
    // Used as a last resort safeguard to break any loops that
    // might still happen, despite all our best intentions.
    static int callDepth = 0;

    if (!content)
        return;

    if (callDepth >= 10) {
        qDebug() << "Too deep recursion while updating scrollbars! Giving up...";
        return;
    }

    ++callDepth;

    auto vsb = verticalScrollBar();
    auto hsb = horizontalScrollBar();

    // We could display this many pixels of the content if both scrollbars were hidden.
    // Must not use contentsMargins! It changes in an unreliable manner with some styles
    // and Qt versions. And maximumViewportSize seems to be affected by the scrollbarPolicies.
    QSize maxSize = size()
         - QSize(viewportMargins().left() + viewportMargins().right(),
                 viewportMargins().top() + viewportMargins().bottom());

    // avoiding an invalid maxSize in case size() is still zero
    if (maxSize.width() < 0)
        maxSize.setWidth(0);
    if (maxSize.height() < 0)
        maxSize.setHeight(0);

    // first determine if we need each scrollbar separately, being conservative about it
    // (better show unnecessarily than not show when it would be needed)

    // Showing the vertical scrollbar if the height of the content is greater than the height
    // of the viewport with visible horizontal scrollbar (even if that one is not really visible right now).
    bool vertNeeded = (content->getLineCount() * lineSpacing) > (maxSize.height() - hsb->height());

    // If we follow the content end ("scroll lock is off"), then the disappearing horizontal scrollbar will
    // move the top content pixel up, not the bottom pixel down. Th
    int topContentPixel = std::max(0, std::min(verticalScrollOffset, (content->getLineCount() * lineSpacing) - maxSize.height()));

    // Showing the horizontal scrollbar if the longest visible line is longer (wider) than the width
    // of the viewport with visible vertical scrollbar (even if that one is not really visible right now).
    // The lines used for width calculation are the ones that have any chance of becoming visible
    // after we are done.
    int maxLineWidth = getMaxVisibleLineWidth(topContentPixel, topContentPixel + maxSize.height());
    bool horizNeeded = maxLineWidth >= (maxSize.width() - vsb->width());

    // show or hide as computed before
    setVerticalScrollBarPolicy(vertNeeded ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(horizNeeded ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAlwaysOff);

    if (!vertNeeded) verticalScrollOffset = 0;
    if (!horizNeeded) horizontalScrollOffset = 0;

    // configure
    vsb->setMinimum(0);
    vsb->setMaximum(std::max(0, content->getLineCount() * lineSpacing - viewport()->height()));
    vsb->setSingleStep(lineSpacing);
    vsb->setPageStep(viewport()->height());

    // following output growth
    if (followContentEnd && vsb->maximum() > vsb->minimum())
        vsb->setValue(vsb->maximum());

    // the horizontal depends on the vertical more
    hsb->setMinimum(0);
    // not reusing the maxLineWidth variable, computing it again, with the updated vertical scrollbar value
    hsb->setMaximum(std::max(0, getMaxVisibleLineWidth() - viewport()->width()));
    hsb->setSingleStep(averageCharWidth);
    hsb->setPageStep(viewport()->width());

    // Adjusting the layout to make all child widgets (the headerview and the toolbar) clear of the toolbar.
    // Note that some style engines on some Qt versions will (wrongly, I believe) adjust the widget's
    // right and bottom contentsMargins based on whether the scrollbars are visible or not.
    // In those cases, they will be added on top of this, and the widgets will be farther in to the left
    // than they should otherwise be, but oh well, not that much of a tragedy...
    if (layout())
        layout()->setContentsMargins(0, 0, (vertNeeded ? vsb->width() : 0), (horizNeeded ? hsb->height() : 0));

    --callDepth;
}

void TextViewerWidget::handleContentChange()
{
    ASSERT2(content->getLineCount() > 0, "content must be at least one line");

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

    auto headers = content->getHeaders();
    headerModel->setHorizontalHeaderLabels(headers);

    contentChangedFlag = false;
}

void TextViewerWidget::revealCaret(const QMargins& margins)
{
    int topPixel = caretLineIndex * lineSpacing;
    int bottomPixel = topPixel + lineSpacing - 1;

    auto vsb = verticalScrollBar();

    int val = vsb->value();

    int minVal = bottomPixel + margins.bottom() - viewport()->height();
    if (val < minVal)
        val = minVal; // have to scroll down

    int maxVal = topPixel - margins.top();
    if (val > maxVal)
        val = maxVal; // have to scroll up

    vsb->setValue(val);


    int caretX = getLineColumnOffset(QFontMetrics(font, viewport()), caretLineIndex, caretColumn);

    auto hsb = horizontalScrollBar();

    val = hsb->value();

    minVal = caretX + margins.right() - viewport()->width() + 1;
    if (val < minVal)
        val = minVal; // have to scroll to the right

    maxVal = caretX - margins.left();
    if (val > maxVal)
        val = maxVal; // have to scroll to the left

    hsb->setValue(val);
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
    updateScrollbars();
    viewport()->update();
}

void TextViewerWidget::copySelection()
{
    QApplication::clipboard()->setText(getSelectedText());
}
void TextViewerWidget::copySelectionUnformatted()
{
    QApplication::clipboard()->setText(getSelectedTextUnformatted());
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
    updateScrollbars();
    viewport()->update();
    header->setOffset(value);
}

void TextViewerWidget::scrolledVertically(int value)
{
    if (value < verticalScrollOffset)
        followContentEnd = false;
    if (value == verticalScrollBar()->maximum())
        followContentEnd = true;
    verticalScrollOffset = value;
    updateScrollbars();
    viewport()->update();
}

Pos::Pos(int line, int column) : line(line), column(column)
{
}

bool Pos::operator<(const Pos& other)
{
    return line != other.line
            ? line < other.line
            : column < other.column;
}

bool Pos::operator==(const Pos& other)
{
    return (line == other.line) && (column == other.column);
}

bool Pos::operator!=(const Pos& other)
{
    return (line != other.line) || (column != other.column);
}

}  // namespace qtenv
}  // namespace omnetpp
