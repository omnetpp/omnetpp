/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.simulation.ui;

import org.eclipse.swt.graphics.Color;


/**
 * Clients may implement the TextViewerContent interface to provide a
 * content for the TextViewer widget.
 *
 * @author Andras
 */
public interface ITextViewerContentProvider {
    /**
     * Return the number of lines. It should never return zero. It is recommended
     * that this provided adds an empty line at the end of all content (so that
     * the caret can be moved after the last line).
     */
    public int getLineCount();

    /**
     * Return the line at the given line index without delimiters.
     * <p>
     *
     * @param lineIndex index of the line to return. Does not include
     *  delimiters of preceding lines. Index 0 is the first line of the
     *  content.
     * @return the line text without delimiters
     */
    public String getLineText(int lineIndex);

    /**
     * Return the color for the specified line.
     *
     * @return the color, or null to indicate using the default foreground color
     */
    public Color getLineColor(int lineIndex);

    /**
     * Called by TextViewer to add itself as an Observer to content changes.
     * See TextChangeListener for a description of the listener methods that
     * are called when text changes occur.
     * <p>
     *
     * @param listener the listener
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT when listener is null</li>
     * </ul>
     */
    public void addTextChangeListener(ITextChangeListener listener);

    /**
     * Remove the specified text changed listener.
     * <p>
     *
     * @param listener the listener which should no longer be notified
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT when listener is null</li>
     * </ul>
     */
    public void removeTextChangeListener(ITextChangeListener listener);
}
