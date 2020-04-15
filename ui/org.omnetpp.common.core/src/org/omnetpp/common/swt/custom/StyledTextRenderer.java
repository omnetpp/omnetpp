/*******************************************************************************
 * Copyright (c) 2000, 2010 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.omnetpp.common.swt.custom;


import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ST;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Device;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.FontMetrics;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.GlyphMetrics;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.graphics.TextLayout;
import org.eclipse.swt.graphics.TextStyle;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.IME;
import org.eclipse.swt.widgets.ScrollBar;

/**
 * A StyledTextRenderer renders the content of a StyledText widget.
 * This class can be used to render to the display or to a printer.
 */
class StyledTextRenderer {
    Device device;
    StyledText styledText;
    StyledTextContent content;

    /* Font info */
    Font regularFont, boldFont, italicFont, boldItalicFont;
    int tabWidth;
    int ascent, descent;
    int averageCharWidth;

    /* Line data */
    int topIndex = -1;
    TextLayout[] layouts;
    int lineCount;
    int[] lineWidth;
    int[] lineHeight;
    LineInfo[] lines;
    int maxWidth;
    int maxWidthLineIndex;
    boolean idleRunning;

    /* Bullet */
    Bullet[] bullets;
    int[] bulletsIndices;
    int[] redrawLines;

    /* Style data */
    int[] ranges;
    int styleCount;
    StyleRange[] styles;
    StyleRange[] stylesSet;
    int stylesSetCount = 0;
    boolean hasLinks, fixedPitch;
    final static int BULLET_MARGIN = 8;

    final static boolean COMPACT_STYLES = true;
    final static boolean MERGE_STYLES = true;

    final static int GROW = 32;
    final static int IDLE_TIME = 50;
    final static int CACHE_SIZE = 128;

    final static int BACKGROUND = 1 << 0;
    final static int ALIGNMENT = 1 << 1;
    final static int INDENT = 1 << 2;
    final static int JUSTIFY = 1 << 3;
    final static int SEGMENTS = 1 << 5;
    final static int TABSTOPS = 1 << 6;
    final static int WRAP_INDENT = 1 << 7;
    final static int SEGMENT_CHARS = 1 << 8;

    static class LineInfo {
        int flags;
        Color background;
        int alignment;
        int indent;
        int wrapIndent;
        boolean justify;
        int[] segments;
        char[] segmentsChars;
        int[] tabStops;

        public LineInfo() {
        }
        public LineInfo(LineInfo info) {
            if (info != null) {
                flags = info.flags;
                background = info.background;
                alignment = info.alignment;
                indent = info.indent;
                wrapIndent = info.wrapIndent;
                justify = info.justify;
                segments = info.segments;
                segmentsChars = info.segmentsChars;
                tabStops = info.tabStops;
            }
        }
    }

StyledTextRenderer(Device device, StyledText styledText) {
    this.device = device;
    this.styledText = styledText;
}
int addMerge(int[] mergeRanges, StyleRange[] mergeStyles, int mergeCount, int modifyStart, int modifyEnd) {
    int rangeCount = styleCount << 1;
    StyleRange endStyle = null;
    int endStart = 0, endLength = 0;
    if (modifyEnd < rangeCount) {
        endStyle = styles[modifyEnd >> 1];
        endStart = ranges[modifyEnd];
        endLength = ranges[modifyEnd + 1];
    }
    int grow = mergeCount - (modifyEnd - modifyStart);
    if (rangeCount + grow >= ranges.length) {
        int[] tmpRanges = new int[ranges.length + grow + (GROW << 1)];
        System.arraycopy(ranges, 0, tmpRanges, 0, modifyStart);
        StyleRange[] tmpStyles = new StyleRange[styles.length + (grow >> 1) + GROW];
        System.arraycopy(styles, 0, tmpStyles, 0, modifyStart >> 1);
        if (rangeCount > modifyEnd) {
            System.arraycopy(ranges, modifyEnd, tmpRanges, modifyStart + mergeCount, rangeCount - modifyEnd);
            System.arraycopy(styles, modifyEnd >> 1, tmpStyles, (modifyStart + mergeCount) >> 1, styleCount - (modifyEnd >> 1));
        }
        ranges = tmpRanges;
        styles = tmpStyles;
    } else {
        if (rangeCount > modifyEnd) {
            System.arraycopy(ranges, modifyEnd, ranges, modifyStart + mergeCount, rangeCount - modifyEnd);
            System.arraycopy(styles, modifyEnd >> 1, styles, (modifyStart + mergeCount) >> 1, styleCount - (modifyEnd >> 1));
        }
    }
    if (MERGE_STYLES) {
        int j = modifyStart;
        for (int i = 0; i < mergeCount; i += 2) {
            if (j > 0 && ranges[j - 2] + ranges[j - 1] == mergeRanges[i] && mergeStyles[i >> 1].similarTo(styles[(j - 2) >> 1])) {
                ranges[j - 1] += mergeRanges[i + 1];
            } else {
                styles[j >> 1] = mergeStyles[i >> 1];
                ranges[j++] = mergeRanges[i];
                ranges[j++] = mergeRanges[i + 1];
            }
        }
        if (endStyle != null && ranges[j - 2] + ranges[j - 1] == endStart && endStyle.similarTo(styles[(j - 2) >> 1])) {
            ranges[j - 1] += endLength;
            modifyEnd += 2;
            mergeCount += 2;
        }
        if (rangeCount > modifyEnd) {
            System.arraycopy(ranges, modifyStart + mergeCount, ranges, j, rangeCount - modifyEnd);
            System.arraycopy(styles, (modifyStart + mergeCount) >> 1, styles, j >> 1, styleCount - (modifyEnd >> 1));
        }
        grow = (j - modifyStart) - (modifyEnd - modifyStart);
    } else {
        System.arraycopy(mergeRanges, 0, ranges, modifyStart, mergeCount);
        System.arraycopy(mergeStyles, 0, styles, modifyStart >> 1, mergeCount >> 1);
    }
    styleCount += grow >> 1;
    return grow;
}
int addMerge(StyleRange[] mergeStyles, int mergeCount, int modifyStart, int modifyEnd) {
    int grow = mergeCount - (modifyEnd - modifyStart);
    StyleRange endStyle = null;
    if (modifyEnd < styleCount) endStyle = styles[modifyEnd];
    if (styleCount + grow >= styles.length) {
        StyleRange[] tmpStyles = new StyleRange[styles.length + grow + GROW];
        System.arraycopy(styles, 0, tmpStyles, 0, modifyStart);
        if (styleCount > modifyEnd) {
            System.arraycopy(styles, modifyEnd, tmpStyles, modifyStart + mergeCount, styleCount - modifyEnd);
        }
        styles = tmpStyles;
    } else {
        if (styleCount > modifyEnd) {
            System.arraycopy(styles, modifyEnd, styles, modifyStart + mergeCount, styleCount - modifyEnd);
        }
    }
    if (MERGE_STYLES) {
        int j = modifyStart;
        for (int i = 0; i < mergeCount; i++) {
            StyleRange newStyle = mergeStyles[i], style;
            if (j > 0 && (style = styles[j - 1]).start + style.length == newStyle.start && newStyle.similarTo(style)) {
                style.length += newStyle.length;
            } else {
                styles[j++] = newStyle;
            }
        }
        StyleRange style = styles[j - 1];
        if (endStyle != null && style.start + style.length == endStyle.start && endStyle.similarTo(style)) {
            style.length += endStyle.length;
            modifyEnd++;
            mergeCount++;
        }
        if (styleCount > modifyEnd) {
            System.arraycopy(styles, modifyStart + mergeCount, styles, j, styleCount - modifyEnd);
        }
        grow = (j - modifyStart) - (modifyEnd - modifyStart);
    } else {
        System.arraycopy(mergeStyles, 0, styles, modifyStart, mergeCount);
    }
    styleCount += grow;
    return grow;
}
void calculate(int startLine, int lineCount) {
    int endLine = startLine + lineCount;
    if (startLine < 0 || endLine > lineWidth.length) {
        return;
    }
    int hTrim = styledText.leftMargin + styledText.rightMargin + styledText.getCaretWidth();
    for (int i = startLine; i < endLine; i++) {
        if (lineWidth[i] == -1 || lineHeight[i] == -1) {
            TextLayout layout = getTextLayout(i);
            Rectangle rect = layout.getBounds();
            lineWidth[i] = rect.width + hTrim;
            lineHeight[i] = rect.height;
            disposeTextLayout(layout);
        }
        if (lineWidth[i] > maxWidth) {
            maxWidth = lineWidth[i];
            maxWidthLineIndex = i;
        }
    }
}
void calculateClientArea () {
    int index = styledText.getTopIndex();
    int lineCount = content.getLineCount();
    int height = styledText.getClientArea().height;
    int y = 0;
    while (height > y && lineCount > index) {
        calculate(index, 1);
        y += lineHeight[index++];
    }
}
void calculateIdle () {
    if (idleRunning) return;
    Runnable runnable = new Runnable() {
        public void run() {
            if (styledText == null) return;
            int i;
            long start = System.currentTimeMillis();
            for (i = 0; i < lineCount; i++) {
                if (lineHeight[i] == -1 || lineWidth[i] == -1) {
                    calculate(i, 1);
                    if (System.currentTimeMillis() - start > IDLE_TIME) break;
                }
            }
            if (i < lineCount) {
                Display display = styledText.getDisplay();
                display.asyncExec(this);
            } else {
                idleRunning = false;
                styledText.setScrollBars(true);
                ScrollBar bar = styledText.getVerticalBar();
                if (bar != null) {
                    bar.setSelection(styledText.getVerticalScrollOffset());
                }
            }
        }
    };
    Display display = styledText.getDisplay();
    display.asyncExec(runnable);
    idleRunning = true;
}
void clearLineBackground(int startLine, int count) {
    if (lines == null) return;
    for (int i = startLine; i < startLine + count; i++) {
        LineInfo info = lines[i];
        if (info != null) {
            info.flags &= ~BACKGROUND;
            info.background = null;
            if (info.flags == 0) lines[i] = null;
        }
    }
}
void clearLineStyle(int startLine, int count) {
    if (lines == null) return;
    for (int i = startLine; i < startLine + count; i++) {
        LineInfo info = lines[i];
        if (info != null) {
            info.flags &= ~(ALIGNMENT | INDENT | WRAP_INDENT | JUSTIFY | TABSTOPS);
            if (info.flags == 0) lines[i] = null;
        }
    }
}
void copyInto(StyledTextRenderer renderer) {
    if (ranges != null) {
        int[] newRanges = renderer.ranges = new int[styleCount << 1];
        System.arraycopy(ranges, 0, newRanges, 0, newRanges.length);
    }
    if (styles != null) {
        StyleRange[] newStyles = renderer.styles = new StyleRange[styleCount];
        for (int i = 0; i < newStyles.length; i++) {
            newStyles[i] = (StyleRange)styles[i].clone();
        }
        renderer.styleCount = styleCount;
    }
    if (lines != null) {
        LineInfo[] newLines = renderer.lines = new LineInfo[lineCount];
        for (int i = 0; i < newLines.length; i++) {
            newLines[i] = new LineInfo(lines[i]);
        }
        renderer.lineCount = lineCount;
    }
}
void dispose() {
    if (boldFont != null) boldFont.dispose();
    if (italicFont != null) italicFont.dispose();
    if (boldItalicFont != null) boldItalicFont.dispose();
    boldFont = italicFont = boldItalicFont = null;
    reset();
    content = null;
    device = null;
    styledText = null;
}
void disposeTextLayout (TextLayout layout) {
    if (layouts != null) {
        for (int i = 0; i < layouts.length; i++) {
            if (layouts[i] == layout) return;
        }
    }
    layout.dispose();
}
void drawBullet(Bullet bullet, GC gc, int paintX, int paintY, int index, int lineAscent, int lineDescent) {
    StyleRange style = bullet.style;
    GlyphMetrics metrics = style.metrics;
    Color color = style.foreground;
    if (color != null) gc.setForeground(color);
    if ((bullet.type & ST.BULLET_DOT) != 0 && StyledText.IS_MOTIF) {
        int size = Math.max(4, (lineAscent + lineDescent) / 4);
        if ((size & 1) == 0) size++;
        if (color == null) {
            Display display = styledText.getDisplay();
            color = display.getSystemColor(SWT.COLOR_BLACK);
        }
        gc.setBackground(color);
        int x = paintX + Math.max(0, metrics.width - size - BULLET_MARGIN);
        gc.fillArc(x, paintY + size, size + 1, size + 1, 0, 360);
        return;
    }
    Font font = style.font;
    if (font != null) gc.setFont(font);
    String string = "";
    int type = bullet.type & (ST.BULLET_DOT|ST.BULLET_NUMBER|ST.BULLET_LETTER_LOWER|ST.BULLET_LETTER_UPPER);
    switch (type) {
        case ST.BULLET_DOT: string = "\u2022"; break;
        case ST.BULLET_NUMBER: string = String.valueOf(index + 1); break;
        case ST.BULLET_LETTER_LOWER: string = String.valueOf((char) (index % 26 + 97)); break;
        case ST.BULLET_LETTER_UPPER: string = String.valueOf((char) (index % 26 + 65)); break;
    }
    if ((bullet.type & ST.BULLET_TEXT) != 0) string += bullet.text;
    Display display = styledText.getDisplay();
    TextLayout layout = new TextLayout(display);
    layout.setText(string);
    layout.setAscent(lineAscent);
    layout.setDescent(lineDescent);
    style = (StyleRange)style.clone();
    style.metrics = null;
    if (style.font == null) style.font = getFont(style.fontStyle);
    layout.setStyle(style, 0, string.length());
    int x = paintX + Math.max(0, metrics.width - layout.getBounds().width - BULLET_MARGIN);
    layout.draw(gc, x, paintY);
    layout.dispose();
}
int drawLine(int lineIndex, int paintX, int paintY, GC gc, Color widgetBackground, Color widgetForeground) {
    TextLayout layout = getTextLayout(lineIndex);
    String line = content.getLine(lineIndex);
    int lineOffset = content.getOffsetAtLine(lineIndex);
    int lineLength = line.length();
    Point selection = styledText.getSelection();
    int selectionStart = selection.x - lineOffset;
    int selectionEnd = selection.y - lineOffset;
    if (styledText.getBlockSelection()) {
        selectionStart = selectionEnd = 0;
    }
    Rectangle client = styledText.getClientArea();
    Color lineBackground = getLineBackground(lineIndex, null);
    StyledTextEvent event = styledText.getLineBackgroundData(lineOffset, line);
    if (event != null && event.lineBackground != null) lineBackground = event.lineBackground;
    int height = layout.getBounds().height;
    if (lineBackground != null) {
        gc.setBackground(lineBackground);
        gc.fillRectangle(client.x, paintY, client.width, height);
    } else {
        gc.setBackground(widgetBackground);
        styledText.drawBackground(gc, client.x, paintY, client.width, height);
    }
    gc.setForeground(widgetForeground);
    if (selectionStart == selectionEnd || (selectionEnd <= 0 && selectionStart > lineLength - 1)) {
        layout.draw(gc, paintX, paintY);
    } else {
        int start = Math.max(0, selectionStart);
        int end = Math.min(lineLength, selectionEnd);
        Color selectionFg = styledText.getSelectionForeground();
        Color selectionBg = styledText.getSelectionBackground();
        int flags;
        if ((styledText.getStyle() & SWT.FULL_SELECTION) != 0) {
            flags = SWT.FULL_SELECTION;
        } else {
            flags = SWT.DELIMITER_SELECTION;
        }
        if (selectionStart <= lineLength && lineLength < selectionEnd ) {
            flags |= SWT.LAST_LINE_SELECTION;
        }
        layout.draw(gc, paintX, paintY, start, end - 1, selectionFg, selectionBg, flags);
    }

    // draw objects
    Bullet bullet = null;
    int bulletIndex = -1;
    if (bullets != null) {
        if (bulletsIndices != null) {
            int index = lineIndex - topIndex;
            if (0 <= index && index < CACHE_SIZE) {
                bullet = bullets[index];
                bulletIndex = bulletsIndices[index];
            }
        } else {
            for (int i = 0; i < bullets.length; i++) {
                bullet = bullets[i];
                bulletIndex = bullet.indexOf(lineIndex);
                if (bulletIndex != -1) break;
            }
        }
    }
    if (bulletIndex != -1 && bullet != null) {
        FontMetrics metrics = layout.getLineMetrics(0);
        int lineAscent = metrics.getAscent() + metrics.getLeading();
        if (bullet.type == ST.BULLET_CUSTOM) {
            bullet.style.start = lineOffset;
            styledText.paintObject(gc, paintX, paintY, lineAscent, metrics.getDescent(), bullet.style, bullet, bulletIndex);
        } else {
            drawBullet(bullet, gc, paintX, paintY, bulletIndex, lineAscent, metrics.getDescent());
        }
    }
    TextStyle[] styles = layout.getStyles();
    int[] ranges = null;
    for (int i = 0; i < styles.length; i++) {
        if (styles[i].metrics != null) {
            if (ranges == null) ranges = layout.getRanges();
            int start = ranges[i << 1];
            int length = ranges[(i << 1) + 1] - start + 1;
            Point point = layout.getLocation(start, false);
            FontMetrics metrics = layout.getLineMetrics(layout.getLineIndex(start));
            StyleRange style = (StyleRange)((StyleRange)styles[i]).clone();
            style.start = start + lineOffset;
            style.length = length;
            int lineAscent = metrics.getAscent() + metrics.getLeading();
            styledText.paintObject(gc, point.x + paintX, point.y + paintY, lineAscent, metrics.getDescent(), style, null, 0);
        }
    }
    disposeTextLayout(layout);
    return height;
}
int getBaseline() {
    return ascent;
}
Font getFont(int style) {
    switch (style) {
        case SWT.BOLD:
            if (boldFont != null) return boldFont;
            return boldFont = new Font(device, getFontData(style));
        case SWT.ITALIC:
            if (italicFont != null) return italicFont;
            return italicFont = new Font(device, getFontData(style));
        case SWT.BOLD | SWT.ITALIC:
            if (boldItalicFont != null) return boldItalicFont;
            return boldItalicFont = new Font(device, getFontData(style));
        default:
            return regularFont;
    }
}
FontData[] getFontData(int style) {
    FontData[] fontDatas = regularFont.getFontData();
    for (int i = 0; i < fontDatas.length; i++) {
        fontDatas[i].setStyle(style);
    }
    return fontDatas;
}
int getHeight () {
    int defaultLineHeight = getLineHeight();
    if (styledText.isFixedLineHeight()) {
        return lineCount * defaultLineHeight + styledText.topMargin + styledText.bottomMargin;
    }
    int totalHeight = 0;
    for (int i = 0; i < lineCount; i++) {
        int height = lineHeight[i];
        if (height == -1) {
            int width = styledText.getWrapWidth(i);
            if (width > 0) {
                int length = content.getLine(i).length();
                height = ((length * averageCharWidth / width) + 1) * defaultLineHeight;
            } else {
                height = defaultLineHeight;
            }
        }
        totalHeight += height;
    }
    return totalHeight + styledText.topMargin + styledText.bottomMargin;
}
boolean hasLink(int offset) {
    if (offset == -1) return false;
    int lineIndex = content.getLineAtOffset(offset);
    int lineOffset = content.getOffsetAtLine(lineIndex);
    String line = content.getLine(lineIndex);
    StyledTextEvent event = styledText.getLineStyleData(lineOffset, line);
    if (event != null) {
        StyleRange[] styles = event.styles;
        if (styles != null) {
            int[] ranges = event.ranges;
            if (ranges != null) {
                for (int i = 0; i < ranges.length; i+=2) {
                    if (ranges[i] <= offset && offset < ranges[i] + ranges[i+1] && styles[i >> 1].underline && styles[i >> 1].underlineStyle == SWT.UNDERLINE_LINK) {
                        return true;
                    }
                }
            } else {
                for (int i = 0; i < styles.length; i++) {
                    StyleRange style = styles[i];
                    if (style.start <= offset && offset < style.start + style.length && style.underline && style.underlineStyle == SWT.UNDERLINE_LINK) {
                        return true;
                    }
                }
            }
        }
    }  else {
        if (ranges != null) {
            int rangeCount = styleCount << 1;
            int index = getRangeIndex(offset, -1, rangeCount);
            if (index >= rangeCount) return false;
            int rangeStart = ranges[index];
            int rangeLength = ranges[index + 1];
            StyleRange rangeStyle = styles[index >> 1];
            if (rangeStart <= offset && offset < rangeStart + rangeLength && rangeStyle.underline && rangeStyle.underlineStyle == SWT.UNDERLINE_LINK) {
                return true;
            }
        }
    }
    return false;
}
int getLineAlignment(int index, int defaultAlignment) {
    if (lines == null) return defaultAlignment;
    LineInfo info = lines[index];
    if (info != null && (info.flags & ALIGNMENT) != 0) {
        return info.alignment;
    }
    return defaultAlignment;
}
Color getLineBackground(int index, Color defaultBackground) {
    if (lines == null) return defaultBackground;
    LineInfo info = lines[index];
    if (info != null && (info.flags & BACKGROUND) != 0) {
        return info.background;
    }
    return defaultBackground;
}
Bullet getLineBullet (int index, Bullet defaultBullet) {
    if (bullets == null) return defaultBullet;
    if (bulletsIndices != null) return defaultBullet;
    for (int i = 0; i < bullets.length; i++) {
        Bullet bullet = bullets[i];
        if (bullet.indexOf(index) != -1) return bullet;
    }
    return defaultBullet;
}
int getLineHeight() {
    return ascent + descent;
}
int getLineHeight(int lineIndex) {
    if (lineHeight[lineIndex] == -1) {
        calculate(lineIndex, 1);
    }
    return lineHeight[lineIndex];
}
int getLineIndent(int index, int defaultIndent) {
    if (lines == null) return defaultIndent;
    LineInfo info = lines[index];
    if (info != null && (info.flags & INDENT) != 0) {
        return info.indent;
    }
    return defaultIndent;
}
int getLineWrapIndent(int index, int defaultWrapIndent) {
    if (lines == null) return defaultWrapIndent;
    LineInfo info = lines[index];
    if (info != null && (info.flags & WRAP_INDENT) != 0) {
        return info.wrapIndent;
    }
    return defaultWrapIndent;
}
boolean getLineJustify(int index, boolean defaultJustify) {
    if (lines == null) return defaultJustify;
    LineInfo info = lines[index];
    if (info != null && (info.flags & JUSTIFY) != 0) {
        return info.justify;
    }
    return defaultJustify;
}
int[] getLineTabStops(int index, int[] defaultTabStops) {
    if (lines == null) return defaultTabStops;
    LineInfo info = lines[index];
    if (info != null && (info.flags & TABSTOPS) != 0) {
        return info.tabStops;
    }
    return defaultTabStops;
}
int getRangeIndex(int offset, int low, int high) {
    if (styleCount == 0) return 0;
    if (ranges != null)  {
        while (high - low > 2) {
            int index = ((high + low) / 2) / 2 * 2;
            int end = ranges[index] + ranges[index + 1];
            if (end > offset) {
                high = index;
            } else {
                low = index;
            }
        }
    } else {
        while (high - low > 1) {
            int index = ((high + low) / 2);
            int end = styles[index].start + styles[index].length;
            if (end > offset) {
                high = index;
            } else {
                low = index;
            }
        }
    }
    return high;
}
int[] getRanges(int start, int length) {
    if (length == 0) return null;
    int[] newRanges;
    int end = start + length - 1;
    if (ranges != null) {
        int rangeCount = styleCount << 1;
        int rangeStart = getRangeIndex(start, -1, rangeCount);
        if (rangeStart >= rangeCount) return null;
        if (ranges[rangeStart] > end) return null;
        int rangeEnd = Math.min(rangeCount - 2, getRangeIndex(end, rangeStart - 1, rangeCount));
        if (ranges[rangeEnd] > end) rangeEnd = Math.max(rangeStart, rangeEnd - 2);
        newRanges = new int[rangeEnd - rangeStart + 2];
        System.arraycopy(ranges, rangeStart, newRanges, 0, newRanges.length);
    } else {
        int rangeStart = getRangeIndex(start, -1, styleCount);
        if (rangeStart >= styleCount) return null;
        if (styles[rangeStart].start > end) return null;
        int rangeEnd = Math.min(styleCount - 1, getRangeIndex(end, rangeStart - 1, styleCount));
        if (styles[rangeEnd].start > end) rangeEnd = Math.max(rangeStart, rangeEnd - 1);
        newRanges = new int[(rangeEnd - rangeStart + 1) << 1];
        for (int i = rangeStart, j = 0; i <= rangeEnd; i++, j += 2) {
            StyleRange style = styles[i];
            newRanges[j] = style.start;
            newRanges[j + 1] = style.length;
        }
    }
    if (start > newRanges[0]) {
        newRanges[1] = newRanges[0] + newRanges[1] - start;
        newRanges[0] = start;
    }
    if (end < newRanges[newRanges.length - 2] + newRanges[newRanges.length - 1] - 1) {
        newRanges[newRanges.length - 1] = end - newRanges[newRanges.length - 2] + 1;
    }
    return newRanges;
}
StyleRange[] getStyleRanges(int start, int length, boolean includeRanges) {
    if (length == 0) return null;
    StyleRange[] newStyles;
    int end = start + length - 1;
    if (ranges != null) {
        int rangeCount = styleCount << 1;
        int rangeStart = getRangeIndex(start, -1, rangeCount);
        if (rangeStart >= rangeCount) return null;
        if (ranges[rangeStart] > end) return null;
        int rangeEnd = Math.min(rangeCount - 2, getRangeIndex(end, rangeStart - 1, rangeCount));
        if (ranges[rangeEnd] > end) rangeEnd = Math.max(rangeStart, rangeEnd - 2);
        newStyles = new StyleRange[((rangeEnd - rangeStart) >> 1) + 1];
        if (includeRanges) {
            for (int i = rangeStart, j = 0; i <= rangeEnd; i += 2, j++) {
                newStyles[j] = (StyleRange)styles[i >> 1].clone();
                newStyles[j].start = ranges[i];
                newStyles[j].length = ranges[i + 1];
            }
        } else {
            System.arraycopy(styles, rangeStart >> 1, newStyles, 0, newStyles.length);
        }
    } else {
        int rangeStart = getRangeIndex(start, -1, styleCount);
        if (rangeStart >= styleCount) return null;
        if (styles[rangeStart].start > end) return null;
        int rangeEnd = Math.min(styleCount - 1, getRangeIndex(end, rangeStart - 1, styleCount));
        if (styles[rangeEnd].start > end) rangeEnd = Math.max(rangeStart, rangeEnd - 1);
        newStyles = new StyleRange[rangeEnd - rangeStart + 1];
        System.arraycopy(styles, rangeStart, newStyles, 0, newStyles.length);
    }
    if (includeRanges || ranges == null) {
        StyleRange style = newStyles[0];
        if (start > style.start) {
            newStyles[0] = style = (StyleRange)style.clone();
            style.length = style.start + style.length - start;
            style.start = start;
        }
        style = newStyles[newStyles.length - 1];
        if (end < style.start + style.length - 1) {
            newStyles[newStyles.length - 1] = style = (StyleRange)style.clone();
            style.length = end - style.start + 1;
        }
    }
    return newStyles;
}
StyleRange getStyleRange(StyleRange style) {
    if (style.underline && style.underlineStyle == SWT.UNDERLINE_LINK) hasLinks = true;
    if (style.start == 0 && style.length == 0 && style.fontStyle == SWT.NORMAL) return style;
    StyleRange clone = (StyleRange)style.clone();
    clone.start = clone.length = 0;
    clone.fontStyle = SWT.NORMAL;
    if (clone.font == null) clone.font = getFont(style.fontStyle);
    return clone;
}
TextLayout getTextLayout(int lineIndex) {
    return getTextLayout(lineIndex, styledText.getOrientation(), styledText.getWrapWidth(lineIndex), styledText.lineSpacing);
}
TextLayout getTextLayout(int lineIndex, int orientation, int width, int lineSpacing) {
    TextLayout layout = null;
    if (styledText != null) {
        int topIndex = styledText.topIndex > 0 ? styledText.topIndex - 1 : 0;
        if (layouts == null || topIndex != this.topIndex) {
            TextLayout[] newLayouts = new TextLayout[CACHE_SIZE];
            if (layouts != null) {
                for (int i = 0; i < layouts.length; i++) {
                    if (layouts[i] != null) {
                        int layoutIndex = (i + this.topIndex) - topIndex;
                        if (0 <= layoutIndex && layoutIndex < newLayouts.length) {
                            newLayouts[layoutIndex] = layouts[i];
                        } else {
                            layouts[i].dispose();
                        }
                    }
                }
            }
            if (bullets != null && bulletsIndices != null && topIndex != this.topIndex) {
                int delta = topIndex - this.topIndex;
                if (delta > 0) {
                    if (delta < bullets.length) {
                        System.arraycopy(bullets, delta, bullets, 0, bullets.length - delta);
                        System.arraycopy(bulletsIndices, delta, bulletsIndices, 0, bulletsIndices.length - delta);
                    }
                    int startIndex = Math.max(0, bullets.length - delta);
                    for (int i = startIndex; i < bullets.length; i++) bullets[i] = null;
                } else {
                    if (-delta < bullets.length) {
                        System.arraycopy(bullets, 0, bullets, -delta, bullets.length + delta);
                        System.arraycopy(bulletsIndices, 0, bulletsIndices, -delta, bulletsIndices.length + delta);
                    }
                    int endIndex = Math.min(bullets.length, -delta);
                    for (int i = 0; i < endIndex; i++) bullets[i] = null;
                }
            }
            this.topIndex = topIndex;
            layouts = newLayouts;
        }
        if (layouts != null) {
            int layoutIndex = lineIndex - topIndex;
            if (0 <= layoutIndex && layoutIndex < layouts.length) {
                layout = layouts[layoutIndex];
                if (layout != null) {
                    if (lineWidth[lineIndex] != -1) return layout;
                } else {
                    layout = layouts[layoutIndex] = new TextLayout(device);
                }
            }
        }
    }
    if (layout == null) layout = new TextLayout(device);
    String line = content.getLine(lineIndex);
    int lineOffset = content.getOffsetAtLine(lineIndex);
    int[] segments = null;
    char[] segmentChars = null;
    int indent = 0;
    int wrapIndent = 0;
    int alignment = SWT.LEFT;
    boolean justify = false;
    int[] tabs = {tabWidth};
    Bullet bullet = null;
    int[] ranges = null;
    StyleRange[] styles = null;
    int rangeStart = 0, styleCount = 0;
    StyledTextEvent event = null;
    if (styledText != null) {
        event = styledText.getBidiSegments(lineOffset, line);
        if (event != null) {
            segments = event.segments;
            segmentChars = event.segmentsChars;
        }
        event = styledText.getLineStyleData(lineOffset, line);
        indent = styledText.indent;
        wrapIndent = styledText.wrapIndent;
        alignment = styledText.alignment;
        justify = styledText.justify;
        if (styledText.tabs != null) tabs = styledText.tabs;
    }
    if (event != null) {
        indent = event.indent;
        wrapIndent = event.wrapIndent;
        alignment = event.alignment;
        justify = event.justify;
        bullet = event.bullet;
        ranges = event.ranges;
        styles = event.styles;
        if (event.tabStops != null) tabs = event.tabStops;
        if (styles != null) {
            styleCount = styles.length;
            if (styledText.isFixedLineHeight()) {
                for (int i = 0; i < styleCount; i++) {
                    if (styles[i].isVariableHeight()) {
                        styledText.verticalScrollOffset = -1;
                        styledText.setVariableLineHeight();
                        styledText.redraw();
                        break;
                    }
                }
            }
        }
        if (bullets == null || bulletsIndices == null) {
            bullets = new Bullet[CACHE_SIZE];
            bulletsIndices = new int[CACHE_SIZE];
        }
        int index = lineIndex - topIndex;
        if (0 <= index && index < CACHE_SIZE) {
            bullets[index] = bullet;
            bulletsIndices[index] = event.bulletIndex;
        }
    } else {
        if (lines != null) {
            LineInfo info = lines[lineIndex];
            if (info != null) {
                if ((info.flags & INDENT) != 0) indent = info.indent;
                if ((info.flags & WRAP_INDENT) != 0) wrapIndent = info.wrapIndent;
                if ((info.flags & ALIGNMENT) != 0) alignment = info.alignment;
                if ((info.flags & JUSTIFY) != 0) justify = info.justify;
                if ((info.flags & SEGMENTS) != 0) segments = info.segments;
                if ((info.flags & SEGMENT_CHARS) != 0) segmentChars = info.segmentsChars;
                if ((info.flags & TABSTOPS) != 0) tabs = info.tabStops;
            }
        }
        if (bulletsIndices != null) {
            bullets = null;
            bulletsIndices = null;
        }
        if (bullets != null) {
            for (int i = 0; i < bullets.length; i++) {
                if (bullets[i].indexOf(lineIndex) != -1) {
                    bullet = bullets[i];
                    break;
                }
            }
        }
        ranges = this.ranges;
        styles = this.styles;
        styleCount = this.styleCount;
        if (ranges != null) {
            rangeStart = getRangeIndex(lineOffset, -1, styleCount << 1);
        } else {
            rangeStart = getRangeIndex(lineOffset, -1, styleCount);
        }
    }
    if (bullet != null) {
        StyleRange style = bullet.style;
        GlyphMetrics metrics = style.metrics;
        indent += metrics.width;
    }
    layout.setFont(regularFont);
    layout.setAscent(ascent);
    layout.setDescent(descent);
    layout.setText(line);
    layout.setOrientation(orientation);
    layout.setSegments(segments);
    layout.setSegmentsChars(segmentChars);
    layout.setWidth(width);
    layout.setSpacing(lineSpacing);
    layout.setTabs(tabs);
    layout.setIndent(indent);
    layout.setWrapIndent(wrapIndent);
    layout.setAlignment(alignment);
    layout.setJustify(justify);

    int lastOffset = 0;
    int length = line.length();
    if (styles != null) {
        if (ranges != null) {
            int rangeCount = styleCount << 1;
            for (int i = rangeStart; i < rangeCount; i += 2) {
                int start, end;
                if (lineOffset > ranges[i]) {
                    start = 0;
                    end = Math.min (length, ranges[i + 1] - lineOffset + ranges[i]);
                } else {
                    start = ranges[i] - lineOffset;
                    end = Math.min(length, start + ranges[i + 1]);
                }
                if (start >= length) break;
                if (lastOffset < start) {
                    layout.setStyle(null, lastOffset, start - 1);
                }
                layout.setStyle(getStyleRange(styles[i >> 1]), start, end);
                lastOffset = Math.max(lastOffset, end);
            }
        } else {
            for (int i = rangeStart; i < styleCount; i++) {
                int start, end;
                if (lineOffset > styles[i].start) {
                    start = 0;
                    end = Math.min (length, styles[i].length - lineOffset + styles[i].start);
                } else {
                    start = styles[i].start - lineOffset;
                    end = Math.min(length, start + styles[i].length);
                }
                if (start >= length) break;
                if (lastOffset < start) {
                    layout.setStyle(null, lastOffset, start - 1);
                }
                layout.setStyle(getStyleRange(styles[i]), start, end);
                lastOffset = Math.max(lastOffset, end);
            }
        }
    }
    if (lastOffset < length) layout.setStyle(null, lastOffset, length);
    if (styledText != null && styledText.ime != null) {
        IME ime = styledText.ime;
        int compositionOffset = ime.getCompositionOffset();
        if (compositionOffset != -1) {
            int commitCount = ime.getCommitCount();
            int compositionLength = ime.getText().length();
            if (compositionLength != commitCount) {
                int compositionLine = content.getLineAtOffset(compositionOffset);
                if (compositionLine == lineIndex) {
                    int[] imeRanges = ime.getRanges();
                    TextStyle[] imeStyles = ime.getStyles();
                    if (imeRanges.length > 0) {
                        for (int i = 0; i < imeStyles.length; i++) {
                            int start = imeRanges[i*2] - lineOffset;
                            int end = imeRanges[i*2+1] - lineOffset;
                            TextStyle imeStyle = imeStyles[i], userStyle;
                            for (int j = start; j <= end; j++) {
                                userStyle = layout.getStyle(j);
                                if (userStyle == null && j > 0) userStyle = layout.getStyle(j - 1);
                                if (userStyle == null && j + 1 < length) userStyle = layout.getStyle(j + 1);
                                if (userStyle == null) {
                                    layout.setStyle(imeStyle, j, j);
                                } else {
                                    TextStyle newStyle = new TextStyle(imeStyle);
                                    if (newStyle.font == null) newStyle.font = userStyle.font;
                                    if (newStyle.foreground == null) newStyle.foreground = userStyle.foreground;
                                    if (newStyle.background == null) newStyle.background = userStyle.background;
                                    layout.setStyle(newStyle, j, j);
                                }
                            }
                        }
                    } else {
                        int start = compositionOffset - lineOffset;
                        int end = start + compositionLength - 1;
                        TextStyle userStyle = layout.getStyle(start);
                        if (userStyle == null) {
                            if (start > 0) userStyle = layout.getStyle(start - 1);
                            if (userStyle == null && end + 1 < length) userStyle = layout.getStyle(end + 1);
                            if (userStyle != null) {
                                TextStyle newStyle = new TextStyle();
                                newStyle.font = userStyle.font;
                                newStyle.foreground = userStyle.foreground;
                                newStyle.background = userStyle.background;
                                layout.setStyle(newStyle, start, end);
                            }
                        }
                    }
                }
            }
        }
    }

    if (styledText != null && styledText.isFixedLineHeight()) {
        int index = -1;
        int lineCount = layout.getLineCount();
        int height = getLineHeight();
        for (int i = 0; i < lineCount; i++) {
            int lineHeight = layout.getLineBounds(i).height;
            if (lineHeight > height) {
                height = lineHeight;
                index = i;
            }
        }
        if (index != -1) {
            FontMetrics metrics = layout.getLineMetrics(index);
            ascent = metrics.getAscent() + metrics.getLeading();
            descent = metrics.getDescent();
            if (layouts != null) {
                for (int i = 0; i < layouts.length; i++) {
                    if (layouts[i] != null && layouts[i] != layout) {
                        layouts[i].setAscent(ascent);
                        layouts[i].setDescent(descent);
                    }
                }
            }
            if (styledText.verticalScrollOffset != 0) {
                int topIndex = styledText.topIndex;
                int topIndexY = styledText.topIndexY;
                int lineHeight = getLineHeight();
                if (topIndexY >= 0) {
                    styledText.verticalScrollOffset = (topIndex - 1) * lineHeight + lineHeight - topIndexY;
                } else {
                    styledText.verticalScrollOffset = topIndex * lineHeight - topIndexY;
                }
            }
            styledText.calculateScrollBars();
            if (styledText.isBidiCaret()) styledText.createCaretBitmaps();
            styledText.caretDirection = SWT.NULL;
            styledText.setCaretLocation();
            styledText.redraw();
        }
    }
    return layout;
}
int getWidth() {
    return maxWidth;
}
void reset() {
    if (layouts != null) {
        for (int i = 0; i < layouts.length; i++) {
            TextLayout layout = layouts[i];
            if (layout != null) layout.dispose();
        }
        layouts = null;
    }
    topIndex = -1;
    stylesSetCount = styleCount = lineCount = 0;
    ranges = null;
    styles = null;
    stylesSet = null;
    lines = null;
    lineWidth = null;
    lineHeight = null;
    bullets = null;
    bulletsIndices = null;
    redrawLines = null;
    hasLinks = false;
}
void reset(int startLine, int lineCount) {
    int endLine = startLine + lineCount;
    if (startLine < 0 || endLine > lineWidth.length) return;
    for (int i = startLine; i < endLine; i++) {
        lineWidth[i] = -1;
        lineHeight[i] = -1;
    }
    if (startLine <= maxWidthLineIndex && maxWidthLineIndex < endLine) {
        maxWidth = 0;
        maxWidthLineIndex = -1;
        if (lineCount != this.lineCount) {
            for (int i = 0; i < this.lineCount; i++) {
                if (lineWidth[i] > maxWidth) {
                    maxWidth = lineWidth[i];
                    maxWidthLineIndex = i;
                }
            }
        }
    }
}
void setContent(StyledTextContent content) {
    reset();
    this.content = content;
    lineCount = content.getLineCount();
    lineWidth = new int[lineCount];
    lineHeight = new int[lineCount];
    reset(0, lineCount);
}
@SuppressWarnings("deprecation")
void setFont(Font font, int tabs) {
    TextLayout layout = new TextLayout(device);
    layout.setFont(regularFont);
    if (font != null) {
        if (boldFont != null) boldFont.dispose();
        if (italicFont != null) italicFont.dispose();
        if (boldItalicFont != null) boldItalicFont.dispose();
        boldFont = italicFont = boldItalicFont = null;
        regularFont = font;
        layout.setText("    ");
        layout.setFont(font);
        layout.setStyle(new TextStyle(getFont(SWT.NORMAL), null, null), 0, 0);
        layout.setStyle(new TextStyle(getFont(SWT.BOLD), null, null), 1, 1);
        layout.setStyle(new TextStyle(getFont(SWT.ITALIC), null, null), 2, 2);
        layout.setStyle(new TextStyle(getFont(SWT.BOLD | SWT.ITALIC), null, null), 3, 3);
        FontMetrics metrics = layout.getLineMetrics(0);
        ascent = metrics.getAscent() + metrics.getLeading();
        descent = metrics.getDescent();
        boldFont.dispose();
        italicFont.dispose();
        boldItalicFont.dispose();
        boldFont = italicFont = boldItalicFont = null;
    }
    layout.dispose();
    layout = new TextLayout(device);
    layout.setFont(regularFont);
    StringBuffer tabBuffer = new StringBuffer(tabs);
    for (int i = 0; i < tabs; i++) {
        tabBuffer.append(' ');
    }
    layout.setText(tabBuffer.toString());
    tabWidth = layout.getBounds().width;
    layout.dispose();
    if (styledText != null) {
        GC gc = new GC(styledText);
        averageCharWidth = gc.getFontMetrics().getAverageCharWidth();
        fixedPitch = gc.stringExtent("l").x == gc.stringExtent("W").x; //$NON-NLS-1$ //$NON-NLS-2$
        gc.dispose();
    }
}
void setLineAlignment(int startLine, int count, int alignment) {
    if (lines == null) lines = new LineInfo[lineCount];
    for (int i = startLine; i < startLine + count; i++) {
        if (lines[i] == null) {
            lines[i] = new LineInfo();
        }
        lines[i].flags |= ALIGNMENT;
        lines[i].alignment = alignment;
    }
}
void setLineBackground(int startLine, int count, Color background) {
    if (lines == null) lines = new LineInfo[lineCount];
    for (int i = startLine; i < startLine + count; i++) {
        if (lines[i] == null) {
            lines[i] = new LineInfo();
        }
        lines[i].flags |= BACKGROUND;
        lines[i].background = background;
    }
}
void setLineBullet(int startLine, int count, Bullet bullet) {
    if (bulletsIndices != null) {
        bulletsIndices = null;
        bullets = null;
    }
    if (bullets == null) {
        if (bullet == null) return;
        bullets = new Bullet[1];
        bullets[0] = bullet;
    }
    int index = 0;
    while (index < bullets.length) {
        if (bullet == bullets[index]) break;
        index++;
    }
    if (bullet != null) {
        if (index == bullets.length) {
            Bullet[] newBulletsList = new Bullet[bullets.length + 1];
            System.arraycopy(bullets, 0, newBulletsList, 0, bullets.length);
            newBulletsList[index] = bullet;
            bullets = newBulletsList;
        }
        bullet.addIndices(startLine, count);
    } else {
        updateBullets(startLine, count, 0, false);
        styledText.redrawLinesBullet(redrawLines);
        redrawLines = null;
    }
}
void setLineIndent(int startLine, int count, int indent) {
    if (lines == null) lines = new LineInfo[lineCount];
    for (int i = startLine; i < startLine + count; i++) {
        if (lines[i] == null) {
            lines[i] = new LineInfo();
        }
        lines[i].flags |= INDENT;
        lines[i].indent = indent;
    }
}
void setLineWrapIndent(int startLine, int count, int wrapIndent) {
    if (lines == null) lines = new LineInfo[lineCount];
    for (int i = startLine; i < startLine + count; i++) {
        if (lines[i] == null) {
            lines[i] = new LineInfo();
        }
        lines[i].flags |= WRAP_INDENT;
        lines[i].wrapIndent = wrapIndent;
    }
}
void setLineJustify(int startLine, int count, boolean justify) {
    if (lines == null) lines = new LineInfo[lineCount];
    for (int i = startLine; i < startLine + count; i++) {
        if (lines[i] == null) {
            lines[i] = new LineInfo();
        }
        lines[i].flags |= JUSTIFY;
        lines[i].justify = justify;
    }
}
void setLineSegments(int startLine, int count, int[] segments) {
    if (lines == null) lines = new LineInfo[lineCount];
    for (int i = startLine; i < startLine + count; i++) {
        if (lines[i] == null) {
            lines[i] = new LineInfo();
        }
        lines[i].flags |= SEGMENTS;
        lines[i].segments = segments;
    }
}
void setLineSegmentChars(int startLine, int count, char[] segmentChars) {
    if (lines == null) lines = new LineInfo[lineCount];
    for (int i = startLine; i < startLine + count; i++) {
        if (lines[i] == null) {
            lines[i] = new LineInfo();
        }
        lines[i].flags |= SEGMENT_CHARS;
        lines[i].segmentsChars = segmentChars;
    }
}
void setLineTabStops(int startLine, int count, int[] tabStops) {
    if (lines == null) lines = new LineInfo[lineCount];
    for (int i = startLine; i < startLine + count; i++) {
        if (lines[i] == null) {
            lines[i] = new LineInfo();
        }
        lines[i].flags |= TABSTOPS;
        lines[i].tabStops = tabStops;
    }
}
void setStyleRanges (int[] newRanges, StyleRange[] newStyles) {
    if (newStyles == null) {
        stylesSetCount = styleCount = 0;
        ranges = null;
        styles = null;
        stylesSet = null;
        hasLinks = false;
        return;
    }
    if (newRanges == null && COMPACT_STYLES) {
        newRanges = new int[newStyles.length << 1];
        StyleRange[] tmpStyles = new StyleRange[newStyles.length];
        if (stylesSet == null) stylesSet = new StyleRange[4];
        for (int i = 0, j = 0; i < newStyles.length; i++) {
            StyleRange newStyle = newStyles[i];
            newRanges[j++] = newStyle.start;
            newRanges[j++] = newStyle.length;
            int index = 0;
            while (index < stylesSetCount) {
                if (stylesSet[index].similarTo(newStyle)) break;
                index++;
            }
            if (index == stylesSetCount) {
                if (stylesSetCount == stylesSet.length) {
                    StyleRange[] tmpStylesSet = new StyleRange[stylesSetCount + 4];
                    System.arraycopy(stylesSet, 0, tmpStylesSet, 0, stylesSetCount);
                    stylesSet = tmpStylesSet;
                }
                stylesSet[stylesSetCount++] = newStyle;
            }
            tmpStyles[i] = stylesSet[index];
        }
        newStyles = tmpStyles;
    }

    if (styleCount == 0) {
        if (newRanges != null) {
            ranges = new int[newRanges.length];
            System.arraycopy(newRanges, 0, ranges, 0, ranges.length);
        }
        styles = new StyleRange[newStyles.length];
        System.arraycopy(newStyles, 0, styles, 0, styles.length);
        styleCount = newStyles.length;
        return;
    }
    if (newRanges != null && ranges == null) {
        ranges = new int[styles.length << 1];
        for (int i = 0, j = 0; i < styleCount; i++) {
            ranges[j++] = styles[i].start;
            ranges[j++] = styles[i].length;
        }
    }
    if (newRanges == null && ranges != null) {
        newRanges = new int[newStyles.length << 1];
        for (int i = 0, j = 0; i < newStyles.length; i++) {
            newRanges[j++] = newStyles[i].start;
            newRanges[j++] = newStyles[i].length;
        }
    }
    if (ranges != null) {
        int rangeCount = styleCount << 1;
        int start = newRanges[0];
        int modifyStart = getRangeIndex(start, -1, rangeCount), modifyEnd;
        boolean insert = modifyStart == rangeCount;
        if (!insert) {
            int end = newRanges[newRanges.length - 2] + newRanges[newRanges.length - 1];
            modifyEnd = getRangeIndex(end, modifyStart - 1, rangeCount);
            insert = modifyStart == modifyEnd && ranges[modifyStart] >= end;
        }
        if (insert) {
            addMerge(newRanges, newStyles, newRanges.length, modifyStart, modifyStart);
            return;
        }
        modifyEnd = modifyStart;
        int[] mergeRanges = new int[6];
        StyleRange[] mergeStyles = new StyleRange[3];
        for (int i = 0; i < newRanges.length; i += 2) {
            int newStart = newRanges[i];
            int newEnd = newStart + newRanges[i + 1];
            if (newStart == newEnd) continue;
            int modifyLast = 0, mergeCount = 0;
            while (modifyEnd < rangeCount) {
                if (newStart >= ranges[modifyStart] + ranges[modifyStart + 1]) modifyStart += 2;
                if (ranges[modifyEnd] + ranges[modifyEnd + 1] > newEnd) break;
                modifyEnd += 2;
            }
            if (ranges[modifyStart] < newStart && newStart < ranges[modifyStart] + ranges[modifyStart + 1]) {
                mergeStyles[mergeCount >> 1] = styles[modifyStart >> 1];
                mergeRanges[mergeCount] = ranges[modifyStart];
                mergeRanges[mergeCount + 1] = newStart - ranges[modifyStart];
                mergeCount += 2;
            }
            mergeStyles[mergeCount >> 1] = newStyles[i >> 1];
            mergeRanges[mergeCount] = newStart;
            mergeRanges[mergeCount + 1] = newRanges[i + 1];
            mergeCount += 2;
            if (modifyEnd < rangeCount && ranges[modifyEnd] < newEnd && newEnd < ranges[modifyEnd] + ranges[modifyEnd + 1]) {
                mergeStyles[mergeCount >> 1] = styles[modifyEnd >> 1];
                mergeRanges[mergeCount] = newEnd;
                mergeRanges[mergeCount + 1] = ranges[modifyEnd] + ranges[modifyEnd + 1] - newEnd;
                mergeCount += 2;
                modifyLast = 2;
            }
            int grow = addMerge(mergeRanges, mergeStyles, mergeCount, modifyStart, modifyEnd + modifyLast);
            rangeCount += grow;
            modifyStart = modifyEnd += grow;
        }
    } else {
        int start = newStyles[0].start;
        int modifyStart = getRangeIndex(start, -1, styleCount), modifyEnd;
        boolean insert = modifyStart == styleCount;
        if (!insert) {
            int end = newStyles[newStyles.length - 1].start + newStyles[newStyles.length - 1].length;
            modifyEnd = getRangeIndex(end, modifyStart - 1, styleCount);
            insert = modifyStart == modifyEnd && styles[modifyStart].start >= end;
        }
        if (insert) {
            addMerge(newStyles, newStyles.length, modifyStart, modifyStart);
            return;
        }
        modifyEnd = modifyStart;
        StyleRange[] mergeStyles = new StyleRange[3];
        for (int i = 0; i < newStyles.length; i++) {
            StyleRange newStyle = newStyles[i], style;
            int newStart = newStyle.start;
            int newEnd = newStart + newStyle.length;
            if (newStart == newEnd) continue;
            int modifyLast = 0, mergeCount = 0;
            while (modifyEnd < styleCount) {
                if (newStart >= styles[modifyStart].start + styles[modifyStart].length) modifyStart++;
                if (styles[modifyEnd].start + styles[modifyEnd].length > newEnd) break;
                modifyEnd++;
            }
            style = styles[modifyStart];
            if (style.start < newStart && newStart < style.start + style.length) {
                style = mergeStyles[mergeCount++] = (StyleRange)style.clone();
                style.length = newStart - style.start;
            }
            mergeStyles[mergeCount++] = newStyle;
            if (modifyEnd < styleCount) {
                style = styles[modifyEnd];
                if (style.start < newEnd && newEnd < style.start + style.length) {
                    style = mergeStyles[mergeCount++] = (StyleRange)style.clone();
                    style.length += style.start - newEnd;
                    style.start = newEnd;
                    modifyLast = 1;
                }
            }
            int grow = addMerge(mergeStyles, mergeCount, modifyStart, modifyEnd + modifyLast);
            modifyStart = modifyEnd += grow;
        }
    }
}
void textChanging(TextChangingEvent event) {
    int start = event.start;
    int newCharCount = event.newCharCount, replaceCharCount = event.replaceCharCount;
    int newLineCount = event.newLineCount, replaceLineCount = event.replaceLineCount;

    updateRanges(start, replaceCharCount, newCharCount);

    int startLine = content.getLineAtOffset(start);
    if (replaceCharCount == content.getCharCount()) lines = null;
    if (replaceLineCount == lineCount) {
        lineCount = newLineCount;
        lineWidth = new int[lineCount];
        lineHeight = new int[lineCount];
        reset(0, lineCount);
    } else {
        int delta = newLineCount - replaceLineCount;
        if (lineCount + delta > lineWidth.length) {
            int[] newWidths = new int[lineCount + delta + GROW];
            System.arraycopy(lineWidth, 0, newWidths, 0, lineCount);
            lineWidth = newWidths;
            int[] newHeights = new int[lineCount + delta + GROW];
            System.arraycopy(lineHeight, 0, newHeights, 0, lineCount);
            lineHeight = newHeights;
        }
        if (lines != null) {
            if (lineCount + delta > lines.length) {
                LineInfo[] newLines = new LineInfo[lineCount + delta + GROW];
                System.arraycopy(lines, 0, newLines, 0, lineCount);
                lines = newLines;
            }
        }
        int startIndex = startLine + replaceLineCount + 1;
        int endIndex = startLine + newLineCount + 1;
        System.arraycopy(lineWidth, startIndex, lineWidth, endIndex, lineCount - startIndex);
        System.arraycopy(lineHeight, startIndex, lineHeight, endIndex, lineCount - startIndex);
        for (int i = startLine; i < endIndex; i++) {
            lineWidth[i] = lineHeight[i] = -1;
        }
        for (int i = lineCount + delta; i < lineCount; i++) {
            lineWidth[i] = lineHeight[i] = -1;
        }
        if (layouts != null) {
            int layoutStartLine = startLine - topIndex;
            int layoutEndLine = layoutStartLine + replaceLineCount + 1;
            for (int i = layoutStartLine; i < layoutEndLine; i++) {
                if (0 <= i && i < layouts.length) {
                    if (layouts[i] != null) layouts[i].dispose();
                    layouts[i] = null;
                    if (bullets != null && bulletsIndices != null) bullets[i] = null;
                }
            }
            if (delta > 0) {
                for (int i = layouts.length - 1; i >= layoutEndLine; i--) {
                    if (0 <= i && i < layouts.length) {
                        endIndex = i + delta;
                        if (0 <= endIndex && endIndex < layouts.length) {
                            layouts[endIndex] = layouts[i];
                            layouts[i] = null;
                            if (bullets != null && bulletsIndices != null) {
                                bullets[endIndex] = bullets[i];
                                bulletsIndices[endIndex] = bulletsIndices[i];
                                bullets[i] = null;
                            }
                        } else {
                            if (layouts[i] != null) layouts[i].dispose();
                            layouts[i] = null;
                            if (bullets != null && bulletsIndices != null) bullets[i] = null;
                        }
                    }
                }
            } else if (delta < 0) {
                for (int i = layoutEndLine; i < layouts.length; i++) {
                    if (0 <= i && i < layouts.length) {
                        endIndex = i + delta;
                        if (0 <= endIndex && endIndex < layouts.length) {
                            layouts[endIndex] = layouts[i];
                            layouts[i] = null;
                            if (bullets != null && bulletsIndices != null) {
                                bullets[endIndex] = bullets[i];
                                bulletsIndices[endIndex] = bulletsIndices[i];
                                bullets[i] = null;
                            }
                        } else {
                            if (layouts[i] != null) layouts[i].dispose();
                            layouts[i] = null;
                            if (bullets != null && bulletsIndices != null) bullets[i] = null;
                        }
                    }
                }
            }
        }
        if (replaceLineCount != 0 || newLineCount != 0) {
            int startLineOffset = content.getOffsetAtLine(startLine);
            if (startLineOffset != start) startLine++;
            updateBullets(startLine, replaceLineCount, newLineCount, true);
            if (lines != null) {
                startIndex = startLine + replaceLineCount;
                endIndex = startLine + newLineCount;
                System.arraycopy(lines, startIndex, lines, endIndex, lineCount - startIndex);
                for (int i = startLine; i < endIndex; i++) {
                    lines[i] = null;
                }
                for (int i = lineCount + delta; i < lineCount; i++) {
                    lines[i] = null;
                }
            }
        }
        lineCount += delta;
        if (maxWidthLineIndex != -1 && startLine <= maxWidthLineIndex && maxWidthLineIndex <= startLine + replaceLineCount) {
            maxWidth = 0;
            maxWidthLineIndex = -1;
            for (int i = 0; i < lineCount; i++) {
                if (lineWidth[i] > maxWidth) {
                    maxWidth = lineWidth[i];
                    maxWidthLineIndex = i;
                }
            }
        }
    }
}
void updateBullets(int startLine, int replaceLineCount, int newLineCount, boolean update) {
    if (bullets == null) return;
    if (bulletsIndices != null) return;
    for (int i = 0; i < bullets.length; i++) {
        Bullet bullet = bullets[i];
        int[] lines = bullet.removeIndices(startLine, replaceLineCount, newLineCount, update);
        if (lines != null) {
            if (redrawLines == null) {
                redrawLines = lines;
            } else {
                int[] newRedrawBullets = new int[redrawLines.length + lines.length];
                System.arraycopy(redrawLines, 0, newRedrawBullets, 0, redrawLines.length);
                System.arraycopy(lines, 0, newRedrawBullets, redrawLines.length, lines.length);
                redrawLines = newRedrawBullets;
            }
        }
    }
    int removed = 0;
    for (int i = 0; i < bullets.length; i++) {
        if (bullets[i].size() == 0) removed++;
    }
    if (removed > 0) {
        if (removed == bullets.length) {
            bullets = null;
        } else {
            Bullet[] newBulletsList = new Bullet[bullets.length - removed];
            for (int i = 0, j = 0; i < bullets.length; i++) {
                Bullet bullet = bullets[i];
                if (bullet.size() > 0) newBulletsList[j++] = bullet;
            }
            bullets = newBulletsList;
        }
    }
}
void updateRanges(int start, int replaceCharCount, int newCharCount) {
    if (styleCount == 0 || (replaceCharCount == 0 && newCharCount == 0)) return;
    if (ranges != null) {
        int rangeCount = styleCount << 1;
        int modifyStart = getRangeIndex(start, -1, rangeCount);
        if (modifyStart == rangeCount) return;
        int end = start + replaceCharCount;
        int modifyEnd = getRangeIndex(end, modifyStart - 1, rangeCount);
        int offset = newCharCount - replaceCharCount;
        if (modifyStart == modifyEnd && ranges[modifyStart] < start && end < ranges[modifyEnd] + ranges[modifyEnd + 1]) {
            if (newCharCount == 0) {
                ranges[modifyStart + 1] -= replaceCharCount;
                modifyEnd += 2;
            } else {
                if (rangeCount + 2 > ranges.length) {
                    int[] newRanges = new int[ranges.length + (GROW << 1)];
                    System.arraycopy(ranges, 0, newRanges, 0, rangeCount);
                    ranges = newRanges;
                    StyleRange[] newStyles = new StyleRange[styles.length + GROW];
                    System.arraycopy(styles, 0, newStyles, 0, styleCount);
                    styles = newStyles;
                }
                System.arraycopy(ranges, modifyStart + 2, ranges, modifyStart + 4, rangeCount - (modifyStart + 2));
                System.arraycopy(styles, (modifyStart + 2) >> 1, styles, (modifyStart + 4) >> 1, styleCount - ((modifyStart + 2) >> 1));
                ranges[modifyStart + 3] = ranges[modifyStart] + ranges[modifyStart + 1] - end;
                ranges[modifyStart + 2] = start + newCharCount;
                ranges[modifyStart + 1] = start - ranges[modifyStart];
                styles[(modifyStart >> 1) + 1] = styles[modifyStart >> 1];
                rangeCount += 2;
                styleCount++;
                modifyEnd += 4;
            }
            if (offset != 0) {
                for (int i = modifyEnd; i < rangeCount; i += 2) {
                    ranges[i] += offset;
                }
            }
        } else {
            if (ranges[modifyStart] < start && start < ranges[modifyStart] + ranges[modifyStart + 1]) {
                ranges[modifyStart + 1] = start - ranges[modifyStart];
                modifyStart += 2;
            }
            if (modifyEnd < rangeCount && ranges[modifyEnd] < end && end < ranges[modifyEnd] + ranges[modifyEnd + 1]) {
                ranges[modifyEnd + 1] = ranges[modifyEnd] + ranges[modifyEnd + 1] - end;
                ranges[modifyEnd] = end;
            }
            if (offset != 0) {
                for (int i = modifyEnd; i < rangeCount; i += 2) {
                    ranges[i] += offset;
                }
            }
            System.arraycopy(ranges, modifyEnd, ranges, modifyStart, rangeCount - modifyEnd);
            System.arraycopy(styles, modifyEnd >> 1, styles, modifyStart >> 1, styleCount - (modifyEnd >> 1));
            styleCount -= (modifyEnd - modifyStart) >> 1;
        }
    } else {
        int modifyStart = getRangeIndex(start, -1, styleCount);
        if (modifyStart == styleCount) return;
        int end = start + replaceCharCount;
        int modifyEnd = getRangeIndex(end, modifyStart - 1, styleCount);
        int offset = newCharCount - replaceCharCount;
        if (modifyStart == modifyEnd && styles[modifyStart].start < start && end < styles[modifyEnd].start + styles[modifyEnd].length) {
            if (newCharCount == 0) {
                styles[modifyStart].length -= replaceCharCount;
                modifyEnd++;
            } else {
                if (styleCount + 1 > styles.length) {
                    StyleRange[] newStyles = new StyleRange[styles.length + GROW];
                    System.arraycopy(styles, 0, newStyles, 0, styleCount);
                    styles = newStyles;
                }
                System.arraycopy(styles, modifyStart + 1, styles, modifyStart + 2, styleCount - (modifyStart + 1));
                styles[modifyStart + 1] = (StyleRange)styles[modifyStart].clone();
                styles[modifyStart + 1].length = styles[modifyStart].start + styles[modifyStart].length - end;
                styles[modifyStart + 1].start = start + newCharCount;
                styles[modifyStart].length = start - styles[modifyStart].start;
                styleCount++;
                modifyEnd += 2;
            }
            if (offset != 0) {
                for (int i = modifyEnd; i < styleCount; i++) {
                    styles[i].start += offset;
                }
            }
        } else {
            if (styles[modifyStart].start < start && start < styles[modifyStart].start + styles[modifyStart].length) {
                styles[modifyStart].length = start - styles[modifyStart].start;
                modifyStart++;
            }
            if (modifyEnd < styleCount && styles[modifyEnd].start < end && end < styles[modifyEnd].start + styles[modifyEnd].length) {
                styles[modifyEnd].length = styles[modifyEnd].start + styles[modifyEnd].length - end;
                styles[modifyEnd].start = end;
            }
            if (offset != 0) {
                for (int i = modifyEnd; i < styleCount; i++) {
                    styles[i].start += offset;
                }
            }
            System.arraycopy(styles, modifyEnd, styles, modifyStart, styleCount - modifyEnd);
            styleCount -= modifyEnd - modifyStart;
        }
    }
}
}
