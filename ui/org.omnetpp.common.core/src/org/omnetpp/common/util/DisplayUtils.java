/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Listener;
import org.omnetpp.common.color.ColorFactory;

public class DisplayUtils {
    public static void runNowOrAsyncInUIThread(Runnable runnable) {
        if (Display.getCurrent() == null)
            Display.getDefault().asyncExec(runnable);
        else
            runnable.run();
    }

    public static void runNowOrSyncInUIThread(Runnable runnable) {
        if (Display.getCurrent() == null)
            Display.getDefault().syncExec(runnable);
        else
            runnable.run();
    }

    /**
     * Adds an event filter in front of existing filters. This is needed if you want
     * to receive all keypresses including hotkeys, because then you want to be in
     * front of the key binding service (which is also an event filter on Display,
     * and it translates keypress events that correspond to hotkeys to something else).
     */
    public static void addAsFirstFilter(int eventType, Listener listener) {
        // first, make room in the table (this adds the listener at the end of the table)
        Display display = Display.getDefault();
        display.addFilter(eventType, listener);

        // shift everything up, then add our listener to slot 0. Sorry about the reflection,
        // there seem to be no other way
        Object filterTable = ReflectionUtils.getFieldValue(display, "filterTable");
        int[] types = (int[]) ReflectionUtils.getFieldValue(filterTable, "types");
        Listener[] listeners = (Listener[]) ReflectionUtils.getFieldValue(filterTable, "listeners");
        System.arraycopy(types, 0, types, 1, types.length-1);
        System.arraycopy(listeners, 0, listeners, 1, listeners.length-1);
        types[0] = eventType;
        listeners[0] = listener;
        for (int i = 1; i < types.length; i++) {
            if (types[i] == eventType && listeners[i] == listener) {
                types[i] = 0; listeners[i] = null; break; // remove instance added first
            }
        }
    }

	public static boolean isDarkTheme() {
		Display display= Display.getCurrent();
		if (display == null) 
			return false; 
		Color bgColor = display.getSystemColor(SWT.COLOR_WIDGET_BACKGROUND);
		double brightness = bgColor.getRed()*0.299 + bgColor.getGreen()*0.587 + bgColor.getBlue()*0.114;
		return brightness < 128.0;
	}

	public static Color getForegroundColor() {
		var display = Display.getCurrent();
		if (display == null)
			return ColorFactory.BLACK;

		return display.getSystemColor(SWT.COLOR_WIDGET_FOREGROUND); 
	}
}
