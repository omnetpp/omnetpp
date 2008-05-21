package org.omnetpp.scave.editors.forms;

import java.util.Map;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.util.Converter;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.charting.properties.ChartProperties;
import org.omnetpp.scave.charting.properties.HistogramChartProperties;
import org.omnetpp.scave.charting.properties.HistogramChartProperties.HistogramBar;
import org.omnetpp.scave.charting.properties.HistogramChartProperties.HistogramDataType;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.HistogramChart;

public class HistogramChartEditForm extends ChartEditForm {

	public static final String TAB_PLOT = "Plot";

	// Bars
	private Text baselineText;
	private Combo barTypeCombo;
	private Combo dataTypeCombo;
	
	public HistogramChartEditForm(HistogramChart chart, EObject parent,
			Map<String, Object> formParameters, ResultFileManager manager) {
		super(chart, parent, formParameters, manager);
	}

	@Override
	protected void populateTabFolder(TabFolder tabfolder) {
		super.populateTabFolder(tabfolder);
		createTab(TAB_PLOT, tabfolder, 2);
	}
	
	@Override
	public void populateTabItem(TabItem item) {
		super.populateTabItem(item);
		String name = item.getText();
		Composite panel = (Composite)item.getControl();
		if (TAB_PLOT.equals(name)) {
			baselineText = createTextField("Baseline", panel);
			barTypeCombo = createComboField("Bar type", panel, HistogramBar.class, false);
			dataTypeCombo = createComboField("Data type", panel, HistogramDataType.class, false);
		}
	}
	
	@Override
	protected void collectProperties(ChartProperties newProps) {
		super.collectProperties(newProps);
		HistogramChartProperties props = (HistogramChartProperties)newProps;
		props.setBarBaseline(Converter.stringToDouble(baselineText.getText()));
		props.setBarType(resolveEnum(barTypeCombo.getText(), HistogramBar.class));
		props.setHistogramDataType(resolveEnum(dataTypeCombo.getText(), HistogramDataType.class));
	}

	@Override
	protected void setProperties(ChartProperties props) {
		super.setProperties(props);
		HistogramChartProperties histogramProps = (HistogramChartProperties)props;
		baselineText.setText(StringUtils.defaultString(Converter.doubleToString(histogramProps.getBarBaseline())));
		barTypeCombo.setText(histogramProps.getBarType()==null ? NO_CHANGE : histogramProps.getBarType().toString());
		dataTypeCombo.setText(histogramProps.getHistogramDataType()==null ? NO_CHANGE : histogramProps.getHistogramDataType().toString());
	}
}
