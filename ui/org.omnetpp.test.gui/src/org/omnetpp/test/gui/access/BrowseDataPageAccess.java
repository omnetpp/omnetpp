package org.omnetpp.test.gui.access;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Table;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.MenuAccess;
import com.simulcraft.test.gui.access.ShellAccess;
import com.simulcraft.test.gui.access.TabFolderAccess;
import com.simulcraft.test.gui.access.TabItemAccess;
import com.simulcraft.test.gui.access.TableAccess;
import com.simulcraft.test.gui.core.InUIThread;
import com.simulcraft.test.gui.core.NotInUIThread;

public class BrowseDataPageAccess extends CompositeAccess {
	
	private static final String
		SCALARS = "Scalars.*",
		VECTORS = "Vectors.*",
		HISTOGRAMS = "Histograms.*";

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
	public TableAccess getTable(String label) {
		TabItem tabitem = getTabItem(label).getWidget();
		return findDataTable((Composite)tabitem.getControl());
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
	
	@InUIThread
	public TableAccess getSelectedTable() {
		Composite panel = (Composite)getTabFolder().getSelection().getControl().getControl();
		return findDataTable(panel);
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
	
	protected TableAccess findDataTable(Composite composite) {
		return (TableAccess)createAccess(Access.findDescendantControl(composite, Table.class));
	}
}
