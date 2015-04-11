/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package com.simulcraft.test.gui.core;

import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;

public class EventTracer {


    public static void start() {
        System.out.println("startDumpingEvents: hooking event filters");

        Listener eventFilter = new Listener() {
            public void handleEvent(Event event) {
                System.out.println(" - " + EventUtils.EventType.lookup(event.type) + ": " + event.toString());
            }
        };

        for (EventUtils.EventType eventType : EventUtils.EventType.getMouseEventTypes())
            Display.getDefault().addFilter(eventType.getTypeCode(), eventFilter);
        for (EventUtils.EventType eventType : EventUtils.EventType.getKeyboardEventTypes())
            Display.getDefault().addFilter(eventType.getTypeCode(), eventFilter);
    }
}
