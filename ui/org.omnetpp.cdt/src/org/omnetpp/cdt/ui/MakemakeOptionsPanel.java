package org.omnetpp.cdt.ui;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.cdt.makefile.MakemakeOptions;
import org.omnetpp.common.ui.EditableList;
import org.omnetpp.common.util.StringUtils;


/**
 *XXX 
 * @author Andras
 */
public class MakemakeOptionsPanel extends Composite {
	// the result
    private MakemakeOptions result;

    private TabFolder tabfolder;

    public MakemakeOptionsPanel(Composite parent,int style, MakemakeOptions initialValue) {
        super(parent, style);
        result = initialValue == null ? new MakemakeOptions() : initialValue.clone();
        createContents();
    }

    protected Control createContents() {
        Composite composite = this;
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        composite.setLayout(new GridLayout(1,false));

        tabfolder = new TabFolder(composite, SWT.TOP);
        tabfolder.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        // create pages
        Composite generalPage = createTabPage("General");
        Composite scopePage = createTabPage("Makefile Scope");
        Composite targetPage = createTabPage("Target");
        Composite includePage = createTabPage("Include");
        Composite linkPage = createTabPage("Link");
        Composite customPage = createTabPage("Custom");

        tabfolder.setSelection(0);
        
        // "General" page
        generalPage.setLayout(new GridLayout(1,false));
        createLabel(generalPage, "Output directory (project relative) (when empty, build artifacts are created in the source directory):");
        Text outputDirText = new Text(generalPage, SWT.BORDER);
        outputDirText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        // "Scope" page
        scopePage.setLayout(new GridLayout(1,false));
        Group group1 = new Group(scopePage, SWT.NONE);
        group1.setText("Select one:");
        group1.setLayout(new GridLayout(1,false));
        group1.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        Button deepRadioButton = createRadioButton(group1, "Deep (process all source files from this subdirectory tree) (RECOMMENDED)");
        Button recurseRadioButton = createRadioButton(group1, "Recursive (process source files in this directory only, and invoke \"make\" in all subdirectories; Makefiles must exist)");
        Button nonrecurseRadioButton = createRadioButton(group1, "Local (process source files in this directory only, and ignore subdirectories)");
        createLabel(group1, "Makefiles will ignore directories marked as \"Excluded\"");
        //createLabel(generalPage, "Ignore the following directories (in addition to directories marked as \"Excluded\") (applies to Deep and Recursive):");
        //EditableList excludedDirsList = new EditableList(generalPage, SWT.BORDER);
        //excludedDirsList.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        createLabel(scopePage, "Additionally, invoke \"make\" in the following directories:");
        EditableList subdirsDirsList = new EditableList(scopePage, SWT.BORDER);
        subdirsDirsList.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        
        // "Target" page
        targetPage.setLayout(new GridLayout(1,false));
        Group group = new Group(targetPage, SWT.NONE);
        group.setText("Target type:");
        group.setLayout(new GridLayout(1,false));
        group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        Button targetExecutableRadioButton = createRadioButton(group, "Executable");
        Button targetSharedLibRadioButton = createRadioButton(group, "Shared library (.dll or .so)");
        Button targetStaticLibRadioButton = createRadioButton(group, "Static library (.lib or .a)");
        Button targetCompileOnlyRadioButton = createRadioButton(group, "Compile only");

        Group targetNameGroup = new Group(targetPage, SWT.NONE);
        targetNameGroup.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        targetNameGroup.setLayout(new GridLayout(2,false));
        targetNameGroup.setText("Target name:");
        Button defaultTargetName = createRadioButton(targetNameGroup, "Default");
        defaultTargetName.setLayoutData(new GridData());
        defaultTargetName.setToolTipText("Default target name will be derived from the directory name");
        ((GridData)defaultTargetName.getLayoutData()).horizontalSpan = 2;
        Button specifyTargetNameRadioButton = createRadioButton(targetNameGroup, "Specify name or relative path: ");
        Text targetNameText = new Text(targetNameGroup, SWT.BORDER);
        targetNameText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        
        // "Include" page
        includePage.setLayout(new GridLayout(1,false));
        Button autoCheckbox = createCheckbox(includePage, "Automatic include path, inferred from #include lines");
        autoCheckbox.setToolTipText(StringUtils.breakLines("Automatically add directories where #included files are located. Only workspace locations (open projects marked as \"referenced project\") are considered.", 60));

        createLabel(includePage, "Additional include directories:");
        EditableList includeList = new EditableList(includePage, SWT.BORDER);
        includeList.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        includeList.setAddDialogTitle("Add Include Directory");
        includeList.setAddDialogMessage("Enter include directory:");  //XXX workspace path? fs path?
        
        // "Link" page
        linkPage.setLayout(new GridLayout(1,false));
        Group linkGroup = createGroup(linkPage, "Link additionally with:");
        linkGroup.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        linkGroup.setLayout(new GridLayout(1,false));
        Button cb1 = createCheckbox(linkGroup, "All object files in this project"); //XXX radiobutton?
        Button cb2 = createCheckbox(linkGroup, "All object files in this project, except in folders with custom Makefiles");
        Button cb3 = createCheckbox(linkGroup, "Code from referenced projects");
        createLabel(linkPage, "Extra object files and libs to link with (wildcards allowed):");
        EditableList linkWith = new EditableList(linkPage, SWT.BORDER);
        linkWith.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        
        // "Custom" page
        customPage.setLayout(new GridLayout(1,false));
        createLabel(customPage, "Extra makemake options:");
        Text extraMakemakeOptionsText = new Text(customPage, SWT.BORDER);
        extraMakemakeOptionsText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        createLabel(customPage, "Code fragment to be inserted into the Makefile:");
        Text makefragText = new Text(customPage, SWT.MULTI | SWT.BORDER);
        makefragText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        
        Dialog.applyDialogFont(composite);
        
        return composite;
    }

	protected Label createLabel(Composite composite, String text) {
		Label label = new Label(composite, SWT.NONE);
		label.setText(text);
		return label;
	}

	protected Group createGroup(Composite composite, String text) {
	    Group group = new Group(composite, SWT.NONE);
	    group.setText(text);
        return group;
    }

	protected Button createCheckbox(Composite parent, String text) {
	    Button button = new Button(parent, SWT.CHECK);
	    button.setText(text);
	    return button;
	}

	protected Button createRadioButton(Composite parent, String text) {
	    Button button = new Button(parent, SWT.RADIO);
	    button.setText(text);
	    return button;
	}

	
	protected Composite createTabPage(String text) {
	    TabItem item = new TabItem(tabfolder, SWT.NONE);
	    item.setText(text);
	    Composite composite = new Composite(tabfolder, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
	    item.setControl(composite);
	    return composite;
	}
	
//	public void setEnabled(boolean enabled) {
//	    for (TabItem item : tabfolder.getItems())
//	        setEnabledRecursive(item.getControl(), enabled);
//	}
//	
//	private static void setEnabledRecursive(Control control, boolean enabled) {
//	    control.setEnabled(enabled);
//	    if (control instanceof Composite)
//	        for (Control child : ((Composite)control).getChildren())
//	            setEnabledRecursive(child, enabled);
//    }

    /**
	 * Returns the result of the dialog.
	 */
	public MakemakeOptions getResult() {
		return result;
	}
}
