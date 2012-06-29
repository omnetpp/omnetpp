/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.simulation.widgets;

import org.eclipse.swt.graphics.Color;


/**
 * An implementation of ITextViewerContent that uses a string as input.
 * 
 * @author Andras
 */
public class TextViewerContent implements ITextViewerContent {
    private String[] lines;  // text, split to lines

    public TextViewerContent(String text) {
        this.lines = text.split("\n");  // XXX split() discards trailing blank lines
        for (int i = 0; i < lines.length; i++)
            lines[i] = lines[i] + "\n";
    }
    
    @Override
    public int getLineCount() {
        return lines.length;
    }

    @Override
    public int getCharCount() {
        int sum = 0;
        for (String line : lines)
            sum += line.length();
        return sum;
    }

    @Override
    public String getLine(int lineIndex) {
        return lines[lineIndex];
    }

    @Override
    public int getLineAtOffset(int offset) {
        for (int i = 0; i < lines.length; i++) {
            offset -= lines[i].length();
            if (offset < 0)
                return i;
        }
        return lines.length;
    }

    @Override
    public int getOffsetAtLine(int lineIndex) {
        int offset = 0;
        for (int i = 0; i < lineIndex; i++)
            offset += lines[i].length();
        return offset;
    }

    @Override
    public Color getLineColor(int lineIndex) {
        return null;
    }

    @Override
    public void addTextChangeListener(ITextChangeListener listener) {
    }

    @Override
    public void removeTextChangeListener(ITextChangeListener listener) {
    }
}
