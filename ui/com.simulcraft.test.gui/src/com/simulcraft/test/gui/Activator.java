package com.simulcraft.test.gui;

import java.io.IOException;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.bindings.keys.ParseException;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

import com.simulcraft.test.gui.core.KeyPressAnimator;
import com.simulcraft.test.gui.core.KeyboardLayout;
import com.simulcraft.test.gui.core.MouseClickAnimator;

/**
 * The activator class controls the plug-in life cycle
 */
public class Activator extends AbstractUIPlugin {
    // The plug-in ID
    public static String PLUGIN_ID;

    // The shared instance
    private static Activator plugin;
    
    private KeyboardLayout keyboardLayout = new KeyboardLayout();

    
    /**
     * The constructor
     */
    public Activator() {
        plugin = this;
    }

    /*
     * (non-Javadoc)
     * @see org.eclipse.ui.plugin.AbstractUIPlugin#start(org.osgi.framework.BundleContext)
     */
    public void start(BundleContext context) throws Exception {
        super.start(context);
        PLUGIN_ID = getBundle().getSymbolicName();

        // note: cannot do loadOrTestKeyboardLayout() here, because SWT is not yet ready (sort of)
        
        KeyPressAnimator keyPressAnimator = new KeyPressAnimator();
        Display.getDefault().addFilter(SWT.KeyDown, keyPressAnimator);
        Display.getDefault().addFilter(SWT.KeyUp, keyPressAnimator);
        Display.getDefault().addFilter(SWT.MouseDown, new MouseClickAnimator());
    }

    /*
     * (non-Javadoc)
     * @see org.eclipse.ui.plugin.AbstractUIPlugin#stop(org.osgi.framework.BundleContext)
     */
    public void stop(BundleContext context) throws Exception {
        plugin = null;
        super.stop(context);
    }

    /**
     * Returns the shared instance
     * @return the shared instance
     */
    public static Activator getDefault() {
        return plugin;
    }

    public KeyboardLayout getKeyboardLayout() {
        return keyboardLayout;
    }

    public void logError(Throwable exception) {
        getLog().log(new Status(IStatus.ERROR, PLUGIN_ID, 0, exception.toString(), exception));
    }
    
    public void loadOrTestKeyboardLayout() {
        String filename = getStateLocation().append("keyboard-layout").toOSString();
        try {
            // exceptions will cause keyboardLayout to remain (or become) empty
            keyboardLayout.loadMapping(filename);
        }
        catch (IOException e) {
            // probably just file not found -- don't log it
        }
        catch (ParseException e) {
            logError(e);
        }

        // if we couldn't load it, re-test keyboard
        if (keyboardLayout.isEmpty()) {
            keyboardLayout.testKeyboard();

            try {
                keyboardLayout.saveMapping(filename);
            }
            catch (IOException e) {
                logError(e); // could not save the result -- will have to re-test it next time as well
            }
        }
    }
    
}
