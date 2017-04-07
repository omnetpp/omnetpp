/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import java.util.concurrent.Callable;

import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.ui.FocusManager;
import org.omnetpp.scave.actions.SetFilterAction2;
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ScaveEditorContributor;
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
import org.omnetpp.scave.model.ResultType;

/**
 * This is the "Browse Data" page of Scave Editor
 */
public class BrowseDataPage extends ScaveEditorPage {

    // UI elements
    private Label label;
    private FilteredDataTabFolder tabFolder;

    private IResultFilesChangeListener fileChangeListener;
    private Runnable scheduledUpdate;

    private SetFilterAction2 setFilterAction = new SetFilterAction2();

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
        configureFilteredDataPanel(tabFolder.getHistogramsPanel());
        configureContextMenu(tabFolder.getAllPanel());
        configureContextMenu(tabFolder.getScalarsPanel());
        configureContextMenu(tabFolder.getVectorsPanel());
        configureContextMenu(tabFolder.getHistogramsPanel());

        // set up contents
        ResultFileManagerEx manager = scaveEditor.getResultFileManager();
        tabFolder.setResultFileManager(manager);

        // ensure that focus gets restored correctly after user goes somewhere else and then comes back
        setFocusManager(new FocusManager(this));

    }

    private void configureContextMenu(FilteredDataPanel panel) {
        // populate the popup menu of the panel
        IMenuManager contextMenuManager = panel.getDataControl().getContextMenuManager();
        ScaveEditorContributor editorContributor = ScaveEditorContributor.getDefault();
        if (editorContributor != null) {
            contextMenuManager.add(editorContributor.getCreateTempChartAction());
            contextMenuManager.add(new Separator());
            contextMenuManager.add(editorContributor.getAddFilterToDatasetAction());
            contextMenuManager.add(editorContributor.getAddSelectedToDatasetAction());
            contextMenuManager.add(new Separator());
            contextMenuManager.add(editorContributor.createExportMenu());
            contextMenuManager.add(editorContributor.getCopyToClipboardAction());
            contextMenuManager.add(new Separator());
            contextMenuManager.add(setFilterAction);
            if (panel.getDataControl() instanceof DataTable)
                contextMenuManager.add(new ChooseTableColumnsAction((DataTable)panel.getDataControl()));
            if (panel.getDataControl() instanceof DataTree)
                ((DataTree)panel.getDataControl()).contributeToContextMenu(contextMenuManager);
            if (ResultType.VECTOR_LITERAL.equals(panel.getType())) {
                contextMenuManager.add(new Separator());
                contextMenuManager.add(editorContributor.getShowOutputVectorViewAction());
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
            public void contentChanged(IDataControl control) {
                showStatusMessage(String.format("Selected %d out of %d rows", control.getSelectionCount(), control.getItemCount()));
            }
        });
    }

    private void hookListeners() {
        // asynchronously update the page contents on result file changes (ie. input file load/unload)
        if (fileChangeListener == null) {
            fileChangeListener = new IResultFilesChangeListener() {
                public void resultFileManagerChanged(ResultFileManagerChangeEvent event) {
                    switch (event.getChangeType()) {
                    case LOAD:
                    case UNLOAD:
                        final ResultFileManager manager = event.getResultFileManager();
                        if (scheduledUpdate == null) {
                            scheduledUpdate = new Runnable() {
                                public void run() {
                                    scheduledUpdate = null;
                                    if (!isDisposed()) {
                                        ResultFileManager.callWithReadLock(manager, new Callable<Object>() {
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
                ScaveEditorContributor editorContributor = ScaveEditorContributor.getDefault();
                if (editorContributor != null)
                    editorContributor.getCreateTempChartAction().run();
            }
        };

        tabFolder.getAllPanel().getDataControl().addSelectionListener(selectionChangeListener);
        tabFolder.getVectorsPanel().getDataControl().addSelectionListener(selectionChangeListener);
        tabFolder.getScalarsPanel().getDataControl().addSelectionListener(selectionChangeListener);
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
        IDList items = manager.getAllItems(false, true);
        IDList vectors = manager.getAllVectors(false);
        IDList scalars = manager.getAllScalars(false, true);
        IDList histograms = manager.getAllHistograms(false);

        tabFolder.getAllPanel().setIDList(items);
        tabFolder.getScalarsPanel().setIDList(scalars);
        tabFolder.getVectorsPanel().setIDList(vectors);
        tabFolder.getHistogramsPanel().setIDList(histograms);
        tabFolder.refreshPanelTitles();
    }

    /**
     * Sets the editor selection to the selection of control of the active panel.
     */
    protected void updateSelection() {
        FilteredDataPanel panel = tabFolder.getActivePanel();
        if (panel != null) {
            IDataControl control = panel.getDataControl();
            scaveEditor.setSelection(new IDListSelection(control.getSelectedIDs(), control.getResultFileManager()));
            setFilterAction.update(panel);
        }
    }
}
