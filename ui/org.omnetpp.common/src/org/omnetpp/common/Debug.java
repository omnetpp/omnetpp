package org.omnetpp.common.util;


public class Debug {
    public static boolean isDebugging() {
        // use property: eclipse.launcher=C:\eclipse\eclipse.exe
        String launcher = System.getProperty("eclipse.launcher");
        return launcher!=null && (launcher.endsWith("eclipse") || launcher.endsWith("eclipse.exe"));
    }
    
    public static void print(String text) {
        if (isDebugging())
            System.out.print(text);
    }

    public static void println(String text) {
        if (isDebugging())
            System.out.println(text);
    }
    
    public static void printf(String format, Object... args) {
        if (isDebugging())
            System.out.printf(format, args);
    }
}
