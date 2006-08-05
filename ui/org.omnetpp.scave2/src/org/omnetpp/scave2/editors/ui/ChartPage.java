package org.omnetpp.scave2.editors.ui;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetType;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave2.charting.ChartFactory;
import org.omnetpp.scave2.charting.InteractiveChart;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.model.DatasetManager;
import org.omnetpp.scave2.model.ScaveModelUtil;

public class ChartPage extends ScaveEditorPage {

	private Chart chart; // the underlying model

	public ChartPage(Composite parent, ScaveEditor editor, Chart chart) {
		super(parent, SWT.V_SCROLL, editor);
		this.chart = chart;
		initialize();
	}
	
	public void updatePage(Notification notification) {
		if (ScaveModelPackage.eINSTANCE.getChart_Name().equals(notification.getFeature())) {
			setPageTitle("Chart: " + chart.getName());
			setFormTitle("Chart: " + chart.getName());
		}
	}
	
	public Composite getChartComposite() {
		return getBody();
	}
	
	public void setChart(Control chart) {
		// set layout data
	}
	
	private void initialize() {
		// set up UI
		setPageTitle("Chart: " + chart.getName());
		setFormTitle("Chart: " + chart.getName());
		setExpandHorizontal(true);
		setExpandVertical(true);
		setBackground(ColorFactory.asColor("white"));
		getBody().setLayout(new FillLayout());
		
		// set up contents
		Composite parent = getChartComposite();
//		Dataset dataset = ScaveModelUtil.findEnclosingObject(chart, Dataset.class);
//		IDList idlist = DatasetManager.getIDListFromDataset(scaveEditor.getResultFileManager(), dataset, chart);
//		DatasetType type = dataset.getType();
//		final InteractiveChart chart = ChartFactory.createChart(parent, type, idlist, scaveEditor.getResultFileManager()); 
		final Control chart = ChartFactory.createChart(parent, this.chart, scaveEditor.getResultFileManager(), -1, -1);
		setChart(chart);
		chart.addMouseListener(new MouseAdapter() {
			public void mouseUp(MouseEvent e) {
				scaveEditor.setSelection(new StructuredSelection(chart));
			}
		});
	}
}
