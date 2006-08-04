package org.omnetpp.scave2.editors.ui;

import org.eclipse.draw2d.ColorConstants;
import org.eclipse.emf.common.notify.Notification;
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
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave2.charting.OutputVectorDataset;
import org.omnetpp.scave2.charting.VectorChart;
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
	
	public void updatePage(Notification notification) {
		if (ScaveModelPackage.eINSTANCE.getChart_Name().equals(notification.getFeature())) {
			setPageTitle("Chart: " + getChartName(chart));
			setFormTitle("Chart: " + getChartName(chart));
		}
	}

	private String getChartName(Chart chart) {
		return chart.getName() != null ? chart.getName() : "<unnamed>";
	}

	public void setChart(Control chart) {
		// set layout data
	}

	private void initialize() {
		// set up UI
		setPageTitle("Chart: " + getChartName(chart));
		setFormTitle("Chart: " + getChartName(chart));
		setExpandHorizontal(true);
		setExpandVertical(true);
		setBackground(ColorFactory.asColor("white"));
		getBody().setLayout(new GridLayout(1,false));

		// set up contents
		Composite parent = getBody();
		Dataset dataset = ScaveModelUtil.findEnclosingObject(chart, Dataset.class);
		ResultFileManager manager = scaveEditor.getResultFileManager();
		XYDataset xydataset = new OutputVectorDataset(DatasetManager.getDataFromDataset(manager, dataset, chart));
		//XXX just testing
		final VectorChart chart = new VectorChart(parent, SWT.DOUBLE_BUFFERED);
		chart.setLayoutData(new GridData(SWT.FILL,SWT.FILL,true,true));
		chart.setSize(getParent().getBounds().width, getParent().getBounds().height); // provide width/height hint until layouting runs
		chart.setDataset(xydataset);
		chart.setCaching(true);

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

		Button antialias = new Button(controlStrip, SWT.CHECK);
		antialias.setText("Antialias");

		Button zoomInX = new Button(controlStrip, SWT.NONE);
		zoomInX.setText("Zoom <->");

		Button zoomOutX = new Button(controlStrip, SWT.NONE);
		zoomOutX.setText("Zoom -><-");

		Button zoomInY = new Button(controlStrip, SWT.NONE);
		zoomInY.setText("Zoom ^");

		Button zoomOutY = new Button(controlStrip, SWT.NONE);
		zoomOutY.setText("Zoom v");

		Combo lineType = new Combo(controlStrip, SWT.NONE);
		lineType.setItems(new String[] {
				"LinesVectorPlotter",
				"PointsVectorPlotter",
				"DotsVectorPlotter",
				"PinsVectorPlotter",
				"SampleHoldVectorPlotter",
		});
		lineType.setVisibleItemCount(lineType.getItemCount());
		lineType.select(0); //XXX

		Combo symbolType = new Combo(controlStrip, SWT.NONE);
		symbolType.setItems(new String[] {
				"SquareSymbol",
				"DiamondSymbol",
				"TriangleSymbol",
				"OvalSymbol",
				"CrossSymbol",
				"PlusSymbol",
		});
		symbolType.setVisibleItemCount(symbolType.getItemCount());
		symbolType.select(0); //XXX

		Combo symbolSize = new Combo(controlStrip, SWT.NONE);
		for (int i=0; i<=20; i++)
			symbolSize.add(""+i);
		symbolSize.setVisibleItemCount(symbolSize.getItemCount());
		symbolSize.select(4); //XXX

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

		antialias.setSelection(chart.getAntialias()==SWT.ON);
		antialias.addSelectionListener(new SelectionAdapter () {
			public void widgetSelected(SelectionEvent e) {
				chart.setAntialias(((Button)e.getSource()).getSelection() ? SWT.ON : SWT.OFF);
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
				int oldSizeHint = chart.getDefaultSymbol().getSizeHint();
				try {
					chart.setDefaultSymbol((IChartSymbol)(Class.forName("org.omnetpp.scave2.charting.plotter."+sel).newInstance()));
					chart.getDefaultSymbol().setSizeHint(oldSizeHint);
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
				chart.clearCanvasCacheAndRedraw();
			}
		});
		return controlStrip;
	}
}
