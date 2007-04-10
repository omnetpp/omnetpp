package org.omnetpp.inifile.editor.form;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;

public abstract class FormPage extends Composite {
	private InifileEditor inifileEditor;
	
	public FormPage(Composite parent, InifileEditor inifileEditor) {
		super(parent, SWT.NONE);
		this.inifileEditor = inifileEditor;
	}

	protected IInifileDocument getInifileDocument() {
		return inifileEditor.getEditorData().getInifileDocument();
	}

	protected InifileAnalyzer getInifileAnalyzer() {
		return inifileEditor.getEditorData().getInifileAnalyzer();
	}
	
	/**
	 * Reads data from the document into the current page.
	 */
	public abstract void reread();

	/**
	 * Writes the edits on this page back into the document.
	 */
	public abstract void commit();

}
