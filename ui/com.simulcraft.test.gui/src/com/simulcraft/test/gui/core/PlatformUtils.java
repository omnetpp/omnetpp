package com.simulcraft.test.gui.core;

import junit.framework.Assert;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.util.ReflectionUtils;

public class PlatformUtils {
    public final static boolean isGtk = "gtk".equals(SWT.getPlatform());
    public final static boolean isWindows = "win32".equals(SWT.getPlatform());
    public final static boolean isCarbon = "carbon".equals(SWT.getPlatform());
    
    public final static Class<?> OS_ = classForName("org.eclipse.swt.internal." + SWT.getPlatform() + ".OS"); 

    private static int _XKeycodeToKeysym;
    
    public static boolean hasPendingUIEvents() {
        // some method like this is supposed to be part of SWT's Display class
        if (isWindows) {
            // return OS.PeekMessage(new MSG(), 0, 0, 0, OS.PM_NOREMOVE);
            return (Boolean)ReflectionUtils.invokeStaticMethod(OS_, "PeekMessage", ReflectionUtils.newInstance("org.eclipse.swt.internal.win32.MSG"), 0, 0, 0, 0x0 /*OS.PM_NOREMOVE*/);
        }
        else if (isGtk) {
            // return OS._g_main_context_pending(0);
            return (Boolean)ReflectionUtils.invokeStaticMethod(OS_, "_g_main_context_pending", 0);
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
	        short vkKeyScan = (Short)ReflectionUtils.invokeStaticMethod(OS_, "VkKeyScan", (short)(int)(Integer)ReflectionUtils.invokeStaticMethod(display, "wcsToMbcs", ch, 0)); 
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

        	// 1. determine keysym (code largely copied out of Display.post(Event))
        	int keysym;
       		switch (ch) {
        		case SWT.BS:  keysym = (Integer) ReflectionUtils.getFieldValue(OS_, "GDK_BackSpace"); break;
        		case SWT.CR:  keysym = (Integer) ReflectionUtils.getFieldValue(OS_, "GDK_Return"); break;
        		case SWT.DEL: keysym = (Integer) ReflectionUtils.getFieldValue(OS_, "GDK_Delete"); break;
        		case SWT.ESC: keysym = (Integer) ReflectionUtils.getFieldValue(OS_, "GDK_Escape"); break;
        		case SWT.TAB: keysym = (Integer) ReflectionUtils.getFieldValue(OS_, "GDK_Tab"); break;
        		case SWT.LF:  keysym = (Integer) ReflectionUtils.getFieldValue(OS_, "GDK_Linefeed"); break;
        		default: keysym = (Character) ReflectionUtils.invokeStaticMethod(Display.class, "wcsToMbcs", ch);
       		}

       		// 2. get keyCode for keysym
       		int xDisplay = (Integer) ReflectionUtils.invokeStaticMethod(OS_, "GDK_DISPLAY");
       		int keyCode = (Integer) ReflectionUtils.invokeStaticMethod(OS_, "XKeysymToKeycode", xDisplay, keysym);
       		if (keyCode == 0) 
       			return 0;  // sorry, cannot be produced on the keyboard

       		// get a pointer to the XKeycodeToKeysym() function in X11, as that one isn't included in SWT's GTK-specific OS class...
        	if (_XKeycodeToKeysym == 0) {
        	    Class<?> Converter_ = classForName("org.eclipse.swt.internal.Converter");
        		byte[] filename = (byte[]) ReflectionUtils.invokeStaticMethod(Converter_, "wcsToMbcs", "UTF8", "libX11.so", true);
        		int libX11 = (Integer) ReflectionUtils.invokeStaticMethod(OS_, "dlopen", filename, 1 /*OS.RTLD_LAZY*/ );
        		Assert.assertTrue("cannot load libX11.so", libX11 != 0);
        		byte[] symbol = (byte[]) ReflectionUtils.invokeStaticMethod(Converter_, "wcsToMbcs", "UTF8", "XKeycodeToKeysym", true);
        		_XKeycodeToKeysym = (Integer) ReflectionUtils.invokeStaticMethod(OS_, "dlsym", libX11, symbol);
        		Assert.assertTrue("cannot find XKeycodeToKeysym in libX11.so", _XKeycodeToKeysym != 0);
        	}

        	// 3. look up our keysym in the keysyms that keyCode can generate
       		int column;
       		int k;
       		final int NoSymbol = 0; // from X.h
       		for (column = 0; (k = (Integer) ReflectionUtils.invokeStaticMethod(OS_, "Call", _XKeycodeToKeysym, xDisplay, keyCode, column)) != NoSymbol; column++) 
       			if (k == keysym)
       				break;

       		//System.out.printf("MODIFIERS: char '%c' --> keysym %x --> keyCode %x --> column %d\n", ch, keysym, keyCode, column);
       		
       		// 4. devise modifiers from the column
       		int modeSwitch = 0; //FIXME ask Xlib somehow...
       		switch(column) {
	       		case 0: return 0;
	       		case 1: return SWT.SHIFT;
	       		case 2: return modeSwitch;
	       		case 3: return SWT.SHIFT | modeSwitch;
	       		default: System.out.println("cannot figure out which modifiers to hold down to produce '"+ch+"' on the keyboard"); return 0;
       		}
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
