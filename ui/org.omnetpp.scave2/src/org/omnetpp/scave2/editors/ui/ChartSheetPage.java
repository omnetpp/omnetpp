package org.omnetpp.scave2.editors.ui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.omnetpp.common.color.ColorFactory;

public class ChartSheetPage extends ScrolledForm {

	private LiveTable chartsArea;
	
	public ChartSheetPage(Composite parent, int style) {
		super(parent, style | SWT.V_SCROLL | SWT.H_SCROLL);
		initialize();
	}
	
	public Composite getChartSheetComposite() {
		//return getBody();
		return chartsArea;
	}
	
	public void addChart(Control chart) {
		chart.setLayoutData(new GridData(320,200));
		chartsArea.configureChild(chart);
	}
	
	private void initialize() {
		setBackground(ColorFactory.asColor("white"));
		setExpandHorizontal(true);
		setExpandVertical(true);
		GridLayout layout = new GridLayout();
		getBody().setLayout(layout);
		
		chartsArea = new LiveTable(getBody(), SWT.NONE);
		chartsArea.setLayoutData(new GridData(SWT.FILL,SWT.FILL, true, true));
		chartsArea.setBackground(ColorFactory.asColor("white"));

		GridLayout gridLayout = new GridLayout(2, true); //2 columns
		gridLayout.horizontalSpacing = 7;
		gridLayout.verticalSpacing = 7;
		chartsArea.setLayout(gridLayout);
	}
}
