package org.omnetpp.test.gui.scave;

import static org.omnetpp.test.gui.access.BrowseDataPageAccess.HISTOGRAMS;
import static org.omnetpp.test.gui.access.BrowseDataPageAccess.NAME;
import static org.omnetpp.test.gui.access.BrowseDataPageAccess.SCALARS;
import static org.omnetpp.test.gui.access.BrowseDataPageAccess.VECTORS;
import junit.framework.Assert;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.access.BrowseDataPageAccess;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.ComboAccess;
import com.simulcraft.test.gui.access.MenuAccess;
import com.simulcraft.test.gui.access.ShellAccess;
import com.simulcraft.test.gui.access.TableAccess;
import com.simulcraft.test.gui.access.TableItemAccess;
import com.simulcraft.test.gui.access.TextAccess;
import com.simulcraft.test.gui.access.TreeAccess;

public class BrowseDataPageTest extends ScaveFileTestCase {

	protected BrowseDataPageAccess browseDataPage;

	@Override
	protected void setUpInternal() throws Exception {
		super.setUpInternal();
		createFiles(2);
		editor = ScaveEditorUtils.openAnalysisFile(projectName, fileName);
		browseDataPage = editor.ensureBrowseDataPageActive();
	}
	
	@Override
	protected void tearDownInternal() throws Exception {
		super.tearDownInternal();
	}
	
	public void testTableContent() throws Exception {
		Assert.assertNotNull(browseDataPage);
		testTableContent(VECTORS);
		testTableContent(SCALARS);
		testTableContent(HISTOGRAMS);
	}
	
	private void testTableContent(String tab) {
		browseDataPage.ensureTabSelected(tab);
		browseDataPage.showAllTableColumns();
		browseDataPage.sortByTableColumn(NAME, SWT.UP);
		browseDataPage.getTable(tab).assertContent(buildTableContent(tab));
	}
	
	public void testBasicFilter() throws Exception {
		testFilterCombos(VECTORS, "vector-1", "vector-1");
		testFilterCombos(SCALARS, "scalar-1", "scalar-1");
		testFilterCombos(HISTOGRAMS, "histogram-1", "histogram-1");
	}
	
	private void testFilterCombos(String tab, String nameFilter, String nameValue) {
		browseDataPage.ensureTabSelected(tab);
		browseDataPage.showAllTableColumns();
		browseDataPage.ensureBasicFilterSelected();
		testFilterCombo(browseDataPage.getRunNameFilter(), "run-1", "Run id", "run-1");
		testFilterCombo(browseDataPage.getModuleNameFilter(), "module-1", "Module", "module-1");
		testFilterCombo(browseDataPage.getDataNameFilter(), nameFilter, "Name", nameValue);
	}
	
	private void testFilterCombo(ComboAccess filterCombo, String filter, String columnName, String columnValue) {
		filterCombo.selectItem(filter);
		browseDataPage.getSelectedTable().assertColumnContentMatches(columnName, columnValue);
		filterCombo.selectItem("\\*");
	}
	
	public void testAdvancedFilter() throws Exception {
		browseDataPage.ensureScalarsSelected();
		browseDataPage.showAllTableColumns();
		browseDataPage.ensureAdvancedFilterSelected();
		testAdvancedFilter("run", "run-1", 1, "Run id", "run-1");
		testAdvancedFilter("module", "module-1", 1, "Module", "module-1");
		testAdvancedFilter("name", "scalar-1", 1, "Name", "scalar-1");
	}
	
	private void testAdvancedFilter(String fieldName, String fieldPattern, int rowCount, String columnName, String columnPattern) throws Exception {
		TextAccess filterText = browseDataPage.getAdvancedFilterText();
		filterText.click();
		filterText.typeOver(String.format("%s(%s)", fieldName, fieldPattern));
		filterText.pressEnter();
		
		TableAccess table = browseDataPage.getSelectedTable();
		table.assertColumnContentMatches(columnName, columnPattern);
	}
	
	public void testSelectAll() {
		String[] tabs = new String[] { SCALARS, VECTORS};
		for (String tab : tabs) {
			browseDataPage.ensureTabSelected(tab);
			TableAccess table = browseDataPage.getSelectedTable();
			table.click();
			table.pressKey('a', SWT.CTRL);
			table.assertAllSelected();
		}
	}
	
	public void testOpenTempChart() {
		String[] tabs = new String[] { VECTORS, SCALARS, HISTOGRAMS };
		for (String tab : tabs) {
			browseDataPage.ensureTabSelected(tab);
			TableAccess table = browseDataPage.getSelectedTable();
			table.click();
			table.pressKey('a', SWT.CTRL);
			MenuAccess menu = table.activateContextMenuWithMouseClick();
			menu.activateMenuItemWithMouse("Plot.*");
			editor.assertActivePage("Chart.*");
			editor.closePage("Chart.*");
		}
	}
	
	public void testAddVectorsToDatasetByFilter() {
		testAddToDatasetByFilter(VECTORS, "vector");
	}
	
	public void testAddScalarsToDatasetByFilter() {
		testAddToDatasetByFilter(SCALARS, "scalar");
	}
	
	public void testAddHistogramsToDatasetByFilter() {
		testAddToDatasetByFilter(HISTOGRAMS, "histogram");
	}
	
	private void testAddToDatasetByFilter(String tab, String resultItemType) {
		browseDataPage.ensureTabSelected(tab);
		browseDataPage.getRunNameFilter().selectItem("run-1");
		TableAccess table = browseDataPage.getSelectedTable();
		MenuAccess menu = table.activateContextMenuWithMouseClick();
		menu.activateMenuItemWithMouse("Add [fF]ilter.*");
		fillSelectDatasetDialog();
		
		editor.assertActivePage("Datasets");
		TreeAccess datasetsTree = editor.ensureDatasetsPageActive().getDatasetsTree();
		datasetsTree.assertContent(
				n("dataset test-dataset",
					n("add " + resultItemType + "s: run(run-1)")));
	}
	
	public void testAddVectorsToDatasetBySelection() {
		testAddToDatasetBySelection(VECTORS, "vector");
	}
	
	public void testAddScalarsToDatasetBySelection() {
		testAddToDatasetBySelection(SCALARS, "scalar");
	}
	
	public void testAddHistogramsToDatasetBySelection() {
		testAddToDatasetBySelection(HISTOGRAMS, "histogram");
	}
	
	private void testAddToDatasetBySelection(String tab, String resultItemType) {
		browseDataPage.ensureTabSelected(tab);
		TableAccess table = browseDataPage.getSelectedTable();
		TableItemAccess item = table.findTableItemByContent(NAME, resultItemType + "-1");
		MenuAccess menu = item.activateContextMenuWithMouseClick();
		menu.activateMenuItemWithMouse("Add [sS]elected.*");
		fillSelectDatasetDialog();
		
		editor.assertActivePage("Datasets");
		TreeAccess datasetsTree = editor.ensureDatasetsPageActive().getDatasetsTree();
		datasetsTree.assertContent(
				n("dataset test-dataset",
					n("add " + resultItemType + "s: run(run-1) AND module(module-1) AND name(" + resultItemType + "-1)")));
	}
	
	private void fillSelectDatasetDialog() {
		ShellAccess dialogShell = Access.findShellWithTitle("Select.*[dD]ataset.*"); 
		dialogShell.findButtonWithLabel("New.*").selectWithMouseClick();
		ShellAccess dialogShell2 = Access.findShellWithTitle("New [dD]ataset.*");
		TextAccess text = dialogShell2.findTextAfterLabel("Enter.*name.*");
		text.typeIn("test-dataset");
		dialogShell2.findButtonWithLabel("OK").selectWithMouseClick();
		dialogShell.findButtonWithLabel("OK").selectWithMouseClick();
	}
	
	public void testShowVectorDataView() {
		browseDataPage.ensureTabSelected(VECTORS);
		TableAccess table = browseDataPage.getSelectedTable();
		MenuAccess menu = table.activateContextMenuWithMouseClick();
		menu.activateMenuItemWithMouse("Show.*[vV]ector.*[vV]iew.*");
		
		Access.getWorkbenchWindow().findViewPartByTitle(".*[vV]ector.*").assertActivated();
	}
	
	public void testContentAssist() {
		browseDataPage.ensureVectorsSelected();
		TextAccess filterText = browseDataPage.ensureAdvancedFilterSelected();
		filterText.click();
		filterText.typeOver("fi");
		filterText.pressKey(' ', SWT.CTRL);
		Access.findContentAssistPopup().chooseWithKeyboard("file\\(\\)");
		filterText.pressKey(' ', SWT.CTRL);
		Access.findContentAssistPopup().chooseWithKeyboard(".*test-1.*");
		filterText.pressKey(SWT.END);
		filterText.pressKey(' ', SWT.CTRL);
		Access.findContentAssistPopup().chooseWithKeyboard("AND");
		filterText.pressKey(' '); // FIXME ' ' should be added after 'AND'
		Access.findContentAssistPopup().chooseWithKeyboard("NOT");
		filterText.pressKey(' '); // FIXME ' ' should be added after 'NOT'
		Access.findContentAssistPopup().chooseWithKeyboard("module\\(.*");
		filterText.pressKey(' ', SWT.CTRL);
		Access.findContentAssistPopup().chooseWithKeyboard("module-2");
		
		filterText.assertTextContent("file\\(/project/test-1\\.vec\\) AND NOT module\\(module-2\\) ");
	}
	
	protected String[][] buildTableContent(String tab) {
		if (tab.equals(VECTORS))
			return buildVectorsTableContent();
		else if (tab.equals(SCALARS))
			return buildScalarsTableContent();
		else if (tab.equals(HISTOGRAMS))
			return buildHistogramsTableContent();
		else
			throw new IllegalArgumentException("Unknown BrowseDataPage tab: " + tab);
	}
	
	protected String[][] buildVectorsTableContent() {
		String[][] content = new String[2][];
		for (int i = 0; i < content.length; ++i)
			content[i] = buildVectorsTableRow(i+1);
		return content;
	}
	
	protected String[][] buildScalarsTableContent() {
		String[][] content = new String[2*7+2][];
		for (int i = 0; i < 2; ++i)
			System.arraycopy(buildStatisticScalarRows(i+1), 0, content, i*7, 7);
		for (int i = 0; i < 2; ++i)
			content[2*7+i] = buildScalarsTableRow(i+1);
		return content;
	}
	
	protected String[][] buildHistogramsTableContent() {
		String[][] content = new String[2][];
		for (int i = 0; i < content.length; ++i)
			content[i] = buildHistogramsTableRow(i+1);
		return content;
	}
}
