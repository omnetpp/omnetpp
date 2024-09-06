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
        return Display.isSystemDarkTheme();
    }

    /*
     * Dumps SWT system colors to the console.
     */
    public static void dumpSwtSystemColors() {
        Object[][] colorConstants = {
            {SWT.COLOR_BLACK, "COLOR_BLACK"},
            {SWT.COLOR_WHITE, "COLOR_WHITE"},
            {SWT.COLOR_RED, "COLOR_RED"},
            {SWT.COLOR_DARK_RED, "COLOR_DARK_RED"},
            {SWT.COLOR_GREEN, "COLOR_GREEN"},
            {SWT.COLOR_DARK_GREEN, "COLOR_DARK_GREEN"},
            {SWT.COLOR_YELLOW, "COLOR_YELLOW"},
            {SWT.COLOR_DARK_YELLOW, "COLOR_DARK_YELLOW"},
            {SWT.COLOR_BLUE, "COLOR_BLUE"},
            {SWT.COLOR_DARK_BLUE, "COLOR_DARK_BLUE"},
            {SWT.COLOR_MAGENTA, "COLOR_MAGENTA"},
            {SWT.COLOR_DARK_MAGENTA, "COLOR_DARK_MAGENTA"},
            {SWT.COLOR_CYAN, "COLOR_CYAN"},
            {SWT.COLOR_DARK_CYAN, "COLOR_DARK_CYAN"},
            {SWT.COLOR_GRAY, "COLOR_GRAY"},
            {SWT.COLOR_DARK_GRAY, "COLOR_DARK_GRAY"},

    	    {SWT.COLOR_WIDGET_DARK_SHADOW, "COLOR_WIDGET_DARK_SHADOW"},
    	    {SWT.COLOR_WIDGET_NORMAL_SHADOW, "COLOR_WIDGET_NORMAL_SHADOW"},
    	    {SWT.COLOR_WIDGET_LIGHT_SHADOW, "COLOR_WIDGET_LIGHT_SHADOW"},
    	    {SWT.COLOR_WIDGET_HIGHLIGHT_SHADOW, "COLOR_WIDGET_HIGHLIGHT_SHADOW"},
    	    {SWT.COLOR_WIDGET_FOREGROUND, "COLOR_WIDGET_FOREGROUND"},
    	    {SWT.COLOR_WIDGET_BACKGROUND, "COLOR_WIDGET_BACKGROUND"},
    	    {SWT.COLOR_WIDGET_BORDER, "COLOR_WIDGET_BORDER"},
    	    {SWT.COLOR_LIST_FOREGROUND, "COLOR_LIST_FOREGROUND"},
    	    {SWT.COLOR_LIST_BACKGROUND, "COLOR_LIST_BACKGROUND"},
    	    {SWT.COLOR_LIST_SELECTION, "COLOR_LIST_SELECTION"},
    	    {SWT.COLOR_LIST_SELECTION_TEXT, "COLOR_LIST_SELECTION_TEXT"},
    	    {SWT.COLOR_INFO_FOREGROUND, "COLOR_INFO_FOREGROUND"},
    	    {SWT.COLOR_INFO_BACKGROUND, "COLOR_INFO_BACKGROUND"},
    	    {SWT.COLOR_TITLE_FOREGROUND, "COLOR_TITLE_FOREGROUND"},
    	    {SWT.COLOR_TITLE_BACKGROUND, "COLOR_TITLE_BACKGROUND"},
    	    {SWT.COLOR_TITLE_BACKGROUND_GRADIENT, "COLOR_TITLE_BACKGROUND_GRADIENT"},
    	    {SWT.COLOR_TITLE_INACTIVE_FOREGROUND, "COLOR_TITLE_INACTIVE_FOREGROUND"},
    	    {SWT.COLOR_TITLE_INACTIVE_BACKGROUND, "COLOR_TITLE_INACTIVE_BACKGROUND"},
    	    {SWT.COLOR_TITLE_INACTIVE_BACKGROUND_GRADIENT, "COLOR_TITLE_INACTIVE_BACKGROUND_GRADIENT"},
    	    {SWT.COLOR_LINK_FOREGROUND, "COLOR_LINK_FOREGROUND"},
    	    {SWT.COLOR_TRANSPARENT, "COLOR_TRANSPARENT"},
    	    {SWT.COLOR_TEXT_DISABLED_BACKGROUND, "COLOR_TEXT_DISABLED_BACKGROUND"},
    	    {SWT.COLOR_WIDGET_DISABLED_FOREGROUND, "COLOR_WIDGET_DISABLED_FOREGROUND"},
        };
        
        var display = Display.getCurrent();
        // Print the name and RGB values of the system colors
        for (Object[] colorConstant : colorConstants) {
            int colorId = (int) colorConstant[0];
            String colorName = (String) colorConstant[1];
            Color color = display.getSystemColor(colorId);
            System.out.println("Color Name: " + colorName + ", RGB: " 
                               + color.getRed() + "," + color.getGreen() + "," + color.getBlue());
        }

    }
}
