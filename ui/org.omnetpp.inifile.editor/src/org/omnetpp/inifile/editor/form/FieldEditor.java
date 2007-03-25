package org.omnetpp.inifile.editor.form;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * Base class for inifile field editors
 * @author Andras
 */
public abstract class FieldEditor extends Composite {
	public static final Color BGCOLOR = InifileFormEditor.BGCOLOR;

	protected ConfigurationEntry entry;
	protected IInifileDocument inifile;

	public FieldEditor(Composite parent, int style, ConfigurationEntry entry, IInifileDocument inifile) {
		super(parent, style);
		this.entry = entry;
		this.inifile = inifile;
		setBackground(BGCOLOR);
	}

	protected String getValueFromFile() {
		String section = entry.getSection(); 
		String key = entry.getName();
		return inifile.getValue(section, key);
	}

	protected void setValueInFile(String value) {
		String section = entry.getSection(); 
		String key = entry.getName();
		if (inifile.getValue(section, key)==null)
			inifile.addEntry(section, key, value, null, null); //XXX there's more: if section doesn't exist, it has to be added first!
		else
			inifile.setValue(section, key, value);
	}

	protected void removeFromFile() {
		String section = entry.getSection(); 
		String key = entry.getName();
		inifile.removeKey(section, key);  //XXX remove section as well, if it became empty?
	}
	
	protected Label createLabel(ConfigurationEntry entry, String labelText) {
		Label label = new Label(this, SWT.NONE);
		label.setBackground(BGCOLOR);
		label.setText(labelText);
		
		String tooltip = entry.getDescription();
		tooltip += "\n\nConfigures: ["+entry.getSection()+(entry.isGlobal() ? "" : "] or [Run X")+"] / "+entry.getName()+"=...";
		IInifileDocument.LineInfo line = inifile.getEntryLineDetails(entry.getSection(), entry.getName()); 
		tooltip += "\n\n"+(line==null ? "Currently set to default." : "Defined at: "+line.getFile().getFullPath().toString()+" line "+line.getLineNumber());
		label.setToolTipText(StringUtils.breakLines(tooltip, 80));  //XXX we'll need to refresh tooltip after each re-parse!
		return label;
	}

	protected Button createResetButton() {
		Button resetButton = new Button(this, SWT.PUSH);
		resetButton.setText("Reset");
		resetButton.setToolTipText("Remove corresponding entry from ini file");
		resetButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				resetToDefault();
			}
		});
		return resetButton;
	}

	public abstract void reread();
	
	public abstract void commit();
	
	public void resetToDefault() {
		removeFromFile();
		reread();
	}
	
}
