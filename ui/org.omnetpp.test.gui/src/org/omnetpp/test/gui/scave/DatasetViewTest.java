package org.omnetpp.test.gui.scave;

import org.omnetpp.test.gui.access.DatasetViewAccess;
import org.omnetpp.test.gui.access.DatasetsAndChartsPageAccess;

import com.simulcraft.test.gui.access.MenuAccess;

public class DatasetViewTest extends ScaveFileTestCase {

	DatasetsAndChartsPageAccess datasetsPage;
	DatasetViewAccess datasetView;
	
	@Override
	protected void setUpInternal() throws Exception {
		super.setUpInternal();
		createFiles();
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
		datasetView.getVectorsTable().assertEmpty();
		
		datasetsPage.getDatasetsTree().findTreeItemByContent("dataset.*vector-dataset.*").click();
		datasetView.assertVectorsPanelActivated();
		datasetView.getVectorsTable().assertContent(buildVectorsTableContent());
		datasetView.ensureScalarsPanelActivated();
		datasetView.getScalarsTable().assertEmpty();
	}
	
	protected String[][] buildScalarsTableContent() {
		String[][] content = new String[2][];
		for (int i = 0; i < content.length; ++i)
			content[i] = buildScalarsTableRow(2-i);
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
	
	protected String[][] buildVectorsTableContent() {
		String[][] content = new String[2][];
		for (int i = 0; i < content.length; ++i)
			content[i] = buildVectorsTableRow(2-i); // TODO sorting
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

	protected void createFiles() throws Exception {
		createFile(
				fileName,

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
				"</scave:Analysis>");
		
		for (int runNumber = 1; runNumber <= 2; ++runNumber) {
			createScalarFile(runNumber);
			createVectorFile(runNumber);
		}
	}
	
	protected void createScalarFile(int runNumber) throws Exception {
		createFile(
				String.format("test-%d.sca", runNumber),
				createScalarFileContent(runNumber));
	}
	
	protected void createVectorFile(int runNumber) throws Exception {
		createFile(
				String.format("test-%d.vec", runNumber),
				createVectorFileContent(runNumber));
	}
}
