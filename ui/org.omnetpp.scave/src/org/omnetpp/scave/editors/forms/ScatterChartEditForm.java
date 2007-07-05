package org.omnetpp.scave.editors.forms;

import java.util.Arrays;
import java.util.Map;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.TabItem;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ScalarFields;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.DatasetManager;
import org.omnetpp.scave.model2.ScaveModelUtil;

public class ScatterChartEditForm extends LineChartEditForm {

	
	private static final EStructuralFeature[] scatterChartFeatures = new EStructuralFeature[] {
		pkg.getChart_Name(),
		pkg.getScatterChart_ModuleName(),
		pkg.getScatterChart_DataName(),
		pkg.getChart_Properties(),
	};
	
	private Combo moduleNameCombo;
	private Combo dataNameCombo;
	
	private String[] moduleNames = ArrayUtils.EMPTY_STRING_ARRAY;
	private String[] dataNames = ArrayUtils.EMPTY_STRING_ARRAY;
	
	public ScatterChartEditForm(Chart chart, EObject parent, Map<String,Object> formParameters, ResultFileManager manager) {
		super(chart, parent, formParameters, manager);
		Dataset dataset = ScaveModelUtil.findEnclosingOrSelf(parent, Dataset.class);
		if (dataset != null) {
			IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, chart, ResultType.SCALAR_LITERAL); 
			moduleNames = ScaveModelUtil.getScalarFields(idlist, ScalarFields.MODULE, manager);
			dataNames = ScaveModelUtil.getScalarFields(idlist, ScalarFields.NAME, manager);
		}
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
		}
	}

	@Override
	public Object getValue(EStructuralFeature feature) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.SCATTER_CHART__MODULE_NAME:
			return moduleNameCombo != null ? moduleNameCombo.getText() : null;
		case ScaveModelPackage.SCATTER_CHART__DATA_NAME:
			return dataNameCombo != null ? dataNameCombo.getText() : null;
		}
		return super.getValue(feature);
	}

	@Override
	public void setValue(EStructuralFeature feature, Object value) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.SCATTER_CHART__MODULE_NAME:
			if (moduleNameCombo != null) {
				String moduleName = Arrays.asList(moduleNames).contains(value) ? (String)value : StringUtils.EMPTY;
				moduleNameCombo.setText(moduleName);
			}
			break;
		case ScaveModelPackage.SCATTER_CHART__DATA_NAME:
			if (dataNameCombo != null) {
				String dataName = Arrays.asList(dataNames).contains(value) ? (String)value : StringUtils.EMPTY;
				dataNameCombo.setText(dataName);
			}
			break;
		default:
			super.setValue(feature, value);
			break;
		}
	}
}
