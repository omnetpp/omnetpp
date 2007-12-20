package org.omnetpp.scave.editors.ui;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryLabelProvider;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.viewers.ILabelProvider;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.actions.EditAction;
import org.omnetpp.scave.actions.NewChartProcessingOpAction;
import org.omnetpp.scave.actions.RemoveObjectAction;
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

public class ChartMenuManager extends MenuManager {

	private Chart chart;
	private ScaveEditorContributor editorContributor;
	private ILabelProvider labelProvider;
	
	public ChartMenuManager(Chart chart, ScaveEditor editor) {
		super("#PopupMenu");
		this.chart = chart;
		this.editorContributor = ScaveEditorContributor.getDefault();
		this.labelProvider = new ScaveModelLabelProvider(new AdapterFactoryLabelProvider(editor.getAdapterFactory()));
		
		setRemoveAllWhenShown(true);
		addMenuListener(new IMenuListener() {
			public void menuAboutToShow(IMenuManager submenuManager) {
				updateContextMenu();
			}
		});
	}
	
	protected void updateContextMenu() {
			add(editorContributor.getZoomInAction());
			add(editorContributor.getZoomOutAction());
			add(editorContributor.getZoomToFitAction());
			add(new Separator());
			add(new EditAction("Chart...", createFormProperties(ChartEditForm.PROP_DEFAULT_TAB, ChartEditForm.TAB_MAIN)));
			if (chart instanceof LineChart || chart instanceof ScatterChart)
				add(new EditAction("Lines...", createFormProperties(ChartEditForm.PROP_DEFAULT_TAB, LineChartEditForm.TAB_LINES)));
			else if (chart instanceof BarChart)
				add(new EditAction("Bars...", createFormProperties(ChartEditForm.PROP_DEFAULT_TAB, BarChartEditForm.TAB_BARS)));
			add(new EditAction("Axes...", createFormProperties(ChartEditForm.PROP_DEFAULT_TAB, ChartEditForm.TAB_AXES)));
			add(new EditAction("Title...", createFormProperties(ChartEditForm.PROP_DEFAULT_TAB, ChartEditForm.TAB_TITLES)));
			add(new EditAction("Legend...", createFormProperties(ChartEditForm.PROP_DEFAULT_TAB, ChartEditForm.TAB_LEGEND)));
			add(new Separator());
			add(editorContributor.getCopyChartToClipboardAction());
			add(new Separator());
			add(createProcessingSubmenu(true));  // "Apply" submenu
			add(createProcessingSubmenu(false)); // "Compute" submenu
			add(createRemoveProcessingSubmenu()); // "Remove" submenu
			add(editorContributor.getGotoChartDefinitionAction());
			add(new Separator());
			add(editorContributor.getUndoRetargetAction());
			add(editorContributor.getRedoRetargetAction());
			add(new Separator());
			add(editorContributor.getCreateChartTemplateAction());
			add(new Separator());
			add(editorContributor.getRefreshChartAction());
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
}
