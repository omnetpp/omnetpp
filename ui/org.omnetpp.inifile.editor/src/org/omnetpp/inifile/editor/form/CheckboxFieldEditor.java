package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.GENERAL;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
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
	private Label problemDecorationLabel;
	private Button resetButton;
	private String section = GENERAL;

	public CheckboxFieldEditor(Composite parent, ConfigurationEntry entry, IInifileDocument inifile, FormPage formPage, String labelText) {
		super(parent, SWT.NONE, entry, inifile, formPage);

		Assert.isTrue(entry.getDataType()==ConfigurationEntry.DataType.CFG_BOOL);

		GridLayout gridLayout = new GridLayout(4, false); 
		gridLayout.marginTop = gridLayout.marginBottom = gridLayout.marginHeight = gridLayout.verticalSpacing = 0;
		gridLayout.marginHeight = 0;
		setLayout(gridLayout);
		
		checkbox = new Button(this, SWT.CHECK);
		checkbox.setBackground(BGCOLOR);
		tooltipSupport.adapt(checkbox);
		problemDecorationLabel = new Label(this, SWT.NONE);
		label = createLabel(entry, labelText);
		resetButton = createResetButton();

		checkbox.setLayoutData(new GridData());
		label.setLayoutData(new GridData());
		problemDecorationLabel.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false)); // align top
		((GridData)problemDecorationLabel.getLayoutData()).widthHint = 8; 
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
		//problemDecorationLabel.setImage(checkbox.getSelection() ? ICON_ERROR_SMALL : null); //XXX for testing
		problemDecorationLabel.setImage(getProblemImage(markers, true));
		
		problemDecorationLabel.setToolTipText(getProblemsText(markers));
		problemDecorationLabel.setVisible(problemDecorationLabel.getImage()!=null);
		
		// hide problemDecorationLabel when not visible
		GridData gridData = (GridData) problemDecorationLabel.getLayoutData();
		boolean oldExclude = gridData.exclude;
		gridData.exclude = !problemDecorationLabel.isVisible();
		if (gridData.exclude != oldExclude) 
			layout();
	}

	@Override
	public void commit() {
		boolean value = checkbox.getSelection();
		setValueInFile(section, value ? "true" : "false");
	}
}
