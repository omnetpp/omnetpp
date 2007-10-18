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
import com.simulcraft.test.gui.core.InUIThread;
import com.simulcraft.test.gui.core.NotInUIThread;

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
	
	@InUIThread
	public TabFolderAccess getTabFolder() {
		return (TabFolderAccess)createAccess(
					findDescendantControl(TabFolder.class));
	}
	
	@InUIThread
	public TabItemAccess getTabItem(String label) {
		return getTabFolder().getItem(label);
	}
	
	@InUIThread
	public TabItemAccess getSelectedTabItem() {
		return getTabFolder().getSelection();
	}
	
	@InUIThread
	public CompositeAccess getSelectedTabControl() {
		return (CompositeAccess)getSelectedTabItem().getControl();
	}

	@InUIThread
	public TableAccess getTable(String label) {
		TabItem tabitem = getTabItem(label).getWidget();
		return findDataTable((Composite)tabitem.getControl());
	}
	
	@InUIThread
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
	
	@NotInUIThread
	public void showAllTableColumns() {
		TableAccess table = getSelectedTable();
		MenuAccess menu = table.activateContextMenuWithMouseClick();
		menu.activateMenuItemWithMouse("Choose table columns.*");
		
		ShellAccess shell = Access.findShellByTitle("Select Columns.*");
		shell.findButtonWithLabel("Select All").activateWithMouseClick();
		shell.findButtonWithLabel("OK").activateWithMouseClick();
	}
	
	@InUIThread
	public void selectTab(String label) {
		getTabFolder().setSelection(label);
	}
	
	@InUIThread
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
	
	@InUIThread
	public void ensureBasicFilterSelected() {
		ButtonAccess button = getSelectedTabControl().tryToFindButtonWithLabel("Basic");
		if (button != null)
			button.activateWithMouseClick();
	}
	
	@InUIThread
	public void ensureAdvancedFilterSelected() {
		ButtonAccess button = getSelectedTabControl().tryToFindButtonWithLabel("Advanced");
		if (button != null)
			button.activateWithMouseClick();
	}
	
	@InUIThread
	public ComboAccess getRunNameFilter() {
		return getSelectedTabControl().findComboAfterLabel("Run.*");
	}
	
	@InUIThread
	public ComboAccess getModuleNameFilter() {
		return getSelectedTabControl().findComboAfterLabel("Module.*");
	}
	
	@InUIThread
	public ComboAccess getDataNameFilter() {
		return getSelectedTabControl().findComboAfterLabel("Name.*");
	}
	
	public TextAccess getAdvancedFilterText() {
		return getSelectedTabControl().findTextAfterLabel("Filter.*");
	}
	
	@InUIThread
	public TableColumnAccess getColumn(String columnName) {
		return getSelectedTable().getTableColumn(columnName);
	}
	
	protected TableAccess findDataTable(Composite composite) {
		return (TableAccess)createAccess(Access.findDescendantControl(composite, Table.class));
	}
}
