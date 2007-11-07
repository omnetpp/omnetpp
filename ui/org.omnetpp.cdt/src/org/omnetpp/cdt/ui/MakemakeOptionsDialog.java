package org.omnetpp.cdt.ui;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.cdt.makefile.MakemakeOptions;


/**
 *XXX 
 * @author Andras
 */
public class MakemakeOptionsDialog extends TitleAreaDialog {
	private String title;

	// the result
    private MakemakeOptions result;

    /**
     * Creates the dialog.
     */
    public MakemakeOptionsDialog(Shell parentShell, MakemakeOptions initialValue) {
        super(parentShell);
        setShellStyle(getShellStyle() | SWT.MAX | SWT.RESIZE);
        this.title = "Makemake Options";
    }

	protected void configureShell(Shell shell) {
		super.configureShell(shell);
		if (title != null)
			shell.setText(title);
	}

    /* (non-Javadoc)
     * Method declared on Dialog.
     */
    protected Control createDialogArea(Composite parent) {
        setTitle("Makemake Options");
        setMessage("Options for makefile creation");
    	
        // page group
        Composite dialogArea = (Composite) super.createDialogArea(parent);

        Composite composite = new Composite(dialogArea, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        composite.setLayout(new GridLayout(1,false));

//      enum Type {EXE, SO, NOLINK};
//      public List<String> args;
//      public String makefile = "Makefile.vc";
        
//      public String baseDir = ""; ==> inputfield with browse button (BASEDIR IS THE PROJECT!!!)
        
//      public Type type = Type.EXE;

//EXE
//SO
//LIB
//NOLINK
//ALL
        
//      public String target = null;   EXE, SO, LIB
//      public boolean linkWithObjects = false;  EXE
//      public boolean tstamp = true;  EXE, SO?
//      public boolean recursive = false; ALL
//      public String userInterface;  EXE
//      public String ccext = null;  ALL
//      public String configFile     ALL
//      public String exportDefOpt   ALL
//      public boolean compileForDll  NOLINK
//      public List<String> fragmentFiles   ALL, inputfield
//      public List<String> subdirs         ALL, inputfield
//      public List<String> exceptSubdirs   ALL, inputfield  -- ignore?
//      public List<String> includeDirs     ALL, listbox
//      public List<String> libDirs =       EXE, SO?
//      public List<String> libs =          EXE, SO?
//      public List<String> importLibs      EXE, SO?
//      public List<String> extraArgs       EXE, SO?

        
        
        
//		// Section combobox
//		Composite comboWithLabel = new Composite(composite, SWT.NONE);
//		comboWithLabel.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
//
//		Label comboLabel = new Label(comboWithLabel, SWT.NONE);
//		comboLabel.setText("Section:");
//		sectionsCombo = new Combo(comboWithLabel, SWT.BORDER | SWT.READ_ONLY);
//		comboWithLabel.setLayout(new GridLayout(2, false));
//		comboLabel.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
//		sectionsCombo.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
//
//		networkNameLabel = createLabel(composite, "Network: n/a");
//		sectionChainLabel = createLabel(composite, "Section fallback chain: n/a  ");
//
//		sectionsCombo.addSelectionListener(new SelectionAdapter() {
//			public void widgetSelected(SelectionEvent e) {
//				buildTableContents();
//			}
//		});
//		
//		// radiobutton group
//        Group group = new Group(composite, SWT.NONE);
//		group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
//		group.setText("Pattern style");
//		group.setLayout(new GridLayout(1, false));
//
//        // radiobuttons
//		createRadioButton(group, "Parameter name only (**.queueSize)", KeyType.PARAM_ONLY);
//		Button b = createRadioButton(group, "Module and parameter only (**.mac.queueSize)", KeyType.MODULE_AND_PARAM);
//		createRadioButton(group, "Full path except network name (*.host[*].mac.queueSize)", KeyType.ANYNETWORK_FULLPATH);
//		createRadioButton(group, "Full path (Network.host[*].mac.queueSize)", KeyType.FULLPATH);
//		b.setSelection(true);
//		keyType = KeyType.MODULE_AND_PARAM; // must agree with selected radiobutton
//        
//		// table group
//        Group group2 = new Group(composite, SWT.NONE);
//		group2.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
//		group2.setText("Select keys to insert");
//		group2.setLayout(new GridLayout(1, false));
//      
//		// checkboxes
//		skipCheckbox = new Button(group2, SWT.CHECK);
//		skipCheckbox.setText("Skip parameters that have a default value");
//		addApplyCheckbox = new Button(group2, SWT.CHECK);
//		addApplyCheckbox.setText("Insert corresponding \"**.apply-default=true\" line into the file");
//		skipCheckbox.addSelectionListener(new SelectionAdapter() {
//			public void widgetSelected(SelectionEvent e) {
//				addApplyCheckbox.setSelection(skipCheckbox.getSelection());
//				buildTableContents();
//			}
//		});
//		GridData gridData = new GridData();
//		gridData.horizontalIndent = 20;
//		addApplyCheckbox.setLayoutData(gridData);
//		
//        // table and buttons
//		listViewer = CheckboxTableViewer.newCheckList(group2, SWT.BORDER);
//        GridData data = new GridData(GridData.FILL_BOTH);
//        data.heightHint = SIZING_SELECTION_WIDGET_HEIGHT;
//        data.widthHint = SIZING_SELECTION_WIDGET_WIDTH;
//        listViewer.getTable().setLayoutData(data);
//
//        listViewer.setContentProvider(new ArrayContentProvider());
//        listViewer.setLabelProvider(new LabelProvider() {
//			@Override
//			public String getText(Object element) {
//				ParamResolution res = (ParamResolution) element;
//				return getKeyFor(res);
//			}
//        });
//
//        addSelectionButtons(group2);
//
//        buildTableContents();
        
        Dialog.applyDialogFont(composite);
        
        return composite;
    }

	protected Label createLabel(Composite composite, String text) {
		Label label = new Label(composite, SWT.NONE);
		label.setLayoutData(new GridData(SWT.END, SWT.BEGINNING, true, false));
		label.setText(text);
		return label;
	}

//    protected Button createRadioButton(Group group, String label, final KeyType value) {
//		Button rb = new Button(group, SWT.RADIO);
//		rb.setText(label);
//		rb.addSelectionListener(new SelectionAdapter() {
//			public void widgetSelected(SelectionEvent e) {
//				if (keyType != value) { 
//					keyType = value;
//					buildTableContents();
//				}
//			}
//		});
//		return rb;
//	}
    
	protected void createButtonsForButtonBar(Composite parent) {
		createButton(parent, IDialogConstants.OK_ID, IDialogConstants.OK_LABEL, true);
		createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);
	}
	
	protected void okPressed() {
        //TODO
        super.okPressed();
    }
	
	/**
	 * Returns the result of the dialog.
	 */
	public MakemakeOptions getResult() {
		return result;
	}
}
