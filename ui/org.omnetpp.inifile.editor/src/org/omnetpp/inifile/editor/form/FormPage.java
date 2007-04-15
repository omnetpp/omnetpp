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
import org.omnetpp.inifile.editor.editors.InifileEditorData;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;

/**
 * Base class for inifile form editor pages.
 *
 * @author Andras
 */
public abstract class FormPage extends Composite {
	public static final int RIGHT_MARGIN = 30; // >= scrollbar width  
	private static Font titleFont = new Font(null, "Arial", 10, SWT.BOLD);
	private InifileEditor inifileEditor;
	
	public FormPage(Composite parent, InifileEditor inifileEditor) {
		super(parent, SWT.NONE);
		this.inifileEditor = inifileEditor;
	}

	protected InifileEditorData getEditorData() {
		return inifileEditor.getEditorData();
	}

	protected IInifileDocument getInifileDocument() {
		return inifileEditor.getEditorData().getInifileDocument();
	}

	protected InifileAnalyzer getInifileAnalyzer() {
		return inifileEditor.getEditorData().getInifileAnalyzer();
	}
	
	/**
	 * String that identifies the form page, and also gets displayed 
	 * in the form editor's tree.
	 */
	public abstract String getPageCategory();
	
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
	protected Composite createTitleArea(Composite parent, String category) {
		Composite titleArea = new Composite(parent, SWT.BORDER);
		GridData gridData = new GridData(SWT.FILL, SWT.BEGINNING, true, false);
		gridData.heightHint = 40;
		titleArea.setLayoutData(gridData);
		titleArea.setBackground(ColorFactory.asColor("white"));
		titleArea.setLayout(new GridLayout(3, false));
		
		Label imageLabel = new Label(titleArea, SWT.NONE);
		imageLabel.setImage(ImageFactory.getImage(ImageFactory.MODEL_IMAGE_FOLDER)); //XXX 
		imageLabel.setBackground(ColorFactory.asColor("white"));
		
		Label title = new Label(titleArea, SWT.NONE);
		title.setText(category);
		title.setFont(titleFont);
		title.setBackground(ColorFactory.asColor("white"));
		title.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, true, true));
		return titleArea;
	}

	/**
	 * Position the cursor on the given section (as far as possible or makes sense)
	 */
	public void gotoSection(String section) {
		// do nothing by default (subclasses are advised to override)
	}

	/**
	 * Position the cursor on the given entry (as far as possible or makes sense)
	 */
	public void gotoEntry(String section, String key) {
		// do nothing by default (subclasses are advised to override)
	}

}
