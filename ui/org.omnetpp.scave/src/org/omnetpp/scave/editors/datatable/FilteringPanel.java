/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.datatable;

import static org.omnetpp.scave.model2.FilterField.EXPERIMENT;
import static org.omnetpp.scave.model2.FilterField.MEASUREMENT;
import static org.omnetpp.scave.model2.FilterField.MODULE;
import static org.omnetpp.scave.model2.FilterField.NAME;
import static org.omnetpp.scave.model2.FilterField.REPLICATION;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.custom.StackLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.contentassist.ContentAssistUtil;
import org.omnetpp.common.ui.FilterCombo;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.assist.FilterContentProposalProvider;
import org.omnetpp.scave.model2.Filter;
import org.omnetpp.scave.model2.FilterField;
import org.omnetpp.scave.model2.FilterHints;
import org.omnetpp.scave.model2.FilterUtil;

/**
 * A composite with UI elements to filter a data table.
 * This is a passive component, needs to be configured
 * to do anything useful.
 * @author andras
 */
public class FilteringPanel extends Composite {

    private final List<FilterField> simpleFilterFields = Collections.unmodifiableList(Arrays.asList(new FilterField[] {EXPERIMENT, MEASUREMENT, REPLICATION, MODULE, NAME}));

    // Switch between "Simple" and "Advanced"
    private Button toggleFilterTypeButton;
    private boolean showingAdvancedFilter;
    private StackLayout stackLayout;  // to set topControl to either advancedFilterPanel or simpleFilterPanel

    // Edit field for the "Advanced" mode
    private Composite advancedFilterPanel;
    private Text advancedFilterText;
    private FilterContentProposalProvider filterProposalProvider;

    // Combo boxes for the "Simple" mode
    private Composite simpleFilterPanel;
    private Combo experimentCombo;
    private Combo measurementCombo;
    private Combo replicationCombo;
    private Combo moduleCombo;
    private Combo nameCombo;

    public FilteringPanel(Composite parent, int style) {
        super(parent, style);
        initialize();
    }

    public Text getAdvancedFilterText() {
        return advancedFilterText;
    }

    public List<FilterField> getSimpleFilterFields() {
        return simpleFilterFields;
    }

    public Combo getExperimentCombo() {
        return experimentCombo;
    }

    public Combo getMeasurementCombo() {
        return measurementCombo;
    }

    public Combo getReplicationCombo() {
        return replicationCombo;
    }

    public Combo getModuleNameCombo() {
        return moduleCombo;
    }

    public Combo getNameCombo() {
        return nameCombo;
    }

    public Combo getFilterCombo(FilterField field) {
        if (field.equals(EXPERIMENT))
            return experimentCombo;
        else if (field.equals(MEASUREMENT))
            return measurementCombo;
        else if (field.equals(REPLICATION))
            return replicationCombo;
        else if (field.equals(MODULE))
            return moduleCombo;
        else if (field.equals(NAME))
            return nameCombo;
        else
            return null;
    }

    public Button getToggleFilterTypeButton() {
        return toggleFilterTypeButton;
    }

    public void setFilterHints(FilterHints hints) {
        setFilterHints(experimentCombo, hints.getHints(EXPERIMENT));
        setFilterHints(measurementCombo, hints.getHints(MEASUREMENT));
        setFilterHints(replicationCombo, hints.getHints(REPLICATION));
        setFilterHints(moduleCombo, hints.getHints(MODULE));
        setFilterHints(nameCombo, hints.getHints(NAME));
        filterProposalProvider.setFilterHints(hints);
    }

    public void setFilterHintsOfCombos(FilterHints hints) {
        for (FilterField field : hints.getFields()) {
            Combo combo = getFilterCombo(field);
            if (combo != null)
                setFilterHints(combo, hints.getHints(field));
        }
    }

    private void setFilterHints(Combo filterCombo, String[] hints) {
        String text = filterCombo.getText();

        String[] items = hints;
        // prevent gtk halting when the item count ~10000
        int maxCount = 1000;
        if (hints.length > maxCount) {
            items = new String[maxCount];
            System.arraycopy(hints, 0, items, 0, maxCount - 1);
            items[maxCount - 1] = String.format("<%d skipped>", hints.length - (maxCount - 1));
        }
        filterCombo.setItems(items);
        int index = filterCombo.indexOf(text);
        if (index >= 0)
            filterCombo.select(index);
    }

    public void showSimpleFilter() {
        stackLayout.topControl = simpleFilterPanel;
        showingAdvancedFilter = false;
        toggleFilterTypeButton.setImage(ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_ADVANCEDFILTER));
        toggleFilterTypeButton.setToolTipText("Switch to Advanced Filter");
        getParent().layout(true, true);
        nameCombo.setFocus();
    }

    public void showAdvancedFilter() {
        stackLayout.topControl = advancedFilterPanel;
        showingAdvancedFilter = true;
        toggleFilterTypeButton.setImage(ScavePlugin.getCachedImage(ScaveImages.IMG_OBJ16_BASICFILTER));
        toggleFilterTypeButton.setToolTipText("Switch to Basic Filter");
        getParent().layout(true, true);
        advancedFilterText.setFocus();
    }

    private String asteriskToEmpty(String filter) {
        return filter.trim().equals("*") ? "" : filter;
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

        String filterPattern = getAdvancedFilterText().getText();
        FilterUtil filterUtil = new FilterUtil(filterPattern, true);
        if (filterUtil.isLossy()) {
            boolean ok = MessageDialog.openConfirm(getShell(), "Filter Too Complex", "The current filter cannot be represented in Basic view without losing some of its details.");
            if (!ok)
                return false;  // user cancelled
        }

        String[] supportedFields = new String[] {EXPERIMENT.getName(), MEASUREMENT.getName(), REPLICATION.getName(), MODULE.getName(), NAME.getName()};
        if (!filterUtil.containsOnly(supportedFields)) {
            boolean ok = MessageDialog.openConfirm(getShell(), "Filter Too Complex", "The current filter contains fields not present in Basic view. These extra fields will be discarded.");
            if (!ok)
                return false;  // user cancelled
        }

        experimentCombo.setText(asteriskToEmpty(filterUtil.getField(EXPERIMENT.getName())));
        measurementCombo.setText(asteriskToEmpty(filterUtil.getField(MEASUREMENT.getName())));
        replicationCombo.setText(asteriskToEmpty(filterUtil.getField(REPLICATION.getName())));
        moduleCombo.setText(asteriskToEmpty(filterUtil.getField(MODULE.getName())));
        nameCombo.setText(asteriskToEmpty(filterUtil.getField(NAME.getName())));

        showSimpleFilter();
        return true;
    }

    public void switchToAdvancedFilter() {
        getAdvancedFilterText().setText(assembleFilterPattern(getSimpleFilterFields()));
        showAdvancedFilter();
    }

    public boolean isShowingAdvancedFilter() {
        return showingAdvancedFilter;
    }

    public boolean isFilterPatternValid() {
        return getFilter().isValid();
    }

    public Filter getFilter() {
        String filterPattern;
        if (isShowingAdvancedFilter())
            filterPattern = getAdvancedFilterText().getText();
        else
            filterPattern = assembleFilterPattern(getSimpleFilterFields());
        return new Filter(filterPattern);
    }

    public Filter getFilterIfValid() {
        Filter filter = getFilter();
        return filter.isValid() ? filter : null;
    }

    public Filter getSimpleFilter(FilterField... includedFields) {
        return getSimpleFilter(Arrays.asList(includedFields));
    }

    public Filter getSimpleFilter(List<FilterField> includedFields) {
        return new Filter(assembleFilterPattern(includedFields));
    }

    public Filter getSimpleFilterExcluding(FilterField... excludedFields) {
        return getSimpleFilterExcluding(Arrays.asList(excludedFields));
    }

    public Filter getSimpleFilterExcluding(List<FilterField> excludedFields) {
        return new Filter(assembleFilterPatternExcluding(excludedFields));
    }

    private String assembleFilterPattern(List<FilterField> includedFields) {
        FilterUtil filter = new FilterUtil();
        for (FilterField field : simpleFilterFields)
            if (includedFields.contains(field))
                filter.setField(field.getName(), getFilterCombo(field).getText());
        String filterPattern = filter.getFilterPattern();
        return filterPattern.equals("*") ? "" : filterPattern;  // replace "*": "" also works, and lets the filter field show the hint text
    }

    private String assembleFilterPatternExcluding(List<FilterField> excludedFields) {
        FilterUtil filter = new FilterUtil();
        for (FilterField field : simpleFilterFields)
            if (!excludedFields.contains(field))
                filter.setField(field.getName(), getFilterCombo(field).getText());
        String filterPattern = filter.getFilterPattern();
        return filterPattern.equals("*") ? "" : filterPattern;  // replace "*": "" also works, and lets the filter field show the hint text
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

        // Toggle button
        toggleFilterTypeButton = new Button(filterContainer, SWT.PUSH);
        toggleFilterTypeButton.setLayoutData(new GridData(SWT.BEGINNING, SWT.FILL, false, true));

        Composite filterFieldsContainer = new Composite(filterContainer, SWT.NONE);
        filterFieldsContainer.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        filterFieldsContainer.setLayout(stackLayout = new StackLayout());

        // the "Advanced" view with the content-assisted input field
        advancedFilterPanel = new Composite(filterFieldsContainer, SWT.NONE);
        advancedFilterPanel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        advancedFilterPanel.setLayout(new GridLayout(1, false));
        ((GridLayout)advancedFilterPanel.getLayout()).marginHeight = 1;
        ((GridLayout)advancedFilterPanel.getLayout()).marginWidth = 0;

        advancedFilterText = new Text(advancedFilterPanel, SWT.SINGLE | SWT.BORDER | SWT.SEARCH);
        advancedFilterText.setLayoutData(new GridData(SWT.FILL, SWT.END, true, true));
        advancedFilterText.setMessage("type filter expression");
        advancedFilterText.setToolTipText("Filter Expression (Ctrl+Space for Content Assist)");
        filterProposalProvider = new FilterContentProposalProvider();
        ContentAssistUtil.configureText(advancedFilterText, filterProposalProvider);

        // the "Basic" view with a series of combo boxes
        simpleFilterPanel = new Composite(filterFieldsContainer, SWT.NONE);
        simpleFilterPanel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        simpleFilterPanel.setLayout(new GridLayout(1, false));
        ((GridLayout)simpleFilterPanel.getLayout()).marginHeight = 0;
        ((GridLayout)simpleFilterPanel.getLayout()).marginWidth = 0;

        SashForm sashForm = new SashForm(simpleFilterPanel, SWT.SMOOTH);
        sashForm.setLayoutData(new GridData(SWT.FILL, SWT.END, true, true));
        experimentCombo = createFilterCombo(sashForm, "experiment filter", "Experiment Filter");
        measurementCombo = createFilterCombo(sashForm, "measurement filter", "Measurement Filter");
        replicationCombo = createFilterCombo(sashForm, "replication filter", "Replication Filter");
        moduleCombo = createFilterCombo(sashForm, "module filter", "Module Filter");
        nameCombo = createFilterCombo(sashForm, "result name filter", "Result Name Filter");
        sashForm.setWeights(new int[] {2,3,1,3,3});

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
