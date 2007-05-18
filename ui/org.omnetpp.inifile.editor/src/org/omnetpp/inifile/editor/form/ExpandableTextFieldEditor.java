package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.GENERAL;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.IInifileChangeListener;
import org.omnetpp.inifile.editor.model.IInifileDocument;

/**
 * An inifile field editor, which displays either a TextFieldEditor or a TableFieldEditor,
 * and lets the user toggle between them. 
 * 
 * @author Andras
 */
public class ExpandableTextFieldEditor extends FieldEditor  {
	private static Image IMAGE_EXPAND = InifileEditorPlugin.getCachedImage("icons/full/elcl16/expand.png");
	private static Image IMAGE_COLLAPSE = InifileEditorPlugin.getCachedImage("icons/full/elcl16/collapse.png");
	
	private String labelText;
	private FieldEditor fieldEditor;
	private boolean isExpanded;
	private ToolItem toggleButton;
	
	private IInifileChangeListener inifileChangeListener = new IInifileChangeListener() {
		public void modelChanged() {
			updateToggleButton();
		}
	};

	public ExpandableTextFieldEditor(Composite parent, ConfigurationEntry entry, IInifileDocument inifile, FormPage formPage, String labelText) {
		super(parent, SWT.NONE, entry, inifile, formPage);
		this.labelText = labelText;
		GridLayout gridLayout = new GridLayout(2, false);
		gridLayout.marginHeight = gridLayout.marginWidth = 0;
		setLayout(gridLayout);

		isExpanded = (!entry.isGlobal() && occursOutsideGeneral(inifile, entry.getKey()));
		createControl();
		
		// need to update the toggle button's state on inifile changes
		inifile.addInifileChangeListener(inifileChangeListener);
		addDisposeListener(new DisposeListener() {
			public void widgetDisposed(DisposeEvent e) {
				ExpandableTextFieldEditor.this.inifile.removeInifileChangeListener(inifileChangeListener);
			}
		});
	}

	protected void recreateControl() {
		for (Control c : getChildren())
			c.dispose();
		createControl();
		formPage.layoutForm();
	}

	protected void createControl() {
		fieldEditor = isExpanded ? 
				new TextTableFieldEditor(this, entry, inifile, formPage, labelText) :
				new TextFieldEditor(this, entry, inifile, formPage, labelText);
		fieldEditor.setLayoutData(new GridData(SWT.CENTER, SWT.CENTER, false, false));
				
		//XXX toggleButton = new Button(this, SWT.FLAT);
		//toggleButton.setLayoutData(new GridData(SWT.CENTER, SWT.BEGINNING, false, false));
		toggleButton = createFlatImageButton(this);
		toggleButton.setImage(isExpanded ? IMAGE_COLLAPSE : IMAGE_EXPAND);
		toggleButton.setToolTipText(isExpanded ? "Collapse": "Expand");
		//toggleButton.setToolTipText(isExpanded ? "": "Specify per-configuration values");
		toggleButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				isExpanded = !isExpanded;
				recreateControl();
			}
		});
		updateToggleButton();
	}

	protected static ToolItem createFlatImageButton(Composite parent) {
        // code from TrayDialog.createHelpImageButton()
		ToolBar toolBar = new ToolBar(parent, SWT.FLAT | SWT.NO_FOCUS);
		toolBar.setLayoutData(new GridData(SWT.CENTER, SWT.BEGINNING, false, false));
		final Cursor cursor = new Cursor(parent.getDisplay(), SWT.CURSOR_HAND);
		toolBar.setCursor(cursor);
		toolBar.addDisposeListener(new DisposeListener() {
			public void widgetDisposed(DisposeEvent e) {
				cursor.dispose();
			}
		});		
        ToolItem item = new ToolItem(toolBar, SWT.NONE);
        return item;
	}
	
	protected static boolean occursOutsideGeneral(IInifileDocument doc, String key) {
		for (String section : doc.getSectionNames())
			if (doc.containsKey(section, key) && !section.equals(GENERAL))
				return true;
		return false;
	}
	
	@Override
	public void commit() {
		fieldEditor.commit();
	}

	@Override
	public void reread() {
		fieldEditor.reread();
	}

	protected void updateToggleButton() {
		// need to update the toggle button's state when the last per-section value is removed
		boolean shouldBeEnabled = !isExpanded || !occursOutsideGeneral(inifile, entry.getKey());
		if (toggleButton.getEnabled() != shouldBeEnabled)
			toggleButton.setEnabled(shouldBeEnabled);
	}

}
