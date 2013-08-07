/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.datatable;

import static org.omnetpp.scave.engine.ResultItemField.MODULE;
import static org.omnetpp.scave.engine.ResultItemField.NAME;
import static org.omnetpp.scave.engine.ResultItemField.RUN;

import java.util.Arrays;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.common.ui.FocusManager;
import org.omnetpp.common.ui.IHasFocusManager;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model2.Filter;
import org.omnetpp.scave.model2.FilterField;
import org.omnetpp.scave.model2.FilterHints;
import org.omnetpp.scave.model2.FilterUtil;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Displays a data control of vectors/scalars/histograms with filter
 * combo boxes.
 *
 * This class is reusable, which means it only knows that it has to
 * display an IDList belonging to a particular ResultFileManager,
 * and has absolutely no reference to the editor, or EMF model objects,
 * or any widgets outside -- nothing.
 *
 * The user is responsible to keep contents up-to-date in case
 * ResultFileManager or IDList contents change. Refreshing can be
 * done by calling setIDList().
 *
 * @author andras
 */
public class FilteredDataPanel extends Composite implements IHasFocusManager {

    private FilteringPanel filterPanel;
    private IDataControl data;
    private IDList idlist; // the unfiltered data list
    private ResultType type;
    private FocusManager focusManager;

    public FilteredDataPanel(Composite parent, int style, ResultType type) {
        super(parent, style);
        this.type = type;
        initialize(type);
        configureFilterPanel();
    }

    public FilteringPanel getFilterPanel() {
        return filterPanel;
    }

    public IDataControl getDataControl() {
        return data;
    }

    public void setIDList(IDList idlist) {
        this.idlist = idlist;
        updateFilterCombos();
        runFilter();
    }

    public IDList getIDList() {
        return idlist;
    }

    public void setResultFileManager(ResultFileManagerEx manager) {
        data.setResultFileManager(manager);
    }

    public ResultFileManager getResultFileManager() {
        return data.getResultFileManager();
    }

    public ResultType getType() {
        return type;
    }

    protected void initialize(ResultType type) {
        GridLayout gridLayout = new GridLayout(1, false);
        gridLayout.marginHeight = 0;
        gridLayout.marginWidth = 0;
        setLayout(gridLayout);
        filterPanel = new FilteringPanel(this, SWT.NONE);
        filterPanel.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));

        if (type == null)
            data = new DataTree(this, SWT.MULTI);
        else
            data = new DataTable(this, SWT.MULTI, type);

        data.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        filterPanel.getToggleFilterTypeButton().addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                if (filterPanel.isShowingAdvancedFilter())
                    trySwitchToSimpleFilter();
                else
                    switchToAdvancedFilter();
            }
        });

        focusManager = new FocusManager(this);
    }

    protected void configureFilterPanel() {
        SelectionListener selectionListener = new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                // check the filter string
                if (!isFilterPatternValid()) {
                    MessageDialog.openWarning(getShell(), "Error in Filter Expression", "Filter expression is invalid, please fix it. Contents are not changed.");
                    return;
                }
                runFilter();

                if (e.widget instanceof Combo)
                    updateFilterCombosExcept((Combo)e.widget);
            }
            @Override
            public void widgetDefaultSelected(SelectionEvent e) {
                widgetSelected(e);  //delegate
            }
        };

        // when the filter button gets pressed, update the content
        filterPanel.getFilterButton().addSelectionListener(selectionListener);
        filterPanel.getAdvancedFilterText().addSelectionListener(selectionListener);
        filterPanel.getRunNameCombo().addSelectionListener(selectionListener);
        filterPanel.getModuleNameCombo().addSelectionListener(selectionListener);
        filterPanel.getNameCombo().addSelectionListener(selectionListener);
    }

    protected void updateFilterCombos() {
        if (!filterPanel.isDisposed())
            filterPanel.setFilterHints(getFilterHints());
    }

    protected void updateFilterCombosExcept(Combo except)
    {
        if (data.getResultFileManager() != null && !filterPanel.isDisposed() && !filterPanel.isShowingAdvancedFilter())
        {
            FilterHints hints = new FilterHints();
            if (filterPanel.getRunNameCombo() != except)
            {
                Filter filter = getSimpleFilter(FilterField.MODULE, FilterField.NAME);
                IDList filteredIDList = computeFilteredIDList(filter);
                hints.addHints(FilterField.RUN, data.getResultFileManager(), filteredIDList);
            }
            if (filterPanel.getModuleNameCombo() != except)
            {
                Filter filter = getSimpleFilter(FilterField.RUN, FilterField.NAME);
                IDList filteredIDList = computeFilteredIDList(filter);
                hints.addHints(FilterField.MODULE, data.getResultFileManager(), filteredIDList);
            }
            if (filterPanel.getNameCombo() != except)
            {
                Filter filter = getSimpleFilter(FilterField.RUN, FilterField.MODULE);
                IDList filteredIDList = computeFilteredIDList(filter);
                hints.addHints(FilterField.NAME, data.getResultFileManager(), filteredIDList);
            }

            filterPanel.setFilterHintsOfCombos(hints);
        }
    }

    public FilterHints getFilterHints() {
        if (data.getResultFileManager() != null)
            return new FilterHints(data.getResultFileManager(), idlist);
        else
            return new FilterHints();
    }

    protected void runFilter() {
        Assert.isTrue(idlist!=null);

        Filter filter = isFilterPatternValid() ? getFilter() : null;
        IDList filteredIDList = computeFilteredIDList(filter);
        data.setIDList(filteredIDList);

        if (getParent() instanceof FilteredDataTabFolder)
            ((FilteredDataTabFolder)getParent()).refreshPanelTitles();
    }

    protected IDList computeFilteredIDList(Filter filter) {
        if (data.getResultFileManager() == null) {
            // no result file manager, return empty list
            return new IDList();
        }
        else if (filter != null) {
            // run the filter on the unfiltered IDList, and return the result
            IDList filteredIDList = ScaveModelUtil.filterIDList(idlist, filter, data.getResultFileManager());
            return filteredIDList;
        }
        else {
            // fallback: if filter is not valid, just show an unfiltered list. This should be
            // done because we get invoked indirectly as well, like when files get added/removed
            // on the Inputs page. For the same reason, this function should not bring up an
            // error dialog (but the Filter button itself may do so).
            return idlist;
        }
    }

    public boolean isFilterPatternValid() {
        try {
            ResultFileManager.checkPattern(getFilter().getFilterPattern());
        } catch (Exception e) {
            return false; // apparently not valid
        }
        return true;
    }

    public Filter getFilter() {
        String filterPattern;
        if (filterPanel.isShowingAdvancedFilter())
            filterPattern = filterPanel.getAdvancedFilterText().getText();
        else
            filterPattern = assembleFilterPattern(filterPanel.getSimpleFilterFields());
        return new Filter(filterPattern);
    }

    public Filter getSimpleFilter(FilterField... includedFields)
    {
        return getSimpleFilter(Arrays.asList(includedFields));
    }

    public Filter getSimpleFilter(List<FilterField> includedFields)
    {
        return new Filter(assembleFilterPattern(includedFields));
    }

    private String assembleFilterPattern(List<FilterField> includedFields) {
        String runId = includedFields.contains(FilterField.RUN) ? filterPanel.getRunNameCombo().getText() : "";
        String moduleName = includedFields.contains(FilterField.MODULE) ? filterPanel.getModuleNameCombo().getText() : "";
        String name = includedFields.contains(FilterField.NAME) ? filterPanel.getNameCombo().getText() : "";
        String filterPattern = new FilterUtil(runId, moduleName, name).getFilterPattern();
        return filterPattern.equals("*") ? "" : filterPattern;  // replace "*": "" also works, and lets the filter field show the hint text
    }

    public void setFilterParams(Filter filter) {
        String filterPattern = filter.getFilterPattern();

        // an arbitrary pattern can only be shown in advanced view -- switch there
        if (!filterPanel.isShowingAdvancedFilter())
            filterPanel.showAdvancedFilter();
        filterPanel.getAdvancedFilterText().setText(filterPattern);
        runFilter();
    }

    /**
     * Switches the filter from "Advanced" to "Basic" mode. If this cannot be done
     * (filter string invalid or too complex), the user is prompted with a dialog,
     * and switching may or may not actually take place depending on the answer.
     * @return true if switching was actually done.
     */
    public boolean trySwitchToSimpleFilter() {
        if (!isFilterPatternValid()) {
            MessageDialog.openWarning(getShell(), "Error in Filter Expression", "Filter expression is invalid, please fix it first. (Or, just delete the whole text.)");
            return false;
        }

        String filterPattern = filterPanel.getAdvancedFilterText().getText();
        FilterUtil filterUtil = new FilterUtil(filterPattern, true);
        if (filterUtil.isLossy()) {
            boolean ok = MessageDialog.openConfirm(getShell(), "Filter Too Complex", "The current filter cannot be represented in Basic view without losing some of its details.");
            if (!ok)
                return false;  // user cancelled
        }

        String[] supportedFields = new String[] {RUN, MODULE, NAME};
        if (!filterUtil.containsOnly(supportedFields)) {
            boolean ok = MessageDialog.openConfirm(getShell(), "Filter Too Complex", "The current filter contains fields not present in Basic view. These extra fields will be discarded.");
            if (!ok)
                return false;  // user cancelled
        }

        filterPanel.getRunNameCombo().setText(filterUtil.getField(RUN));
        filterPanel.getModuleNameCombo().setText(filterUtil.getField(MODULE));
        filterPanel.getNameCombo().setText(filterUtil.getField(NAME));

        filterPanel.showSimpleFilter();
        runFilter();
        return true;
    }

    /**
     * Switches the filter from "Basic" to "Advanced" mode. This is always successful (unlike the opposite way).
     */
    public void switchToAdvancedFilter() {
        filterPanel.getAdvancedFilterText().setText(assembleFilterPattern(filterPanel.getSimpleFilterFields()));
        filterPanel.showAdvancedFilter();
        runFilter();
    }

    /**
     * Shows/hides the filter panel.
     */
    public void showFilterPanel(boolean show) {
        if (show != isFilterPanelVisible()) {
            filterPanel.setVisible(show);
            GridData data = (GridData)filterPanel.getLayoutData();
            data.exclude = !show;
            layout(true, true);
        }
    }

    /**
     * Returns {@code true} iff the filter panel is visible.
     */
    public boolean isFilterPanelVisible() {
        GridData data = (GridData)filterPanel.getLayoutData();
        return !data.exclude;
    }

    @Override
    public boolean setFocus() {
        // try to restore focus where it was last time
        if (focusManager.setFocus())
            return true;
        return super.setFocus();
    }
}
