package org.omnetpp.scave.editors.ui;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.charting.ChartCanvas;
import org.omnetpp.scave.charting.ChartFactory;
import org.omnetpp.scave.charting.ChartUpdater;
import org.omnetpp.scave.charting.properties.ChartProperties;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.ScaveModelPackage;

public class ChartSheetPage extends ScaveEditorPage {

	private ChartSheet chartsheet; // the underlying model

	private LiveTable chartsArea;
	private List<ChartUpdater> updaters = new ArrayList<ChartUpdater>();
	
	public ChartSheetPage(Composite parent, ScaveEditor editor, ChartSheet chartsheet) {
		super(parent, SWT.V_SCROLL | SWT.H_SCROLL, editor);
		this.chartsheet = chartsheet;
		initialize();
	}
	
	public void updatePage(Notification notification) {
		Object notifier = notification.getNotifier();

		if (notifier == chartsheet) {
			switch (notification.getFeatureID(ChartSheet.class)) {
			case ScaveModelPackage.CHART_SHEET__NAME:
				setPageTitle("Charts: " + getChartSheetName(chartsheet));
				setFormTitle("Charts: " + getChartSheetName(chartsheet));
				break;
			case ScaveModelPackage.CHART_SHEET__CHARTS:
				synchronize();
				break;
			}
		}
		else {
			updateCharts();
		}
	}
	
	public void updateCharts() {
		for (ChartUpdater updater : updaters)
			updater.updateDataset();
	}
	
	public Composite getChartSheetComposite() {
		//return getBody();
		return chartsArea;
	}
	
	private void addChartUpdater(Chart chart, ChartCanvas view) {
		updaters.add(new ChartUpdater(chart, view, scaveEditor.getResultFileManager()));
	}
	
	private void removeChartUpdater(Chart chart) {
		for (Iterator<ChartUpdater> iterator = updaters.iterator(); iterator.hasNext(); ) {
			ChartUpdater updater = iterator.next();
			if (updater.getChart().equals(chart)) {
				iterator.remove();
				break;
			}
		}
	}
	
	private Chart findChart(Control view) {
		for (ChartUpdater updater : updaters)
			if (updater.getChartView().equals(view))
				return updater.getChart();
		return null;
	}
	
	private ChartCanvas findChartView(Chart chart) {
		for (ChartUpdater updater : updaters)
			if (updater.getChart().equals(chart))
				return updater.getChartView();
		return null;
	}
	
	
	
	private ChartCanvas addChartView(final Chart chart) {
		ChartCanvas view = ChartFactory.createChart(chartsArea, chart, scaveEditor.getResultFileManager());
		Assert.isNotNull(view);

		//
		view.setLayoutData(new GridData(320,200));
		chartsArea.configureChild(view);
		addChartUpdater(chart, view);
		MenuManager menuManager = new ChartMenuManager(chart, scaveEditor);
		view.setMenu(menuManager.createContextMenu(view));
		
		// hide legend
		view.setProperty(ChartProperties.PROP_DISPLAY_LEGEND, "false");
		//
		view.addMouseListener(new MouseAdapter() {
			// double click in the view opens the chart in a separate page
			public void mouseDoubleClick(MouseEvent e) {
				scaveEditor.openChart(chart);
			}
		
			// mouse click on the view selects the chart object in the model
			public void mouseUp(MouseEvent e) {
				scaveEditor.setSelection(new StructuredSelection(chart));
			}
		});

		return view;
	}
	
	private void removeChartView(Chart chart) {
		ChartCanvas view = findChartView(chart);
		removeChartUpdater(chart);
		if (view != null && !view.isDisposed())
			view.dispose();
	}
	
	private void synchronize() {
		List<ChartCanvas> currentViews = new ArrayList<ChartCanvas>();
		for (Object child : chartsArea.getChildren())
			if (child instanceof ChartCanvas)
				currentViews.add((ChartCanvas)child);
		
		List<Chart> charts = new ArrayList<Chart>(chartsheet.getCharts());
		List<ChartCanvas> views = new ArrayList<ChartCanvas>(charts.size());
		
		for (Chart chart : charts) {
			ChartCanvas view = findChartView(chart);
			if (view == null)
				view = addChartView(chart);
			views.add(view);
		}
		
		for (ChartCanvas view : currentViews) {
			if (!views.contains(view)) {
				Chart chart = findChart(view);
				removeChartView(chart);
			}
		}
		
		chartsArea.setChildOrder(views);
		chartsArea.layout();
		chartsArea.redraw();
	}
	
	@SuppressWarnings("unchecked")
	private void initialize() {
		// set up UI
		setPageTitle("Charts: " + getChartSheetName(chartsheet));
		setFormTitle("Charts: " + getChartSheetName(chartsheet));
		setBackground(ColorFactory.WHITE);
		//setExpandHorizontal(true);
		//setExpandVertical(true);
		GridLayout layout = new GridLayout();
		getBody().setLayout(layout);
		
		chartsArea = new LiveTable(getBody(), SWT.DOUBLE_BUFFERED);
		chartsArea.setLayoutData(new GridData(SWT.FILL,SWT.FILL, true, true));
		chartsArea.setBackground(ColorFactory.WHITE);

		GridLayout gridLayout = new GridLayout(2, true); //2 columns
		gridLayout.horizontalSpacing = 7;
		gridLayout.verticalSpacing = 7;
		chartsArea.setLayout(gridLayout);
		
		chartsArea.addSelectionChangedListener(new ISelectionChangedListener() {
			public void selectionChanged(SelectionChangedEvent event) {
				scaveEditor.setSelection(event.getSelection());
			}
		});
		
		chartsArea.addChildOrderChangedListener(new LiveTable.IChildOrderChangedListener() {
			public void childOrderChanged(LiveTable table) {
				if (table == chartsArea && !chartsArea.isDisposed())
					handleChildOrderChanged();
			}
		});
		
		// set up contents
		for (final Chart chart : chartsheet.getCharts())
			addChartView(chart);
		
		getContent().setSize(getContent().computeSize(SWT.DEFAULT, SWT.DEFAULT));
	}
	
	@Override
	public boolean setFocus() {
		if (chartsArea != null)
			return chartsArea.setFocus();
		else
			return super.setFocus();
	}

	private static String getChartSheetName(ChartSheet chartSheet) {
		return StringUtils.defaultString(chartSheet.getName(), "<unnamed>");
	}

	@Override
	public boolean gotoObject(Object object) {
		if (object == chartsheet) {
			return true;
		}
		if (object instanceof EObject) {
			EObject eobject = (EObject)object;
			for (Chart chart : chartsheet.getCharts())
				if (chart == eobject) {
					// TODO scroll to chart
					return true;
				}
		}
		return false;
	}

	@Override
	public ChartCanvas getActiveChartCanvas() {
		ISelection selection = chartsArea.getSelection();
		if (selection instanceof IStructuredSelection) {
			IStructuredSelection structuredSelection = (IStructuredSelection)selection;
			if (structuredSelection.size() == 1 &&
					structuredSelection.getFirstElement() instanceof ChartCanvas)
				return (ChartCanvas)structuredSelection.getFirstElement();
		}
		return null;
	}
	
	protected void handleChildOrderChanged() {
		List<Chart> charts = new ArrayList<Chart>();
		for (Control child : chartsArea.getChildren()) {
			if (child instanceof ChartCanvas) {
				Chart chart = findChart(child);
				if (chart != null)
					charts.add(chart);
			}
		}
		
		EditingDomain domain = scaveEditor.getEditingDomain();
		CompoundCommand command = new CompoundCommand("Move");
		EStructuralFeature feature = ScaveModelPackage.eINSTANCE.getChartSheet_Charts();
		command.append(SetCommand.create(domain, chartsheet, feature, charts));
		scaveEditor.executeCommand(command);
	}
}
