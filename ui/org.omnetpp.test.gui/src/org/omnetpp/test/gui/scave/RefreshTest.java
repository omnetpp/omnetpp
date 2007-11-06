package org.omnetpp.test.gui.scave;

import org.apache.commons.lang.ArrayUtils;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.test.gui.access.BrowseDataPageAccess;
import org.omnetpp.test.gui.access.DatasetViewAccess;
import org.omnetpp.test.gui.access.InputsPageAccess;

import com.simulcraft.test.gui.util.WorkbenchUtils;

public class RefreshTest extends ScaveFileTestCase {
	
	DatasetViewAccess datasetView;
	@Override
	protected void setUpInternal() throws Exception {
		super.setUpInternal();
		createFiles();
		editor = ScaveEditorUtils.openAnalysisFile(projectName, fileName);
		//DatasetsAndChartsPageAccess datasetsPage = editor.ensureDatasetsPageActive();
		// TODO open charts
		datasetView = ScaveEditorUtils.ensureDatasetView();
	}
	
	public void testInitialContent() {
		assertFileRunViewContent(buildFileRunViewContent(1, 2));
		assertRunFileViewContent(buildRunFileViewContent(1, 2));
		assertLogicalViewContent(buildLogicalViewContent(1, 2));
		assertBrowseDataPageScalarsTableContent(buildScalarsTableContent(1, 2));
		assertBrowseDataPageVectorsTableContent(buildVectorsTableContent(1, 2));
		assertDatasetViewScalarsTableContent(buildScalarsTableContent(1, 2));
		assertDatasetViewVectorsTableContent(buildVectorsTableContent(1, 2));
	}
	
    public void testRemoveFileFromWorkspace() throws Exception {
    	removeFile("test-2.sca");
    	removeFile("test-2.vec");
    	WorkbenchUtils.refreshProjectFromProjectExplorerView(projectName);

    	assertFileRunViewContent(buildFileRunViewContent(1));
    	assertRunFileViewContent(buildRunFileViewContent(1));
    	assertLogicalViewContent(buildLogicalViewContent(1));
		assertBrowseDataPageScalarsTableContent(buildScalarsTableContent(1));
		assertBrowseDataPageVectorsTableContent(buildVectorsTableContent(1));
		assertDatasetViewScalarsTableContent(buildScalarsTableContent(1));
		assertDatasetViewVectorsTableContent(buildVectorsTableContent(1));
    }
    
    public void testAddFileToWorkspace() throws Exception {
    	createFile("test-3.sca", createScalarFileContent(3));
    	createFile("test-3.vec", createVectorFileContent(3));
    	WorkbenchUtils.refreshProjectFromProjectExplorerView(projectName);
    	
    	assertFileRunViewContent(buildFileRunViewContent(1, 2, 3));
    	assertRunFileViewContent(buildRunFileViewContent(1, 2, 3));
    	assertLogicalViewContent(buildLogicalViewContent(1, 2, 3));
		assertBrowseDataPageScalarsTableContent(buildScalarsTableContent(1, 2, 3));
		assertBrowseDataPageVectorsTableContent(buildVectorsTableContent(1, 2, 3));
		assertDatasetViewScalarsTableContent(buildScalarsTableContent(1, 2, 3));
		assertDatasetViewVectorsTableContent(buildVectorsTableContent(1, 2, 3));
    }
    
    public void testRemoveFileFromInputs() {
    	InputsPageAccess inputsPage = editor.ensureInputsPageActive();
        inputsPage.removeInputFile("/project/test-2\\.vec");
        inputsPage.removeInputFile("/project/test-2\\.sca");
        
    	assertFileRunViewContent(buildFileRunViewContent(1));
    	assertRunFileViewContent(buildRunFileViewContent(1));
    	assertLogicalViewContent(buildLogicalViewContent(1));
		assertBrowseDataPageScalarsTableContent(buildScalarsTableContent(1));
		assertBrowseDataPageVectorsTableContent(buildVectorsTableContent(1));
		assertDatasetViewScalarsTableContent(buildScalarsTableContent(1));
		assertDatasetViewVectorsTableContent(buildVectorsTableContent(1));
    }

    public void testAddFileToInputs() {
    	InputsPageAccess inputsPage = editor.ensureInputsPageActive();
    	inputsPage.addFileWithWildcard("/project/test-4.vec");
    	inputsPage.addFileWithWildcard("/project/test-4.sca");

    	assertFileRunViewContent(buildFileRunViewContent(1, 2, 4));
    	assertRunFileViewContent(buildRunFileViewContent(1, 2, 4));
    	assertLogicalViewContent(buildLogicalViewContent(1, 2, 4));
		assertBrowseDataPageScalarsTableContent(buildScalarsTableContent(1, 2, 4));
		assertBrowseDataPageVectorsTableContent(buildVectorsTableContent(1, 2, 4));
		assertDatasetViewScalarsTableContent(buildScalarsTableContent(1, 2, 4));
		assertDatasetViewVectorsTableContent(buildVectorsTableContent(1, 2, 4));
    }
    
	protected void createFiles() throws Exception {
		createFile(
				fileName,
				
				"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" +
				"<scave:Analysis xmi:version=\"2.0\" xmlns:xmi=\"http://www.omg.org/XMI\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:scave=\"http://www.omnetpp.org/omnetpp/scave\">\n" +
				"  <inputs>\n" +
				"    <inputs name=\"/project/test-1.vec\"/>\n" +
				"    <inputs name=\"/project/test-1.sca\"/>\n" +
				"    <inputs name=\"/project/test-2.vec\"/>\n" +
				"    <inputs name=\"/project/test-2.sca\"/>\n" +
				"    <inputs name=\"/project/test-3.vec\"/>\n" +
				"    <inputs name=\"/project/test-3.sca\"/>\n" +
				"  </inputs>\n" +
				"  <datasets>\n" +
				"    <datasets name=\"test-dataset\">\n" +
				"      <items xsi:type=\"scave:Add\" filterPattern=\"\" type=\"VECTOR\"/>\n" +
				"      <items xsi:type=\"scave:Add\" filterPattern=\"\" type=\"SCALAR\"/>\n" +
				"      <items xsi:type=\"scave:LineChart\" name=\"test-linechart\" lineNameFormat=\"\"/>\n" +
				"      <items xsi:type=\"scave:BarChart\" name=\"test-barchart\"/>\n" +
				"      <items xsi:type=\"scave:HistogramChart\" name=\"test-histogramchart\"/>\n" +
				"      <items xsi:type=\"scave:ScatterChart\" name=\"test-scatterchart\" xDataPattern=\"module(module-1) AND name(&quot;mean(vector-1)&quot;)\"/>\n" +
				"    </datasets>\n" +
				"  </datasets>\n" +
				"  <chartSheets>\n" +
				"    <chartSheets name=\"default\" charts=\"//@datasets/@datasets.0/@items.3 //@datasets/@datasets.0/@items.2 //@datasets/@datasets.0/@items.4 //@datasets/@datasets.0/@items.5\"/>\n" +
				"    <chartSheets name=\"test-chartsheet\"/>\n" +
				"  </chartSheets>\n" +
				"</scave:Analysis>\n");
		
		for (int runNumber = 1; runNumber <= 4; ++runNumber) {
			if (runNumber == 3)
				continue;
			createFile(
					String.format("test-%d.sca", runNumber),
					createScalarFileContent(runNumber));
			createFile(
					String.format("test-%d.vec", runNumber),
					createVectorFileContent(runNumber));
		}
	}
	
	protected GenericTreeNode[] buildFileRunViewContent(int... runNumbers) {
		GenericTreeNode[] content = new GenericTreeNode[runNumbers.length * 2];
		for (int i = 0; i < runNumbers.length; i++) {
			content[2*i] = n(String.format("/project/test-%d.sca", runNumbers[i]),
								n(String.format("run \"run-%d\"", runNumbers[i])));
			content[2*i+1] = n(String.format("/project/test-%d.vec", runNumbers[i]),
								n(String.format("run \"run-%d\"", runNumbers[i])));
		}
		return content;
	}
	
	protected GenericTreeNode[] buildRunFileViewContent(int... runNumbers) {
		GenericTreeNode[] content = new GenericTreeNode[runNumbers.length];
		for (int i = 0; i < runNumbers.length; i++) {
			content[i] = n(String.format("run \"run-%d\"", runNumbers[i]),
								n(String.format("/project/test-%d.sca", runNumbers[i])),
								n(String.format("/project/test-%d.vec", runNumbers[i])));
		}
		return content;
	}
	
	protected GenericTreeNode[] buildLogicalViewContent(int... runNumbers) {
		GenericTreeNode[] content = new GenericTreeNode[runNumbers.length];
		for (int i = 0; i < runNumbers.length; i++) {
			content[i] = n(String.format("experiment \"%d\"", runNumbers[i]),
								n(String.format("measurement \"%d\"", runNumbers[i]),
									n(String.format("replication \"%d\"", runNumbers[i]),
										n(String.format("run \"run-%d\"", runNumbers[i]),
											n(String.format("/project/test-%d.sca", runNumbers[i])),
											n(String.format("/project/test-%d.vec", runNumbers[i]))))));
		}
		return content;
	}
	
	protected String[][] buildScalarsTableContent(int... runNumbers) {
		String[][] table = new String[runNumbers.length][];
		for (int i = 0; i < runNumbers.length; ++i) {
			table[i] = row(
							"/project/",
							String.format("test-%d.sca", runNumbers[i]),
							String.format("config-%d", runNumbers[i]),
							String.format("%d", runNumbers[i]),
							String.format("run-%d", runNumbers[i]),
							String.format("module-%d", runNumbers[i]),
							String.format("scalar-%d", runNumbers[i]),
							String.format("%d", runNumbers[i]),
							String.format("%d", runNumbers[i]),
							String.format("%d", runNumbers[i]),
							String.format("%.1f", (double)runNumbers[i]));
		}
		return table;
	}
	
	protected String[][] buildVectorsTableContent(int... runNumbers) {
		String[][] table = new String[runNumbers.length][];
		for (int i = 0; i < runNumbers.length; ++i) {
			table[i] = row(
							"/project/",
							String.format("test-%d.vec", runNumbers[i]),
							String.format("config-%d", runNumbers[i]),
							String.format("%d", runNumbers[i]),
							String.format("run-%d", runNumbers[i]),
							String.format("module-%d", runNumbers[i]),
							String.format("vector-%d", runNumbers[i]),
							String.format("%d", runNumbers[i]),
							String.format("%d", runNumbers[i]),
							String.format("%d", runNumbers[i]),
							"1",
							String.format("%.1f", (double)runNumbers[i]),
							"n.a.",
							String.format("%.1f", (double)runNumbers[i]),
							String.format("%.1f", (double)runNumbers[i]));
		}
		return table;
	}
	
	protected GenericTreeNode buildDatasetsTreeContent() {
		return	n("dataset test-dataset",
					n("add vectors: all"),
					n("add scalars: all"),
					n("line chart test-linechart"),
					n("bar chart test-barchart"),
					n("histogram chart test-histogramchart"),
					n("scatter chart test-scatterchart"));
	}
	
	protected void assertFileRunViewContent(GenericTreeNode... content) {
		InputsPageAccess inputsPage = editor.ensureInputsPageActive();
		inputsPage.ensureFileRunViewVisible();
		inputsPage.getFileRunViewTree().assertContent(content);
	}
	
	protected void assertRunFileViewContent(GenericTreeNode... content) {
		InputsPageAccess inputsPage = editor.ensureInputsPageActive();
		inputsPage.ensureRunFileViewVisible();
		inputsPage.getRunFileViewTree().assertContent(content);
	}
	
	protected void assertLogicalViewContent(GenericTreeNode... content) {
		InputsPageAccess inputsPage = editor.ensureInputsPageActive();
		inputsPage.ensureLogicalViewVisible();
		inputsPage.getLogicalViewTree().assertContent(content);
	}
	
	protected void assertBrowseDataPageScalarsTableContent(String[]... content) {
		BrowseDataPageAccess browseDataPage = editor.ensureBrowseDataPageActive();
		browseDataPage.ensureScalarsSelected();
		browseDataPage.getScalarsTable().assertContent(content);
	}
	
	protected void assertBrowseDataPageVectorsTableContent(String[]... content) {
		BrowseDataPageAccess browseDataPage = editor.ensureBrowseDataPageActive();
		browseDataPage.ensureVectorsSelected();
		browseDataPage.getVectorsTable().assertContent(content);
	}

	protected void assertDatasetViewScalarsTableContent(String[]... content) {
		editor.ensureDatasetsPageActive().getDatasetsTree().findTreeItemByContent("dataset.*test-dataset.*").click();
		datasetView.activateWithMouseClick();
		datasetView.ensureScalarsPanelActivated();
		datasetView.getScalarsTable().assertContent(content);
	}
	
	protected void assertDatasetViewVectorsTableContent(String[]... content) {
		editor.ensureDatasetsPageActive().getDatasetsTree().findTreeItemByContent("dataset.*test-dataset.*").click();
		datasetView.activateWithMouseClick();
		datasetView.ensureVectorsPanelActivated();
		datasetView.getVectorsTable().assertContent(content);
	}
	
	protected String[][] table(String[]... rows) {
		return rows;
	}
	
	protected String[] row(String... row) {
		return row;
	}
	
	protected GenericTreeNode[] remove(GenericTreeNode[] array, int index) {
		return (GenericTreeNode[])ArrayUtils.remove(array, index);
	}

	protected String[][] remove(String[][] array, int index) {
		return (String[][])ArrayUtils.remove(array, index);
	}
	
	protected GenericTreeNode[] add(GenericTreeNode[] array, GenericTreeNode... elements) {
		GenericTreeNode[] result = array;
		for (GenericTreeNode element : elements)
			result = (GenericTreeNode[])ArrayUtils.add(result, element); 
		return result;
	}
}
