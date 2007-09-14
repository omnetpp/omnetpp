package org.omnetpp.scave.editors.forms;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableItem;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.ScatterChart;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.DatasetManager;
import org.omnetpp.scave.model2.ModuleAndData;
import org.omnetpp.scave.model2.ScaveModelUtil;

// XXX remove (or disable) the data that was selected as X data in the iso table
public class ScatterChartEditForm extends BaseLineChartEditForm {

	
	private static final EStructuralFeature[] scatterChartFeatures = new EStructuralFeature[] {
		pkg.getChart_Name(),
		pkg.getScatterChart_XDataPattern(),
		pkg.getScatterChart_IsoDataPattern(),
		pkg.getScatterChart_AverageReplications(),
		pkg.getChart_Properties(),
	};
	
	private static final String TAB_CONTENT = "Content";
	
	private Combo xModuleAndDataCombo;
	private Table isoModuleAndDataTable;
	private Button avgReplicationsCheckbox;
	
	private ModuleAndData[] data = new ModuleAndData[0];
	
	public ScatterChartEditForm(ScatterChart chart, EObject parent, Map<String,Object> formParameters, ResultFileManager manager) {
		super(chart, parent, formParameters, manager);
		updateLineNames(null);
		Dataset dataset = ScaveModelUtil.findEnclosingOrSelf(parent, Dataset.class);
		if (dataset != null) {
			IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, chart, ResultType.SCALAR_LITERAL);
			idlist.merge(DatasetManager.getIDListFromDataset(manager, dataset, chart, ResultType.VECTOR_LITERAL));
			data = ScaveModelUtil.getModuleAndDataPairs(idlist, manager);
		}
	}
	
	protected void updateLineNames(String formatString) {
		lineNames = ArrayUtils.EMPTY_STRING_ARRAY;
		IXYDataset xydataset;
		
		try {
			Dataset dataset = ScaveModelUtil.findEnclosingOrSelf(parent, Dataset.class);
			xydataset = DatasetManager.createScatterPlotDataset((ScatterChart)chart, dataset, manager, null);
		}
		catch (Exception e) {
			xydataset = null;
		}

		if (xydataset != null) {
			lineNames = new String[xydataset.getSeriesCount()];
			for (int i = 0; i < xydataset.getSeriesCount(); ++i)
				lineNames[i] = xydataset.getSeriesKey(i);
		}

		if (linesTableViewer != null)
			linesTableViewer.setInput(lineNames);
	}
	
	
	/**
	 * Returns the features edited on this form.
	 */
	public EStructuralFeature[] getFeatures() {
		return scatterChartFeatures;
	}
	
	

	@Override
	protected void populateTabFolder(TabFolder tabfolder) {
		super.populateTabFolder(tabfolder);
		createTab(TAB_CONTENT, tabfolder, 2);
	}

	@Override
	protected void populateTabItem(TabItem item) {
		super.populateTabItem(item);
		String name = item.getText();
		Composite panel = (Composite)item.getControl();
		Group group;
		GridData gridData;
		if (TAB_CONTENT.equals(name)) {
			// x data
			group = createGroup("X data", panel, 2, 2);
			createLabel("Select the scalar whose values displayed on the X axis.",
					group, 2);
			String[] items = new String [data.length];
			for (int i = 0; i < items.length; ++i)
				items[i] = data[i].asListItem();
			xModuleAndDataCombo = createComboField("", group, items);
			if (items.length > 0)
				xModuleAndDataCombo.setText(items[0]);
			
			// iso data
			group = createGroup("Iso lines", panel, 2, 1);
			createLabel("Select scalars whose values must be equal on data points connected by lines.",
					group, 1);
			isoModuleAndDataTable = new Table(group,
					SWT.BORDER | SWT.CHECK | SWT.HIDE_SELECTION | SWT.V_SCROLL);
			gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
			int itemsVisible = Math.max(Math.min(data.length, 15), 3);
			gridData.heightHint = itemsVisible * isoModuleAndDataTable.getItemHeight();
			isoModuleAndDataTable.setLayoutData(gridData);
			for (int i = 0; i < data.length; ++i) {
				TableItem tableItem = new TableItem(isoModuleAndDataTable, SWT.NONE);
				tableItem.setChecked(false);
				tableItem.setText(data[i].asListItem());
				tableItem.setData(data[i].asFilterPattern());
			}
			
			group = createGroup("Average replications", panel, 2, 1);
			createLabel("Check if the values that are the replications of the same measurement must be averaged.", group, 1);
			avgReplicationsCheckbox = createCheckboxField("average replications", group);
			avgReplicationsCheckbox.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
			avgReplicationsCheckbox.setSelection(true);
		}
	}
	
	private Label createLabel(String text, Composite parent, int columns) {
		Composite panel = new Composite(parent, SWT.NONE);
		panel.setLayoutData(new GridData(SWT.LEFT, SWT.CENTER, false, false, columns, 1));
		panel.setLayout(new RowLayout(SWT.VERTICAL));
		
		Label label = new Label(panel, SWT.WRAP); // XXX WRAP does not work
		label.setText(text);
		return label;
	}
	
	@Override
	public Object getValue(EStructuralFeature feature) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.SCATTER_CHART__XDATA_PATTERN:
			int index = xModuleAndDataCombo.getSelectionIndex();
			return index >= 0 ? data[index].asFilterPattern() : null; 
		case ScaveModelPackage.SCATTER_CHART__ISO_DATA_PATTERN:
			List<String> patterns = new ArrayList<String>();
			for (TableItem item : isoModuleAndDataTable.getItems()) {
				if (item.getChecked())
					patterns.add((String)item.getData());
			}
			return patterns;
		case ScaveModelPackage.SCATTER_CHART__AVERAGE_REPLICATIONS:
			return avgReplicationsCheckbox.getSelection();
		}
		return super.getValue(feature);
	}

	@SuppressWarnings("unchecked")
	@Override
	public void setValue(EStructuralFeature feature, Object value) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.SCATTER_CHART__XDATA_PATTERN:
			if (xModuleAndDataCombo != null) {
				ModuleAndData xModuleAndData = null;
				if (value instanceof String) {
					int index = ArrayUtils.indexOf(data, ModuleAndData.fromFilterPattern((String)value));
					if (index >= 0)
						xModuleAndData = data[index];
				}
				
				if (xModuleAndData != null)
					xModuleAndDataCombo.setText(xModuleAndData.asListItem());
				else
					xModuleAndDataCombo.deselectAll();
			}
			break;
		case ScaveModelPackage.SCATTER_CHART__ISO_DATA_PATTERN:
			if (isoModuleAndDataTable != null) {
				if (value instanceof List) {
					List<String> patterns = (List<String>)value;
					for (TableItem item : isoModuleAndDataTable.getItems()) {
						item.setChecked(patterns.contains(item.getData()));
					}
				}
			}
			break;
		case ScaveModelPackage.SCATTER_CHART__AVERAGE_REPLICATIONS:
			if (avgReplicationsCheckbox != null) {
				avgReplicationsCheckbox.setSelection(value != null ? ((Boolean)value) : true);
			}
		default:
			super.setValue(feature, value);
			break;
		}
	}
}
