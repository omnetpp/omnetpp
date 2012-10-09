/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.datatable;

import static org.omnetpp.scave.model2.FilterField.MODULE;
import static org.omnetpp.scave.model2.FilterField.NAME;
import static org.omnetpp.scave.model2.FilterField.RUN;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.custom.StackLayout;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.ui.FilterCombo;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.model2.FilterHints;

/**
 * A composite with UI elements to filter a data table.
 * This is a passive component, needs to be configured
 * to do anything useful.
 * @author andras
 */
public class FilteringPanel extends Composite {
    private Image IMG_BASICFILTER = ScavePlugin.getImage("icons/full/obj16/basicfilter.png");
    private Image IMG_ADVANCEDFILTER = ScavePlugin.getImage("icons/full/obj16/advancedfilter.png");
    private Image IMG_RUNFILTER = ScavePlugin.getImage("icons/full/obj16/runfilter.png");

    // Switch between "Simple" and "Advanced"
    private Button toggleFilterTypeButton;
    private boolean showingAdvancedFilter;
    private StackLayout stackLayout;  // to set topControl to either advancedFilterPanel or simpleFilterPanel

    // Edit field for the "Advanced" mode
    private Composite advancedFilterPanel;
    private FilterField advancedFilter;

    // Combo boxes for the "Simple" mode
    private Composite simpleFilterPanel;
    private Combo runCombo;
    private Combo moduleCombo;
    private Combo dataCombo;

    // The "Go" button
    private Button filterButton;

    public FilteringPanel(Composite parent, int style) {
        super(parent, style);
        initialize();
    }

    public Text getAdvancedFilterText() {
        return advancedFilter.getText();
    }

    public Combo getModuleNameCombo() {
        return moduleCombo;
    }

    public Combo getNameCombo() {
        return dataCombo;
    }

    public Combo getRunNameCombo() {
        return runCombo;
    }

    public Button getFilterButton() {
        return filterButton;
    }

    public Button getToggleFilterTypeButton() {
        return toggleFilterTypeButton;
    }

    public void setFilterHints(FilterHints hints) {
        setFilterHints(runCombo, hints.getHints(RUN));
        setFilterHints(moduleCombo, hints.getHints(MODULE));
        setFilterHints(dataCombo, hints.getHints(NAME));
        advancedFilter.setFilterHints(hints);
    }

    private void setFilterHints(Combo filterCombo, String[] hints) {
        String[] items = hints;
        // prevent gtk halting when the item count ~10000
        int maxCount = 1000;
        if (hints.length > maxCount) {
            items = new String[maxCount];
            System.arraycopy(hints, 0, items, 0, maxCount - 1);
            items[maxCount - 1] = String.format("<%d skipped>", hints.length - (maxCount - 1));
        }
        filterCombo.setItems(items);
    }

    public void showSimpleFilter() {
        stackLayout.topControl = simpleFilterPanel;
        showingAdvancedFilter = false;
        toggleFilterTypeButton.setImage(IMG_ADVANCEDFILTER);
        toggleFilterTypeButton.setToolTipText("Switch to Advanced Filter");
        getParent().layout(true, true);
    }

    public void showAdvancedFilter() {
        stackLayout.topControl = advancedFilterPanel;
        showingAdvancedFilter = true;
        toggleFilterTypeButton.setImage(IMG_BASICFILTER);
        toggleFilterTypeButton.setToolTipText("Switch to Basic Filter");
        getParent().layout(true, true);
    }

    public boolean isShowingAdvancedFilter() {
        return showingAdvancedFilter;
    }

    private void initialize() {
        GridLayout gridLayout;

        gridLayout = new GridLayout();
        gridLayout.marginHeight = 0;
        this.setLayout(gridLayout);

        Composite filterContainer = new Composite(this, SWT.NONE);
        filterContainer.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
        gridLayout = new GridLayout(3, false); // filter panel, [ExecuteFilter], [Basic/Advanced]
        gridLayout.marginHeight = 0;
        gridLayout.marginWidth = 0;
        filterContainer.setLayout(gridLayout);

        Composite filterFieldsContainer = new Composite(filterContainer, SWT.NONE);
        filterFieldsContainer.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        filterFieldsContainer.setLayout(stackLayout = new StackLayout());

        // the "Advanced" view with the content-assisted input field
        advancedFilterPanel = new Composite(filterFieldsContainer, SWT.NONE);
        advancedFilterPanel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        advancedFilterPanel.setLayout(new GridLayout(1, false));
        ((GridLayout)advancedFilterPanel.getLayout()).marginHeight = 1;
        ((GridLayout)advancedFilterPanel.getLayout()).marginWidth = 0;

        advancedFilter = new FilterField(advancedFilterPanel, SWT.SINGLE | SWT.BORDER | SWT.SEARCH);
        advancedFilter.getLayoutControl().setLayoutData(new GridData(SWT.FILL, SWT.END, true, true));
        advancedFilter.getText().setMessage("type filter expression");
        advancedFilter.getText().setToolTipText("Filter Expression (Ctrl+Space for Content Assist)");

        // the "Basic" view with a series of combo boxes
        simpleFilterPanel = new Composite(filterFieldsContainer, SWT.NONE);
        simpleFilterPanel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        simpleFilterPanel.setLayout(new GridLayout(1, false));
        ((GridLayout)simpleFilterPanel.getLayout()).marginHeight = 0;
        ((GridLayout)simpleFilterPanel.getLayout()).marginWidth = 0;

        Composite sashForm = new SashForm(simpleFilterPanel, SWT.SMOOTH);
        sashForm.setLayoutData(new GridData(SWT.FILL, SWT.END, true, true));
        runCombo = createFilterCombo(sashForm, "runID filter", "RunID Filter");
        moduleCombo = createFilterCombo(sashForm, "module filter", "Module Filter");
        dataCombo = createFilterCombo(sashForm, "statistic name filter", "Statistic Name Filter");

        // Filter button
        filterButton = new Button(filterContainer, SWT.NONE);
        filterButton.setImage(IMG_RUNFILTER);
        filterButton.setToolTipText("Execute Filter");
        filterButton.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false));

        // Toggle button
        toggleFilterTypeButton = new Button(filterContainer, SWT.PUSH);
        toggleFilterTypeButton.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false));

        showSimpleFilter();
    }

    private Combo createFilterCombo(Composite parent, String filterMessage, String tooltipText) {
        FilterCombo combo = new FilterCombo(parent, SWT.BORDER);
        combo.setMessage(filterMessage);
        combo.setToolTipText(tooltipText);
        combo.setVisibleItemCount(20);

        return combo;
    }
}
