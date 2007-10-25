package com.simulcraft.test.gui.core;

import org.eclipse.swt.SWT;
import org.omnetpp.common.util.ReflectionUtils;

public class PlatformUtils {
    public final static boolean isGtk = "gtk".equals(SWT.getPlatform());
    public final static boolean isWindows = "win32".equals(SWT.getPlatform());
    public final static boolean isCarbon = "carbon".equals(SWT.getPlatform());
    
    private static Class<?> OS_class = classForName("org.eclipse.swt.internal." + SWT.getPlatform() + ".OS"); 
    
    public static boolean hasPendingUIEvents() {
        // some method like this is supposed to be part of SWT's Display class
        if (isWindows) {
            // return OS.PeekMessage(new MSG(), 0, 0, 0, OS.PM_NOREMOVE);
            return (Boolean)ReflectionUtils.invokeStaticMethod(OS_class, "PeekMessage", ReflectionUtils.newInstance("org.eclipse.swt.internal.win32.MSG"), 0, 0, 0, 0x0 /*OS.PM_NOREMOVE*/);
        }
        else if (isGtk) {
            // return OS._g_main_context_pending(0);
            return (Boolean)ReflectionUtils.invokeStaticMethod(OS_class, "_g_main_context_pending", 0);
        }
        else {
        	throw new RuntimeException("unsupported window system: " + SWT.getPlatform());
        }
    }
    
    private static Class<?> classForName(String className) {
        try {
            return Class.forName(className);
        }
        catch (ClassNotFoundException e) {
            throw new RuntimeException(e);
        }
    }

}
