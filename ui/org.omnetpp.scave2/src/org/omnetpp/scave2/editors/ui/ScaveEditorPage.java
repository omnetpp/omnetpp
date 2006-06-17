package org.omnetpp.scave2.editors.ui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * Common functionality of Scave multi-page editor pages.
 * @author andras
 */
public class ScaveEditorPage extends ScrolledForm {

	protected ScaveEditor scaveEditor = null;  // backreference to the containing editor
	private String pageTitle = "untitled";
	
	public ScaveEditorPage(Composite parent, int style, ScaveEditor scaveEditor) {
		super(parent, style);
		this.scaveEditor = scaveEditor;
	}

	public void setFormTitle(String title) {
		setFont(new Font(null, "Arial", 12, SWT.BOLD));
		setForeground(new Color(null, 0, 128, 255));
		setText(title);
	}

	/**
	 * Return the text that should appear on the page's tab.
	 */
	public String getPageTitle() {
		return pageTitle;
	}

	/**
	 * Sets the text that should appear on the page's tab.
	 */
	public void setPageTitle(String pageTitle) {
		this.pageTitle = pageTitle;
	}
}
