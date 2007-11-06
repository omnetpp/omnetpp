package org.omnetpp.test.gui.access;

import static org.omnetpp.scave.TestSupport.CHARTSHEETS_TREE_ID;
import static org.omnetpp.scave.TestSupport.DATASETS_PAGE_PALETTE_ID;
import static org.omnetpp.scave.TestSupport.DATASETS_TREE_ID;
import static org.omnetpp.test.gui.access.ModelObjectPaletteAccess.ADD_BUTTON;
import static org.omnetpp.test.gui.access.ModelObjectPaletteAccess.APPLY_BUTTON;
import static org.omnetpp.test.gui.access.ModelObjectPaletteAccess.BAR_CHART_BUTTON;
import static org.omnetpp.test.gui.access.ModelObjectPaletteAccess.CHARTSHEET_BUTTON;
import static org.omnetpp.test.gui.access.ModelObjectPaletteAccess.COMPUTE_BUTTON;
import static org.omnetpp.test.gui.access.ModelObjectPaletteAccess.DATASET_BUTTON;
import static org.omnetpp.test.gui.access.ModelObjectPaletteAccess.DISCARD_BUTTON;
import static org.omnetpp.test.gui.access.ModelObjectPaletteAccess.HISTOGRAM_CHART_BUTTON;
import static org.omnetpp.test.gui.access.ModelObjectPaletteAccess.LINE_CHART_BUTTON;
import static org.omnetpp.test.gui.access.ModelObjectPaletteAccess.SCATTER_CHART_BUTTON;

import org.eclipse.swt.widgets.Composite;
import org.omnetpp.common.util.Predicate;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.ShellAccess;
import com.simulcraft.test.gui.access.TreeAccess;

public class DatasetsAndChartsPageAccess extends CompositeAccess {
	
	public DatasetsAndChartsPageAccess(Composite composite) {
		super(composite);
	}

	public TreeAccess getDatasetsTree() {
		return (TreeAccess)createAccess(
				findDescendantControl(
						Predicate.hasID(DATASETS_TREE_ID)));
	}
	
	public TreeAccess getChartsheetsTree() {
		return (TreeAccess)createAccess(
				findDescendantControl(
						Predicate.hasID(CHARTSHEETS_TREE_ID)));
	}
	
	public ModelObjectPaletteAccess getPalette() {
		return new ModelObjectPaletteAccess(
						(Composite)findDescendantControl(
									Predicate.hasID(DATASETS_PAGE_PALETTE_ID)));
	}
	
	public void selectDataset(String datasetName) {
		getDatasetsTree().findTreeItemByContent("dataset.*" + datasetName + ".*").reveal().click();
	}
	
	public DatasetsAndChartsPageAccess createDataset(String datasetName) {
		getPalette().clickButton(DATASET_BUTTON);
		ShellAccess dialog = Access.findShellWithTitle("New Object");
		dialog.findTextAfterLabel("Name.*").typeIn(datasetName);
		dialog.findButtonWithLabel("Finish").selectWithMouseClick();
		return this;
	}
	
	public DatasetsAndChartsPageAccess createAdd(String datasetName, String dataType) {
		return createAddDiscard(datasetName, ADD_BUTTON, dataType);
	}
	
	public DatasetsAndChartsPageAccess createDiscard(String datasetName, String dataType) {
		return createAddDiscard(datasetName, DISCARD_BUTTON, dataType);
	}
	
	public DatasetsAndChartsPageAccess createAddDiscard(String datasetName, String button, String dataType) {
		selectDataset(datasetName);
		getPalette().clickButton(button);
		ShellAccess dialog = Access.findShellWithTitle("New Object");
		dialog.findComboAfterLabel("Data type.*").selectItem(dataType);
		dialog.findButtonWithLabel("Finish").selectWithMouseClick();
		return this;
	}
	
	public DatasetsAndChartsPageAccess createApply(String datasetName, String operation) {
		return createProcessingOperation(datasetName, APPLY_BUTTON, operation);
	}
	
	public DatasetsAndChartsPageAccess createCompute(String datasetName, String operation) {
		return createProcessingOperation(datasetName, COMPUTE_BUTTON, operation);
	}

	public DatasetsAndChartsPageAccess createProcessingOperation(String datasetName, String button, String operation) {
		selectDataset(datasetName);
		getPalette().clickButton(button);
		ShellAccess dialog = Access.findShellWithTitle("New Object");
		if (operation != null)
			dialog.findCombo().selectItem(operation);
		// TODO params
		dialog.findButtonWithLabel("Finish").selectWithMouseClick();
		return this;
	}
	
	public DatasetsAndChartsPageAccess createBarChart(String datasetName, String chartName) {
		return createChart(datasetName, BAR_CHART_BUTTON, chartName);
	}
	
	public DatasetsAndChartsPageAccess createLineChart(String datasetName, String chartName) {
		return createChart(datasetName, LINE_CHART_BUTTON, chartName);
	}
	
	public DatasetsAndChartsPageAccess createHistogramChart(String datasetName, String chartName) {
		return createChart(datasetName, HISTOGRAM_CHART_BUTTON, chartName);
	}

	public DatasetsAndChartsPageAccess createScatterChart(String datasetName, String chartName) {
		return createChart(datasetName, SCATTER_CHART_BUTTON, chartName);
	}
	
	public DatasetsAndChartsPageAccess createChart(String datasetName, String chartButton, String chartName) {
		selectDataset(datasetName);
		getPalette().clickButton(chartButton);
		ShellAccess dialog = Access.findShellWithTitle("New Object");
		dialog.findTextAfterLabel("Chart name.*").typeIn(chartName);
		dialog.findButtonWithLabel("Finish").selectWithMouseClick();
		return this;
	}
	
	public DatasetsAndChartsPageAccess createChartsheet(String chartsheetName) {
		getPalette().clickButton(CHARTSHEET_BUTTON);
		ShellAccess dialog = Access.findShellWithTitle("New Object");
		dialog.findTextAfterLabel("Name.*").typeIn(chartsheetName);
		dialog.findButtonWithLabel("Finish").selectWithMouseClick();
		return this;
	}
	
}
