/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.sequencechart.editors;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
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
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.eventlog.EventLogEditor;
import org.omnetpp.common.eventlog.IEventLogSelection;
import org.omnetpp.common.eventlog.ModuleTreeItem;
import org.omnetpp.eventlog.EventLogEntry;
import org.omnetpp.eventlog.IEvent;
import org.omnetpp.eventlog.IEventLog;
import org.omnetpp.eventlog.IMessageDependency;
import org.omnetpp.eventlog.entry.ComponentMethodBeginEntry;
import org.omnetpp.sequencechart.SequenceChartPlugin;
import org.omnetpp.sequencechart.widgets.SequenceChart;

/**
 * Sequence chart display tool. (It is not actually an editor; it is only named so
 * because it extends EditorPart).
 *
 * @author levy
 */
public class SequenceChartEditor
    extends EventLogEditor
    implements ISequenceChartProvider, INavigationLocationProvider, IGotoMarker, IShowInSource, IShowInTargetList
{
    private ResourceChangeListener resourceChangeListener = new ResourceChangeListener();

    private SequenceChart sequenceChart;

    private ISelectionListener selectionListener;

    public SequenceChart getSequenceChart() {
        return sequenceChart;
    }

    @Override
    public void init(IEditorSite site, IEditorInput input) throws PartInitException {
        super.init(site, input);
        ResourcesPlugin.getWorkspace().addResourceChangeListener(resourceChangeListener);

        IContextService contextService = (IContextService)site.getService(IContextService.class);
        contextService.activateContext("org.omnetpp.context.SequenceChart");

        // try to open the log view after all events are processed
        Display.getDefault().asyncExec(new Runnable() {
            @Override
            public void run() {
                try {
                    // Eclipse feature: during startup, showView() throws "Abnormal Workbench Condition" because perspective is null
                    IWorkbenchPage page = getSite().getPage();
                    if (page != null && page.getPerspective() != null)
                        page.showView("org.omnetpp.eventlogtable.editors.EventLogTableView", null, IWorkbenchPage.VIEW_VISIBLE);
                }
                catch (PartInitException e) {
                    SequenceChartPlugin.getDefault().logException(e);
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
    public void createPartControl(Composite parent) {
        sequenceChart = new SequenceChart(parent, SWT.DOUBLE_BUFFERED);
        sequenceChart.setInput(eventLogInput);
        sequenceChart.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        sequenceChart.setWorkbenchPart(this);

        getSite().setSelectionProvider(sequenceChart);
        addLocationProviderPaintListener(sequenceChart);

        // follow selection
        selectionListener = new ISelectionListener() {
            public void selectionChanged(IWorkbenchPart part, ISelection selection) {
                if (followSelection && part != SequenceChartEditor.this && selection instanceof IEventLogSelection) {
                    IEventLogSelection eventLogSelection = (IEventLogSelection)selection;

                    if (eventLogSelection.getEventLogInput() == sequenceChart.getInput()) {
                        sequenceChart.setSelection(selection);
                        markLocation();
                    }
                }
            }
        };
        getSite().getPage().addSelectionListener(selectionListener);
    }

    @Override
    public void setFocus() {
        sequenceChart.setFocus();
    }

    public INavigationLocation createEmptyNavigationLocation() {
        return new SequenceChartLocation(this, org.omnetpp.common.engine.BigDecimal.getZero(), org.omnetpp.common.engine.BigDecimal.getNaN());
    }

    @Override
    protected boolean canCreateNavigationLocation() {
        return !eventLogInput.isCanceled() && eventLogInput.getSequenceChartFacade().getTimelineCoordinateSystemOriginEventNumber() != -1 && super.canCreateNavigationLocation();
    }

    public INavigationLocation createNavigationLocation() {
        try {
            if (!canCreateNavigationLocation())
                return null;
            else
                return new SequenceChartLocation(this, sequenceChart.getViewportLeftSimulationTime(), sequenceChart.getViewportRightSimulationTime());
        }
        catch (RuntimeException e) {
            eventLogInput.handleRuntimeException(e);
            return null;
        }
    }

    public void gotoMarker(IMarker marker)
    {
        IEventLog eventLog = eventLogInput.getEventLog();
        String kind = marker.getAttribute("Kind", null);
        String eventNumberString = marker.getAttribute("EventNumber", null);
        if (kind.equals("Event")) {
            IEvent event = eventLog.getEventForEventNumber(Long.parseLong(eventNumberString));
            if (event != null)
                sequenceChart.scrollToEvent(event);
        }
        else if (kind.equals("Axis")) {
            String modulePath = marker.getAttribute("ModulePath", null);
            ModuleTreeItem moduleTreeItem = eventLogInput.getModuleTreeRoot().findDescendantModule(modulePath);
            if (moduleTreeItem != null)
                sequenceChart.scrollToAxisModule(moduleTreeItem);
        }
        else if (kind.equals("MessageDependency")) {
            IEvent event = eventLog.getEventForEventNumber(Long.parseLong(eventNumberString));
            if (event != null) {
                String messageDependencyIndexString = marker.getAttribute("MessageDependencyIndex", null);
                IMessageDependency messageDependency = event.getConsequences().get(Integer.parseInt(messageDependencyIndexString));
                String messageIdString = marker.getAttribute("MessageId", null);
                if (messageDependency.getBeginMessageDescriptionEntry().getMessageId() == Integer.parseInt(messageIdString))
                    sequenceChart.scrollToMessageDependency(messageDependency);
            }
        }
        else if (kind.equals("ComponentMethodCall")) {
            IEvent event = eventLog.getEventForEventNumber(Long.parseLong(eventNumberString));
            if (event != null) {
                String eventLogEntryIndexString = marker.getAttribute("EventLogEntryIndex", null);
                EventLogEntry eventLogEntry = event.getEventLogEntry(Integer.parseInt(eventLogEntryIndexString));
                if (eventLogEntry instanceof ComponentMethodBeginEntry) {
                    ComponentMethodBeginEntry componentMethodBeginEntry = (ComponentMethodBeginEntry)eventLogEntry;
                    String methodNameString = marker.getAttribute("MethodName", null);
                    if (componentMethodBeginEntry.getMethodName().equals(methodNameString))
                        sequenceChart.scrollToComponentMethodCall(componentMethodBeginEntry);
                }
            }
        }
        else if (kind.equals("Position")) {
            String simulationTimeString = marker.getAttribute("SimulationTime", null);
            BigDecimal simulationTime = BigDecimal.parse(simulationTimeString);
            sequenceChart.scrollToSimulationTime(simulationTime);
        }
    }

    private class ResourceChangeListener implements IResourceChangeListener, IResourceDeltaVisitor {
        public void resourceChanged(IResourceChangeEvent event) {
            try {
                // close editor on project close
                if (event.getType() == IResourceChangeEvent.PRE_CLOSE) {
                    final IEditorPart thisEditor = SequenceChartEditor.this;
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
                            if (!sequenceChart.isDisposed())
                                sequenceChart.clearCanvasCacheAndRedraw();
                        }
                        catch (RuntimeException e) {
                            sequenceChart.handleRuntimeException(e);
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
                IConstants.EVENTLOG_VIEW_ID,
                IConstants.ANIMATION_VIEW_ID,
                //TODO IConstants.MODULEHIERARCHY_VIEW_ID,
                };
    }
}
