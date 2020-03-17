/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common;

import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.Callable;

import org.apache.commons.lang3.ArrayUtils;
import org.omnetpp.common.util.StringUtils;

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
    public static final String DEBUG_ENVVAR = "OMNETPP_DEBUG";

    private static Set<String> queriedChannels = new HashSet<String>();

    public static boolean isDebugging() {
        return debug;
    }

    public static boolean isChannelEnabled(String channel) {
        String[] channels = StringUtils.nullToEmpty(System.getenv(DEBUG_ENVVAR)).split(",");
        boolean result = ArrayUtils.contains(channels, channel) || ArrayUtils.contains(channels, "all");
        if (!queriedChannels.contains(channel)) {
            println("debug channel: " + channel + (result ? " : enabled" : " : disabled"));
            queriedChannels.add(channel);
        }
        return result;
    }

    static {
        // test property: eclipse.launcher=C:\eclipse\eclipse.exe
        String launcher = System.getProperty("eclipse.launcher");
        debug = StringUtils.isNotEmpty(System.getenv(DEBUG_ENVVAR)) || (launcher!=null && (launcher.endsWith("eclipse") || launcher.endsWith("eclipse.exe")));
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

    /**
     * Measure the runtime of a code fragment, and print the result if it's
     * over a given threshold.
     */
    public static void time(String what, long thresholdMillis, Runnable runnable) {
        time(what, true, thresholdMillis, runnable);
    }

    /**
     * Measure the runtime of a code fragment, and print the result if it's
     * over a given threshold. Measurement is conditional on the 'debug' argument.
     */
    public static void time(String what, boolean debug, long thresholdMillis, Runnable runnable) {
        if (!debug) {
            runnable.run();
            return;
        }

        long start = System.nanoTime();
        try {
            runnable.run();
        }
        finally {
            long durationNanos = System.nanoTime() - start;
            if (durationNanos > 1000000*thresholdMillis)
                Debug.println(what + ": took " + formatNanos(durationNanos));
        }
    }

    /**
     * Measure the runtime of a code fragment  that returns a value, and
     * print the result if it's over a given threshold.
     */
    public static <T> T timed(String what, long thresholdMillis, Callable<T> callable) {
        return timed(what, true, thresholdMillis, callable);
    }

    /**
     * Measure the runtime of a code fragment that returns a value, and print
     * the result if it's over a given threshold. Measurement is conditional
     * on the 'debug' argument.
     */
    public static <T> T timed(String what, boolean debug, long thresholdMillis, Callable<T> callable) {
        if (!debug) {
            try {
                return callable.call();
            }
            catch (RuntimeException e) {
                throw e;
            }
            catch (Exception e) {
                throw new RuntimeException(e);
            }
        }

        long start = System.nanoTime();
        try {
            return callable.call();
        }
        catch (RuntimeException e) {
            throw e;
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
        finally {
            long durationNanos = System.nanoTime() - start;
            if (durationNanos > 1000000*thresholdMillis)
                Debug.println(what + ": took " + formatNanos(durationNanos));
        }
    }

    private static String formatNanos(long durationNanos) {
        if (durationNanos < 10000000) // 10ms
            return (durationNanos/1000000f+"00000").substring(0,5) + "ms";
        else
            return (durationNanos+500000)/1000000 + "ms";
    }


}
