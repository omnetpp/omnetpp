/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.eventlogtable.editors;

import java.lang.reflect.InvocationTargetException;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.operation.IRunnableContext;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.INavigationLocation;
import org.eclipse.ui.INavigationLocationProvider;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.contexts.IContextService;
import org.eclipse.ui.ide.IGotoMarker;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.part.IShowInSource;
import org.eclipse.ui.part.IShowInTargetList;
import org.eclipse.ui.part.ShowInContext;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.eventlog.EventLogEditor;
import org.omnetpp.common.eventlog.EventLogEntryReference;
import org.omnetpp.common.eventlog.IEventLogSelection;
import org.omnetpp.common.ui.TimeTriggeredProgressMonitorDialog2;
import org.omnetpp.eventlog.IEvent;
import org.omnetpp.eventlogtable.EventLogTablePlugin;
import org.omnetpp.eventlogtable.widgets.EventLogTable;

/**
 * Event log table display tool. (It is not actually an editor; it is only named so
 * because it extends EditorPart).
 *
 * @author levy
 */
public class EventLogTableEditor
    extends EventLogEditor
    implements IEventLogTableProvider, INavigationLocationProvider, IGotoMarker, IShowInSource, IShowInTargetList
{
    private ResourceChangeListener resourceChangeListener = new ResourceChangeListener();

    EventLogTable eventLogTable;

    private ISelectionListener selectionListener;

    public EventLogTable getEventLogTable() {
        return eventLogTable;
    }

    @Override
    public void init(IEditorSite site, IEditorInput input) throws PartInitException {
        super.init(site, input);
        ResourcesPlugin.getWorkspace().addResourceChangeListener(resourceChangeListener);

        IContextService contextService = (IContextService)site.getService(IContextService.class);
        contextService.activateContext("org.omnetpp.context.EventLogTable");

        // try to open the log view after all events are processed
        Display.getDefault().asyncExec(new Runnable() {
            @Override
            public void run() {
                // try to open the sequence chart view
                try {
                    // Eclipse feature: during startup, showView() throws "Abnormal Workbench Condition" because perspective is null
                    if (getSite().getPage().getPerspective() != null)
                        getSite().getPage().showView("org.omnetpp.sequencechart.editors.SequenceChartView", null, IWorkbenchPage.VIEW_VISIBLE);
                }
                catch (PartInitException e) {
                    EventLogTablePlugin.getDefault().logException(e);
                }
            }
        });
    }

    @Override
    public void dispose() {
        if (resourceChangeListener != null)
            ResourcesPlugin.getWorkspace().removeResourceChangeListener(resourceChangeListener);
        if (selectionListener != null)
            getSite().getPage().removeSelectionListener(selectionListener);
        getSite().setSelectionProvider(null);
        super.dispose();
    }

    @Override
    public void createPartControl(final Composite parent) {
        eventLogTable = new EventLogTable(parent, SWT.NONE);
        eventLogTable.setInput(eventLogInput);
        eventLogTable.setWorkbenchPart(this);
        eventLogTable.setRunnableContextForLongRunningOperations(new IRunnableContext() {
            @Override
            public void run(boolean fork, boolean cancelable, IRunnableWithProgress runnable) throws InvocationTargetException, InterruptedException {
                TimeTriggeredProgressMonitorDialog2 dialog = new TimeTriggeredProgressMonitorDialog2(parent.getShell(), 1000);
                dialog.setCancelable(cancelable);
                dialog.run(fork, cancelable, runnable);
            }
        });


        getSite().setSelectionProvider(eventLogTable);
        addLocationProviderPaintListener(eventLogTable.getCanvas());

        // follow selection
        selectionListener = new ISelectionListener() {
            public void selectionChanged(IWorkbenchPart part, ISelection selection) {
                if (followSelection && part != EventLogTableEditor.this && selection instanceof IEventLogSelection) {
                    IEventLogSelection eventLogSelection = (IEventLogSelection)selection;

                    if (eventLogSelection.getEventLogInput() == eventLogTable.getInput()) {
                        eventLogTable.setSelection(selection);
                        markLocation();
                    }
                }
            }
        };
        getSite().getPage().addSelectionListener(selectionListener);
    }

    @Override
    public void setFocus() {
        eventLogTable.setFocus();
    }

    public INavigationLocation createEmptyNavigationLocation() {
        return new EventLogTableLocation(this, 0);
    }

    @Override
    protected boolean canCreateNavigationLocation() {
        return !eventLogTable.isDisposed() && super.canCreateNavigationLocation();
    }

    public INavigationLocation createNavigationLocation() {
        try {
            if (!canCreateNavigationLocation())
                return null;
            else {
                EventLogEntryReference eventLogEntryReference = eventLogTable.getTopVisibleElement();
                if (eventLogEntryReference == null)
                    return null;
                else
                    return new EventLogTableLocation(this, eventLogEntryReference.getEventNumber());
            }
        }
        catch (RuntimeException e) {
            eventLogInput.handleRuntimeException(e);
            return null;
        }
    }

    public void gotoMarker(IMarker marker) {
        long eventNumber = Long.parseLong(marker.getAttribute("EventNumber", "-1"));
        IEvent event = eventLogInput.getEventLog().getEventForEventNumber(eventNumber);

        if (event != null)
            eventLogTable.gotoElement(new EventLogEntryReference(event.getEventEntry()));
    }

    private class ResourceChangeListener implements IResourceChangeListener, IResourceDeltaVisitor {
        public void resourceChanged(IResourceChangeEvent event) {
            try {
                // close editor on project close
                if (event.getType() == IResourceChangeEvent.PRE_CLOSE) {
                    final IEditorPart thisEditor = EventLogTableEditor.this;
                    final IResource resource = event.getResource();
                    Display.getDefault().asyncExec(new Runnable(){
                        public void run(){
                            if (((FileEditorInput)thisEditor.getEditorInput()).getFile().getProject().equals(resource)) {
                                thisEditor.getSite().getPage().closeEditor(thisEditor, true);
                            }
                        }
                    });
                }

                IResourceDelta delta = event.getDelta();

                if (delta != null)
                    delta.accept(this);
            }
            catch (CoreException e) {
                throw new RuntimeException(e);
            }
        }

        public boolean visit(IResourceDelta delta) {
            // TODO: why do we redraw here, the eventlog is not reread automatically!
            if (delta != null && delta.getResource() != null && delta.getResource().equals(eventLogInput.getFile())) {
                Display.getDefault().asyncExec(new Runnable() {
                    public void run() {
                        try {
                            if (!eventLogTable.isDisposed())
                                eventLogTable.redraw();
                        }
                        catch (RuntimeException e) {
                            eventLogTable.handleRuntimeException(e);
                        }
                    }
                });
            }

            return true;
        }
    }

    /* (non-Javadoc)
     * Method declared on IShowInSource
     */
    public ShowInContext getShowInContext() {
        return new ShowInContext(getEditorInput(), getSite().getSelectionProvider().getSelection());
    }

    /* (non-Javadoc)
     * Method declared on IShowInTargetList
     */
    public String[] getShowInTargetIds() {
        // contents of the "Show In..." context menu
        return new String[] {
                IConstants.SEQUENCECHART_VIEW_ID,
                IConstants.ANIMATION_VIEW_ID,
                //TODO IConstants.MODULEHIERARCHY_VIEW_ID,
                };
    }
}
