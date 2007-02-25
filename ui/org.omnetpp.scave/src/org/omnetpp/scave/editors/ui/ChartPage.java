package org.omnetpp.scave.editors.ui;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.charting.ChartCanvas;
import org.omnetpp.scave.charting.ChartFactory;
import org.omnetpp.scave.charting.ChartUpdater;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ScaveEditorContributor;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ScaveModelPackage;

public class ChartPage extends ScaveEditorPage {

	private Chart chart; // the underlying model
	private ChartCanvas chartView;
	private ChartUpdater updater;

	// holds actions for the context menu of this chart 
	private MenuManager contextMenuManager = new MenuManager("#PopupMenu"); 

	public ChartPage(Composite parent, ScaveEditor editor, Chart chart) {
		super(parent, SWT.V_SCROLL, editor);
		this.chart = chart;
		initialize();
		this.updater = new ChartUpdater(chart, chartView, scaveEditor.getResultFileManager());
	}
	
	public Chart getChart() {
		return chart;
	}
	
	public void setChart(Control chart) {
		// set layout data
	}
	
	public ChartCanvas getChartView() {
		return chartView;
	}
	
	public ChartUpdater getChartUpdater() {
		return updater;
	}
	
	public void updatePage(Notification notification) {
		if (!(notification.getNotifier() instanceof EObject) ||
			(notification.getNotifier() != chart &&
					((EObject)notification.getNotifier()).eContainer() != chart))
			return;
		
		ScaveModelPackage pkg = ScaveModelPackage.eINSTANCE;
		if (pkg.getChart_Name().equals(notification.getFeature())) {
			setPageTitle("Chart: " + getChartName(chart));
			setFormTitle("Chart: " + getChartName(chart));
		}
		updater.updateChart(notification);
	}

	private String getChartName(Chart chart) {
		return chart.getName() != null ? chart.getName() : "<unnamed>";
	}

	private void initialize() {
		// set up UI
		setPageTitle("Chart: " + getChartName(chart));
		setFormTitle("Chart: " + getChartName(chart));
		setExpandHorizontal(true);
		setExpandVertical(true);
		setBackground(ColorFactory.asColor("white"));
		getBody().setLayout(new GridLayout(2,false));

		// set up contents
		Composite parent = getBody();
		chartView = (ChartCanvas) ChartFactory.createChart(parent, this.chart, scaveEditor.getResultFileManager());
		chartView.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
		configureChartView(chartView, chart); //FIXME bring this method into this class
		
		// add context menu to chart, and populate it
		chartView.setMenu(contextMenuManager.createContextMenu(chartView));
		ScaveEditorContributor editorContributor = ScaveEditorContributor.getDefault();
		contextMenuManager.add(editorContributor.getZoomOutAction());
		contextMenuManager.add(editorContributor.getZoomInAction());
		contextMenuManager.add(new Separator());
		contextMenuManager.add(editorContributor.getEditAction());
		contextMenuManager.add(editorContributor.getCopyChartToClipboardAction());
		contextMenuManager.add(new Separator());
		contextMenuManager.add(editorContributor.getCreateChartTemplateAction());
		contextMenuManager.add(new Separator());
		contextMenuManager.add(editorContributor.getRefreshChartAction());
	}
}
