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

	public ChartSheetPage(Composite parent, int style) {
		super(parent, style | SWT.V_SCROLL | SWT.H_SCROLL);
		initialize();
	}
	
	public Composite getChartSheetComposite() {
		return getBody();
	}
	
	public void addChart(Control chart) {
		// set layout data
	}
	
	private void initialize() {
		setBackground(ColorFactory.asColor("white"));
		setExpandHorizontal(true);
		setExpandVertical(true);
		GridLayout layout = new GridLayout();
		getBody().setLayout(layout);
		
		LiveTable chartsArea = new LiveTable(getBody(), SWT.NONE);
		chartsArea.setLayoutData(new GridData(SWT.FILL,SWT.FILL, true, true));
		chartsArea.setBackground(ColorFactory.asColor("white"));

		//XXX some testing code
		for (int i=0; i<14; i++) {
			Canvas canvas = new Canvas(this, SWT.NONE);
			canvas.setBackground(new Color(null, 0, 255-i*10, 0));
			chartsArea.addChild(canvas);
		}
		
	}
}
