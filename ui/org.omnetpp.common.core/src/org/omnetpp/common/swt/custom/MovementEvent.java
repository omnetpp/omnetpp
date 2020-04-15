/*******************************************************************************
 * Copyright (c) 2000, 2008 IBM Corporation and others.
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
 * This event is sent when a new offset is required based on the current
 * offset and a movement type.
 *
 * @see <a href="http://www.eclipse.org/swt/">Sample code and further information</a>
 *
 * @since 3.3
 */
public class MovementEvent extends TypedEvent {

    /**
     * line start offset (input)
     */
    public int lineOffset;

    /**
     * line text (input)
     */
    public String lineText;

    /**
     * the current offset (input)
     */
    public int offset;

    /**
     * the new offset  (input, output)
     */
    public int newOffset;

    /**
     * the movement type (input)
     *
     * @see org.eclipse.swt.SWT#MOVEMENT_WORD
     * @see org.eclipse.swt.SWT#MOVEMENT_WORD_END
     * @see org.eclipse.swt.SWT#MOVEMENT_WORD_START
     * @see org.eclipse.swt.SWT#MOVEMENT_CHAR
     * @see org.eclipse.swt.SWT#MOVEMENT_CLUSTER
     */
    public int movement;

    static final long serialVersionUID = 3978765487853324342L;

/**
 * Constructs a new instance of this class based on the
 * information in the given event.
 *
 * @param e the event containing the information
 */
public MovementEvent(StyledTextEvent e) {
    super(e);
    lineOffset = e.detail;
    lineText = e.text;
    movement = e.count;
    offset = e.start;
    newOffset = e.end;
}
}


