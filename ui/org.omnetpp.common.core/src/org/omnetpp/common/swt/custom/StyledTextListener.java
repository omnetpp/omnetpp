/*******************************************************************************
 * Copyright (c) 2000, 2009 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.omnetpp.common.swt.custom;

import org.eclipse.swt.events.VerifyEvent;
import org.eclipse.swt.internal.SWTEventListener;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.TypedListener;

class StyledTextListener extends TypedListener {
/**
 */
StyledTextListener(SWTEventListener listener) {
    super(listener);
}
/**
 * Process StyledText events by invoking the event's handler.
 *
 * @param e the event to handle
 */
public void handleEvent(Event e) {

    switch (e.type) {
        case StyledText.ExtendedModify:
            ExtendedModifyEvent extendedModifyEvent = new ExtendedModifyEvent((StyledTextEvent) e);
            ((ExtendedModifyListener) eventListener).modifyText(extendedModifyEvent);
            break;
        case StyledText.LineGetBackground:
            LineBackgroundEvent lineBgEvent = new LineBackgroundEvent((StyledTextEvent) e);
            ((LineBackgroundListener) eventListener).lineGetBackground(lineBgEvent);
            ((StyledTextEvent) e).lineBackground = lineBgEvent.lineBackground;
            break;
        case StyledText.LineGetSegments:
            BidiSegmentEvent segmentEvent = new BidiSegmentEvent((StyledTextEvent) e);
            ((BidiSegmentListener) eventListener).lineGetSegments(segmentEvent);
            ((StyledTextEvent) e).segments = segmentEvent.segments;
            ((StyledTextEvent) e).segmentsChars = segmentEvent.segmentsChars;
            break;
        case StyledText.LineGetStyle:
            LineStyleEvent lineStyleEvent = new LineStyleEvent((StyledTextEvent) e);
            ((LineStyleListener) eventListener).lineGetStyle(lineStyleEvent);
            ((StyledTextEvent) e).ranges = lineStyleEvent.ranges;
            ((StyledTextEvent) e).styles = lineStyleEvent.styles;
            ((StyledTextEvent) e).alignment = lineStyleEvent.alignment;
            ((StyledTextEvent) e).indent = lineStyleEvent.indent;
            ((StyledTextEvent) e).wrapIndent = lineStyleEvent.wrapIndent;
            ((StyledTextEvent) e).justify = lineStyleEvent.justify;
            ((StyledTextEvent) e).bullet = lineStyleEvent.bullet;
            ((StyledTextEvent) e).bulletIndex = lineStyleEvent.bulletIndex;
            ((StyledTextEvent) e).tabStops = lineStyleEvent.tabStops;
            break;
        case StyledText.PaintObject:
            PaintObjectEvent paintObjectEvent = new PaintObjectEvent((StyledTextEvent) e);
            ((PaintObjectListener) eventListener).paintObject(paintObjectEvent);
            break;
        case StyledText.VerifyKey:
            VerifyEvent verifyEvent = new VerifyEvent(e);
            ((VerifyKeyListener) eventListener).verifyKey(verifyEvent);
            e.doit = verifyEvent.doit;
            break;
        case StyledText.TextChanged: {
            TextChangedEvent textChangedEvent = new TextChangedEvent((StyledTextContent) e.data);
            ((TextChangeListener) eventListener).textChanged(textChangedEvent);
            break;
        }
        case StyledText.TextChanging:
            TextChangingEvent textChangingEvent = new TextChangingEvent((StyledTextContent) e.data, (StyledTextEvent) e);
            ((TextChangeListener) eventListener).textChanging(textChangingEvent);
            break;
        case StyledText.TextSet: {
            TextChangedEvent textChangedEvent = new TextChangedEvent((StyledTextContent) e.data);
            ((TextChangeListener) eventListener).textSet(textChangedEvent);
            break;
        }
        case StyledText.WordNext: {
            MovementEvent wordBoundaryEvent = new MovementEvent((StyledTextEvent) e);
            ((MovementListener) eventListener).getNextOffset(wordBoundaryEvent);
            ((StyledTextEvent) e).end = wordBoundaryEvent.newOffset;
            break;
        }
        case StyledText.WordPrevious: {
            MovementEvent wordBoundaryEvent = new MovementEvent((StyledTextEvent) e);
            ((MovementListener) eventListener).getPreviousOffset(wordBoundaryEvent);
            ((StyledTextEvent) e).end = wordBoundaryEvent.newOffset;
            break;
        }
        case StyledText.CaretMoved: {
            CaretEvent caretEvent = new CaretEvent((StyledTextEvent) e);
            ((CaretListener) eventListener).caretMoved(caretEvent);
            ((StyledTextEvent) e).end = caretEvent.caretOffset;
            break;
        }
    }
}
}


