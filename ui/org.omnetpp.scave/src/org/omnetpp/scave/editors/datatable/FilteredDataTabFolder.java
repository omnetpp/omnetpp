package org.omnetpp.scave.editors.datatable;

import java.text.NumberFormat;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engineext.ResultFileManagerEx;

/**
 * A TabFolder that contains All, Vectors, Scalars, Histograms tabs with
 * corresponding data trees/tables. This is a passive UI component.
 *
 * @author Levy
 */
public class FilteredDataTabFolder extends TabFolder {
    protected TabItem allTab;
    protected TabItem vectorsTab;
    protected TabItem scalarsTab;
    protected TabItem parametersTab;
    protected TabItem histogramsTab;

    protected FilteredDataPanel allPanel;
    protected FilteredDataPanel vectorsPanel;
    protected FilteredDataPanel scalarsPanel;
    protected FilteredDataPanel parametersPanel;
    protected FilteredDataPanel histogramsPanel;
    protected NumberFormat numFormat; // for item counts in the tabs

    public FilteredDataTabFolder(Composite parent, int style) {
        super(parent, style);
        numFormat = NumberFormat.getIntegerInstance();
        numFormat.setGroupingUsed(true);
        initialize();
    }

    /**
     * Override the ban on subclassing of TabFolder, after having read the doc of
     * checkSubclass(). In this class we only build upon the public interface
     * of TabFolder, so there can be no unwanted side effects. We prefer subclassing
     * to delegating all 1,000,000 TabFolder methods to an internal TabFolder instance.
     */
    @Override
    protected void checkSubclass() {
    }

    protected void initialize() {
        // create pages
        allPanel = new FilteredDataPanel(this, SWT.NONE, PanelType.ALL);
        parametersPanel = new FilteredDataPanel(this, SWT.NONE, PanelType.PARAMETERS);
        scalarsPanel = new FilteredDataPanel(this, SWT.NONE, PanelType.SCALARS);
        vectorsPanel = new FilteredDataPanel(this, SWT.NONE, PanelType.VECTORS);
        histogramsPanel = new FilteredDataPanel(this, SWT.NONE, PanelType.HISTOGRAMS);

        // create tabs (note: tab labels will be refreshed from initialize())
        allTab = addItem(allPanel);
        parametersTab = addItem(parametersPanel);
        scalarsTab = addItem(scalarsPanel);
        histogramsTab = addItem(histogramsPanel);
        vectorsTab = addItem(vectorsPanel);
        refreshPanelTitles();
        setActivePanel(allPanel);

        // when tab gets clicked, transfer focus to its panel
        addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                getActivePanel().setFocus();
            }
        });
    }

    protected TabItem addItem(Control control) {
        TabItem tabItem = new TabItem(this, SWT.NONE);
        tabItem.setControl(control);

        return tabItem;
    }

    public TabItem getAllTab() {
        return allTab;
    }

    public TabItem getScalarsTab() {
        return scalarsTab;
    }

    public TabItem getParametersTab() {
        return parametersTab;
    }

    public TabItem getVectorsTab() {
        return vectorsTab;
    }

    public TabItem getHistogramsTab() {
        return histogramsTab;
    }

    public FilteredDataPanel getAllPanel() {
        return allPanel;
    }

    public FilteredDataPanel getScalarsPanel() {
        return scalarsPanel;
    }

    public FilteredDataPanel getParametersPanel() {
        return parametersPanel;
    }

    public FilteredDataPanel getVectorsPanel() {
        return vectorsPanel;
    }

    public FilteredDataPanel getHistogramsPanel() {
        return histogramsPanel;
    }

    public FilteredDataPanel getActivePanel() {
        int index = getSelectionIndex();
        return index < 0 ? null : (FilteredDataPanel)getItem(index).getControl();
    }

    public void setActivePanel(Control control) {
        TabItem[] items = getItems();

        for (int i = 0; i < items.length; i++) {
            TabItem tabItem = items[i];

            if (tabItem.getControl() == control) {
                setSelection(i);
                return;
            }
        }

        throw new IllegalArgumentException();
    }

    public FilteredDataPanel getPanel(PanelType type) {
        switch (type) {
        case ALL: return allPanel;
        case PARAMETERS: return parametersPanel;
        case SCALARS: return scalarsPanel;
        case VECTORS: return vectorsPanel;
        case HISTOGRAMS: return histogramsPanel;
        default: throw new IllegalArgumentException();
        }
    }

    public void switchToNonEmptyPanel() {
        FilteredDataPanel panel = getActivePanel();

        if (panel == null || panel.getIDList().isEmpty()) {
            if (!scalarsPanel.getIDList().isEmpty())
                setActivePanel(scalarsPanel);
            if (!parametersPanel.getIDList().isEmpty())
                setActivePanel(parametersPanel);
            else if (!vectorsPanel.getIDList().isEmpty())
                setActivePanel(vectorsPanel);
            else if (!histogramsPanel.getIDList().isEmpty())
                setActivePanel(histogramsPanel);
            else
                setActivePanel(allPanel);
        }
    }

    public void setResultFileManager(ResultFileManagerEx manager) {
        allPanel.setResultFileManager(manager);
        scalarsPanel.setResultFileManager(manager);
        parametersPanel.setResultFileManager(manager);
        vectorsPanel.setResultFileManager(manager);
        histogramsPanel.setResultFileManager(manager);
    }

    public void refreshPanelTitles() {
        setPanelTitle(allTab, "&All");
        setPanelTitle(vectorsTab, "&Vectors");
        setPanelTitle(scalarsTab, "&Scalars");
        setPanelTitle(parametersTab, "&Parameters");
        setPanelTitle(histogramsTab, "&Histograms");
    }

    private void setPanelTitle(TabItem tab, String title) {
        FilteredDataPanel panel = (FilteredDataPanel)tab.getControl();
        IDList filtered = panel.getDataControl().getIDList();
        IDList total = panel.getIDList();
        boolean truncated = panel.isTruncated();

        if (total == null)
            tab.setText(title);
        else
            tab.setText(title + " (" + (filtered == null ? "?" : numFormat.format(filtered.size()) + (truncated ? "+" : "")) + " / " + numFormat.format(total.size()) + ")");
    }
}
