package org.omnetpp.scave.editors.forms;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.TableEditor;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.scave.engine.NodeType;
import org.omnetpp.scave.engine.NodeTypeRegistry;
import org.omnetpp.scave.engine.NodeTypeVector;
import org.omnetpp.scave.engine.StringMap;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.model.Apply;
import org.omnetpp.scave.model.Param;
import org.omnetpp.scave.model.ProcessingOp;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * Edit form for processing operations (Apply/Compute).
 * The form contains a type field and params fields depending on the type.
 *
 * @author tomi
 */
public class ProcessingOperationEditForm implements IScaveObjectEditForm {

	protected static final EStructuralFeature[] features = new EStructuralFeature[] {
		ScaveModelPackage.eINSTANCE.getProcessingOp_Operation(),
		ScaveModelPackage.eINSTANCE.getProcessingOp_Params()
	};
	
	// the edited value
	private ProcessingOp processingOp;
	
	// operation types
	private NodeType[] operationTypes;
	// operation names (to fill the combo)
	private String[] operationNames;
	
	// controls
	private Combo operationCombo;
	private Label description;
	private Table paramsTable;
	private TableEditor tableEditor;
	
	/**
	 * Number of visible items in combos.
	 */
	private static final int VISIBLE_ITEM_COUNT = 15;
	
	
	// columns in the params table
	private static final int
		COLUMN_NAME = 0,
		COLUMN_VALUE = 1,
		COLUMN_DESC = 2;
	
	public ProcessingOperationEditForm(ProcessingOp processingOp, EObject parent) {
		this.processingOp = processingOp;
		NodeTypeVector types = NodeTypeRegistry.getInstance().getNodeTypes();
		List<NodeType> filterTypes = new ArrayList<NodeType>();
		List<String> filterNames = new ArrayList<String>();
		for (int i = 0; i < types.size(); ++i) {
			NodeType nodeType = types.get(i);
			if ("filter".equals(nodeType.getCategory())) {
				filterTypes.add(nodeType);
				filterNames.add(nodeType.getName());
			}
		}
		this.operationTypes = filterTypes.toArray(new NodeType[filterTypes.size()]);
		this.operationNames = filterNames.toArray(new String[filterNames.size()]);
	}
	
	public String getTitle() {
		return processingOp instanceof Apply ? "Apply" : "Compute";
	}

	public String getDescription() {
		return "Select the operation and set the parameters.";
	}
	public EStructuralFeature[] getFeatures() {
		return features;
	}

	public void populatePanel(Composite panel) {
		panel.setLayout(new GridLayout());
		
		Group group;
		
		group = new Group(panel, SWT.NONE);
		group.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		group.setLayout(new GridLayout());
		group.setText("Operation");
		
		operationCombo = new Combo(group, SWT.BORDER | SWT.READ_ONLY);
		operationCombo.setVisibleItemCount(VISIBLE_ITEM_COUNT);
		operationCombo.setItems(operationNames);
		operationCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		operationCombo.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				handleOperationTypeChanged();
			}
		});
		
		description = new Label(group, SWT.WRAP);
		description.setText("");
		description.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
		
		group = new Group(panel, SWT.NONE);
		group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		group.setLayout(new GridLayout());
		group.setText("Parameters");
		
		paramsTable = new Table(group, SWT.BORDER | SWT.SINGLE | SWT.FULL_SELECTION);
		paramsTable.setHeaderVisible(true);
		paramsTable.setLinesVisible(true);
		GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
		gridData.minimumHeight = paramsTable.getHeaderHeight() + paramsTable.getItemHeight() * 5; 
		paramsTable.setLayoutData(gridData);
		TableColumn column = new TableColumn(paramsTable, SWT.NONE);
		column.setText("Name");
		column.setWidth(100);
		column = new TableColumn(paramsTable, SWT.NONE);
		column.setText("Value");
		column.setWidth(100);
		column = new TableColumn(paramsTable, SWT.NONE);
		column.setText("Description");
		column.setWidth(200);
		
		tableEditor = new TableEditor(paramsTable);
		tableEditor.horizontalAlignment = SWT.LEFT;
		tableEditor.grabHorizontal = true;
		tableEditor.minimumWidth = 50;
		paramsTable.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				handleParamsTableSelectionChange(e);
			}
		});
	}
	
	private void handleOperationTypeChanged() {
		updateDescription();
		updateTable();
	}
	
	private void handleParamsTableSelectionChange(SelectionEvent e) {
		// Clean up any previous editor control
		Control oldEditor = tableEditor.getEditor();
		if (oldEditor != null) oldEditor.dispose();

		// Identify the selected row
		TableItem item = (TableItem)e.item;
		if (item == null) return;

		// The control that will be the editor must be a child of the Table
		Text newEditor = new Text(paramsTable, SWT.NONE);
		newEditor.setText(item.getText(COLUMN_VALUE));
		newEditor.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				Text text = (Text)tableEditor.getEditor();
				tableEditor.getItem().setText(COLUMN_VALUE, text.getText());
			}
		});
		newEditor.selectAll();
		newEditor.setFocus();
		tableEditor.setEditor(newEditor, item, COLUMN_VALUE);
	}
	
	private void updateDescription() {
		int index = operationCombo.getSelectionIndex();
		NodeType type = index >= 0 ? operationTypes[index] : null;
		description.setText(type != null ? type.getDescription() : "");
		description.getParent().getParent().layout();
	}
	
	private void updateTable() {
		if (tableEditor.getEditor() != null) {
			tableEditor.getEditor().dispose();
			tableEditor.setEditor(null);
		}
		
		int index = operationCombo.getSelectionIndex();
		NodeType type = index >= 0 ? operationTypes[index] : null;
		paramsTable.removeAll();
		if (type != null) {
			StringMap params = new StringMap();
			StringMap defaults = new StringMap();
			type.getAttributes(params);
			type.getAttrDefaults(defaults);

			StringVector names = params.keys();
			for (int i = 0; i < names.size(); ++i) {
				String name = names.get(i);
				String defaultValue = defaults.has_key(name) ? defaults.get(name) : "";
				TableItem item = new TableItem(paramsTable, SWT.NONE);
				item.setText(COLUMN_NAME, name);
				item.setText(COLUMN_VALUE, defaultValue);
				item.setText(COLUMN_DESC, params.get(name));
			}
		}
		paramsTable.getParent().getParent().layout(true, true);
	}
	
	public Object getValue(EStructuralFeature feature) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.PROCESSING_OP__OPERATION:
			int index = operationCombo.getSelectionIndex();
			return index >= 0 ? operationNames[index] : null;
		case ScaveModelPackage.PROCESSING_OP__PARAMS:
			EList<Param> params = new BasicEList<Param>();
			for (int i = 0; i < paramsTable.getItemCount(); ++i) {
				TableItem item = paramsTable.getItem(i);
				Param param = ScaveModelFactory.eINSTANCE.createParam();
				param.setName(item.getText(COLUMN_NAME));
				param.setValue(item.getText(COLUMN_VALUE));
				params.add(param);
			}
			return params;
		}
		return null;
	}

	@SuppressWarnings("unchecked")
	public void setValue(EStructuralFeature feature, Object value) {
		switch (feature.getFeatureID()) {
		case ScaveModelPackage.PROCESSING_OP__OPERATION:
			operationCombo.select(Arrays.asList(operationNames).indexOf(value));
			handleOperationTypeChanged();
			break;
		case ScaveModelPackage.PROCESSING_OP__PARAMS:
			EList<Param> params = (EList<Param>)value;
			for (Param param : params) {
				setParamValue(param.getName(), param.getValue());
			}
			break;
		}
	}

	private void setParamValue(String name, String value) {
		for (int i = 0; i < paramsTable.getItemCount(); ++i) {
			TableItem item = paramsTable.getItem(i);
			if (item.getText(COLUMN_NAME).equals(name)) {
				item.setText(COLUMN_VALUE, value);
				break;
			}
		}
	}
}
