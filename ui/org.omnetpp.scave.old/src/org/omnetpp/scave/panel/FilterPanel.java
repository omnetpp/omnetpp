/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.panel;

import java.text.DecimalFormat;

import org.eclipse.jface.action.IStatusLineManager;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.util.Assert;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.dnd.Clipboard;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Table;

import org.omnetpp.scave.dialogs.ExtendDatasetDialog;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.model.IDListModel;
import org.omnetpp.scave.model.IDListTransfer;
import org.omnetpp.scave.model.IDatasetStrategy;
import org.omnetpp.scave.model.IModelChangeListener;
import org.omnetpp.scave.plugin.ScavePlugin;

/**
 * TODO table selection: cannot just drag with the mouse
 * TODO need [Clear filter] button
 * FIXME problem: Del, Ctrl-C, Ctrl-V etc work with the scalars, even in the filter combos
 * FIXME cut-copy-paste is always enabled (try SelectionProvider business & remove local Clipboard)
 * TODO option to choose substring/full string match? (this would turn filtering into incremental search)
 * FIXME ResultFileManager::getModuleFilterHints (in C++) is incomplete
 */
public class FilterPanel {

    // allows us common handling of scalars and vectors
    private IDatasetStrategy strategy;

    // the composite to hold the controls (filter combos + table)
    private FilterPanelComposite thePanel;

    // attached to the table in the panel
    private TableViewer tableViewer;

    // reference to the dataset we work on
    private IDListModel dataset = null;

    // these ones are displayed in the table after filtering
    private IDList filteredSubset;

    // whether DEL and INS keys work
    private boolean editingAllowed = true;

    // the IDList clipboard (this might become redundant if we convert to the ISelectionProvider thing)
    private Clipboard clipboard;

    // the status line of the editor we're in (can be null)
    private IStatusLineManager statusLineManager;


    public FilterPanel(Composite parent, int style, IDatasetStrategy strategy) {
        this.strategy = strategy;
        thePanel = strategy.createFilterPanelComposite(parent, style);
        clipboard = new Clipboard(thePanel.getDisplay());

        // set up table viewer
        tableViewer = new TableViewer(thePanel.getTable());
        tableViewer.setLabelProvider(strategy.createFilterPanelLabelProvider());
        tableViewer.setContentProvider(new IDListContentProvider());
        TableContentSorter.makeTableSortable(tableViewer, strategy.createTableSorter());

        //thePanel.getTable().setFocus(); -- this takes no effect here

        // configure Filter button to apply filters; also combo selection and editing will re-run it
        // Note: we reuse the same listener object for all widgets
        Listener listener = new Listener() {
            public void handleEvent(Event event) {
                applyFilter();
            }
        };
        thePanel.getFilterButton().addListener(SWT.Selection, listener);
        thePanel.getRunNameCombo().addListener(SWT.Selection, listener);
        thePanel.getModuleNameCombo().addListener(SWT.Selection, listener);
        thePanel.getNameCombo().addListener(SWT.Selection, listener);

        // using Modify or even FocusOut might be confusing for new users
        // (when they type a wrong filter, table goes blank -- huh?)
        //thePanel.getRunNameCombo().addListener(SWT.FocusOut, listener);
        //thePanel.getModuleNameCombo().addListener(SWT.FocusOut, listener);
        //thePanel.getNameCombo().addListener(SWT.FocusOut, listener);
    }

    public void dispose() {
        clipboard.dispose();
    }

    public void setDataset(IDListModel dataset) {
        Assert.isTrue(this.dataset==null);

        this.dataset = dataset;
        dataset.addListener(new IModelChangeListener() {
            public void handleChange() {
                refresh();
            }
        });
        refresh();
    }

    public void setStatusLineManager(IStatusLineManager mgr) {
        statusLineManager = mgr;
    }

    public FilterPanelComposite getPanel() {
        return thePanel;
    }

    /**
     * Returns the stored dataset.
     */
    public IDListModel getDataset() {
        return dataset;
    }

    /**
     * Returns the subset of the dataset currently displayed in the table.
     */
    public IDList getFilteredSubset() {
        return filteredSubset;
    }

    /**
     * Returns the items selected in the table, or all items displayed
     * (i.e. the filtered subset) if none are selected.
     */
    public IDList getSelectedOrAllItems() {
        IDList sel = getSelection();
        if (sel.size()==0)
            return filteredSubset;
        return sel;
    }

    /**
     * Returns the items selected in the table.
     */
    public IDList getSelection() {
        long t0 = System.currentTimeMillis();
        Table table = thePanel.getTable();
        int[] items = table.getSelectionIndices();

        // for () loop was very slow, we use native code
        IDList sel = filteredSubset.getSubsetByIndices(items);
        System.out.println("getSelection: " + (System.currentTimeMillis()-t0));
        return sel;
    }

    /**
     * Removes from the dataset all the items selected in the table,
     * or the current item if none are selected.
     */
    public void deleteSelected() {
        Assert.isTrue(editingAllowed, "editing disabled");
        dataset.substract(getSelection());
    }

    /**
     * Replaces the dataset with the currently selected or shown items.
     */
    public void deleteRest() {
        Assert.isTrue(editingAllowed, "editing disabled");
        dataset.set(getSelectedOrAllItems());
    }

    /**
     * Refreshed the table contents. Normally this gets invoked automatically,
     * by a listener attached to IDListModel.
     */
    public void refresh() {
        Table table = thePanel.getTable();
        int index = table.getSelectionIndex();
        applyFilter();
        table.setSelection(index);
        refreshFilterCombos();
    }

    /**
     * Selects all items displayed in the table
     */
    public void selectAll() {
        Table table = thePanel.getTable();
        table.selectAll();
    }

    /**
     * Deselects all items in the table
     */
    public void deselectAll() {
        Table table = thePanel.getTable();
        table.deselectAll();
    }

    /**
     * Inverts table selection
     */
    public void invertSelection() {
        Table table = thePanel.getTable();
        int[] sel = table.getSelectionIndices();
        table.selectAll();
        table.deselect(sel);
    }

    /**
     * Pops up the "Extend Dataset" dialog
     */
    public void extendDataset() {
        Assert.isTrue(editingAllowed, "editing disabled");

        ExtendDatasetDialog dialog = new ExtendDatasetDialog(new Shell(), strategy);
        dialog.setDatasetToExtend(getDataset());
        dialog.open();
    }

    /**
     * Clipboard operation on the stored dataset
     */
    public void doCut() {
        IDList sel = getSelection();
        if (sel.size()==0)
            return;
        clipboard.setContents(new Object[] {sel}, new Transfer[] {IDListTransfer.getInstance()});
        deleteSelected();
    }

    /**
     * Clipboard operation on the stored dataset
     */
    public void doCopy() {
        IDList sel = getSelection();
        if (sel.size() > 0) {
            clipboard.setContents(new Object[] {sel}, new Transfer[] {IDListTransfer.getInstance()});
        }
    }

    /**
     * Clipboard operation on the stored dataset
     */
    public void doPaste() {
        IDList sel = (IDList) clipboard.getContents(IDListTransfer.getInstance());
        if (sel != null && sel.size() > 0) {
            if (strategy.isCompatible(sel)) {
                dataset.merge(sel);
            }
            else {
                MessageDialog.openError(new Shell(), "Error",
                        "Cannot drop or paste scalars into a vector dataset, or vica versa.");
            }
        }
    }

    private void refreshFilterCombos() {
        long t0 = System.currentTimeMillis();
        refreshRunNameCombo();
        refreshModuleNameCombo();
        refreshNameCombo();
        System.out.println("refreshFilterCombos: " + (System.currentTimeMillis()-t0));
    }

    private void refreshRunNameCombo() {
        ResultFileManager resultFileManager = ScavePlugin.getDefault().resultFileManager;
        StringVector filterWords = resultFileManager.getRunNameFilterHints(dataset.get());

//      HashMap map = new HashMap();
//      for (int i=0; i<(int)runs.size(); i++) {
//          String a = runs.get(i).getFileAndRunName();
//          map.put(a, null);
//
//          if (a.contains("#")) {
//              String b = a.replaceFirst("#.*", "#*");
//              map.put(b, null);
//          }
//      }
//
//      // convert to string array, sort, and put it into the combo
//      String [] filterWords = (String[]) map.keySet().toArray(new String[0]);
//      Arrays.sort(filterWords);
        fillCombo(thePanel.getRunNameCombo(), filterWords);
    }

    private void refreshNameCombo() {
        ResultFileManager resultFileManager = ScavePlugin.getDefault().resultFileManager;
        StringVector filterWords = resultFileManager.getNameFilterHints(dataset.get());
//      StringVector names = resultFileManager.getUniqueNames(dataset.get()).keys();
//
//      HashMap map = new HashMap();
//      for (int i=0; i<(int)names.size(); i++) {
//          String a = names.get(i);
//          map.put(a, null);
//
//          // break it up along spaces
//          // TODO: leave out wildcard expression if they occur only once (count in the value field of hashmap)
//          StringTokenizer st = new StringTokenizer(a);
//          String prefix = "";
//          String suffix = " *";
//          while (st.hasMoreTokens()) {
//              String w = st.nextToken();
//              if (!st.hasMoreTokens()) suffix = "";
//              map.put(prefix+w+suffix, null);
//              prefix = "* ";
//          }
//      }
//      // FIXME first the full ones, then the wildcard ones!
//      // convert to string array, sort, and put it into the combo
//      String [] filterWords = (String[]) map.keySet().toArray(new String[0]);
//      Arrays.sort(filterWords);
        fillCombo(thePanel.getNameCombo(), filterWords);
    }

    private void refreshModuleNameCombo() {
        ResultFileManager resultFileManager = ScavePlugin.getDefault().resultFileManager;
        StringVector filterWords = resultFileManager.getModuleFilterHints(dataset.get());

//      StringVector modNames = resultFileManager.getUniqueModuleNames(dataset.get()).keys();
//
//      HashMap map = new HashMap();
//      Pattern bracketedNumPattern = Pattern.compile("\\[[0-9]+\\]");
//      for (int i=0; i<(int)modNames.size(); i++) {
//          // first, replace bracketed numbers with "[*]"
//          String a = modNames.get(i);
//          map.put(a, null);
//          String b = bracketedNumPattern.matcher(a).replaceAll("[*]");
//          map.put(b, null);
//
//          // break it up along dots
//          // TODO: leave out wildcard expression if they occur only once
//          StringTokenizer st = new StringTokenizer(b, ".");
//          String prefix = "";
//          String suffix = ".*";
//          while (st.hasMoreTokens()) {
//              String w = st.nextToken();
//              if (!st.hasMoreTokens()) suffix = "";
//              map.put(prefix+w+suffix, null);
//              prefix = "*.";
//          }
//      }
//      // convert to string array, sort, and put it into the combo
//      String [] filterWords = (String[]) map.keySet().toArray(new String[0]);
//      Arrays.sort(filterWords);
        fillCombo(thePanel.getModuleNameCombo(), filterWords);
    }

    private void fillCombo(CCombo combo, StringVector words) {
        combo.removeAll();
        for (int i=0; i<(int)words.size(); i++)
            combo.add(words.get(i));
        if (combo.getText()=="")
            combo.setText("*");
    }

    protected void applyFilter() {
        String fileAndRunFilter = thePanel.getRunNameCombo().getText();
        String moduleFilter = thePanel.getModuleNameCombo().getText();
        String nameFilter = thePanel.getNameCombo().getText();

        long t0 = System.currentTimeMillis();
        ResultFileManager resultFileManager = ScavePlugin.getDefault().resultFileManager;
        filteredSubset = resultFileManager.getFilteredList(dataset.get(), fileAndRunFilter, moduleFilter, nameFilter);
        System.out.println("filtering: " + (System.currentTimeMillis()-t0));

        long t1 = System.currentTimeMillis();
        tableViewer.getTable().setItemCount((int)filteredSubset.size()); // JFace bug #95811: setInput() should do this
        tableViewer.setInput(filteredSubset);
        System.out.println("refreshing table: " + (System.currentTimeMillis()-t1));

        if (statusLineManager!=null) {
            DecimalFormat f = new DecimalFormat();
            statusLineManager.setMessage("Showing "+f.format(filteredSubset.size())+
                    " out of "+f.format(dataset.size())+" items in the dataset");
        }
    }

    public boolean isEditingAllowed() {
        return editingAllowed;
    }

    public void setEditingAllowed(boolean editingAllowed) {
        this.editingAllowed = editingAllowed;
    }
}
