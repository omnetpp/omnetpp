package org.omnetpp.scave2.editors.ui;

import org.eclipse.draw2d.ColorConstants;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.jfree.data.xy.XYDataset;
import org.omnetpp.common.canvas.RubberbandSupport;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetType;
import org.omnetpp.scave2.charting.ChartHelper;
import org.omnetpp.scave2.charting.VectorChart;
import org.omnetpp.scave2.charting.plotter.DotsVectorPlotter;
import org.omnetpp.scave2.charting.plotter.IChartSymbol;
import org.omnetpp.scave2.charting.plotter.IVectorPlotter;
import org.omnetpp.scave2.editors.ScaveEditor;
import org.omnetpp.scave2.model.DatasetManager;
import org.omnetpp.scave2.model.ScaveModelUtil;

public class ChartPage2 extends ScaveEditorPage {

	private Chart chart; // the underlying model

	public ChartPage2(Composite parent, ScaveEditor editor, Chart chart) {
		super(parent, SWT.V_SCROLL, editor);
		this.chart = chart;
		initialize();
	}

	public void setChart(Control chart) {
		// set layout data
	}

	private void initialize() {
		// set up UI
		setPageTitle("EXPERIMENTAL Chart: " + chart.getName());
		setFormTitle("Chart: " + chart.getName());
		setExpandHorizontal(true);
		setExpandVertical(true);
		setBackground(ColorFactory.asColor("white"));
		getBody().setLayout(new GridLayout(1,false));

		// set up contents
		Composite parent = getBody();
		Dataset dataset = ScaveModelUtil.findEnclosingObject(chart, Dataset.class);
		IDList idlist = DatasetManager.getIDListFromDataset(scaveEditor.getResultFileManager(), dataset, chart);
		DatasetType type = dataset.getType();

		//XXX just testing
		XYDataset xydataset = ChartHelper.createXYDataSet(idlist, scaveEditor.getResultFileManager());
		final VectorChart chart = new VectorChart(parent, SWT.DOUBLE_BUFFERED);
		chart.setLayoutData(new GridData(SWT.FILL,SWT.FILL,true,true));
		chart.setSize(getParent().getBounds().width, getParent().getBounds().height); // provide width/height hint until layouting runs
		chart.setDataset(xydataset);
		chart.setCaching(false);
		chart.setDefaultLineType(new DotsVectorPlotter());
		//chart.setDefaultPlotter(new PinsVectorPlotter());
		//chart.setDefaultPlotter(new SampleHoldVectorPlotter());

		chart.setBackground(ColorConstants.white);
		new RubberbandSupport(chart, SWT.CTRL) {
			@Override
			public void rubberBandSelectionMade(Rectangle r) {
				chart.zoomToRectangle(new org.eclipse.draw2d.geometry.Rectangle(r));
			}
		};

		// create control strip (XXX temp code)
		Composite controlStrip = createControlStrip(parent, chart);
		controlStrip.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
		controlStrip.moveAbove(chart);
	}

	//XXX temporary code
	private Composite createControlStrip(Composite parent, final VectorChart chart) {
		Composite controlStrip = new Composite(parent, SWT.NONE);
		RowLayout layout = new RowLayout();
		controlStrip.setLayout(layout);

		Button canvasCaching = new Button(controlStrip, SWT.CHECK);
		canvasCaching.setText("Caching");

		Button zoomInX = new Button(controlStrip, SWT.NONE);
		zoomInX.setText("X+");

		Button zoomInY = new Button(controlStrip, SWT.NONE);
		zoomInY.setText("Y+");

		Button zoomOutX = new Button(controlStrip, SWT.NONE);
		zoomOutX.setText("X-");

		Button zoomOutY = new Button(controlStrip, SWT.NONE);
		zoomOutY.setText("Y-");

		Combo lineType = new Combo(controlStrip, SWT.NONE);
		lineType.setItems(new String[] {
				"DotsVectorPlotter",
				"LinesVectorPlotter",
				"PinsVectorPlotter",
				"PointsVectorPlotter",
				"SampleHoldVectorPlotter"
		});
		lineType.setVisibleItemCount(lineType.getItemCount());

		Combo symbolType = new Combo(controlStrip, SWT.NONE);
		symbolType.setItems(new String[] {
				"OvalSymbol",
				"CrossSymbol",
				"DiamondSymbol",
				"PlusSymbol",
				"SquareSymbol",
				"TriangleSymbol"
		});
		symbolType.setVisibleItemCount(symbolType.getItemCount());

		Combo symbolSize = new Combo(controlStrip, SWT.NONE);
		for (int i=0; i<=20; i++)
			symbolSize.add(""+i);
		symbolSize.setVisibleItemCount(symbolSize.getItemCount());

		// add event handlers
		zoomInX.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				chart.zoomXBy(1.5);
			}});

		zoomOutX.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				chart.zoomXBy(1/1.5);
			}});
		zoomInY.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				chart.zoomYBy(1.5);
			}});

		zoomOutY.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				chart.zoomYBy(1/1.5);
			}});

		canvasCaching.setSelection(chart.getCaching());
		canvasCaching.addSelectionListener(new SelectionAdapter () {
			public void widgetSelected(SelectionEvent e) {
				chart.setCaching(((Button)e.getSource()).getSelection());
				chart.redraw();
			}
		});

		lineType.addSelectionListener(new SelectionAdapter () {
			public void widgetSelected(SelectionEvent e) {
				Combo combo = ((Combo)e.getSource());
				String sel = combo.getItem(combo.getSelectionIndex());
				try {
					chart.setDefaultLineType((IVectorPlotter)(Class.forName("org.omnetpp.scave2.charting.plotter."+sel).newInstance()));
				} catch (Exception e1) {
					e1.printStackTrace();
				}
			}
		});

		symbolType.addSelectionListener(new SelectionAdapter () {
			public void widgetSelected(SelectionEvent e) {
				Combo combo = ((Combo)e.getSource());
				String sel = combo.getItem(combo.getSelectionIndex());
				int oldSize = chart.getDefaultSymbol().getSizeHint();
				try {
					chart.setDefaultSymbol((IChartSymbol)(Class.forName("org.omnetpp.scave2.charting.plotter."+sel).newInstance()));
					chart.getDefaultSymbol().setSizeHint(oldSize);
				} catch (Exception e1) {
					e1.printStackTrace();
				}
			}
		});
		
		symbolSize.addSelectionListener(new SelectionAdapter () {
			public void widgetSelected(SelectionEvent e) {
				Combo combo = ((Combo)e.getSource());
				String sel = combo.getItem(combo.getSelectionIndex());
				chart.getDefaultSymbol().setSizeHint(Integer.parseInt(sel));
			}
		});
		return controlStrip;
	}
}
