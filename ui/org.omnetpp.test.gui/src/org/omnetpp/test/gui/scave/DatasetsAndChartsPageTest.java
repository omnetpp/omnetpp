package org.omnetpp.test.gui.scave;

import org.omnetpp.test.gui.access.DatasetsAndChartsPageAccess;

public class DatasetsAndChartsPageTest extends ScaveFileTestCase {
	
	
	DatasetsAndChartsPageAccess datasetsPage;
	
	@Override
	protected void setUpInternal() throws Exception {
		super.setUpInternal();
		createFiles();
		editor = ScaveEditorUtils.openAnalysisFile(projectName, fileName);
		datasetsPage = editor.ensureDatasetsPageActive();
	}
	
	@Override
	protected void tearDownInternal() throws Exception {
		super.tearDownInternal();
	}

	public void testCreateEmptyDataset() {
		datasetsPage.createDataset("test-dataset");
		
		datasetsPage.getDatasetsTree().assertContent(
				n("dataset test-dataset"));
	}
	
	public void testCreateEmptyChartsheet() {
		datasetsPage.createChartsheet("test-chartsheet");
		
		datasetsPage.getChartsheetsTree().assertContent(
				n("chart sheet test-chartsheet (0 charts)"));
	}
	
	public void testCreateDatasetWithContent() {
		String dataset = "test-dataset";
		datasetsPage.createDataset(dataset)
					.createAdd(dataset, "vector")
					.createDiscard(dataset, "scalar")
					.createApply(dataset, "mean")
					.createCompute(dataset, null)
					.createBarChart(dataset, "test-barchart")
					.createLineChart(dataset, "test-linechart")
					.createHistogramChart(dataset, "test-histogramchart")
					.createScatterChart(dataset, "test-scatterchart")
					.createChartsheet("test-chartsheet");

		datasetsPage.getDatasetsTree().assertContent(
				n("dataset test-dataset",
					n("add vectors: all"),
					n("discard scalars: all"),
					n("apply mean"),
					n("compute <undefined>"),
					n("bar chart test-barchart"),
					n("line chart test-linechart"),
					n("histogram chart test-histogramchart"),
					n("scatter chart test-scatterchart")));
		datasetsPage.getChartsheetsTree().assertContent(
				forest(
					n("chart sheet default (4 charts)",
						n("bar chart test-barchart"),
						n("line chart test-linechart"),
						n("histogram chart test-histogramchart"),
						n("scatter chart test-scatterchart")),
					n("chart sheet test-chartsheet (0 charts)")));
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
				createScalarFileContent(runNumber));
	}
	
	protected void createVectorFile(int runNumber) throws Exception {
		createFile(
				String.format("test-%d.vec", runNumber),
				createVectorFileContent(runNumber));
	}
}
