package com.simulcraft.test.gui;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

import com.simulcraft.test.gui.core.KeyPressAnimator;
import com.simulcraft.test.gui.core.ModeSwitcher;
import com.simulcraft.test.gui.core.MouseClickAnimator;

/**
 * The activator class controls the plug-in life cycle
 */
public class Activator extends AbstractUIPlugin {
    // The plug-in ID
    public static String PLUGIN_ID;

    // The shared instance
    private static Activator plugin;
    
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
        
        KeyPressAnimator keyPressAnimator = new KeyPressAnimator();
        Display.getDefault().addFilter(SWT.KeyDown, keyPressAnimator);
        Display.getDefault().addFilter(SWT.KeyUp, keyPressAnimator);
        
        Display.getDefault().addFilter(SWT.MouseDown, new MouseClickAnimator());
        
        ModeSwitcher modeSwitcher = new ModeSwitcher();
        Display.getDefault().addFilter(SWT.KeyDown, modeSwitcher);
        Display.getDefault().addFilter(SWT.KeyUp, modeSwitcher);
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

    public static void logError(Throwable exception) {
        getDefault().getLog().log(new Status(IStatus.ERROR, PLUGIN_ID, 0, exception.toString(), exception));
    }
}
