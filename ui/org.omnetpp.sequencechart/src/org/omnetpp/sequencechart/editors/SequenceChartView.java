/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.sequencechart.editors;

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
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.sequencechart.widgets.SequenceChart;

/**
 * View for displaying causes and consequences of events.
 */
public class SequenceChartView extends EventLogView implements ISequenceChartProvider {
    private SequenceChart sequenceChart;

    private SequenceChartContributor sequenceChartContributor;

    private ISelectionListener selectionListener;

    private IPartListener partListener;

    public SequenceChart getSequenceChart() {
        return sequenceChart;
    }

    @Override
    public void createPartControl(Composite parent) {
        super.createPartControl(parent);
        IViewSite viewSite = (IViewSite)getSite();
        viewSite.setSelectionProvider(sequenceChart);
        IContextService contextService = (IContextService)viewSite.getService(IContextService.class);
        contextService.activateContext("org.omnetpp.context.SequenceChart");
        // contribute to toolbar
        sequenceChartContributor = new SequenceChartContributor(sequenceChart);
        sequenceChart.setSequenceChartContributor(sequenceChartContributor);
        sequenceChartContributor.contributeToToolBar(viewSite.getActionBars().getToolBarManager(), true);
        // follow selection
        selectionListener = new ISelectionListener() {
            public void selectionChanged(IWorkbenchPart part, ISelection selection) {
                if (followSelection && part != SequenceChartView.this && selection instanceof IEventLogSelection)
                    setSelection(selection);
            }
        };
        viewSite.getPage().addSelectionListener(selectionListener);
        // follow active editor changes
        partListener = new IPartListener() {
            public void partActivated(IWorkbenchPart part) {
            }

            public void partBroughtToTop(IWorkbenchPart part) {
                if (followSelection && part instanceof IEditorPart && !sequenceChart.isDisposed())
                    setSelectionFromActiveEditor();
            }

            public void partClosed(IWorkbenchPart part) {
                if (followSelection && part instanceof IEditorPart && !sequenceChart.isDisposed())
                    setSelectionFromActiveEditor();
            }

            public void partDeactivated(IWorkbenchPart part) {
            }

            public void partOpened(IWorkbenchPart part) {
                if (followSelection && part instanceof IEditorPart && !sequenceChart.isDisposed())
                    setSelectionFromActiveEditor();
            }
        };
        viewSite.getPage().addPartListener(partListener);
        // bootstrap with current selection
        selectionListener.selectionChanged(null, getActiveEditorSelection());
    }

    @Override
    public IEventLog getEventLog() {
        return sequenceChart.getEventLog();
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
        sequenceChart.setFocus();
    }

    @Override
    protected Control createViewControl(Composite parent) {
        sequenceChart = new SequenceChart(parent, SWT.DOUBLE_BUFFERED);
        sequenceChart.setWorkbenchPart(this);
        return sequenceChart;
    }

    @Override
    public void setSelection(ISelection selection) {
        if (selection instanceof IEventLogSelection) {
            hideMessage();
            sequenceChart.setSelection(selection);
        }
        else {
            sequenceChart.setInput(null);
            showMessage("No event log available");
        }
    }
}
