package org.omnetpp.scave.editors.forms;

import java.util.Map;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.editors.ui.ResultItemNamePatternField;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.DatasetManager;
import org.omnetpp.scave.model2.ResultItemFields;
import org.omnetpp.scave.model2.ScaveModelUtil;

public class LineChartEditForm extends BaseLineChartEditForm {
	
	/**
	 * Features edited on this form.
	 */
	private static final EStructuralFeature[] features = new EStructuralFeature[] {
		pkg.getChart_Name(),
		pkg.getLineChart_LineNameFormat(),
		pkg.getChart_Properties(),
	};
	
	private Text lineNamePattern;
	

	public LineChartEditForm(LineChart chart, EObject parent,
			Map<String, Object> formParameters, ResultFileManager manager) {
		super(chart, parent, formParameters, manager);
		updateLineNames(chart.getLineNameFormat());
	}
	

	@Override
	protected void populateTabItem(final TabItem item) {
		super.populateTabItem(item);
		String name = item.getText();
		if (TAB_MAIN.equals(name)) {
			lineNamePattern = createTextField("Line names:", nameGroup);
			new ResultItemNamePatternField(lineNamePattern, ResultItemFields.getFieldNames());
		}
		else if (TAB_LINES.equals(name)) {
			TabFolder tabfolder = item.getParent();
			final boolean[] updateLineNames = new boolean[1];
			lineNamePattern.addModifyListener(new ModifyListener() {
				public void modifyText(ModifyEvent e) {
					updateLineNames[0] = true;
				}
			});
			tabfolder.addSelectionListener(new SelectionAdapter() {
				public void widgetSelected(SelectionEvent e) {
					if (e.item == item && updateLineNames[0])
						updateLineNames(lineNamePattern.getText());
				}
			});
		}
	}
	
	protected void updateLineNames(String formatString) {
		Dataset dataset = ScaveModelUtil.findEnclosingOrSelf(parent, Dataset.class);
		IXYDataset xydataset = DatasetManager.createVectorDataset((LineChart)chart, dataset, formatString, false, manager, null);
		lineNames = ArrayUtils.EMPTY_STRING_ARRAY;
		if (xydataset != null) {
			lineNames = new String[xydataset.getSeriesCount()];
			for (int i = 0; i < xydataset.getSeriesCount(); ++i)
				lineNames[i] = xydataset.getSeriesKey(i);
		}

		if (linesTableViewer != null)
			linesTableViewer.setInput(lineNames);
	}

	@Override
	public EStructuralFeature[] getFeatures() {
		return features;
	}


	@Override
	public Object getValue(EStructuralFeature feature) {
		switch (pkg.getLineChart().getFeatureID(feature)) {
		case ScaveModelPackage.LINE_CHART__LINE_NAME_FORMAT:
			return lineNamePattern.getText();
		default:
			return super.getValue(feature);
		}
	}


	@Override
	public void setValue(EStructuralFeature feature, Object value) {
		switch (pkg.getLineChart().getFeatureID(feature)) {
		case ScaveModelPackage.LINE_CHART__LINE_NAME_FORMAT:
			lineNamePattern.setText(value == null ? "" : (String)value);
			break;
		default:
			super.setValue(feature, value);
		}
	}
}
