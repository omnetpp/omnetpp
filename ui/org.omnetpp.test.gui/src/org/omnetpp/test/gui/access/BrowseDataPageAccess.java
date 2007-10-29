package org.omnetpp.test.gui.access;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Table;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.ButtonAccess;
import com.simulcraft.test.gui.access.ComboAccess;
import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.MenuAccess;
import com.simulcraft.test.gui.access.ShellAccess;
import com.simulcraft.test.gui.access.TabFolderAccess;
import com.simulcraft.test.gui.access.TabItemAccess;
import com.simulcraft.test.gui.access.TableAccess;
import com.simulcraft.test.gui.access.TableColumnAccess;
import com.simulcraft.test.gui.access.TextAccess;
import com.simulcraft.test.gui.core.UIStep;
import com.simulcraft.test.gui.core.InBackgroundThread;

public class BrowseDataPageAccess extends CompositeAccess {
	
	// tab names
	public static final String
		SCALARS = "Scalars.*",
		VECTORS = "Vectors.*",
		HISTOGRAMS = "Histograms.*";
	
	// column names
	public static final String
		DIRECTORY = "Dir.*",
		FILE_NAME = "File.*",
		CONFIG_NAME = "Config.*",
		RUN_NUMBER = "Run num.*",
		RUN_ID = "Run [iI][dD].*",
		MODULE = "Module.*",
		NAME = "Name.*",
		EXPERIMENT = "Experiment.*",
		MEASUREMENT = "Measurement.*",
		REPLICATION = "Replication.*",
		VALUE = "Value.*",
		COUNT = "Count.*",
		MEAN = "Mean.*",
		STDDEV = "St.*[dD]ev.*",
		MIN = "Min.*",
		MAX = "Max.*";

	public BrowseDataPageAccess(Composite composite) {
		super(composite);
	}
	
	@UIStep
	public TabFolderAccess getTabFolder() {
		return (TabFolderAccess)createAccess(
					findDescendantControl(TabFolder.class));
	}
	
	@UIStep
	public TabItemAccess getTabItem(String label) {
		return getTabFolder().findItemByText(label);
	}
	
	@UIStep
	public TabItemAccess getSelectedTabItem() {
		return getTabFolder().getSelection();
	}
	
	@UIStep
	public CompositeAccess getSelectedTabControl() {
		return (CompositeAccess) getSelectedTabItem().getClientControl();
	}

	@UIStep
	public TableAccess getTable(String label) {
		TabItem tabitem = getTabItem(label).getWidget();
		return findDataTable((Composite)tabitem.getControl());
	}
	
	@UIStep
	public TableAccess getSelectedTable() {
		return findDataTable(getSelectedTabControl().getControl());
	}

	public TableAccess getScalarsTable() {
		return getTable(SCALARS);
	}
	
	public TableAccess getVectorsTable() {
		return getTable(VECTORS);
	}
	
	public TableAccess getHistogramsTable() {
		return getTable(HISTOGRAMS);
	}
	
	@InBackgroundThread
	public void showAllTableColumns() {
		TableAccess table = getSelectedTable();
		MenuAccess menu = table.activateContextMenuWithMouseClick();
		menu.activateMenuItemWithMouse("Choose table columns.*");
		
		ShellAccess shell = Access.findShellWithTitle("Select Columns.*");
		shell.findButtonWithLabel("Select All").selectWithMouseClick();
		shell.findButtonWithLabel("OK").selectWithMouseClick();
	}
	
	@UIStep
	public void selectTab(String label) {
		getTabFolder().selectItem(label);
	}
	
	@UIStep
	public void ensureTabSelected(String label) {
		if (!getTabFolder().isSelected(label))
			selectTab(label);
	}
	
	public void ensureVectorsSelected() {
		ensureTabSelected(VECTORS);
	}
	
	public void ensureScalarsSelected() {
		ensureTabSelected(SCALARS);
	}
	
	public void ensureHistogramsSelected() {
		ensureTabSelected(HISTOGRAMS);
	}
	
	@UIStep
	public void ensureBasicFilterSelected() {
		ButtonAccess button = getSelectedTabControl().tryToFindButtonWithLabel("Basic");
		if (button != null)
			button.selectWithMouseClick();
	}
	
	@InBackgroundThread
	public TextAccess ensureAdvancedFilterSelected() {
		ButtonAccess button = getSelectedTabControl().tryToFindButtonWithLabel("Advanced");
		if (button != null)
			button.selectWithMouseClick();
		return getAdvancedFilterText();
	}
	
	@UIStep
	public ComboAccess getRunNameFilter() {
		return getSelectedTabControl().findComboAfterLabel("Run.*");
	}
	
	@UIStep
	public ComboAccess getModuleNameFilter() {
		return getSelectedTabControl().findComboAfterLabel("Module.*");
	}
	
	@UIStep
	public ComboAccess getDataNameFilter() {
		return getSelectedTabControl().findComboAfterLabel("Name.*");
	}
	
	@UIStep
	public TextAccess getAdvancedFilterText() {
		return getSelectedTabControl().findTextAfterLabel("Filter.*");
	}
	
	@UIStep
	public TableColumnAccess getColumn(String columnName) {
		return getSelectedTable().getTableColumn(columnName);
	}
	
	protected TableAccess findDataTable(Composite composite) {
		return (TableAccess)createAccess(Access.findDescendantControl(composite, Table.class));
	}
}
