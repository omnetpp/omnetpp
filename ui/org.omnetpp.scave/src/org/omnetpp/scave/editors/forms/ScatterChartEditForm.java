package org.omnetpp.scave.editors.forms;

import java.util.Arrays;
import java.util.Map;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.TabItem;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItemFields;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.ScatterChart;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.DatasetManager;
import org.omnetpp.scave.model2.ScaveModelUtil;

public class ScatterChartEditForm extends BaseLineChartEditForm {

	
	private static final EStructuralFeature[] scatterChartFeatures = new EStructuralFeature[] {
		pkg.getChart_Name(),
		pkg.getScatterChart_XDataModule(),
		pkg.getScatterChart_XDataName(),
		pkg.getScatterChart_AverageReplications(),
		pkg.getChart_Properties(),
	};
	
	private Combo moduleNameCombo;
	private Combo dataNameCombo;
	private Button avgReplicationsCheckbox;
	
	private String[] moduleNames = ArrayUtils.EMPTY_STRING_ARRAY;
	private String[] dataNames = ArrayUtils.EMPTY_STRING_ARRAY;
	
	public ScatterChartEditForm(ScatterChart chart, EObject parent, Map<String,Object> formParameters, ResultFileManager manager) {
		super(chart, parent, formParameters, manager);
		updateLineNames(null);
		Dataset dataset = ScaveModelUtil.findEnclosingOrSelf(parent, Dataset.class);
		if (dataset != null) {
			IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, chart, ResultType.SCALAR_LITERAL);
			idlist.merge(DatasetManager.getIDListFromDataset(manager, dataset, chart, ResultType.VECTOR_LITERAL));
			moduleNames = ScaveModelUtil.getFieldValues(idlist, ResultItemFields.MODULE, manager);
			dataNames = ScaveModelUtil.getFieldValues(idlist, ResultItemFields.NAME, manager);
		}
	}
	
	protected void updateLineNames(String formatString) {
		lineNames = ArrayUtils.EMPTY_STRING_ARRAY;
		IXYDataset xydataset;
		
		try {
			Dataset dataset = ScaveModelUtil.findEnclosingOrSelf(parent, Dataset.class);
			xydataset = DatasetManager.createScatterPlotDataset((ScatterChart)chart, dataset, manager, null);
		}
		catch (Exception e) {
			xydataset = null;
		}

		if (xydataset != null) {
			lineNames = new String[xydataset.getSeriesCount()];
			for (int i = 0; i < xydataset.getSeriesCount(); ++i)
				lineNames[i] = xydataset.getSeriesKey(i);
		}

		if (linesTableViewer != null)
			linesTableViewer.setInput(lineNames);
	}
	
	
	/**
	 * Returns the features edited on this form.
	 */
	public EStructuralFeature[] getFeatures() {
		return scatterChartFeatures;
	}

	@Override
	protected void populateTabItem(TabItem item) {
		super.populateTabItem(item);
		String name = item.getText();
		Composite panel = (Composite)item.getControl();
		if ("Main".equals(name)) {
			Group group = createGroup("Choose variable for X axis", panel);
			group.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
			moduleNameCombo = createComboField("Module name", group, moduleNames);
			if (moduleNames.length > 0)
				moduleNameCombo.setText(moduleNames[0]);
			
			dataNameCombo = createComboField("Data name", group, dataNames);
			if (dataNames.length > 0)
				dataNameCombo.setText(dataNames[0]);
			
			avgReplicationsCheckbox = createCheckboxField("average replications", group);
			avgReplicationsCheckbox.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
			avgReplicationsCheckbox.setSelection(true);
		}
	}

	@Override
	public Object getValue(EStructuralFeature feature) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.SCATTER_CHART__XDATA_MODULE:
			return moduleNameCombo != null ? moduleNameCombo.getText() : null;
		case ScaveModelPackage.SCATTER_CHART__XDATA_NAME:
			return dataNameCombo != null ? dataNameCombo.getText() : null;
		case ScaveModelPackage.SCATTER_CHART__AVERAGE_REPLICATIONS:
			return avgReplicationsCheckbox.getSelection();
		}
		return super.getValue(feature);
	}

	@Override
	public void setValue(EStructuralFeature feature, Object value) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.SCATTER_CHART__XDATA_MODULE:
			if (moduleNameCombo != null) {
				String moduleName = Arrays.asList(moduleNames).contains(value) ? (String)value : StringUtils.EMPTY;
				moduleNameCombo.setText(moduleName);
			}
			break;
		case ScaveModelPackage.SCATTER_CHART__XDATA_NAME:
			if (dataNameCombo != null) {
				String dataName = Arrays.asList(dataNames).contains(value) ? (String)value : StringUtils.EMPTY;
				dataNameCombo.setText(dataName);
			}
			break;
		case ScaveModelPackage.SCATTER_CHART__AVERAGE_REPLICATIONS:
			if (avgReplicationsCheckbox != null) {
				avgReplicationsCheckbox.setSelection(value != null ? ((Boolean)value) : true);
			}
		default:
			super.setValue(feature, value);
			break;
		}
	}
}
