/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.eventlog;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.INavigationHistory;
import org.eclipse.ui.INavigationLocation;
import org.eclipse.ui.INavigationLocationProvider;
import org.eclipse.ui.IPathEditorInput;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.eclipse.ui.views.properties.IPropertySheetPage;
import org.eclipse.ui.views.properties.PropertySheetPage;
import org.omnetpp.common.util.DetailedPartInitException;
import org.omnetpp.eventlog.EventLog;
import org.omnetpp.eventlog.IEventLog;
import org.omnetpp.eventlog.engine.FileReader;
import org.omnetpp.eventlog.entry.SimulationBeginEntry;

/**
 * Serves as a base class for editors which show an event log file.
 */
@SuppressWarnings({"rawtypes", "unchecked"})
public abstract class EventLogEditor extends EditorPart implements IEventLogProvider, INavigationLocationProvider, IFollowSelectionSupport {
    protected boolean followSelection;
    protected Runnable locationTimer;
    protected IFile file;
    protected EventLogInput eventLogInput;
    protected INavigationLocation lastLocation;
    protected PropertySheetPage propertySheetPage;

    public PropertySheetPage getPropertySheetPage() {
        return propertySheetPage;
    }

    public boolean getFollowSelection() {
        return followSelection;
    }

    public void setFollowSelection(boolean followSelection) {
        this.followSelection = followSelection;
    }

    public IEventLog getEventLog() {
        return eventLogInput.getEventLog();
    }

    public IFile getFile() {
        return file;
    }

    @Override
    public void init(IEditorSite site, IEditorInput input) throws PartInitException {
        try {
            locationTimer = new Runnable() {
                public void run() {
                    try {
                        Assert.isTrue(Display.getCurrent() != null);
                        markLocation();
                    }
                    catch (RuntimeException e) {
                        if (!eventLogInput.isFileChangedException(e))
                            throw e;
                    }
                }
            };
            setSite(site);
            setInput(input);
            setPartName(input.getName());
            IFile file = null;
            String logFileName;
            if (input instanceof IFileEditorInput) {
                IFileEditorInput fileInput = (IFileEditorInput)input;
                file = fileInput.getFile();
                logFileName = fileInput.getFile().getLocation().toFile().getAbsolutePath();
            }
            else if (input instanceof IPathEditorInput) {
                IPathEditorInput pathFileInput = (IPathEditorInput)input;
                logFileName = pathFileInput.getPath().toFile().getAbsolutePath();
            }
            else
                throw new DetailedPartInitException("Invalid input, it must be a file in the workspace: " + input.getName(),
                    "Please make sure the project is open before trying to open a file in it.");

            if (logFileName.endsWith("elog")) {
                IEventLog eventLog = new EventLog(new FileReader(logFileName, 64 * 1024, /* EventLog will delete it */false));
                eventLogInput = new EventLogInput(file, eventLog);
            }
        }
        catch (RuntimeException e) {
            throw new PartInitException(e.getMessage(), e);
        }
    }

    @Override
    public Object getAdapter(Class key) {
        if (key.equals(IPropertySheetPage.class)) {
            if (propertySheetPage == null) {
                propertySheetPage = new PropertySheetPage();
                propertySheetPage.setPropertySourceProvider(new EventLogPropertySourceProvider(this));
            }

            return propertySheetPage;
        }
        else {
            return super.getAdapter(key);
        }
    }

    @Override
    public String getTitleToolTip() {
        if (eventLogInput == null)
            return super.getTitleToolTip();
        else {
            IEventLog eventLog = eventLogInput.getEventLog();
            if (eventLog == null)
                return super.getTitleToolTip();
            else {
                SimulationBeginEntry entry = eventLog.getSimulationBeginEntry();
                if (entry == null)
                    return super.getTitleToolTip();
                else
                    return super.getTitleToolTip() + " : " + entry.getRunId();
            }
        }
    }

    @Override
    public void dispose() {
        super.dispose();
        if (eventLogInput != null)
            eventLogInput.dispose();
    }

    protected void addLocationProviderPaintListener(Control control) {
        control.addPaintListener(new PaintListener() {
            public void paintControl(PaintEvent event) {
                try {
                    if (eventLogInput != null && canCreateNavigationLocation()) {
                        INavigationLocation currentLocation = createNavigationLocation();

                        if (currentLocation != null && !currentLocation.equals(lastLocation)) {
                            lastLocation = currentLocation;
                            Display.getDefault().timerExec(3000, locationTimer);
                        }
                    }
                }
                catch (RuntimeException e) {
                    if (!eventLogInput.isFileChangedException(e))
                        throw e;
                }
            }
        });
    }

    protected boolean canCreateNavigationLocation() {
        return eventLogInput != null && !eventLogInput.isCanceled() && !eventLogInput.getEventLog().isEmpty();
    }

    public void markLocation() {
        Assert.isTrue(Display.getCurrent() != null);
        IWorkbenchPage page = getSite().getPage();
        if (page != null) {
            INavigationHistory navigationHistory = page.getNavigationHistory();
            if (navigationHistory != null)
                navigationHistory.markLocation(this);
        }
    }

    @Override
    public void doSave(IProgressMonitor monitor) {
        // void
    }

    @Override
    public void doSaveAs() {
        // void
    }

    @Override
    public boolean isDirty() {
        return false;
    }

    @Override
    public boolean isSaveAsAllowed() {
        return false;
    }
}
