/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.omnetpp.common.ui.FocusManager;
import org.omnetpp.common.ui.IconGridViewer;
import org.omnetpp.scave.actions.NewBarChartAction;
import org.omnetpp.scave.actions.NewChartSheetAction;
import org.omnetpp.scave.actions.NewHistogramChartAction;
import org.omnetpp.scave.actions.NewLineChartAction;
import org.omnetpp.scave.actions.NewScatterChartAction;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ScaveEditorContributor;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.Charts;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Scave page which displays datasets and charts
 * @author Andras
 */
//TODO ChartDefinitionsPage? ChartListPage?
public class ChartsPage extends ScaveEditorPage {
    private FormToolkit formToolkit = null;
    private IconGridViewer viewer;

    /**
     * Constructor
     */
    public ChartsPage(Composite parent, ScaveEditor scaveEditor) {
        super(parent, SWT.NONE, scaveEditor);
        initialize();
    }

    private void initialize() {
        // set up UI
        setPageTitle("Charts");
        setFormTitle("Charts");
        getContent().setLayout(new GridLayout(2, false));

        Label label = new Label(getContent(), SWT.WRAP);
        label.setText("Here you can edit chart definitions.");
        label.setBackground(this.getBackground());
        label.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false, 2, 1));

        viewer = new IconGridViewer(getContent());
        viewer.getControl().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        viewer.setBackground(Display.getCurrent().getSystemColor(SWT.COLOR_LIST_BACKGROUND));

        ScaveEditorContributor contributor = ScaveEditorContributor.getDefault();
        boolean withEditDialog = false;
        addToToolbar(new NewBarChartAction(withEditDialog));
        addToToolbar(new NewLineChartAction(withEditDialog));
        addToToolbar(new NewScatterChartAction(withEditDialog));
        addToToolbar(new NewHistogramChartAction(withEditDialog));
        addToToolbar(new NewChartSheetAction(withEditDialog));
        addSeparatorToToolbar();
        addToToolbar(contributor.getEditAction());
        addToToolbar(contributor.getRemoveAction());
        addToToolbar(contributor.getOpenAction());

        viewer.setFocus();

        // configure viewers
        scaveEditor.configureIconGridViewer(getViewer());

        viewer.addDropListener(new IconGridViewer.IDropListener() {
            @Override
            public void drop(Object[] draggedElements, Point p) {
                Object target = viewer.getElementAt(p.x, p.y);
                if (target instanceof ChartSheet) {
                    ChartSheet chartSheet = (ChartSheet)target;
                    ScaveModelUtil.addElementsToChartSheet(scaveEditor.getEditingDomain(), draggedElements, chartSheet);
                    viewer.setSelection(new StructuredSelection(chartSheet));
                }
                else {
                    Object insertionPoint = viewer.getElementAtOrAfter(p.x, p.y);
                    System.out.println("DRAGGED BEFORE: " + insertionPoint);
                    if (insertionPoint == null ||!ArrayUtils.contains(draggedElements, insertionPoint)) {
                        Charts charts = scaveEditor.getAnalysis().getCharts();
                        int index = insertionPoint == null ? charts.getItems().size() : charts.getItems().indexOf(insertionPoint);
                        ScaveModelUtil.moveElements(scaveEditor.getEditingDomain(), charts, draggedElements, index);
                        viewer.refresh();
                    }
                }
            }
        });

        viewer.setRenameAdapter(new IconGridViewer.IRenameAdapter() {
            @Override
            public boolean isRenameable(Object element) {
                return (element instanceof AnalysisItem);
            }

            @Override
            public boolean isNameValid(Object element, String name) {
                return true;
            }

            @Override
            public String getName(Object element) {
                return StringUtils.nullToEmpty(((AnalysisItem) element).getName());
            }

            @Override
            public void setName(Object element, String name) {
                ((AnalysisItem) element).setName(name);
            }
        });

        // set contents
        Analysis analysis = scaveEditor.getAnalysis();
        getViewer().setInput(analysis.getCharts());

        // ensure that focus gets restored correctly after user goes somewhere else and then comes back
        setFocusManager(new FocusManager(this));
    }

    /**
     * This method initializes formToolkit
     */
    private FormToolkit getFormToolkit() {
        if (formToolkit == null)
            formToolkit = new FormToolkit2(Display.getCurrent());
        return formToolkit;
    }

    public IconGridViewer getViewer() {
        return viewer;
    }

    @Override
    public boolean gotoObject(Object object) {
        if (object instanceof EObject) {
            EObject eobject = (EObject)object;
            if (ScaveModelUtil.findEnclosingOrSelf(eobject, Charts.class) != null) {
                getViewer().reveal(eobject);
                return true;
            }
        }
        return false;
    }

    @Override
    public void selectionChanged(ISelection selection) {
        setViewerSelectionNoNotify(getViewer(), selection);
    }
}


