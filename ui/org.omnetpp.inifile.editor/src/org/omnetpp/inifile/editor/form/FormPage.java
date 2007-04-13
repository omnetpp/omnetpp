package org.omnetpp.inifile.editor.form;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;

public abstract class FormPage extends Composite {
	private static Font titleFont = new Font(null, "Arial", 10, SWT.BOLD);
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

	/**
	 * Utility method for subclasses: add a title to the form
	 */
	protected Composite createTitle(String text) {
		// title area (XXX a bit ugly -- re-think layout)
		Composite titleArea = new Composite(this, SWT.BORDER);
		GridData gridData = new GridData(SWT.FILL, SWT.BEGINNING, true, false);
		gridData.heightHint = 40;
		titleArea.setLayoutData(gridData);
		titleArea.setBackground(ColorFactory.asColor("white"));
		titleArea.setLayout(new GridLayout(3, false));
		
		Label imageLabel = new Label(titleArea, SWT.NONE);
		imageLabel.setImage(ImageFactory.getImage(ImageFactory.MODEL_IMAGE_FOLDER)); //XXX 
		imageLabel.setBackground(ColorFactory.asColor("white"));
		
		Label title = new Label(titleArea, SWT.NONE);
		title.setText(text);
		title.setFont(titleFont);
		title.setBackground(ColorFactory.asColor("white"));
		title.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, true, true));
		return titleArea;
	}

}
