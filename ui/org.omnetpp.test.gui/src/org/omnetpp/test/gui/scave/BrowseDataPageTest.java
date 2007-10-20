package org.omnetpp.test.gui.scave;

import static org.omnetpp.test.gui.access.BrowseDataPageAccess.RUN_ID;
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
import com.simulcraft.test.gui.access.TextAccess;
import com.simulcraft.test.gui.access.TreeAccess;

public class BrowseDataPageTest extends ScaveFileTestCase {

	protected BrowseDataPageAccess browseDataPage;

	@Override
	protected void setUpInternal() throws Exception {
		super.setUpInternal();
		createFiles();
		editor = ScaveEditorUtils.openAnalysisFile(projectName, fileName);
		browseDataPage = editor.ensureBrowseDataPageActive();
	}
	
	@Override
	protected void tearDownInternal() throws Exception {
		super.tearDownInternal();
	}
	
	public void testTableContent() throws Exception {
		Assert.assertNotNull(browseDataPage);
		browseDataPage.ensureScalarsSelected();
		browseDataPage.showAllTableColumns();
		browseDataPage.getScalarsTable().assertContent(buildScalarsTableContent());
		
		browseDataPage.ensureVectorsSelected();
		browseDataPage.showAllTableColumns();
		browseDataPage.getVectorsTable().assertContent(buildVectorsTableContent());

		browseDataPage.ensureHistogramsSelected();
		browseDataPage.showAllTableColumns();
		browseDataPage.getHistogramsTable().assertEmpty();
	}
	
	public void testBasicFilter() throws Exception {
		browseDataPage.ensureScalarsSelected();
		browseDataPage.showAllTableColumns();
		browseDataPage.ensureBasicFilterSelected();
		testFilterCombo(browseDataPage.getRunNameFilter(), "run-1", "Run id", "run-1");
		testFilterCombo(browseDataPage.getModuleNameFilter(), "module-1", "Module", "module-1");
		testFilterCombo(browseDataPage.getDataNameFilter(), "scalar-1", "Name", "scalar-1");

		browseDataPage.ensureVectorsSelected();
		browseDataPage.showAllTableColumns();
		browseDataPage.ensureBasicFilterSelected();
		testFilterCombo(browseDataPage.getRunNameFilter(), "run-1", "Run id", "run-1");
		testFilterCombo(browseDataPage.getModuleNameFilter(), "module-1", "Module", "module-1");
		testFilterCombo(browseDataPage.getDataNameFilter(), "vector-1", "Name", "vector-1");
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
		table.assertItemCount(rowCount);
		table.assertColumnContentMatches(columnName, columnPattern);
	}
	
	public void testSelectAll() {
		String[] tabs = new String[] { SCALARS, VECTORS};
		for (String tab : tabs) {
			browseDataPage.ensureTabSelected(tab);
			TableAccess table = browseDataPage.getSelectedTable();
			table.click();
			table.pressKey('a', SWT.CTRL);
			table.assertSelectionCount(2);
		}
	}
	
	public void testOpenTempChart() {
		String[] tabs = new String[] { SCALARS, VECTORS };
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
	
	public void testAddScalarsToDatasetByFilter() {
		testAddToDatasetByFilter(SCALARS, "scalar");
	}
	
	public void testAddVectorsToDatasetByFilter() {
		testAddToDatasetByFilter(VECTORS, "vector");
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
	
	public void testAddScalarsToDatasetBySelection() {
		testAddToDatasetBySelection(SCALARS, "scalar");
	}
	
	public void testAddVectorsToDatasetBySelection() {
		testAddToDatasetBySelection(VECTORS, "vector");
	}

	private void testAddToDatasetBySelection(String tab, String resultItemType) {
		browseDataPage.ensureTabSelected(tab);
		TableAccess table = browseDataPage.getSelectedTable();
		table.findTableItemByContent(RUN_ID, "run-1").click();
		MenuAccess menu = table.activateContextMenuWithMouseClick();
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
	
	protected String[][] buildVectorsTableContent() {
		String[][] content = new String[2][];
		for (int i = 0; i < content.length; ++i)
			content[i] = buildVectorsTableRow(2-i); // TODO sorting
		return content;
	}
	
	protected String[][] buildFilteredVectorsTableContent() {
		String[][] content = new String[1][];
		content[0] = buildScalarsTableRow(1);
		return content;
	}
	
	protected String[] buildVectorsTableRow(int rowIndex) {
		String[] row = new String[15];
		int i = 0;
		row[i++] = "/project/";
		row[i++] = String.format("test-%d.vec", rowIndex);
		row[i++] = String.format("config-%d", rowIndex); // config name
		row[i++] = String.format("%d", rowIndex); // run number
		row[i++] = String.format("run-%d", rowIndex);
		row[i++] = String.format("module-%s", rowIndex);
		row[i++] = String.format("vector-%d", rowIndex);
		row[i++] = String.format("%d", rowIndex); // experiment
		row[i++] = String.format("%d", rowIndex); // measurement
		row[i++] = String.format("%d", rowIndex); // replication
		row[i++] = "1"; // count
		row[i++] = String.format("%s", (double)rowIndex); // mean
		row[i++] = "n.a."; // stddev
		row[i++] = String.format("%s", (double)rowIndex); // min
		row[i++] = String.format("%s", (double)rowIndex); // max
		return row;
	}
	
	protected String[][] buildScalarsTableContent() {
		String[][] content = new String[2][];
		for (int i = 0; i < content.length; ++i)
			content[i] = buildScalarsTableRow(2-i);
		return content;
	}
	
	protected String[][] buildFilteredScalarsTableContent() {
		String[][] content = new String[1][];
		content[0] = buildScalarsTableRow(1);
		return content;
	}
	
	protected String[] buildScalarsTableRow(int rowIndex) {
		String[] row = new String[11];
		int i = 0;
		row[i++] = "/project/";
		row[i++] = String.format("test-%d.sca", rowIndex);
		row[i++] = String.format("config-%d", rowIndex); // config name
		row[i++] = String.format("%d", rowIndex); // run number
		row[i++] = String.format("run-%d", rowIndex);
		row[i++] = String.format("module-%s", rowIndex);
		row[i++] = String.format("scalar-%d", rowIndex);
		row[i++] = String.format("%d", rowIndex); // experiment
		row[i++] = String.format("%d", rowIndex); // measurement
		row[i++] = String.format("%d", rowIndex); // replication
		row[i++] = String.format("%s", (double)rowIndex); // value
		return row;
	}
	
	protected void createFiles() throws Exception {
		createFile(
				fileName,

				"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" +
				"<scave:Analysis xmi:version=\"2.0\" xmlns:xmi=\"http://www.omg.org/XMI\" xmlns:scave=\"http://www.omnetpp.org/omnetpp/scave\">" +
				"<inputs>" +
				"<inputs name=\"test-*.vec\"/>" +
				"<inputs name=\"test-*.sca\"/>" +
				"</inputs>" +
				"<datasets/>" +
				"<chartSheets/>" +
				"</scave:Analysis>");
		
		for (int runNumber = 1; runNumber <= 2; ++runNumber) {
			createScalarFile(runNumber);
			createVectorFile(runNumber);
		}
	}
	
	protected void createScalarFile(int runNumber) throws Exception {
		createFile(
				String.format("test-%d.sca", runNumber),
				
				String.format(
					"run run-%1$d\n" +
					"attr run-number %1$d\n" +
					"attr config config-%1$d\n" +
					"attr experiment %1$d\n" +
					"attr measurement %1$d\n" +
					"attr replication %1$d\n" +
					"scalar module-%1$d scalar-%1$d %1$d\n",
					runNumber));
	}
	
	protected void createVectorFile(int runNumber) throws Exception {
		createFile(
				String.format("test-%d.vec", runNumber),
				
				String.format(
					"run run-%1$d\n" +
					"attr run-number %1$d\n" +
					"attr config config-%1$d\n" +
					"attr experiment %1$d\n" +
					"attr measurement %1$d\n" +
					"attr replication %1$d\n" +
					"vector 1 module-%1$d vector-%1$d TV\n" +
					"1	0.0	%2$f\n",
					runNumber, (double)runNumber));
	}
}
