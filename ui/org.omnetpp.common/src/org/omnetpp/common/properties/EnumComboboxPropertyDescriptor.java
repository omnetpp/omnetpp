package org.omnetpp.common.properties;

import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.ICellEditorValidator;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.util.StringUtils;

/**
 * Editable combobox property descriptor, to be used with IDisplayString.EnumSpec.
 * 
 * @author Andras
 */
public class EnumComboboxPropertyDescriptor extends PropertyDescriptor {

	/** The list of possible values to display in the combo box. */
	private IDisplayString.EnumSpec enumSpec;

	/** The control of this property descriptor. */
	private EditableComboBoxCellEditor editor;


	public EnumComboboxPropertyDescriptor(Object id, String displayName, IDisplayString.EnumSpec enumSpec) {
		super(id, displayName);
		this.enumSpec = enumSpec;
		setLabelProvider(new LabelProvider() {
			public String getText(Object element) {
				String text = element.toString();
				if (text.equals(""))
					return text;
				String name = EnumComboboxPropertyDescriptor.this.enumSpec.getNameFor(text);
				if (name == null)
					return text + " (invalid value)";
				else if (name.equals(text))
					return text;
				else
					return text + " (" + name + ")";
			}
		});
	}

	@Override
	protected ICellEditorValidator getValidator() {
		return new ICellEditorValidator() {
			public String isValid(Object value) {
				String text = value.toString();
				if (!text.equals("") && enumSpec.getNameFor(text) == null)
					return "Must be one of: " + StringUtils.join(enumSpec.getNames(), ", ") + 
					"; or an allowed abbreviation like " + 
					StringUtils.join(enumSpec.getShorthands(), ", ");
				return null;
			}
		};
	}
	
	@Override
	public CellEditor createPropertyEditor(Composite parent) {
		editor = new EditableComboBoxCellEditor(parent, enumSpec.getNames());
		
		// customize combobox behavior a bit: when the user chooses something
		// from the dropdown list, replace it with the standard abbreviation
		final CCombo combo = ((CCombo)editor.getControl());
		combo.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				String text = combo.getText();
				String shorthand = enumSpec.getShorthandFor(text);
				if (shorthand != null)
					combo.setText(shorthand);
			}
		});

		if (getValidator() != null)
			editor.setValidator(getValidator());

		return editor;
	}
}
