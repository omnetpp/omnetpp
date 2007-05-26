package org.omnetpp.scave.editors.forms;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.scave.charting.ChartProperties;
import org.omnetpp.scave.charting.ChartProperties.BarPlacement;
import org.omnetpp.scave.charting.ChartProperties.ScalarChartProperties;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ScalarFields;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * Edit form of bar charts.
 *
 * @author tomi
 */
public class BarChartEditForm extends ChartEditForm {

	private static final EStructuralFeature[] features = new EStructuralFeature[] {
		pkg.getChart_Name(),
		pkg.getBarChart_GroupBy(),
		pkg.getChart_Properties(),
	};

	// Main
	private Button[] groupByCheckboxes;
	// Bars
	private Text baselineText;
	private Combo barPlacementCombo;

	public BarChartEditForm(Chart chart, EObject parent, Map<String,Object> formParameters, ResultFileManager manager) {
		super(chart, parent, formParameters, manager);
	}
	
	public void populateTabFolder(TabFolder tabfolder) {
		super.populateTabFolder(tabfolder);
		createTab("Bars", tabfolder, 2);
	}
	
	public void populateTabItem(TabItem item) {
		super.populateTabItem(item);
		String name = item.getText();
		Composite panel = (Composite)item.getControl();
		if ("Main".equals(name)) {
			Group group = createGroup("Group scalars by", panel, 2, 1);
			StringVector fields = ScalarFields.getFieldNames();
			groupByCheckboxes = new Button[(int)fields.size()];
			for (int i=0; i < fields.size(); ++i) {
				String field = fields.get(i);
				groupByCheckboxes[i] = createCheckboxField(field, group, field);
			}
		}
		else if ("Bars".equals(name)) {
			if (chart instanceof BarChart) {
				baselineText = createTextField("Baseline", panel);
				barPlacementCombo = createComboField("Bar placement", panel, BarPlacement.class, false);
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
	
	@Override
	protected void collectProperties(ChartProperties newProps) {
		super.collectProperties(newProps);
		ScalarChartProperties props = (ScalarChartProperties)newProps;
		props.setBarBaseline(baselineText.getText());
		props.setBarPlacement(getSelection(barPlacementCombo, BarPlacement.class));
	}

	@Override
	protected void setProperties(ChartProperties props) {
		super.setProperties(props);
		ScalarChartProperties scalarProps = (ScalarChartProperties)props;
		baselineText.setText(scalarProps.getBarBaseline());
		setSelection(barPlacementCombo, scalarProps.getBarPlacement());
	}
}
