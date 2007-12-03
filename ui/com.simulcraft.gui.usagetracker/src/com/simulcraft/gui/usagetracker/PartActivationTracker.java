package com.simulcraft.gui.usagetracker;

import org.eclipse.ui.IPartListener;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PlatformUI;

/**
 * Records part activations into the log.
 *   
 * @author Andras
 */
public class PartActivationTracker implements IWorkbenchListener, IPartListener {
    protected boolean detailed = false;

    // for eliminating "Part Activated" message just after "Part Opened"
    protected IWorkbenchPart partJustOpened; 
    
    public PartActivationTracker() {
    }
    
    public void hookListeners() {
        PlatformUI.getWorkbench().addWorkbenchListener(this);
        PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage().addPartListener(this);
    }

    public void unhookListeners() {
        PlatformUI.getWorkbench().removeWorkbenchListener(this);
        PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage().removePartListener(this);
    }

    public void partActivated(IWorkbenchPart part) {
        if (part == partJustOpened)  // to reduce amount of log messages
            Activator.logMessage("Part opened & activated: " + toString(part));
        else
            Activator.logMessage("Part activated: " + toString(part));
        partJustOpened = null;
    }

    public void partBroughtToTop(IWorkbenchPart part) {
        if (detailed)
            Activator.logMessage("Part brought to top: " + toString(part));
    }

    public void partClosed(IWorkbenchPart part) {
        Activator.logMessage("Part closed: " + toString(part));
    }

    public void partDeactivated(IWorkbenchPart part) {
        if (detailed)
            Activator.logMessage("Part deactivated: " + toString(part));
    }

    public void partOpened(IWorkbenchPart part) {
        if (detailed)
            Activator.logMessage("Part opened: " + toString(part));
        partJustOpened = part; // see logging in partActivated()
    }

    protected String toString(IWorkbenchPart part) {
        return part.getTitle() + " [" + part.getClass().getSimpleName() + "]";
    }
    
    public void postShutdown(IWorkbench workbench) {
        Activator.logMessage("Workbench shutdown completed");
    }

    public boolean preShutdown(IWorkbench workbench, boolean forced) {
        Activator.logMessage("Begin workbench " + (forced?"forced ":"")+ "shutdown");
        return true;
    }
}

