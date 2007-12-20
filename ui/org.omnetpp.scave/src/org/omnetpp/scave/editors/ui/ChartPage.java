package org.omnetpp.scave.editors.ui;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryLabelProvider;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TreePath;
import org.eclipse.jface.viewers.TreeSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.IMemento;
import org.omnetpp.common.canvas.LargeScrollableCanvas;
import org.omnetpp.common.canvas.RectangularArea;
import org.omnetpp.common.canvas.ZoomableCachingCanvas;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.actions.EditAction;
import org.omnetpp.scave.actions.NewChartProcessingOpAction;
import org.omnetpp.scave.actions.RemoveObjectAction;
import org.omnetpp.scave.charting.ChartCanvas;
import org.omnetpp.scave.charting.ChartFactory;
import org.omnetpp.scave.charting.ChartUpdater;
import org.omnetpp.scave.charting.IChartSelection;
import org.omnetpp.scave.charting.IChartSelectionListener;
import org.omnetpp.scave.charting.VectorChartSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ScaveEditorContributor;
import org.omnetpp.scave.editors.forms.BarChartEditForm;
import org.omnetpp.scave.editors.forms.ChartEditForm;
import org.omnetpp.scave.editors.forms.LineChartEditForm;
import org.omnetpp.scave.editors.treeproviders.ScaveModelLabelProvider;
import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.Param;
import org.omnetpp.scave.model.ProcessingOp;
import org.omnetpp.scave.model.ScatterChart;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.ChartDataPoint;
import org.omnetpp.scave.model2.ChartLine;

public class ChartPage extends ScaveEditorPage {

	private Chart chart; // the underlying model
	private ChartCanvas chartView;
	private ChartUpdater updater;

	// holds actions for the context menu of this chart 
	private MenuManager contextMenuManager = new MenuManager("#PopupMenu");
	
	private IChartSelectionListener chartSelectionListener;
	private IPropertyChangeListener chartPropertyChangeListener;

	public ChartPage(Composite parent, ScaveEditor editor, Chart chart) {
		super(parent, SWT.V_SCROLL, editor);
		this.chart = chart;
		initialize();
		this.updater = new ChartUpdater(chart, chartView, scaveEditor.getResultFileManager());
		hookListeners();
	}
	
	@Override
	public void dispose() {
		unhookListeners();
		super.dispose();
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
	
	public void updateChart() {
		if (updater != null)
			updater.updateDataset();
	}
	
	/**
	 * The only chart on this page is always active.
	 */
	@Override
	public ChartCanvas getActiveChartCanvas() {
		return chartView;
	}

	public void updatePage(Notification notification) {
		if (notification.isTouch() || !(notification.getNotifier() instanceof EObject))
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

	protected void initialize() {
		// set up UI
		setPageTitle("Chart: " + getChartName(chart));
		setFormTitle("Chart: " + getChartName(chart));
		setExpandHorizontal(true);
		setExpandVertical(true);
		//setBackground(ColorFactory.asColor("lightGray"));
		getBody().setLayout(new GridLayout(2,false));

		// set up contents
		Composite parent = getBody();
		chartView = (ChartCanvas) ChartFactory.createChart(parent, this.chart, scaveEditor.getResultFileManager());
		chartView.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
		// configureChartView(chartView, chart); //FIXME bring this method into this class
		
		// add context menu to chart, and populate it
		// (in fact, only the Remove submenu would need to be dynamic, but looks like
		// menuAboutToShow() does not get called for submenus)
		contextMenuManager.setRemoveAllWhenShown(true);
		contextMenuManager.addMenuListener(new IMenuListener() {
			public void menuAboutToShow(IMenuManager submenuManager) {
				updateContextMenu();
			}
		});
		chartView.setMenu(contextMenuManager.createContextMenu(chartView));
	}
	
	private void hookListeners() {
		if (chartSelectionListener == null) {
			chartSelectionListener = new IChartSelectionListener() {
				public void selectionChanged(IChartSelection selection) {
					if (selection instanceof VectorChartSelection) {
						VectorChartSelection chartSelection = (VectorChartSelection)selection;
						if (chartSelection.getIndex() >= 0) {
							ChartDataPoint point = new ChartDataPoint(
											chart,
											chartSelection.getSeries(),
											chartSelection.getSeriesKey(),
											chartSelection.getID(),
											chartSelection.getIndex(), 
											chartSelection.getEventNum(),
											chartSelection.getPreciseX(),
											chartSelection.getX(), 
											chartSelection.getY(),
											updater.getResultFileManager());
							scaveEditor.setSelection(new TreeSelection(new TreePath(new Object[] {chart, point})));
						}
						else {
							ChartLine line = new ChartLine(
									chart,
									chartSelection.getSeries(),
									chartSelection.getSeriesKey(),
									chartSelection.getID(),
									updater.getResultFileManager());
							scaveEditor.setSelection(new TreeSelection(new TreePath(new Object[] {chart, line})));
						}
					}
					else {
						scaveEditor.setSelection(new StructuredSelection(chart));
					}
				}
			};
			chartView.addChartSelectionListener(chartSelectionListener);
		}
		if (chartPropertyChangeListener == null) {
			chartPropertyChangeListener = new IPropertyChangeListener() {
				public void propertyChange(PropertyChangeEvent event) {
					if (event.getProperty() == LargeScrollableCanvas.PROP_VIEW_X ||
							event.getProperty() == LargeScrollableCanvas.PROP_VIEW_Y ||
							event.getProperty() == ZoomableCachingCanvas.PROP_ZOOM_X ||
							event.getProperty() == ZoomableCachingCanvas.PROP_ZOOM_Y) {
						scaveEditor.markNavigationLocation();
					}
				}
			};
			chartView.addPropertyChangeListener(chartPropertyChangeListener);
		}
	}
	
	private void unhookListeners() {
		if (chartSelectionListener != null) {
			chartView.removeChartSelectionListener(chartSelectionListener);
			chartSelectionListener = null;
		}
		if (chartPropertyChangeListener != null) {
			chartView.removePropertyChangeListener(chartPropertyChangeListener);
			chartPropertyChangeListener = null;
		}
	}

	protected void updateContextMenu() {
		ScaveEditorContributor editorContributor = ScaveEditorContributor.getDefault();
		if (editorContributor != null) {
			contextMenuManager.add(editorContributor.getZoomInAction());
			contextMenuManager.add(editorContributor.getZoomOutAction());
			contextMenuManager.add(editorContributor.getZoomToFitAction());
			contextMenuManager.add(new Separator());
			contextMenuManager.add(new EditAction("Chart...", createFormProperties(ChartEditForm.PROP_DEFAULT_TAB, ChartEditForm.TAB_MAIN)));
			if (chart instanceof LineChart || chart instanceof ScatterChart)
				contextMenuManager.add(new EditAction("Lines...", createFormProperties(ChartEditForm.PROP_DEFAULT_TAB, LineChartEditForm.TAB_LINES)));
			else if (chart instanceof BarChart)
				contextMenuManager.add(new EditAction("Bars...", createFormProperties(ChartEditForm.PROP_DEFAULT_TAB, BarChartEditForm.TAB_BARS)));
			contextMenuManager.add(new EditAction("Axes...", createFormProperties(ChartEditForm.PROP_DEFAULT_TAB, ChartEditForm.TAB_AXES)));
			contextMenuManager.add(new EditAction("Title...", createFormProperties(ChartEditForm.PROP_DEFAULT_TAB, ChartEditForm.TAB_TITLES)));
			contextMenuManager.add(new EditAction("Legend...", createFormProperties(ChartEditForm.PROP_DEFAULT_TAB, ChartEditForm.TAB_LEGEND)));
			contextMenuManager.add(new Separator());
			contextMenuManager.add(editorContributor.getCopyChartToClipboardAction());
			contextMenuManager.add(new Separator());
			contextMenuManager.add(createProcessingSubmenu(true));  // "Apply" submenu
			contextMenuManager.add(createProcessingSubmenu(false)); // "Compute" submenu
			contextMenuManager.add(createRemoveProcessingSubmenu()); // "Remove" submenu
			contextMenuManager.add(editorContributor.getGotoChartDefinitionAction());
			contextMenuManager.add(new Separator());
			contextMenuManager.add(editorContributor.getUndoRetargetAction());
			contextMenuManager.add(editorContributor.getRedoRetargetAction());
			contextMenuManager.add(new Separator());
			contextMenuManager.add(editorContributor.getCreateChartTemplateAction());
			contextMenuManager.add(new Separator());
			contextMenuManager.add(editorContributor.getRefreshChartAction());
		}
	}

	protected IMenuManager createProcessingSubmenu(boolean isApply) {
		IMenuManager submenuManager = new MenuManager(isApply ? "Apply" : "Compute");
		submenuManager.add(new NewChartProcessingOpAction("Mean", createOp(isApply, "mean")));
		submenuManager.add(new NewChartProcessingOpAction("Window Batch Average", createOp(isApply, "winavg", "windowSize", "10")));
		submenuManager.add(new NewChartProcessingOpAction("Sliding Window Average", createOp(isApply, "slidingwinavg", "windowSize", "10")));
		submenuManager.add(new NewChartProcessingOpAction("Moving Average", createOp(isApply, "movingavg", "alpha", "0.1")));
		submenuManager.add(new NewChartProcessingOpAction("Time Average", createOp(isApply, "timeavg")));
		submenuManager.add(new Separator());
		submenuManager.add(new NewChartProcessingOpAction("Remove Repeated Values", createOp(isApply, "removerepeats")));
		submenuManager.add(new Separator());
		submenuManager.add(new NewChartProcessingOpAction("Sum", createOp(isApply, "sum")));
		submenuManager.add(new NewChartProcessingOpAction("Integrate", createOp(isApply, "integrate")));
		submenuManager.add(new NewChartProcessingOpAction("Difference", createOp(isApply, "difference")));
		submenuManager.add(new NewChartProcessingOpAction("Time Difference", createOp(isApply, "timediff")));
		submenuManager.add(new NewChartProcessingOpAction("Divide By Time", createOp(isApply, "divtime")));
		submenuManager.add(new NewChartProcessingOpAction("Difference Quotient", createOp(isApply, "diffquot")));
		submenuManager.add(new Separator());
		submenuManager.add(new NewChartProcessingOpAction("Other...", createOp(isApply, null)));
		return submenuManager;
	}

	protected IMenuManager createRemoveProcessingSubmenu() {
		IMenuManager submenuManager = new MenuManager("Remove");
		ILabelProvider labelProvider = new ScaveModelLabelProvider(new AdapterFactoryLabelProvider(scaveEditor.getAdapterFactory()));

		// list all chart processing operations in the menu
		//XXX when Chart would remain the only item in the Group, ungroup it!
		for (EObject child : chart.eContainer().eContents()) {
			if (child == chart) 
				break; // only list objects *before* the chart
			if (child instanceof ProcessingOp) {
				String text = StringUtils.capitalize(labelProvider.getText(child));
				submenuManager.add(new RemoveObjectAction(child, text));
			}
		}
		return submenuManager;
	}

	protected static ProcessingOp createOp(boolean isApply, String operation) {
		ScaveModelFactory factory = ScaveModelFactory.eINSTANCE;
		ProcessingOp applyOrCompute = isApply ? factory.createApply() : factory.createCompute();
		applyOrCompute.setOperation(operation);
		return applyOrCompute;
	}

	protected static ProcessingOp createOp(boolean isApply, String operation, String paramName, String value) {
		ProcessingOp applyOrCompute = createOp(isApply, operation);
		Param param = ScaveModelFactory.eINSTANCE.createParam();
		param.setName(paramName);
		param.setValue(value);
		applyOrCompute.getParams().add(param);
		return applyOrCompute;
	}
	
	protected static Map<String,Object> createFormProperties(String key1, Object value1) {
		Map<String,Object> map = new HashMap<String, Object>();
		map.put(key1, value1);
		return map;
	}

	@Override
	public boolean gotoObject(Object object) {
		if (object instanceof EObject) {
			EObject eobject = (EObject)object;
			if (EcoreUtil.isAncestor(chart, eobject)) {
				return true;
			}
		}
		return false;
	}

	@Override
	public void saveState(IMemento memento) {
		memento.putString("Zoom", chartView.getZoomedArea().toString());
	}
	
	@Override
	public void restoreState(IMemento memento) {
		String areaStr = memento.getString("Zoom");
		RectangularArea area = areaStr != null ? RectangularArea.fromString(areaStr) : null;
		if (area != null)
			chartView.setZoomedArea(area);
	}
}
