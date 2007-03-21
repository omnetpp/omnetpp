package org.omnetpp.inifile.editor.editors;

import org.eclipse.jface.window.ApplicationWindow;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.forms.widgets.ScrolledForm;

/**
 * Common functionality for form-based pages of the inifile multi-page editor.
 * @author andras
 */
public class InifileEditorFormPage extends ScrolledForm {
	protected InifileEditor inifileEditor = null;  // backreference to the containing editor
	
	public InifileEditorFormPage(Composite parent, int style, InifileEditor inifileEditor) {
		super(parent, style);
		this.inifileEditor = inifileEditor;
	}

	public void setFormTitle(String title) {
		setFont(new Font(null, "Arial", 12, SWT.BOLD));
		setForeground(new Color(null, 0, 128, 255));
		setText(title);
	}
	
	public void showStatusMessage(String message) {
		IWorkbenchWindow window = inifileEditor.getSite().getWorkbenchWindow();
		if (window instanceof ApplicationWindow)
			((ApplicationWindow)window).setStatus(message);
	}
	
	/**
	 * Notification about the selection of the page of the
	 * multipage editor.
	 */
	public void pageSelected() {
	}

}
