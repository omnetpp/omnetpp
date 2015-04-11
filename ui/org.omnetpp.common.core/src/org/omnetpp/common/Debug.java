/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common;


/**
 * Debug helper class, to be used instead of System.out.println().
 * Output is automatically disabled when the app is run standalone (i.e. not from Eclipse).
 *
 * There is also an org.eclipse.osgi.framework.debug.Debug class but
 * it does not do this trick.
 *
 * @author Andras
 */
public class Debug {
    public static boolean debug;

    public static boolean isDebugging() {
        return debug;
    }

    static {
        // test property: eclipse.launcher=C:\eclipse\eclipse.exe
        String launcher = System.getProperty("eclipse.launcher");
        debug = launcher!=null && (launcher.endsWith("eclipse") || launcher.endsWith("eclipse.exe"));
    }

    public static void print(String text) {
        if (isDebugging())
            System.out.print(text);
    }

    public static void println(String text) {
        if (isDebugging())
            System.out.println(text);
    }

    public static void println() {
        if (isDebugging())
            System.out.println();
    }

    public static void printf(String format, Object... args) {
        if (isDebugging())
            System.out.printf(format, args);
    }

    public static void format(String format, Object... args) {
        if (isDebugging())
            System.out.format(format, args);
    }

}
