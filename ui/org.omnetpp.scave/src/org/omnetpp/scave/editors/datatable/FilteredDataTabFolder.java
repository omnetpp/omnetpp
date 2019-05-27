package org.omnetpp.scave.editors.datatable;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
import org.omnetpp.scave.model.ResultType;

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
    protected TabItem histogramsTab;

    protected FilteredDataPanel allPanel;
    protected FilteredDataPanel vectorsPanel;
    protected FilteredDataPanel scalarsPanel;
    protected FilteredDataPanel histogramsPanel;

    public FilteredDataTabFolder(Composite parent, int style) {
        super(parent, style);
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
        allPanel = new FilteredDataPanel(this, SWT.NONE, null);
        vectorsPanel = new FilteredDataPanel(this, SWT.NONE, ResultType.VECTOR);
        scalarsPanel = new FilteredDataPanel(this, SWT.NONE, ResultType.SCALAR);
        histogramsPanel = new FilteredDataPanel(this, SWT.NONE, ResultType.HISTOGRAM);

        // create tabs (note: tab labels will be refreshed from initialize())
        allTab = addItem(allPanel);
        vectorsTab = addItem(vectorsPanel);
        scalarsTab = addItem(scalarsPanel);
        histogramsTab = addItem(histogramsPanel);
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

    public FilteredDataPanel getVectorsPanel() {
        return vectorsPanel;
    }

    public FilteredDataPanel getHistogramsPanel() {
        return histogramsPanel;
    }

    public FilteredDataPanel getFilteredDataPanel(long id) {
        int type = ResultFileManager.getTypeOf(id);

        if (type == ResultFileManager.SCALAR)
            return scalarsPanel;
        else if (type == ResultFileManager.VECTOR)
            return vectorsPanel;
        else if (type == ResultFileManager.HISTOGRAM)
            return histogramsPanel;
        else
            return allPanel;
    }

    public FilteredDataPanel getActivePanel() {
        int index = getSelectionIndex();

        if (index >= 0)
            return (FilteredDataPanel)getItem(index).getControl();
        else
            return null;
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

        throw new IllegalStateException();
    }

    public void setActivePanel(ResultType type) {
        if (type == ResultType.VECTOR)
            setActivePanel(vectorsPanel);
        else if (type == ResultType.SCALAR)
            setActivePanel(scalarsPanel);
        else if (type == ResultType.HISTOGRAM)
            setActivePanel(histogramsPanel);
        else
            setActivePanel(allPanel);
    }

    public ResultType getActivePanelType() {
        FilteredDataPanel activePanel = getActivePanel();

        if (activePanel == allPanel)
            return null;
        else if (activePanel == vectorsPanel)
            return ResultType.VECTOR;
        else if (activePanel == scalarsPanel)
            return ResultType.SCALAR;
        else if (activePanel == histogramsPanel)
            return ResultType.HISTOGRAM;
        else
            throw new IllegalStateException();
    }

    public void switchToNonEmptyPanel() {
        FilteredDataPanel panel = getActivePanel();

        if (panel == null || panel.getIDList().isEmpty()) {
            if (!scalarsPanel.getIDList().isEmpty())
                setActivePanel(scalarsPanel);
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
        vectorsPanel.setResultFileManager(manager);
        histogramsPanel.setResultFileManager(manager);
    }

    public void refreshPanelTitles() {
        setPanelTitle(allTab, "&All");
        setPanelTitle(vectorsTab, "&Vectors");
        setPanelTitle(scalarsTab, "&Scalars");
        setPanelTitle(histogramsTab, "&Histograms");
    }

    private void setPanelTitle(TabItem tab, String title) {
        FilteredDataPanel panel = (FilteredDataPanel)tab.getControl();
        IDList filter = panel.getDataControl().getIDList();
        IDList total = panel.getIDList();

        if (total == null)
            tab.setText(title);
        else
            tab.setText(title + " (" + (filter == null ? "?" : filter.size()) + " / " + total.size() + ")");
    }
}
