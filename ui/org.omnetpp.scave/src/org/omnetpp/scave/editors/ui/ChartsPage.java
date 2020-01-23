/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.viewers.DoubleClickEvent;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.IStructuredContentProvider;
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
import org.omnetpp.common.ui.IconGridViewer.ViewMode;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.NewChartFromTemplateAction;
import org.omnetpp.scave.charttemplates.ChartTemplate;
import org.omnetpp.scave.charttemplates.ChartTemplateRegistry;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ScaveEditorActions;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Charts;
import org.omnetpp.scave.model.Folder;
import org.omnetpp.scave.model.ModelChangeEvent;
import org.omnetpp.scave.model.ModelObject;
import org.omnetpp.scave.model.commands.CommandStack;
import org.omnetpp.scave.model.commands.SetChartNameCommand;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Scave page which displays datasets and charts
 * @author Andras
 */
//TODO ChartDefinitionsPage? ChartListPage?
//TODO add "Rename" to context menu
public class ChartsPage extends FormEditorPage {
    private IconGridViewer viewer;
    protected CommandStack commandStack = new CommandStack("ChartsPage");


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
        viewer.setViewMode(ViewMode.ICONS);

        ScaveEditorActions actions = scaveEditor.getActions();

        List<ChartTemplate> templates = new ArrayList<ChartTemplate>();

        for (ChartTemplate templ : scaveEditor.getChartTemplateRegistry().getAllTemplates())
            if (templ.getToolbarOrder() >= 0)
                templates.add(templ);

        templates.sort((ChartTemplate a, ChartTemplate b) ->  {
            return Integer.compare(a.getToolbarOrder(), b.getToolbarOrder());
        });

        for (ChartTemplate templ : templates)
            addToToolbar(new NewChartFromTemplateAction(templ));

        addSeparatorToToolbar();
        addToToolbar(actions.editAction);
        addToToolbar(actions.removeAction);
        addToToolbar(actions.openAction);

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

    public CommandStack getCommandStack() {
		return commandStack;
	}

    @Override
    public boolean gotoObject(Object object) {
        if (object instanceof ModelObject) {
            ModelObject modelObject = (ModelObject)object;
            if (ScaveModelUtil.findEnclosingOrSelf(modelObject, Charts.class) != null) {
                getViewer().setSelection(new StructuredSelection(modelObject), true);
                return true;
            }
        }
        return false;
    }

    public void updatePage(ModelChangeEvent event) {
        viewer.refresh();
    }

    protected void configureViewer(IconGridViewer modelViewer) {

        ILabelProvider labelProvider = new LabelProvider() {
            @Override
            public Image getImage(Object element) {
                if (element instanceof Chart) {
                    String iconPath = ((Chart)element).getIconPath();
                    if (iconPath == null || iconPath.isEmpty())
                        iconPath = ScaveImages.IMG_OBJ_CHART;
                    return ScavePlugin.getCachedImage(iconPath);
                }
                else if (element instanceof Folder)
                    return ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ_FOLDER);
                else
                    return null;
            }

            @Override
            public String getText(Object element) {
                if (element instanceof AnalysisItem)
                    return StringUtils.defaultIfBlank(((AnalysisItem) element).getName(), "<unnamed>");
                return element == null ? "" : element.toString();
            }
        };

        modelViewer.setContentProvider(new IStructuredContentProvider() {
            @Override
            public Object[] getElements(Object inputElement) {
                if (inputElement instanceof Charts) {
                    Charts charts = (Charts)inputElement;
                    return charts.getCharts().toArray();
                }
                return new Object[0];
            }
        });
        modelViewer.setLabelProvider(labelProvider);

        viewer.addDropListener(new IconGridViewer.IDropListener() {
            @Override
            public void drop(Object[] draggedElements, Point p) {
                Object insertionPoint = viewer.getElementAtOrAfter(p.x, p.y);
                if (insertionPoint == null ||!ArrayUtils.contains(draggedElements, insertionPoint)) {
                    List<AnalysisItem> charts = scaveEditor.getAnalysis().getCharts().getCharts();
                    int index = insertionPoint == null ? charts.size() - 1 : charts.indexOf(insertionPoint);
                    ScaveModelUtil.moveElements(commandStack, scaveEditor.getAnalysis().getCharts(), draggedElements, index);
                    viewer.refresh();
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
                commandStack.execute(new SetChartNameCommand((AnalysisItem)element, name));
            }
        });

        modelViewer.addSelectionChangedListener(scaveEditor.getSelectionChangedListener());

        IMenuListener menuListener = new IMenuListener() {
            @Override
            public void menuAboutToShow(IMenuManager menuManager) {
                scaveEditor.getActions().populateContextMenu(menuManager);
            }
        };

        UIUtils.createContextMenuFor(modelViewer.getCanvas(), true, menuListener);

        // on double-click, open (the dataset or chart), or bring up the Properties dialog
        modelViewer.addDoubleClickListener(new IDoubleClickListener() {
            @Override
            public void doubleClick(DoubleClickEvent event) {
                ScaveEditorActions actions = scaveEditor.getActions();

                if (actions.openAction.isEnabled())
                    actions.openAction.run();
                else if (actions.editAction.isEnabled())
                    actions.editAction.run();
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


