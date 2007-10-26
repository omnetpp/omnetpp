package com.simulcraft.test.gui.core;

import org.eclipse.swt.SWT;
import org.omnetpp.common.util.ReflectionUtils;

public class PlatformUtils {
    public final static boolean isGtk = "gtk".equals(SWT.getPlatform());
    public final static boolean isWindows = "win32".equals(SWT.getPlatform());
    public final static boolean isCarbon = "carbon".equals(SWT.getPlatform());
    
    public final static Class<?> OS = classForName("org.eclipse.swt.internal." + SWT.getPlatform() + ".OS"); 
    
    public static boolean hasPendingUIEvents() {
        // some method like this is supposed to be part of SWT's Display class
        if (isWindows) {
            // return OS.PeekMessage(new MSG(), 0, 0, 0, OS.PM_NOREMOVE);
            return (Boolean)ReflectionUtils.invokeStaticMethod(OS, "PeekMessage", ReflectionUtils.newInstance("org.eclipse.swt.internal.win32.MSG"), 0, 0, 0, 0x0 /*OS.PM_NOREMOVE*/);
        }
        else if (isGtk) {
            // return OS._g_main_context_pending(0);
            return (Boolean)ReflectionUtils.invokeStaticMethod(OS, "_g_main_context_pending", 0);
        }
        else {
        	throw new RuntimeException("unsupported window system: " + SWT.getPlatform());
        }
    }
    
	public static int getModifiersFor(char ch) {
	    if (isWindows) {
	        // see VkKeyScan documentation in the Windows API, and usage in Display.post()
            // short vkKeyScan = OS.VkKeyScan((short)wcsToMbcs(ch, 0));
	        Class<?> display = classForName("org.eclipse.swt.widgets.Display"); 
	        short vkKeyScan = (Short)ReflectionUtils.invokeStaticMethod(OS, "VkKeyScan", (short)(int)(Integer)ReflectionUtils.invokeStaticMethod(display, "wcsToMbcs", ch, 0)); 
	        byte vkModifier = (byte)(vkKeyScan >> 8);
	        int modifier = 0;
	        if ((vkModifier & 1) !=0) modifier |= SWT.SHIFT;
	        if ((vkModifier & 2) !=0) modifier |= SWT.CONTROL;
	        if ((vkModifier & 4) !=0) modifier |= SWT.ALT;
	        return modifier;
	    }
        else if (isGtk) {
            // looks like it roughly goes like this:
            // 1. determine the keysym to be produced (see code in post(Event), also copied below)
            // 2. determine keycode from keysym using XKeysymToKeycode -- this is the "main" key only
            // 3. look up what keysyms that keycode can generate, using XKeycodeToKeysym(display, keycode, index),
            //    and determine which index generates the desired keysym
            // 4. then decode index as follows:
            //      =0  None of Shift and Mode_switch are pressed
            //      =1  Shift is pressed
            //      =2  Mode_switch is pressed
            //      =3  Both Shift and Mode_switch are pressed
            // Looks like mode_switch is a keysym which has to be looked up separately.
            //
            // See also:
            //  http://homepage3.nifty.com/tsato/xvkbd/events.html
            //  http://lists.freedesktop.org/archives/xorg/2007-February/022193.html
            //  http://tronche.com/gui/x/xlib/utilities/keyboard/XKeycodeToKeysym.html
            //  http://tronche.com/gui/x/xlib/input/keyboard-encoding.html#KeySym
            //

//        	// 1. determine keysym (code largely copied out of Display.post(Event))
//        	int keysym;
//       		switch (ch) {
//        		case SWT.BS: keysym = org.eclipse.swt.internal.gtk.OS.GDK_BackSpace; break;
//        		case SWT.CR: keysym = org.eclipse.swt.internal.gtk.OS.GDK_Return; break;
//        		case SWT.DEL: keysym = org.eclipse.swt.internal.gtk.OS.GDK_Delete; break;
//        		case SWT.ESC: keysym = org.eclipse.swt.internal.gtk.OS.GDK_Escape; break;
//        		case SWT.TAB: keysym = org.eclipse.swt.internal.gtk.OS.GDK_Tab; break;
//        		case SWT.LF: keysym = org.eclipse.swt.internal.gtk.OS.GDK_Linefeed; break;
//        		default: keysym = (Integer) ReflectionUtils.invokeStaticMethod(Display.class, "wcsToMbcs", ch);
//       		}
//       		
//       		// 2. get keyCode for keysym
//       		int xDisplay = org.eclipse.swt.internal.gtk.OS.GDK_DISPLAY();
//       		int keyCode = org.eclipse.swt.internal.gtk.OS.XKeysymToKeycode(xDisplay, keysym);
//       		if (keyCode == 0) return 0;  // sorry
//
//       		// 3. look up our keysym in the keysyms that keyCode can generate
//       		int col;
//       		int k;
//       		final int NoSymbol = 0; // from X.h
//       		for (col = 0; (k = org.eclipse.swt.internal.gtk.OS.XKeycodeToKeysym(xDisplay, keyCode, col)) != NoSymbol; col++) //FIXME sucks!!! no XKeycodeToKeysym() in the OS class!
//       			if (k == keysym)
//       				break;
//       		
//       		// 4. devise modifiers from the column
//       		switch(col) {
//	       		case 0: return 0;
//	       		case 1: return SWT.SHIFT;
//	       		case 2: return Mode_Switch; //XXX whatever that is
//	       		case 3: return SWT.SHIFT | Mode_Switch;  //XXX
//       		}
        	return 0; // we don't know
        }
        else {
            throw new RuntimeException("unsupported window system: " + SWT.getPlatform());
        }
	}

	/**
	 * Class.forName without typed exceptions
	 */
    private static Class<?> classForName(String className) {
        try {
            return Class.forName(className);
        }
        catch (ClassNotFoundException e) {
            throw new RuntimeException(e);
        }
    }

}
