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
import org.eclipse.ui.INavigationLocation;
import org.eclipse.ui.INavigationLocationProvider;
import org.eclipse.ui.IPathEditorInput;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.eclipse.ui.views.properties.IPropertySheetPage;
import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySourceProvider;
import org.eclipse.ui.views.properties.PropertySheetPage;
import org.omnetpp.common.simulation.ConnectionModel;
import org.omnetpp.common.simulation.ConnectionModelPropertySource;
import org.omnetpp.common.simulation.GateModel;
import org.omnetpp.common.simulation.GateModelPropertySource;
import org.omnetpp.common.simulation.MessageModel;
import org.omnetpp.common.simulation.MessageModelPropertySource;
import org.omnetpp.common.simulation.ModuleModel;
import org.omnetpp.common.simulation.ModuleModelPropertySource;
import org.omnetpp.common.simulation.QueueModel;
import org.omnetpp.common.simulation.QueueModelPropertySource;
import org.omnetpp.common.util.DetailedPartInitException;
import org.omnetpp.eventlog.engine.EventLog;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.EventLogFacade;
import org.omnetpp.eventlog.engine.FileReader;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.SimulationBeginEntry;

/**
 * Serves as a base class for editors which show an event log file.
 */
public abstract class EventLogEditor extends EditorPart implements IEventLogProvider, INavigationLocationProvider, IFollowSelectionSupport {
    protected boolean followSelection;

    protected Runnable locationTimer;

    protected EventLogInput eventLogInput;

    protected INavigationLocation lastLocation;

    protected PropertySheetPage propertySheetPage;

    public boolean getFollowSelection() {
        return followSelection;
    }

    public void setFollowSelection(boolean followSelection) {
        this.followSelection = followSelection;
    }

    public IEventLog getEventLog() {
        return eventLogInput.getEventLog();
    }

    public EventLogFacade getSequenceChartFacade() {
        return eventLogInput.getSequenceChartFacade();
    }

    public IFile getFile() {
        return eventLogInput.getFile();
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
                    catch (RuntimeException x) {
                        if (eventLogInput.isFileChangedException(x))
                            eventLogInput.synchronize(x);
                        else
                            throw x;
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
            IEventLog eventLog = new EventLog(new FileReader(logFileName, /* EventLog will delete it */false));
            eventLogInput = new EventLogInput(file, eventLog);
        }
        catch (RuntimeException e) {
            throw new PartInitException(e.getMessage(), e);
        }
    }

    @Override
    public Object getAdapter(@SuppressWarnings("rawtypes") Class key) {
        if (key.equals(IPropertySheetPage.class)) {
            if (propertySheetPage == null) {
                propertySheetPage = new PropertySheetPage();
                propertySheetPage.setPropertySourceProvider(new IPropertySourceProvider() {
                    public IPropertySource getPropertySource(Object object) {
                        // TODO: kill this case and let the others live as soon as
                        // IEventLogSelection does not return the event numbers only and we can get more than that here
                        if (object instanceof IPropertySource)
                            return (IPropertySource)object;
                        else if (object instanceof Long) {
                            IEvent event = eventLogInput.getEventLog().getEventForEventNumber((Long)object);
                            if (event == null)
                                return null;
                            else
                                return new EventLogEntryPropertySource(event.getEventEntry());
                        }
                        else if (object instanceof EventLogEntry)
                            return new EventLogEntryPropertySource((EventLogEntry)object);
                        else if (object instanceof IEvent)
                            return new EventLogEntryPropertySource(((IEvent)object).getEventEntry());
                        else if (object instanceof ModuleModel)
                            return new ModuleModelPropertySource(this, (ModuleModel)object);
                        else if (object instanceof ConnectionModel)
                            return new ConnectionModelPropertySource(this, (ConnectionModel)object);
                        else if (object instanceof GateModel)
                            return new GateModelPropertySource(this, (GateModel)object);
                        else if (object instanceof MessageModel)
                            return new MessageModelPropertySource(this, (MessageModel)object);
                        else if (object instanceof QueueModel)
                            return new QueueModelPropertySource(this, (QueueModel)object);
                        else
                            return null;
                    }
                });
            }

            return propertySheetPage;
        }
        else {
            return super.getAdapter(key);
        }
    }

    @Override
    public String getTitleToolTip() {
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

    @Override
    public void dispose() {
        super.dispose();

        if (eventLogInput != null)
            eventLogInput.dispose();
    }

    protected void addLocationProviderPaintListener(Control control) {
        control.addPaintListener(new PaintListener() {
            public void paintControl(PaintEvent e) {
                try {
                    if (eventLogInput != null && canCreateNavigationLocation()) {
                        INavigationLocation currentLocation = createNavigationLocation();

                        if (currentLocation != null && !currentLocation.equals(lastLocation)) {
                            lastLocation = currentLocation;
                            Display.getDefault().timerExec(3000, locationTimer);
                        }
                    }
                }
                catch (RuntimeException x) {
                    if (eventLogInput.isFileChangedException(x))
                        eventLogInput.synchronize(x);
                    else
                        throw x;
                }
            }
        });
    }

    protected boolean canCreateNavigationLocation() {
        return !eventLogInput.getEventLog().isEmpty();
    }

    public void markLocation() {
        Assert.isTrue(Display.getCurrent() != null);
        getSite().getPage().getNavigationHistory().markLocation(this);
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
