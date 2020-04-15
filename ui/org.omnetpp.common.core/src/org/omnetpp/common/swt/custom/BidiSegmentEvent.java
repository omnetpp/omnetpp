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


import org.eclipse.swt.events.TypedEvent;

/**
 * This event is sent to BidiSegmentListeners when a line is to
 * be measured or rendered in a bidi locale.
 * The segments field can be used in conjunction with the segmentsChars
 * field or by itself. Setting only the segmentsChars field has no effect.
 * When used by itself, the segments field specify text ranges in the line
 * that should be treated as separate segments for bidi reordering.  Each
 * segment will be reordered and rendered separately.
 * <p>
 * The elements in the segments field specify the start offset of
 * a segment relative to the start of the line. They must follow
 * the following rules:
 * <ul>
 * <li>first element must be 0
 * <li>elements must be in ascending order and must not have duplicates
 * <li>elements must not exceed the line length
 * </ul>
 * In addition, the last element may be set to the end of the line
 * but this is not required.
 *
 * The segments field may be left null if the entire line should
 * be reordered as is.
 * </p>
 * A BidiSegmentListener may be used when adjacent segments of
 * right-to-left text should not be reordered relative to each other.
 * For example, within a Java editor, you may wish multiple
 * right-to-left string literals to be reordered differently than the
 * bidi algorithm specifies.
 *
 * Example:
 * <pre>
 *  stored line = "R1R2R3" + "R4R5R6"
 *      R1 to R6 are right-to-left characters. The quotation marks
 *      are part of the line text. The line is 13 characters long.
 *
 *  segments = null:
 *      entire line will be reordered and thus the two R2L segments
 *      swapped (as per the bidi algorithm).
 *      visual line (rendered on screen) = "R6R5R4" + "R3R2R1"
 *
 *  segments = [0, 5, 8]
 *      "R1R2R3" will be reordered, followed by [blank]+[blank] and
 *      "R4R5R6".
 *      visual line = "R3R2R1" + "R6R5R4"
 * </pre>
 *
 * <p>
 * The segments and segementsChars fields can be used together to obtain different
 * types of bidi reordering and text display. The application can use these two fields
 * to insert Unicode Control Characters in specific offsets in the line, the character
 * at segmentsChars[i] is inserted at the offset specified by segments[i]. When both fields
 * are set, the rules for the segments field are less restrictive:
 * <ul>
 * <li>elements must be in ascending order, duplicates are allowed
 * <li>elements must not exceed the line length
 * </ul>
 * </p>
 *
 * @see <a href="http://www.eclipse.org/swt/">Sample code and further information</a>
 */
public class BidiSegmentEvent extends TypedEvent {

    /**
     * line start offset
     */
    public int lineOffset;

    /**
     * line text
     */
    public String lineText;

    /**
     * bidi segments, see above
     */
    public int[] segments;

    /**
     * characters to be used in the segment boundaries (optional)
     *
     * @since 3.6
     */
    public char[] segmentsChars;

    static final long serialVersionUID = 3257846571587547957L;

BidiSegmentEvent(StyledTextEvent e) {
    super(e);
    lineOffset = e.detail;
    lineText = e.text;
}
}
