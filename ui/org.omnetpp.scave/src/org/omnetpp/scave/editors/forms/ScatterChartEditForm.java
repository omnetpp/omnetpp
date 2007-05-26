package org.omnetpp.scave.editors.forms;

import java.util.Map;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ScaveModelPackage;

public class ScatterChartEditForm extends LineChartEditForm {

	
	private static final EStructuralFeature[] scatterChartFeatures = new EStructuralFeature[] {
		pkg.getChart_Name(),
		pkg.getScatterChart_ModuleName(),
		pkg.getScatterChart_DataName(),
		pkg.getChart_Properties(),
	};
	
	private Text moduleNameText;
	private Text dataNameText;
	
	public ScatterChartEditForm(Chart chart, EObject parent, Map<String,Object> formParameters, ResultFileManager manager) {
		super(chart, parent, formParameters, manager);
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
			moduleNameText = createTextField("Module name", panel);
			dataNameText = createTextField("Data name", panel);
		}
	}

	@Override
	public Object getValue(EStructuralFeature feature) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.SCATTER_CHART__MODULE_NAME:
			return moduleNameText != null ? moduleNameText.getText() : null;
		case ScaveModelPackage.SCATTER_CHART__DATA_NAME:
			return dataNameText != null ? dataNameText.getText() : null;
		}
		return super.getValue(feature);
	}

	@Override
	public void setValue(EStructuralFeature feature, Object value) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.SCATTER_CHART__MODULE_NAME:
			if (moduleNameText != null)
				moduleNameText.setText(value == null ? "" : (String)value);
			break;
		case ScaveModelPackage.SCATTER_CHART__DATA_NAME:
			if (dataNameText != null)
				dataNameText.setText(value == null ? "" : (String)value);
			break;
		default:
			super.setValue(feature, value);
			break;
		}
	}
}
