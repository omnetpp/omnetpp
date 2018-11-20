/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryContentProvider;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.viewers.DoubleClickEvent;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.ui.FocusManager;
import org.omnetpp.common.ui.IconGridViewer;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.NewBarChartAction;
import org.omnetpp.scave.actions.NewChartSheetAction;
import org.omnetpp.scave.actions.NewHistogramChartAction;
import org.omnetpp.scave.actions.NewLineChartAction;
import org.omnetpp.scave.actions.NewScatterChartAction;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ScaveEditorContributor;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.Charts;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.Folder;
import org.omnetpp.scave.model.HistogramChart;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.ScatterChart;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Scave page which displays datasets and charts
 * @author Andras
 */
//TODO ChartDefinitionsPage? ChartListPage?
//TODO add "Rename" to context menu
public class ChartsPage extends FormEditorPage {
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
        configureViewer(viewer);

        // set contents
        Analysis analysis = scaveEditor.getAnalysis();
        getViewer().setInput(analysis.getCharts());

        // ensure that focus gets restored correctly after user goes somewhere else and then comes back
        setFocusManager(new FocusManager(this));
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

    public void updatePage(Notification notification) {
        if (notification.isTouch() || !(notification.getNotifier() instanceof EObject))
            return;
        viewer.refresh();
    }
    
    protected void configureViewer(IconGridViewer modelViewer) {
//        ILabelProvider labelProvider =
//            new DecoratingLabelProvider(
//                new ScaveModelLabelProvider(new AdapterFactoryLabelProvider(adapterFactory)),
//                new ScaveModelLabelDecorator());

        ILabelProvider labelProvider = new LabelProvider() {
            @Override
            public Image getImage(Object element) {
                if (element instanceof LineChart)
                    return ScavePlugin.getImage(ScaveImages.IMG_OBJ_LINECHART);
                else if (element instanceof BarChart)
                    return ScavePlugin.getImage(ScaveImages.IMG_OBJ_BARCHART);
                else if (element instanceof ScatterChart)
                    return ScavePlugin.getImage(ScaveImages.IMG_OBJ_SCATTERCHART);
                else if (element instanceof HistogramChart)
                    return ScavePlugin.getImage(ScaveImages.IMG_OBJ_HISTOGRAMCHART);
                else if (element instanceof Dataset)
                    return ScavePlugin.getImage(ScaveImages.IMG_OBJ_DATASET);
                else if (element instanceof Folder)
                    return ScavePlugin.getImage(ScaveImages.IMG_OBJ_FOLDER);
                else if (element instanceof ChartSheet)
                    return ScavePlugin.getImage(ScaveImages.IMG_OBJ_CHARTSHEET);
                else
                    return null;
            }

            @Override
            public String getText(Object element) {
                if (element instanceof AnalysisItem) {
                    String text = StringUtils.defaultIfBlank(((AnalysisItem) element).getName(), "<unnamed>");
                    if (element instanceof ChartSheet)
                        text += " (" + ((ChartSheet)element).getCharts().size() + ")";
                    return text;
                }
                return element == null ? "" : element.toString();
            }
        };

        modelViewer.setContentProvider(new AdapterFactoryContentProvider(scaveEditor.getAdapterFactory()));
        modelViewer.setLabelProvider(labelProvider);

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

        modelViewer.addSelectionChangedListener(scaveEditor.getSelectionChangedListener());

        IMenuListener menuListener = new IMenuListener() {
            @Override
            public void menuAboutToShow(IMenuManager menuManager) {
                scaveEditor.getActionBarContributor().populateContextMenu(menuManager);
            }
        };
        
        UIUtils.createContextMenuFor(modelViewer.getCanvas(), true, menuListener);
        
        // on double-click, open (the dataset or chart), or bring up the Properties dialog
        modelViewer.addDoubleClickListener(new IDoubleClickListener() {
            @Override
            public void doubleClick(DoubleClickEvent event) {
                ScaveEditorContributor contributor = ScaveEditorContributor.getDefault();
                if (contributor != null) {
                    if (contributor.getOpenAction().isEnabled())
                        contributor.getOpenAction().run();
                    else if (contributor.getEditAction().isEnabled())
                        contributor.getEditAction().run();
                }
            }
        });

//        new HoverSupport().adapt(modelViewer.getTree(), new IHoverInfoProvider() {
//            @Override
//            public HtmlHoverInfo getHoverFor(Control control, int x, int y) {
//                Item item = modelViewer.getTree().getItem(new Point(x,y));
//                Object element = item==null ? null : item.getData();
//                if (element != null && modelViewer.getLabelProvider() instanceof DecoratingLabelProvider) {
//                    ILabelProvider labelProvider = ((DecoratingLabelProvider)modelViewer.getLabelProvider()).getLabelProvider();
//                    if (labelProvider instanceof ScaveModelLabelProvider) {
//                        ScaveModelLabelProvider scaveLabelProvider = (ScaveModelLabelProvider)labelProvider;
//                        return new HtmlHoverInfo(HoverSupport.addHTMLStyleSheet(scaveLabelProvider.getTooltipText(element)));
//                    }
//                }
//                return null;
//            }
//        });
    }
    
}


