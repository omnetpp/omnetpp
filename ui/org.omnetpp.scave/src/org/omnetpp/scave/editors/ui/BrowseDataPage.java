/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import java.util.List;
import java.util.concurrent.Callable;

import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.ui.DropdownAction;
import org.omnetpp.common.ui.FocusManager;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.CopyChartFilterAction;
import org.omnetpp.scave.actions.CreateTempChartFromTemplateAction;
import org.omnetpp.scave.actions.DecreaseDecimalPlacesAction;
import org.omnetpp.scave.actions.FlatModuleTreeAction;
import org.omnetpp.scave.actions.IncreaseDecimalPlacesAction;
import org.omnetpp.scave.actions.SetChartFilterAction;
import org.omnetpp.scave.actions.SetFilterAction2;
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ScaveEditorActions;
import org.omnetpp.scave.editors.datatable.ChooseTableColumnsAction;
import org.omnetpp.scave.editors.datatable.DataTable;
import org.omnetpp.scave.editors.datatable.DataTree;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.editors.datatable.FilteredDataTabFolder;
import org.omnetpp.scave.editors.datatable.IDataControl;
import org.omnetpp.scave.editors.datatable.IDataListener;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engineext.IResultFilesChangeListener;
import org.omnetpp.scave.engineext.ResultFileManagerChangeEvent;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartTemplate;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * This is the "Browse Data" page of Scave Editor
 */
//TODO Statistics result items should be added to the histogram page
//TODO remove page description; display "empty-table message" instead (stacklayout?)
//TODO save filter bar sash positions
//TODO focus issue (currently a toolbar icon gets the focus by default?)
public class BrowseDataPage extends FormEditorPage {

    // UI elements
    private Label label;
    private FilteredDataTabFolder tabFolder;

    private IResultFilesChangeListener fileChangeListener;
    private Runnable scheduledUpdate;

    private SetFilterAction2 setFilterAction = new SetFilterAction2();
    private int numericPrecision = 6;

    private DropdownAction treeLevelsAction;
    private FlatModuleTreeAction flatModuleTreeAction;
    private ChooseTableColumnsAction chooseTableColumnsAction;

    public BrowseDataPage(Composite parent, ScaveEditor editor) {
        super(parent, SWT.NONE, editor);
        initialize();
        hookListeners();
    }

    @Override
    public void dispose() {
        unhookListeners();
        super.dispose();
    }

    public FilteredDataPanel getAllPanel() {
        return tabFolder.getAllPanel();
    }

    public FilteredDataPanel getScalarsPanel() {
        return tabFolder.getScalarsPanel();
    }

    public FilteredDataPanel getParametersPanel() {
        return tabFolder.getParametersPanel();
    }

    public FilteredDataPanel getVectorsPanel() {
        return tabFolder.getVectorsPanel();
    }

    public FilteredDataPanel getHistogramsPanel() {
        return tabFolder.getHistogramsPanel();
    }

    public FilteredDataPanel getActivePanel() {
        return tabFolder.getActivePanel();
    }

    public ResultType getActivePanelType() {
        return tabFolder.getActivePanelType();
    }

    private void initialize() {
        // set up UI
        setPageTitle("Browse Data");
        setFormTitle("Browse Data");
        //setBackground(ColorFactory.WHITE);
        getContent().setLayout(new GridLayout());
        label = new Label(getContent(), SWT.WRAP);
        label.setText("Here you can see all data that come from the files specified in the Inputs page.");
        label.setBackground(this.getBackground());

        tabFolder = new FilteredDataTabFolder(getContent(), SWT.NONE);
        tabFolder.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        configureFilteredDataPanel(tabFolder.getAllPanel());
        configureFilteredDataPanel(tabFolder.getVectorsPanel());
        configureFilteredDataPanel(tabFolder.getScalarsPanel());
        configureFilteredDataPanel(tabFolder.getParametersPanel());
        configureFilteredDataPanel(tabFolder.getHistogramsPanel());
        configureContextMenu(tabFolder.getAllPanel());
        configureContextMenu(tabFolder.getScalarsPanel());
        configureContextMenu(tabFolder.getParametersPanel());
        configureContextMenu(tabFolder.getVectorsPanel());
        configureContextMenu(tabFolder.getHistogramsPanel());

        // set up contents
        ResultFileManagerEx manager = scaveEditor.getResultFileManager();
        tabFolder.setResultFileManager(manager);

        setNumericPrecision(getNumericPrecision()); // make sure it takes effect

        // ensure that focus gets restored correctly after user goes somewhere else and then comes back
        setFocusManager(new FocusManager(this));

        // add actions to the toolbar
        ScaveEditorActions actions = scaveEditor.getActions();
        addToToolbar(actions.copyDataToClipboardAction);

        DropdownAction exportDataAction = new DropdownAction("Export Data", "Export In: ", ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_EXPORT), false);
        actions.createExportDataMenu(exportDataAction.getMenuManager());
        addToToolbar(exportDataAction);

        addSeparatorToToolbar();

        treeLevelsAction = new DropdownAction("Tree Levels", "Switch To: ", ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_TREELEVELS), true);
        DataTree dataTree = (DataTree)getAllPanel().getDataControl();
        dataTree.contributeTreeLevelsActionsTo(treeLevelsAction.getMenuManager());
        addToToolbar(treeLevelsAction);

        flatModuleTreeAction = ((DataTree)getAllPanel().getDataControl()).getFlatModuleTreeAction();
        addToToolbar(flatModuleTreeAction);

        chooseTableColumnsAction = new ChooseTableColumnsAction(null);
        addToToolbar(chooseTableColumnsAction);

        addSeparatorToToolbar();

        addToToolbar(new IncreaseDecimalPlacesAction());
        addToToolbar(new DecreaseDecimalPlacesAction());  //TODO get these refreshed when min/max precision is reached
        addSeparatorToToolbar();
        addToToolbar(actions.plotAction);

        // show/hide actions that are specific to tab pages
        tabFolder.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                updateIconVisibility();
            }
        });
        updateIconVisibility();
    }

    private void updateIconVisibility() {
        boolean isAllPanelActive = (getActivePanel() == getAllPanel());
        setToolbarActionVisible(flatModuleTreeAction, isAllPanelActive);
        setToolbarActionVisible(treeLevelsAction, isAllPanelActive);
        setToolbarActionVisible(chooseTableColumnsAction, !isAllPanelActive);
        if (!isAllPanelActive)
            chooseTableColumnsAction.setDataTable((DataTable)getActivePanel().getDataControl());
    }

    private void configureContextMenu(FilteredDataPanel panel) {
        // populate the popup menu of the panel
        IMenuManager contextMenuManager = panel.getDataControl().getContextMenuManager();
        contextMenuManager.removeAll();
        ScaveEditorActions actions = scaveEditor.getActions();
        if (actions != null) {
            IDList selectedIDs = panel.getDataControl().getSelectedIDs();
            if (selectedIDs != null) {
                int selectedItemTypes = selectedIDs.getItemTypes();
                List<ChartTemplate> supportingChartTemplates = scaveEditor.getChartTemplateRegistry().getChartTemplatesForResultTypes(selectedItemTypes);

                for (ChartTemplate templ : supportingChartTemplates)
                    contextMenuManager.add(new CreateTempChartFromTemplateAction(templ));

                contextMenuManager.add(new Separator());
            }

            contextMenuManager.add(new CopyChartFilterAction());
            MenuManager setFilterSubmenu = new MenuManager("Set Filter of Chart", ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_SETFILTER), null);

            for (AnalysisItem i : scaveEditor.getAnalysis().getCharts().getCharts())
                if (i instanceof Chart && ScaveModelUtil.getChartProperty((Chart)i, "filter") != null) // TODO: menu is not updated when charts are added to or removed from the analysis
                    setFilterSubmenu.add(new SetChartFilterAction((Chart)i));
            contextMenuManager.add(setFilterSubmenu);
            contextMenuManager.add(new Separator());

            contextMenuManager.add(actions.copyDataToClipboardAction);
            contextMenuManager.add(actions.createExportDataMenu("Export Data"));
            contextMenuManager.add(new Separator());

            contextMenuManager.add(setFilterAction);
            if (panel.getDataControl() instanceof DataTable)
                contextMenuManager.add(new ChooseTableColumnsAction((DataTable)panel.getDataControl()));
            if (panel.getDataControl() instanceof DataTree)
                ((DataTree)panel.getDataControl()).contributeToContextMenu(contextMenuManager);
            if (ResultType.VECTOR.equals(panel.getType())) {
                contextMenuManager.add(new Separator());
                contextMenuManager.add(actions.showOutputVectorViewAction);
            }
        }
        // XXX call getSite().registerContexMenu() ?
    }

    /**
     * Utility function configure data panel to display selection count in the status bar.
     */
    private void configureFilteredDataPanel(FilteredDataPanel panel) {
        final IDataControl control = panel.getDataControl();
        control.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent event) {
                showStatusMessage(String.format("Selected %d out of %d rows", control.getSelectionCount(), control.getItemCount()));
            }
        });
        control.addDataListener(new IDataListener() {
            @Override
            public void contentChanged(IDataControl control) {
                showStatusMessage(String.format("Selected %d out of %d rows", control.getSelectionCount(), control.getItemCount()));
            }
        });
    }

    private void hookListeners() {
        // asynchronously update the page contents on result file changes (ie. input file load/unload)
        if (fileChangeListener == null) {
            fileChangeListener = new IResultFilesChangeListener() {
                @Override
                public void resultFileManagerChanged(ResultFileManagerChangeEvent event) {
                    switch (event.getChangeType()) {
                    case LOAD:
                    case UNLOAD:
                        final ResultFileManager manager = event.getResultFileManager();
                        if (scheduledUpdate == null) {
                            scheduledUpdate = new Runnable() {
                                @Override
                                public void run() {
                                    scheduledUpdate = null;
                                    if (!isDisposed()) {
                                        ResultFileManager.callWithReadLock(manager, new Callable<Object>() {
                                            @Override
                                            public Object call() {
                                                refreshPage(manager);
                                                return null;
                                            }
                                        });
                                    }
                                }
                            };
                            getDisplay().asyncExec(scheduledUpdate);
                        }
                    }
                }
            };
            scaveEditor.getResultFileManager().addChangeListener(fileChangeListener);
        }

        // when they double-click in the vectors panel, open chart
        SelectionListener selectionChangeListener = new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                FilteredDataPanel panel = tabFolder.getActivePanel();
                if (panel != null) {
                    Object source = e.getSource();
                    if (source == panel.getDataControl() || source == tabFolder) {
                        ResultFileManager.callWithReadLock(panel.getResultFileManager(), new Callable<Object>() {
                            @Override
                            public Object call() throws Exception {
                                updateSelection();
                                return null;
                            }
                        });
                    }
                }
            }

            @Override
            public void widgetDefaultSelected(SelectionEvent e) {
                scaveEditor.getActions().plotAction.run();
            }
        };

        tabFolder.getAllPanel().getDataControl().addSelectionListener(selectionChangeListener);
        tabFolder.getVectorsPanel().getDataControl().addSelectionListener(selectionChangeListener);
        tabFolder.getScalarsPanel().getDataControl().addSelectionListener(selectionChangeListener);
        tabFolder.getParametersPanel().getDataControl().addSelectionListener(selectionChangeListener);
        tabFolder.getHistogramsPanel().getDataControl().addSelectionListener(selectionChangeListener);
        tabFolder.addSelectionListener(selectionChangeListener);
    }

    private void unhookListeners() {
        if (fileChangeListener != null) {
            ResultFileManagerEx manager = scaveEditor.getResultFileManager();
            manager.removeChangeListener(fileChangeListener);
            fileChangeListener = null;
        }
    }

    /**
     * Updates the page: retrieves the list of vectors, scalars, etc from
     * ResultFileManager, updates the data controls with them, and updates
     * the tab labels as well.
     */
    protected void refreshPage(ResultFileManager manager) {
        boolean showFields = false; // on the scalars page --TODO make configurable!!!
        IDList items = manager.getAllItems(false, true); // exclude computed and fields
        IDList vectors = manager.getAllVectors();
        IDList scalars = manager.getAllScalars(showFields, true);
        IDList parameters = manager.getAllParameters();
        IDList histograms = manager.getAllStatistics();
        histograms.merge(manager.getAllHistograms());

        tabFolder.getAllPanel().setIDList(items);
        tabFolder.getScalarsPanel().setIDList(scalars);
        tabFolder.getParametersPanel().setIDList(parameters);
        tabFolder.getVectorsPanel().setIDList(vectors);
        tabFolder.getHistogramsPanel().setIDList(histograms);
        tabFolder.refreshPanelTitles();
    }

    @Override
    public void pageActivated() {
        updateSelection();
    }

    /**
     * Sets the editor selection to the selection of control of the active panel.
     */
    protected void updateSelection() {
        FilteredDataPanel panel = tabFolder.getActivePanel();
        if (panel != null) {
            IDataControl control = panel.getDataControl();
            IDList idList = control.getSelectedIDs();
            if (idList == null)
                idList = IDList.EMPTY;
            scaveEditor.setSelection(new IDListSelection(idList, control.getResultFileManager()));
            setFilterAction.update(panel);

            // TODO FIXME HACK this is just to make the "available templates" actually update when the selection changes,
            // but actually, we do not need to be this eager, we would be fine if we did this every time
            // the user requests a context menu
            configureContextMenu(tabFolder.getAllPanel());
            configureContextMenu(tabFolder.getScalarsPanel());
            configureContextMenu(tabFolder.getParametersPanel());
            configureContextMenu(tabFolder.getVectorsPanel());
            configureContextMenu(tabFolder.getHistogramsPanel());
        }
    }

    public void setNumericPrecision(int prec) {
        this.numericPrecision = prec;
        getAllPanel().getDataControl().setNumericPrecision(prec);
        getScalarsPanel().getDataControl().setNumericPrecision(prec);
        getParametersPanel().getDataControl().setNumericPrecision(prec);
        getVectorsPanel().getDataControl().setNumericPrecision(prec);
        getHistogramsPanel().getDataControl().setNumericPrecision(prec);
    }

    public int getNumericPrecision() {
        return numericPrecision;
    }
}
