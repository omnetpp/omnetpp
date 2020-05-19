/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.contentassist;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.swt.graphics.Image;
import org.omnetpp.common.engine.Common;

/**
 * A "standard" implementation of IContentProposal as an immutable value class.
 *
 * The proposal specify a replacement of a range of text in the field
 * with the new content. The content can be decorated to add quotes,
 * parenthesis.
 *
 * @author Andras
 */
public class ContentProposalEx implements IContentProposalEx, Comparable<Object> {
    private String content;
    private String label;
    private String description;
    private int cursorPosition; // position of the cursor relative to content after the proposal accepted
    private int startIndex; // start of the replaced range
    private int endIndex; // end of the replaced range
    private Image image;

    public ContentProposalEx(String content) {
        this(content, content, null, content.length(), null);
    }

    public ContentProposalEx(String content, String label) {
        this(content, label, null);
        this.cursorPosition = content.length();
    }

    public ContentProposalEx(String content, String label, String description) {
        this(content, label, description, content.length(), null);
    }

    public ContentProposalEx(String content, String label, String description, Image image) {
        this(content, label, description, content.length(), image);
    }

    public ContentProposalEx(String content, String label, String description, int cursorPosition, Image image) {
        this.content = content;
        this.label = label;
        this.description = description;
        this.cursorPosition = cursorPosition;
        this.image = image;
    }

    public String getContent() {
        return content;
    }

    public int getStartIndex() {
        return startIndex;
    }

    public int getEndIndex() {
        return endIndex;
    }

    public int getCursorPosition() {
        return cursorPosition;
    }

    public String getDescription() {
        return description;
    }

    public String getLabel() {
        return label;
    }

    public Image getImage() {
        return image;
    }

    public void setStartIndex(int index) {
        startIndex = index;
    }

    public void setEndIndex(int index) {
        endIndex = index;
    }

    public boolean startsWith(String prefix) {
        return content.startsWith(prefix);
    }

    public int compareTo(Object o) {
        return label.compareTo(((IContentProposal)o).getLabel());
    }

    private static boolean needsQuotes(String pattern) {
        return !pattern.matches("[a-zA-Z0-9:_]+"); // a conservative approach
    }

    private static String quoteStringIfNeeded(String str) {
        return needsQuotes(str) ? Common.quoteString(str) : str;
    }

    @Override
    public String toString() {
        return "\"" + content + "\"";
    }
}