package org.omnetpp.scave.editors.forms;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Map;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.edit.ui.dnd.LocalTransfer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.DragSource;
import org.eclipse.swt.dnd.DragSourceEvent;
import org.eclipse.swt.dnd.DragSourceListener;
import org.eclipse.swt.dnd.DropTarget;
import org.eclipse.swt.dnd.DropTargetAdapter;
import org.eclipse.swt.dnd.DropTargetEvent;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.List;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.scave.charting.ChartProperties;
import org.omnetpp.scave.charting.ChartProperties.BarPlacement;
import org.omnetpp.scave.charting.ChartProperties.ScalarChartProperties;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItemFields;
import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * Edit form of bar charts.
 *
 * @author tomi
 */
public class BarChartEditForm extends ChartEditForm {

	private static final EStructuralFeature[] features = new EStructuralFeature[] {
		pkg.getChart_Name(),
		pkg.getBarChart_GroupBy(),
		pkg.getBarChart_BarFields(),
		pkg.getChart_Properties(),
	};
	
	private static final String[] fieldNames = ResultItemFields.getFieldNames().toArray();

	// Main
	//private Button[] groupByCheckboxes;
	private List groupFieldsList;
	private List barFieldsList;
	private List averagingFieldsList;

	// Bars
	private Text baselineText;
	private Combo barPlacementCombo;

	public BarChartEditForm(Chart chart, EObject parent, Map<String,Object> formParameters, ResultFileManager manager) {
		super(chart, parent, formParameters, manager);
	}
	
	public void populateTabFolder(TabFolder tabfolder) {
		super.populateTabFolder(tabfolder);
		createTab("Bars", tabfolder, 2);
	}
	
	public void populateTabItem(TabItem item) {
		super.populateTabItem(item);
		String name = item.getText();
		Composite panel = (Composite)item.getControl();
		if ("Main".equals(name)) {
			Group group = createGroup("Content", panel, 1, 3);
			Label label = new Label(group, SWT.WRAP);
			label.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 3, 1));
			label.setText("You can move fields by dragging.");
			createLabel("Groups", group);
			createLabel("Bars", group);
			createLabel("Averaged", group);
			groupFieldsList = createFieldsList(group);
			barFieldsList = createFieldsList(group);
			averagingFieldsList = createFieldsList(group);
			barFieldsList.setItems(fieldNames);
		}
		else if ("Bars".equals(name)) {
			if (chart instanceof BarChart) {
				baselineText = createTextField("Baseline", panel);
				barPlacementCombo = createComboField("Bar placement", panel, BarPlacement.class, false);
			}
		}
	}
	
	private List createFieldsList(Composite parent) {
		final List control = new List(parent, SWT.BORDER | SWT.MULTI);
		control.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		
		// configure as drag source
		final Transfer transfer = LocalTransfer.getInstance();
		DragSource dragSource = new DragSource(control, DND.DROP_MOVE);
		dragSource.setTransfer(new Transfer[] { transfer });
		dragSource.addDragListener(new DragSourceListener() {
			public void dragStart(DragSourceEvent event) {
				event.doit = control.getSelectionCount() > 0;
			}
			
			public void dragSetData(DragSourceEvent event) {
				if (transfer.isSupportedType(event.dataType)) {
					event.data = control.getSelection();
				}
			}

			public void dragFinished(DragSourceEvent event) {
				if (event.doit && event.detail == DND.DROP_MOVE) {
					String[] selection = control.getSelection();
					for (String item : selection)
						control.remove(item);
				}
			}
		});
		
		// configure drop target
		DropTarget dropTarget = new DropTarget(control, DND.DROP_MOVE);
		dropTarget.setTransfer(new Transfer[] { transfer });
		dropTarget.addDropListener(new DropTargetAdapter() {
			public void drop(DropTargetEvent event) {
				if (transfer.isSupportedType(event.currentDataType) && (event.data instanceof String[])) {
					java.util.List<String> items = new ArrayList<String>();
					items.addAll(Arrays.asList(control.getItems()));
					items.addAll(Arrays.asList((String[])event.data));
					control.setItems(sortFields(items));
				}
				else {
					event.detail = DND.DROP_NONE;
				}
			}
		});
		
		return control;
	}
	
	private String[] sortFields(Collection<String> fields) {
		String[] result = new String[fields.size()];
		int i=0;
		for (String field : fieldNames) {
			if (fields.contains(field))
				result[i++] = field;
		}
		Assert.isTrue(i == result.length);
		return result;
	}

	public EStructuralFeature[] getFeatures() {
		return features;
	}
	
	public Object getValue(EStructuralFeature feature) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.BAR_CHART__GROUP_BY:
			return Arrays.asList(groupFieldsList.getItems());
		case ScaveModelPackage.BAR_CHART__BAR_FIELDS:
			return Arrays.asList(barFieldsList.getItems());
		}
		return super.getValue(feature);
	}
	
	@SuppressWarnings("unchecked")
	public void setValue(EStructuralFeature feature, Object value) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.BAR_CHART__GROUP_BY:
			String[] groupingFields = ((java.util.List<String>)value).toArray(new String[0]);
			setContent(groupFieldsList, groupingFields);
			break;
		case ScaveModelPackage.BAR_CHART__BAR_FIELDS:
			String[] barFields = ((java.util.List<String>)value).toArray(new String[0]);
			setContent(barFieldsList, barFields);
			break;
		}
		super.setValue(feature, value);
	}
	
	private void setContent(List list, String[] fields) {
		Assert.isTrue(list == groupFieldsList || list == barFieldsList);
		List otherList = list == groupFieldsList ? barFieldsList : groupFieldsList;
		String[] otherFields = otherList.getItems();

		list.setItems(fields);
		for (String field : fields)
			if (ArrayUtils.contains(otherFields, field))
				otherList.remove(field);
		
		averagingFieldsList.removeAll();
		for (String field : fieldNames)
			if (!ArrayUtils.contains(fields, field) && !ArrayUtils.contains(otherFields, fields))
				averagingFieldsList.add(field);
	}
	
	@Override
	protected void collectProperties(ChartProperties newProps) {
		super.collectProperties(newProps);
		ScalarChartProperties props = (ScalarChartProperties)newProps;
		props.setBarBaseline(baselineText.getText());
		props.setBarPlacement(resolveEnum(barPlacementCombo.getText(), BarPlacement.class));
	}

	@Override
	protected void setProperties(ChartProperties props) {
		super.setProperties(props);
		ScalarChartProperties scalarProps = (ScalarChartProperties)props;
		baselineText.setText(scalarProps.getBarBaseline());
		barPlacementCombo.setText(scalarProps.getBarPlacement()==null ? NO_CHANGE : scalarProps.getBarPlacement().toString());
	}
}
