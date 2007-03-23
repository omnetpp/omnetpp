package org.omnetpp.inifile.editor.editors;

import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * Base class for inifile field editors
 * @author Andras
 */
public abstract class FieldEditor extends Composite {
	public static final Color BGCOLOR = InifileEditorFormPage.BGCOLOR;

	protected ConfigurationEntry entry;
	protected IInifileDocument inifile;

	public FieldEditor(Composite parent, int style, ConfigurationEntry entry, IInifileDocument inifile) {
		super(parent, style);
		this.entry = entry;
		this.inifile = inifile;
		setBackground(BGCOLOR);
	}

	protected String getAsString() {
		String section = entry.getSection(); 
		String key = entry.getName();
		return inifile.getValue(section, key);
	}

	protected void setAsString(String value) {
		String section = entry.getSection(); 
		String key = entry.getName();
		inifile.setValue(section, key, value);
	}

	protected void resetValue() {
		String section = entry.getSection(); 
		String key = entry.getName();
		inifile.removeKey(section, key);
	}
	
	public abstract void reread();
	
	public abstract void commit();
	
	public abstract void resetToDefault();
	
}
