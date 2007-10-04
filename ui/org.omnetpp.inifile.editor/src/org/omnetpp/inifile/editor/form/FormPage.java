package org.omnetpp.inifile.editor.form;

import java.util.List;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.util.DelayedJob;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.editors.InifileEditorData;
import org.omnetpp.inifile.editor.model.ConfigKey;
import org.omnetpp.inifile.editor.model.IInifileChangeListener;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;

/**
 * Base class for inifile form editor pages.
 * 
 * Note: no commit() method: all fields or controls on the page 
 *should auto-commit, latest when they lose the focus. 
 *
 * @author Andras
 */
public abstract class FormPage extends Composite {
	public static final int RIGHT_MARGIN = 20; // >= scrollbar width  
	private static Font titleFont = new Font(null, "Arial", 10, SWT.BOLD);
	private InifileEditor inifileEditor;
	private HoverSupport hoverSupport = new HoverSupport();
	
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
	protected IInifileChangeListener delayedRereadListener = new IInifileChangeListener() {
		public void modelChanged() {
			// we only need to schedule an update if the form editor is displayed;
			// if text editor is displayed, switching to form mode will re-read
			// form contents anyway.
			if (inifileEditor.isFormPageDisplayed())
				delayedRereadJob.restartTimer();
		}
	};

	protected DelayedJob delayedRereadJob = new DelayedJob(1000) {
		public void run() {
		    // reread if page is still displayed (and exists)
		    if (inifileEditor.isFormPageDisplayed() && !FormPage.this.isDisposed() && 
		            inifileEditor.getFormEditor().getActiveCategoryPage()==FormPage.this) {
		        System.out.println("delayedJob: rereading form page");
		        reread();
		    }
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
	 * FormPage's implementation does nothing except canceling the delayed-reread timer --
	 * subclasses must override and actually implement the rereading! 
	 */
	public void reread() {
		delayedRereadJob.cancel();
	}

	/**
	 * Set the focus to an appropriate control in the form page. 
	 */
	@Override
	public abstract boolean setFocus();

	/**
	 * Returns the list of keys that this page can edit.
	 *  
	 * This method is currently only used for code maintenance,
	 * to check that every registered config key appears on 
	 * some form page.
	 */
	public abstract List<ConfigKey> getSupportedKeys();

	/**
	 * Utility method for subclasses: add a title to the form
	 */
	protected Composite createTitleArea(Composite parent, String category) {
		Composite titleArea = new Composite(parent, SWT.BORDER);
		GridData gridData = new GridData(SWT.FILL, SWT.BEGINNING, true, false);
		gridData.heightHint = 44;
		titleArea.setLayoutData(gridData);
		titleArea.setBackground(ColorFactory.WHITE);
		titleArea.setLayout(new GridLayout(3, false));
		((GridLayout)titleArea.getLayout()).horizontalSpacing = 12;
		((GridLayout)titleArea.getLayout()).marginWidth = 10;
		
		Label imageLabel = new Label(titleArea, SWT.NONE);
		imageLabel.setImage(InifileEditorPlugin.getCachedImage("icons/full/misc/formpage_banner2.png")); //XXX better image needed!!!
		imageLabel.setBackground(ColorFactory.WHITE);
		
		Label title = new Label(titleArea, SWT.NONE);
		title.setText(category);
		title.setFont(titleFont);
		title.setBackground(ColorFactory.WHITE);
		title.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, true, true));
		return titleArea;
	}

	/**
	 * Should be called whenever for content changes. layout(true,true) is not enough
	 * because ScrolledFormPage has extra things to do here.  
	 */
	public void layoutForm() {
		layout(true);
	}
	
	/**
	 * Add tooltip (hover) support to the given control. Call this instead of instantiating
	 * HoverSupport for each control or field editor, otherwise "F2 to focus" will only
	 * work when hovering the control that owns the focus.
	 */
	protected void addTooltipSupport(Control control, IHoverTextProvider hoverTextProvider) {
		hoverSupport.adapt(control, hoverTextProvider);
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
