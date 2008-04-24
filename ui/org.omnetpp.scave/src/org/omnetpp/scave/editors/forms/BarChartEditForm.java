package org.omnetpp.scave.editors.forms;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.collections.set.ListOrderedSet;
import org.eclipse.core.runtime.Assert;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.edit.ui.dnd.LocalTransfer;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.DragSourceEvent;
import org.eclipse.swt.dnd.DragSourceListener;
import org.eclipse.swt.dnd.DropTargetEvent;
import org.eclipse.swt.dnd.TableDropTargetEffect;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Widget;
import org.omnetpp.common.ui.ListContentProvider;
import org.omnetpp.common.util.Converter;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.charting.properties.ChartDefaults;
import org.omnetpp.scave.charting.properties.ChartProperties;
import org.omnetpp.scave.charting.properties.ScalarChartProperties;
import org.omnetpp.scave.charting.properties.ChartProperties.BarPlacement;
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
	
	public static final String TAB_BARS = "Bars";
	public static final String TAB_CONTENT = "Content";

	public static final List<String> EMPTY_STRING_LIST = Collections.unmodifiableList(new ArrayList<String>());
	
	private static final EStructuralFeature[] features = new EStructuralFeature[] {
		pkg.getChart_Name(),
		pkg.getBarChart_GroupByFields(),
		pkg.getBarChart_BarFields(),
		pkg.getBarChart_AveragedFields(),
		pkg.getChart_Properties(),
	};
	
	private static final String[] fieldNames = ResultItemFields.getFieldNames().toArray();

	// Main
	private Viewer unusedFieldsList;
	private Viewer groupFieldsList;
	private Viewer barFieldsList;
	private Viewer averagedFieldsList;
	
	// Labels
	private Button wrapLabelsCheckbox;

	// Bars
	private Text baselineText;
	private Combo barPlacementCombo;

	public BarChartEditForm(Chart chart, EObject parent, Map<String,Object> formParameters, ResultFileManager manager) {
		super(chart, parent, formParameters, manager);
	}
	
	public void populateTabFolder(TabFolder tabfolder) {
		super.populateTabFolder(tabfolder);
		createTab(TAB_CONTENT, tabfolder, 1);
		createTab(TAB_BARS, tabfolder, 2);
	}
	
	public void populateTabItem(TabItem item) {
		super.populateTabItem(item);
		String name = item.getText();
		Composite panel = (Composite)item.getControl();
		if (TAB_TITLES.equals(name)) {
			wrapLabelsCheckbox = createCheckboxField("wrap labels", axisTitlesGroup);
			wrapLabelsCheckbox.setSelection(ChartDefaults.DEFAULT_WRAP_LABELS);
		}
		else if (TAB_CONTENT.equals(name)) {
			Group group = createGroup("Content", panel, 1, 3, true);
			Label label = new Label(group, SWT.WRAP);
			label.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 3, 1));
			label.setText("You can move fields by dragging.");
			unusedFieldsList = createFieldsList(group, true);
			unusedFieldsList.getControl().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 3, 1));
			unusedFieldsList.setInput(Arrays.asList(fieldNames));
			createLabel("Groups", group);
			createLabel("Bars", group);
			createLabel("Averaged", group);
			groupFieldsList = createFieldsList(group, false);
			barFieldsList = createFieldsList(group, false);
			averagedFieldsList = createFieldsList(group, true);
		}
		else if (TAB_BARS.equals(name)) {
			if (chart instanceof BarChart) {
				baselineText = createTextField("Baseline", panel);
				barPlacementCombo = createComboField("Bar placement", panel, BarPlacement.class, false);
			}
		}
	}
	
	private TableViewer createFieldsList(Composite parent, final boolean sorted) {
		final TableViewer viewer = new TableViewer(parent, SWT.BORDER | SWT.MULTI | SWT.V_SCROLL);
		viewer.setLabelProvider(new LabelProvider());
		viewer.setContentProvider(new ListContentProvider());
		viewer.setInput(EMPTY_STRING_LIST);
		final Table control = viewer.getTable();  
		GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
		gridData.minimumHeight = control.getItemHeight() * fieldNames.length; 
		control.setLayoutData(gridData);
		control.setLinesVisible(false);
		control.setHeaderVisible(false);
		
		final Widget[] dropTarget = new Widget[1], dragSource = new Widget[1];
		
		// configure as drag source
		final Transfer transfer = LocalTransfer.getInstance();
		viewer.addDragSupport(DND.DROP_MOVE, new Transfer[] { transfer }, 
			new DragSourceListener() {
				public void dragStart(DragSourceEvent event) {
					dropTarget[0] = null;
					ISelection selection = viewer.getSelection();
					if (!selection.isEmpty()) {
						event.doit = true;
						dragSource[0] = control;
						
					}
					else
						event.doit = false;
				}
				
				public void dragSetData(DragSourceEvent event) {
					if (transfer.isSupportedType(event.dataType)) {
						ISelection selection = viewer.getSelection();
						event.data = (selection instanceof IStructuredSelection ?
										((IStructuredSelection)selection).toList() :
										EMPTY_STRING_LIST);
					}
				}
	
				public void dragFinished(DragSourceEvent event) {
					if (event.doit && event.detail == DND.DROP_MOVE &&  dropTarget[0] != control) {
						ISelection selection = viewer.getSelection();
						if (selection instanceof IStructuredSelection) {
							List<String> input = new ArrayList<String>(getInput(viewer));
							input.removeAll(((IStructuredSelection)selection).toList());
							viewer.setInput(input);
						}
					}
					dragSource[0] = null;
				}
		});
		
		// configure drop target
		viewer.addDropSupport(DND.DROP_MOVE, new Transfer[] { transfer },
			new TableDropTargetEffect(control) {
				@Override
				public void dragEnter(DropTargetEvent event) {
					event.feedback = DND.FEEDBACK_NONE;
					super.dragEnter(event);
				}
	
				@Override
				public void dragLeave(DropTargetEvent event) {
					event.feedback = DND.FEEDBACK_NONE;
					super.dragLeave(event);
				}
	
				@Override
				public void dragOver(DropTargetEvent event) {
					if (!sorted)
						event.feedback = DND.FEEDBACK_SELECT;
					else if (dragSource[0] == control)
						event.detail = DND.DROP_NONE;
					super.dragOver(event);
				}
	
				@SuppressWarnings("unchecked")
				public void drop(DropTargetEvent event) {
					if (transfer.isSupportedType(event.currentDataType) && (event.data instanceof List)) {
						dropTarget[0] = control;
						List<String> data = (List<String>)event.data;
						ListOrderedSet items = new ListOrderedSet();
						items.addAll(getInput(viewer));
						
						if (sorted) {
							items.addAll(data);
							viewer.setInput(sortFields((Set<String>)items));
						}
						else {
							int index = getInsertionIndex(event);
							items.removeAll(data);
							items.addAll(index, data);
							viewer.setInput(new ArrayList<String>(items));
						}
					}
					else {
						event.detail = DND.DROP_NONE;
					}
				}
				
				@SuppressWarnings("unchecked")
				private int getInsertionIndex(DropTargetEvent event) {
					if (event.item instanceof Item) {
						List<String> data = (List<String>)event.data;
						int index = 0;
						for (int i=0; i < control.getItemCount(); ++i) {
							Item item = control.getItem(i);
							if (item == event.item)
								return index;
							if (!data.contains(item.getText()))
								++index;
						}
					}
					return control.getItemCount();
				}
			});
		
		return viewer;
	}
	
	private List<String> sortFields(Set<String> fields) {
		List<String> result = new ArrayList<String>(fields.size());
		
		for (String field : fieldNames) {
			if (fields.contains(field))
				result.add(field);
		}
		Assert.isTrue(fields.size() == result.size());
		return result;
	}

	public EStructuralFeature[] getFeatures() {
		return features;
	}
	
	public Object getValue(EStructuralFeature feature) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.BAR_CHART__GROUP_BY_FIELDS:
			return getInput(groupFieldsList);
		case ScaveModelPackage.BAR_CHART__BAR_FIELDS:
			return getInput(barFieldsList);
		case ScaveModelPackage.BAR_CHART__AVERAGED_FIELDS:
			return getInput(averagedFieldsList);
		}
		return super.getValue(feature);
	}
	
	@SuppressWarnings("unchecked")
	public void setValue(EStructuralFeature feature, Object value) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.BAR_CHART__GROUP_BY_FIELDS:
			setContent(groupFieldsList, (List<String>)value);
			break;
		case ScaveModelPackage.BAR_CHART__BAR_FIELDS:
			setContent(barFieldsList, (List<String>)value);
			break;
		case ScaveModelPackage.BAR_CHART__AVERAGED_FIELDS:
			setContent(averagedFieldsList, (List<String>)value);
			break;
		}
		super.setValue(feature, value);
	}
	
	@SuppressWarnings("unchecked")
	private List<String> getInput(Viewer list) {
		return (List<String>)list.getInput();
	}
	
	private void setContent(Viewer list, List<String> fields) {
		list.setInput(fields);
		List<String> unusedFields = new ArrayList<String>(getInput(unusedFieldsList));
		unusedFields.removeAll(fields);
		unusedFieldsList.setInput(unusedFields);
	}
	
	@Override
	protected void collectProperties(ChartProperties newProps) {
		super.collectProperties(newProps);
		ScalarChartProperties props = (ScalarChartProperties)newProps;
		props.setBarBaseline(Converter.stringToDouble(baselineText.getText()));
		props.setBarPlacement(resolveEnum(barPlacementCombo.getText(), BarPlacement.class));
		props.setWrapLabels(wrapLabelsCheckbox.getSelection());
	}

	@Override
	protected void setProperties(ChartProperties props) {
		super.setProperties(props);
		ScalarChartProperties scalarProps = (ScalarChartProperties)props;
		baselineText.setText(StringUtils.defaultString(Converter.doubleToString(scalarProps.getBarBaseline())));
		barPlacementCombo.setText(scalarProps.getBarPlacement()==null ? NO_CHANGE : scalarProps.getBarPlacement().toString());
		wrapLabelsCheckbox.setSelection(scalarProps.getWrapLabels());
	}
}
