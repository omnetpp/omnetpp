package org.omnetpp.test.gui.scave;

import static org.omnetpp.test.gui.access.DatasetViewAccess.FILE_NAME;

import org.eclipse.swt.SWT;
import org.omnetpp.test.gui.access.DatasetViewAccess;
import org.omnetpp.test.gui.access.DatasetsAndChartsPageAccess;

import com.simulcraft.test.gui.access.MenuAccess;

public class DatasetViewTest extends ScaveFileTestCase {

	DatasetsAndChartsPageAccess datasetsPage;
	DatasetViewAccess datasetView;
	
	@Override
	protected void setUpInternal() throws Exception {
		super.setUpInternal();
		createFiles(2);
		editor = ScaveEditorUtils.openAnalysisFile(projectName, fileName);
		datasetsPage = editor.ensureDatasetsPageActive();
		datasetView = ScaveEditorUtils.ensureDatasetView();
		//datasetView.ensureAllColumnsVisible();
	}
	
	public void testOpenDatasetView() {
		MenuAccess menu = datasetsPage.getDatasetsTree().activateContextMenuWithMouseClick();
		menu.activateMenuItemWithMouse(".*[dD]ataset.*[vV]iew.*");
		datasetView.assertActivated();
	}
	
	public void testDatasetViewContent() {
		editor.ensureDatasetsPageActive();
		MenuAccess menu = datasetsPage.getDatasetsTree().activateContextMenuWithMouseClick();
		menu.activateMenuItemWithMouse(".*[dD]ataset.*[vV]iew.*");
		editor.ensureActivated();

		datasetsPage.getDatasetsTree().findTreeItemByContent("dataset.*scalar-dataset.*").click();
		datasetView.assertScalarsPanelActivated();
		datasetView.getScalarsTable().assertContent(buildScalarsTableContent());
		datasetView.ensureVectorsPanelActivated();
		datasetView.sortByTableColumn(FILE_NAME, SWT.UP);
		datasetView.getVectorsTable().assertEmpty();
		
		datasetsPage.getDatasetsTree().findTreeItemByContent("dataset.*vector-dataset.*").click();
		datasetView.assertVectorsPanelActivated();
		datasetView.getVectorsTable().assertContent(buildVectorsTableContent());
		datasetView.ensureScalarsPanelActivated();
		datasetView.sortByTableColumn(FILE_NAME, SWT.UP);
		datasetView.getScalarsTable().assertEmpty();
	}
	
	protected String[][] buildScalarsTableContent() {
		String[][] content = new String[2][];
		for (int i = 0; i < content.length; ++i)
			content[i] = buildScalarsTableRow(i+1);
		return content;
	}
	
	protected String[][] buildVectorsTableContent() {
		String[][] content = new String[2][];
		for (int i = 0; i < content.length; ++i)
			content[i] = buildVectorsTableRow(i+1);
		return content;
	}
	
	@Override
	protected String createAnalysisFileContent() {
		return
			"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" +
			"<scave:Analysis xmi:version=\"2.0\" xmlns:xmi=\"http://www.omg.org/XMI\" xmlns:scave=\"http://www.omnetpp.org/omnetpp/scave\">" +
			"<inputs>" +
			"<inputs name=\"test-*.vec\"/>" +
			"<inputs name=\"test-*.sca\"/>" +
			"</inputs>" +
			"<datasets>" +
		    "  <datasets name=\"scalar-dataset\">" +
		    "    <items xsi:type=\"scave:Add\" filterPattern=\"\"/>" +
		    "  </datasets>" +
		    "  <datasets name=\"vector-dataset\">" +
		    "    <items xsi:type=\"scave:Add\" filterPattern=\"\" type=\"VECTOR\"/>" +
		    "  </datasets>" +
		    "</datasets>" +
			"<chartSheets/>" +
			"</scave:Analysis>";
	}
}
