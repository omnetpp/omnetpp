package org.omnetpp.inifile.editor.editors;

import org.eclipse.swt.widgets.Composite;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * Base class for inifile field editors
 * @author Andras
 */
//FIXME finish...
public abstract class FieldEditor extends Composite {
	protected ConfigurationEntry entry;
	protected IInifileDocument inifile;

	public FieldEditor(Composite parent, int style, ConfigurationEntry entry, IInifileDocument inifile) {
		super(parent, style);
		this.entry = entry;
		this.inifile = inifile;
	}

	protected String getAsString() {
		String section = entry.getSection(); 
		String key = entry.getName();
		return inifile.getValue(section, key);
	}
}
