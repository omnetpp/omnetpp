package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.GENERAL;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.fieldassist.ControlDecoration;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;

/**
 * Checkbox-based editor for boolean inifile entries. This editor commits changes
 * immediately, not only on losing the focus.
 * 
 * NOTE: This class edits the [General] section ONLY. All other sections
 * are ignored. For example, the Reset button only removes the setting
 * from the [General] section. When the setting is present outside
 * [General], the table-based field editor has to be used.
 * 
 * @author Andras
 */
public class CheckboxFieldEditor extends FieldEditor {
	private Button checkbox;
	private Label label;
	private ControlDecoration problemDecoration;
	private Button resetButton;
	private String section = GENERAL;

	public CheckboxFieldEditor(Composite parent, ConfigurationEntry entry, IInifileDocument inifile, FormPage formPage, String labelText) {
		super(parent, SWT.NONE, entry, inifile, formPage);

		Assert.isTrue(entry.getDataType()==ConfigurationEntry.DataType.CFG_BOOL);

		GridLayout gridLayout = new GridLayout(4, false); 
		gridLayout.marginHeight = 0;
		setLayout(gridLayout);
		
		checkbox = new Button(this, SWT.CHECK);
		checkbox.setBackground(BGCOLOR);
		tooltipSupport.adapt(checkbox);
		problemDecoration = new ControlDecoration(checkbox, SWT.RIGHT | SWT.TOP);
		problemDecoration.setShowOnlyOnFocus(false);
		label = createLabel(entry, labelText);
		resetButton = createResetButton();

		checkbox.setLayoutData(new GridData());
		label.setLayoutData(new GridData());
		((GridData)label.getLayoutData()).horizontalIndent = 5; // room for the problem decoration
		resetButton.setLayoutData(new GridData());

		reread();

		// enable Reset button on editing
		checkbox.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				commit();
				resetButton.setEnabled(true);
			}
		});
		
		// commit on losing focus, etc.
		addFocusListenerTo(checkbox);
		
		addFocusTransfer(label, checkbox);
		addFocusTransfer(this, checkbox);
	}

	@Override
	public void reread() {
		String value = getValueFromFile(section);
		if (value==null) {
			boolean defaultValue = entry.getDefaultValue()==null ? false : (Boolean)entry.getDefaultValue(); 
			checkbox.setSelection(defaultValue);
			resetButton.setEnabled(false);
		}
		else {
			checkbox.setSelection(InifileUtils.parseAsBool(value));
			resetButton.setEnabled(true);
		}

		// update problem decoration
		IMarker[] markers = InifileUtils.getProblemMarkersFor(section, entry.getKey(), inifile);
		//problemDecoration.setImage(checkbox.getSelection() ? ICON_ERROR_SMALL : null); //XXX for testing
		//problemDecoration.setDescriptionText(checkbox.getSelection() ? "Some error" : null); // ditto
		problemDecoration.setImage(getProblemImage(markers, true));
		problemDecoration.setDescriptionText(getProblemsText(markers));
		redraw();
	}

	@Override
	public void commit() {
		boolean value = checkbox.getSelection();
		setValueInFile(section, value ? "true" : "false");
	}
}
