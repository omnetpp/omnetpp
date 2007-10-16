package org.omnetpp.test.gui.scave;

import junit.framework.Assert;

import org.omnetpp.test.gui.access.BrowseDataPageAccess;
import org.omnetpp.test.gui.access.ScaveEditorAccess;

public class BrowseDataPageTest extends ScaveFileTestCase {

	protected ScaveEditorAccess editor;
	protected BrowseDataPageAccess browseDataPage;

	@Override
	protected void setUpInternal() throws Exception {
		super.setUpInternal();
		createFiles();
		editor = ScaveEditorUtils.openAnalysisFile(projectName, fileName);
		browseDataPage = editor.ensureBrowseDataPageActive();
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
	
	protected String[][] buildVectorsTableContent() {
		String[][] content = new String[2][];
		for (int i = 0; i < content.length; ++i)
			content[i] = buildVectorsTableRow(2-i);
		return content;
	}
	
	protected String[] buildVectorsTableRow(int rowIndex) {
		String[] row = new String[15];
		int i = 0;
		row[i++] = "/project/";
		row[i++] = String.format("test-%d.vec", rowIndex);
		row[i++] = "n.a."; // config name
		row[i++] = "n.a."; // run number
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
	
	protected String[] buildScalarsTableRow(int rowIndex) {
		String[] row = new String[11];
		int i = 0;
		row[i++] = "/project/";
		row[i++] = String.format("test-%d.sca", rowIndex);
		row[i++] = "n.a."; // config name
		row[i++] = "n.a."; // run number
		row[i++] = String.format("run-%d", rowIndex);
		row[i++] = String.format("module-%s", rowIndex);
		row[i++] = String.format("scalar-%d", rowIndex);
		row[i++] = String.format("%d", rowIndex); // experiment
		row[i++] = String.format("%d", rowIndex); // measurement
		row[i++] = String.format("%d", rowIndex); // replication
		row[i++] = String.format("%s", (double)rowIndex); // value
		return row;
	}
	
	@Override
	protected void tearDownInternal() throws Exception {
		super.tearDownInternal();
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
					"attr experiment %1$d\n" +
					"attr measurement %1$d\n" +
					"attr replication %1$d\n" +
					"vector 1 module-%1$d vector-%1$d TV\n" +
					"1	0.0	%2$f\n",
				
					runNumber, (double)runNumber));
	}
}
