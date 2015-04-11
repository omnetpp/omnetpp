/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.eventlogtable.editors;

import java.lang.reflect.InvocationTargetException;

import org.eclipse.jface.operation.IRunnableContext;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IPartListener;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IViewSite;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.contexts.IContextService;
import org.omnetpp.common.eventlog.EventLogView;
import org.omnetpp.common.eventlog.IEventLogSelection;
import org.omnetpp.common.ui.TimeTriggeredProgressMonitorDialog;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlogtable.widgets.EventLogTable;

/**
 * View for displaying and navigating event log entries.
 */
public class EventLogTableView extends EventLogView implements IEventLogTableProvider {
    private EventLogTable eventLogTable;

    private EventLogTableContributor eventLogTableContributor;

    private ISelectionListener selectionListener;

    private IPartListener partListener;

    @Override
    public void createPartControl(Composite parent) {
        super.createPartControl(parent);
        IViewSite viewSite = (IViewSite)getSite();
        viewSite.setSelectionProvider(eventLogTable);
        IContextService contextService = (IContextService)viewSite.getService(IContextService.class);
        contextService.activateContext("org.omnetpp.context.EventLogTable");
        // contribute to toolbar
        eventLogTableContributor = new EventLogTableContributor(eventLogTable);
        eventLogTable.setEventLogTableContributor(eventLogTableContributor);
        eventLogTableContributor.contributeToToolBar(viewSite.getActionBars().getToolBarManager(), true);
        // follow selection
        selectionListener = new ISelectionListener() {
            public void selectionChanged(IWorkbenchPart part, ISelection selection) {
                if (followSelection && part != EventLogTableView.this && selection instanceof IEventLogSelection)
                    setSelection(selection);
            }
        };
        viewSite.getPage().addSelectionListener(selectionListener);
        // follow active editor changes
        partListener = new IPartListener() {
            public void partActivated(IWorkbenchPart part) {
            }

            public void partBroughtToTop(IWorkbenchPart part) {
                if (followSelection && part instanceof IEditorPart && !eventLogTable.isDisposed())
                    setSelectionFromActiveEditor();
            }

            public void partClosed(IWorkbenchPart part) {
                if (followSelection && part instanceof IEditorPart && !eventLogTable.isDisposed())
                    setSelectionFromActiveEditor();
            }

            public void partDeactivated(IWorkbenchPart part) {
            }

            public void partOpened(IWorkbenchPart part) {
                if (followSelection && part instanceof IEditorPart && !eventLogTable.isDisposed())
                    setSelectionFromActiveEditor();
            }
        };
        viewSite.getPage().addPartListener(partListener);
        // bootstrap with current selection
        selectionListener.selectionChanged(null, getActiveEditorSelection());
    }

    @Override
    public IEventLog getEventLog() {
        return eventLogTable.getEventLog();
    }

    @Override
    public void dispose() {
        IViewSite viewSite = (IViewSite)getSite();

        if (selectionListener != null)
            viewSite.getPage().removeSelectionListener(selectionListener);

        if (partListener != null)
            viewSite.getPage().removePartListener(partListener);

        super.dispose();
    }

    @Override
    public void setFocus() {
        eventLogTable.setFocus();
    }

    @Override
    protected Control createViewControl(final Composite parent) {
        eventLogTable = new EventLogTable(parent, SWT.NONE);
        eventLogTable.setWorkbenchPart(this);
        eventLogTable.setRunnableContextForLongRunningOperations(new IRunnableContext() {
            @Override
            public void run(boolean fork, boolean cancelable, IRunnableWithProgress runnable) throws InvocationTargetException, InterruptedException {
                TimeTriggeredProgressMonitorDialog dialog = new TimeTriggeredProgressMonitorDialog(parent.getShell(), 1000);
                dialog.setCancelable(cancelable);
                dialog.run(fork, cancelable, runnable);
            }
        });
        return eventLogTable;
    }

    @Override
    public void setSelection(ISelection selection) {
        if (selection instanceof IEventLogSelection) {
            hideMessage();
            eventLogTable.setSelection(selection);
        }
        else {
            eventLogTable.setInput(null);
            showMessage("No event log available");
        }
        updateTitleToolTip();
    }

    public EventLogTable getEventLogTable() {
        return eventLogTable;
    }
}
