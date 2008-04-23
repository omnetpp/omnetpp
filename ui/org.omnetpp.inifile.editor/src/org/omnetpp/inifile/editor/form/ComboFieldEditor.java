package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;

import java.util.List;

import org.eclipse.core.resources.IMarker;
import org.eclipse.jface.fieldassist.ControlDecoration;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.inifile.editor.model.ConfigKey;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.ConfigKey.DataType;

/**
 * Combobox-based editor for inifile entries.
 *
 * NOTE: This class edits the [General] section ONLY. All other sections
 * are ignored. For example, the Reset button only removes the setting
 * from the [General] section. When a setting is present outside
 * [General], the table-based field editor has to be used.
 *
 * @author Andras
 */
//FIXME this is unfinished code.
//XXX disable Combo when value is not editable (comes from included file)?
//XXX set up content assist (like in TextFieldEditor)
public class ComboFieldEditor extends FieldEditor {
	private Combo combo;
	private Label label;
	private Button resetButton;
	private boolean isEdited;
	private ControlDecoration problemDecoration;

	public ComboFieldEditor(Composite parent, ConfigKey entry, IInifileDocument inifile, FormPage formPage, String labelText) {
		super(parent, SWT.NONE, entry, inifile, formPage);

		// layout
		GridLayout gridLayout = new GridLayout(3, false);
		gridLayout.marginHeight = 2;
		setLayout(gridLayout);

		// child widgets
		label = createLabel(entry, labelText+":");
		combo = createCombo();
		addTooltipSupport(combo);
		resetButton = createResetButton();

		problemDecoration = new ControlDecoration(combo, SWT.LEFT | SWT.TOP);
		problemDecoration.setShowOnlyOnFocus(false);

		// set layout data
		label.setLayoutData(new GridData());
		int width = (entry.getDataType()==DataType.CFG_INT || entry.getDataType()==DataType.CFG_DOUBLE) ? 80 : 250;
		combo.setLayoutData(new GridData(width, SWT.DEFAULT));
		((GridData)combo.getLayoutData()).horizontalIndent = 3; // room for the decoration
		resetButton.setLayoutData(new GridData());

		reread();

		// enable Reset button on editing
		combo.addModifyListener(new ModifyListener() {
			public void modifyText(ModifyEvent e) {
				if (!isEdited) {
					isEdited = true;
					combo.setForeground(null);
					resetButton.setEnabled(true);
				}
			}
		});
		// commit the editfield on Enter
		combo.addSelectionListener(new SelectionAdapter() {
			public void widgetDefaultSelected(SelectionEvent e) {
				commit();
			}
		});

		// commit on losing focus
		addFocusListenerTo(combo);

		// when the background gets clicked, transfer focus to the text widget
		addFocusTransfer(label, combo);
		addFocusTransfer(this, combo);

	}

	protected Combo createCombo() {
		Combo combo = new Combo(this, SWT.SINGLE | SWT.BORDER);
		return combo;
	}

	@Override
	public void reread() {
		// update text and reset button
		String key = entry.isPerObject() ? "**."+entry.getKey() : entry.getKey();
		String value = getValueFromFile(GENERAL, key);
		if (value==null) {
			String defaultValue = entry.getDefaultValue()==null ? "" : entry.getDefaultValue().toString();
			combo.setText(defaultValue);
			//textField.setForeground(ColorFactory.asColor("darkGreen"));
			combo.setForeground(ColorFactory.GREY50);
			resetButton.setEnabled(false);
		}
		else {
			combo.setText(value);
			combo.setForeground(null);
			resetButton.setEnabled(true);
		}
		isEdited = false;

		// update problem decoration
		IMarker[] markers = InifileUtils.getProblemMarkersFor(GENERAL, key, inifile);
		problemDecoration.setImage(getProblemImage(markers, true));
		problemDecoration.setDescriptionText(getProblemsText(markers));
		redraw(); // otherwise an obsolete error decoration doesn't disappear
	}

	public void setComboContents(List<String> list) {
        String oldValue = combo.getText();
	    combo.removeAll();
	    for (String i : list)
	        combo.add(i);
	    combo.setText(oldValue);
	    reread();
	}

	@Override
	public void commit() {
		if (isEdited) {
			String key = entry.isPerObject() ? "**."+entry.getKey() : entry.getKey();
			String value = combo.getText();
			setValueInFile(GENERAL, key, value);
			isEdited = false;
		}
	}
}
