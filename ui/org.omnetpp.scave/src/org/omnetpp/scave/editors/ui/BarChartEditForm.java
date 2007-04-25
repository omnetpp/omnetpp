package org.omnetpp.scave.editors.ui;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ScalarFields;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ScaveModelPackage;

public class BarChartEditForm extends ChartEditForm {

	private static final ScaveModelPackage pkg = ScaveModelPackage.eINSTANCE;
	
	private static final EStructuralFeature[] features = new EStructuralFeature[] {
		pkg.getChart_Name(),
		pkg.getBarChart_GroupBy(),
		pkg.getChart_Properties(),
	};
	
	private Button[] groupByCheckboxes; // for BarChart only
	

	public BarChartEditForm(Chart chart, EObject parent, ResultFileManager manager) {
		super(chart, parent, manager);
	}
	
	public void populateTab(String name, Composite panel) {
		super.populateTab(name, panel);
		if ("Main".equals(name)) {
			Group group = createGroup("Group scalars by", panel, 2, 1);
			StringVector fields = ScalarFields.getFieldNames();
			groupByCheckboxes = new Button[(int)fields.size()];
			for (int i=0; i < fields.size(); ++i) {
				String field = fields.get(i);
				groupByCheckboxes[i] = createCheckboxField(field, group, field);
			}
		}
	}

	public EStructuralFeature[] getFeatures() {
		return features;
	}
	
	public Object getValue(EStructuralFeature feature) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.BAR_CHART__GROUP_BY:
			List<String> fields = new ArrayList<String>();
			for (Button cb : groupByCheckboxes)
				if (cb.getSelection())
					fields.add((String)cb.getData(USER_DATA_KEY));
			return fields;
		}
		return super.getValue(feature);
	}
	
	@SuppressWarnings("unchecked")
	public void setValue(EStructuralFeature feature, Object value) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.BAR_CHART__GROUP_BY:
			List<String> fields = (List<String>)value;
			for (Button cb : groupByCheckboxes) {
				cb.setSelection(fields != null && fields.contains(cb.getData(USER_DATA_KEY)));
			}
			break;
		}
		super.setValue(feature, value);
	}
}
