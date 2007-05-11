package org.omnetpp.inifile.editor.form;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.ITooltipTextProvider;
import org.omnetpp.common.ui.TooltipSupport;
import org.omnetpp.common.util.DelayedJob;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.editors.InifileEditorData;
import org.omnetpp.inifile.editor.model.IInifileChangeListener;
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
	private TooltipSupport tooltipSupport = new TooltipSupport();
	
	// IMPLEMENTATION NOTE: HOW TO KEEP FORM PAGES UP-TO-DATE.
	//
	// Form pages should keep themselves up-to-date (e.g. by invoking their 
	// reread() method) whenever they directly change the document. For changes 
	// caused by other events (e.g. toolbar actions like AddMissingKeys),
	// there is an InifileListener which rereads the form after 1000ms
	// if that wasn't done until that time automatically.
	//
	// See fields "delayedRereadListener" and "delayedRereadJob" which participate 
	// in this mechanism.
	//
	private IInifileChangeListener delayedRereadListener = new IInifileChangeListener() {
		public void modelChanged() {
			// we only need to schedule an update if the form editor is displayed;
			// if text editor is displayed, switching to form mode will re-read
			// form contents anyway.
			if (inifileEditor.isFormPageDisplayed())
				delayedRereadJob.restartTimer();
		}
	};

	private DelayedJob delayedRereadJob = new DelayedJob(1000) {
		public void run() {
			reread();  //XXX TODO: reread() should cancel the timer!!!
		}
	};
	
	public FormPage(Composite parent, InifileEditor inifileEditor) {
		super(parent, SWT.NONE);
		this.inifileEditor = inifileEditor;
		getInifileDocument().addInifileChangeListener(delayedRereadListener);
	}

	@Override
	public void dispose() {
		getInifileDocument().removeInifileChangeListener(delayedRereadListener);
		super.dispose();
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

	// used by field editors as well
	public void setEditorSelection(String section, String key) {
		inifileEditor.setSelection(section, key);
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
	 * Set the focus to an appropriate control in the form page. 
	 */
	public abstract boolean setFocus();
	
	/**
	 * Utility method for subclasses: add a title to the form
	 */
	protected Composite createTitleArea(Composite parent, String category) {
		Composite titleArea = new Composite(parent, SWT.BORDER);
		GridData gridData = new GridData(SWT.FILL, SWT.BEGINNING, true, false);
		gridData.heightHint = 44;
		titleArea.setLayoutData(gridData);
		titleArea.setBackground(ColorFactory.asColor("white"));
		titleArea.setLayout(new GridLayout(3, false));
		((GridLayout)titleArea.getLayout()).horizontalSpacing = 12;
		((GridLayout)titleArea.getLayout()).marginWidth = 10;
		
		Label imageLabel = new Label(titleArea, SWT.NONE);
		imageLabel.setImage(InifileEditorPlugin.getImage("icons/formpage_banner.png")); //XXX better image needed!!!
		imageLabel.setBackground(ColorFactory.asColor("white"));
		
		Label title = new Label(titleArea, SWT.NONE);
		title.setText(category);
		title.setFont(titleFont);
		title.setBackground(ColorFactory.asColor("white"));
		title.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, true, true));
		return titleArea;
	}

	/**
	 * Add tooltip (hover) support to the given control. Call this instead of instantiating
	 * TooltipSupport for each control or field editor, otherwise "F2 to focus" will only
	 * work when hovering the control that owns the focus.
	 */
	protected void addTooltipSupport(Control control, ITooltipTextProvider tooltipTextProvider) {
		tooltipSupport.adapt(control, tooltipTextProvider);
	}
	
	/**
	 * Position the cursor on the given section inside the form page (as far as possible or makes sense)
	 */
	public void gotoSection(String section) {
		// do nothing by default (subclasses are advised to override)
	}

	/**
	 * Position the cursor on the given entry inside the form page (as far as possible or makes sense)
	 */
	public void gotoEntry(String section, String key) {
		// do nothing by default (subclasses are advised to override)
	}

	protected void showErrorDialog(RuntimeException e) {
		reread(); // restore "legal" form contents
		MessageDialog.openError(getShell(), "Error", e.getMessage()+".");
	}

}
