package org.omnetpp.cdt.ui;

import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.cdt.makefile.MakemakeOptions;
import org.omnetpp.cdt.makefile.MakemakeOptions.Type;
import org.omnetpp.common.ui.EditableList;
import org.omnetpp.common.util.StringUtils;


/**
 * UI for editing MakemakeOptions
 * @author Andras
 */
//XXX use CDT's FileListControl for file lists?
//TODO update enable/disable state on changes
//TODO preview page
public class MakemakeOptionsPanel extends Composite {
    // controls
    private TabFolder tabfolder;
    private Composite scopePage;
    private Composite targetPage;
    private Composite includePage;
    private Composite compilePage;
    private Composite linkPage;
    private Composite customPage;
    private Composite previewPage;

    // "General" page
    private Text outputDirText;

    // "Scope" page
    private Button deepMakefileRadioButton;
    private Button recursiveMakefileRadioButton;
    private Button localMakefileRadioButton;

    // "Target" page
    private EditableList subdirsDirsList;
    private Button targetExecutableRadioButton;
    private Button targetSharedLibRadioButton;
    private Button targetStaticLibRadioButton;
    private Button targetCompileOnlyRadioButton;
    private Button defaultTargetName;
    private Button specifyTargetNameRadioButton;
    private Text targetNameText;

    // "Include" page
    private Button autoIncludePathCheckbox;

    // "Compile" page
    private Combo ccextCombo;
    private Button compileForDllCheckbox;
    private Text exportDefText;

    // "Link" page
    private Combo userInterfaceCombo;
    private EditableList libsList;
    private EditableList linkObjectsList;

    // "Custom" page
    private Text makefragText;
    private EditableList makefragsList;

    
    public MakemakeOptionsPanel(Composite parent, int style) {
        super(parent, style);
        createContents();
    }

    protected Control createContents() {
        Composite composite = this;
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        composite.setLayout(new GridLayout(1,false));

        tabfolder = new TabFolder(composite, SWT.TOP);
        tabfolder.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        scopePage = createTabPage("Scope");
        targetPage = createTabPage("Target");
        includePage = createTabPage("Include");
        compilePage = createTabPage("Compile");
        linkPage = createTabPage("Link");
        customPage = createTabPage("Custom");
        previewPage = createTabPage("Preview");
        tabfolder.setSelection(0);

        // "Scope" page
        scopePage.setLayout(new GridLayout(1,false));
        Group group1 = createGroup(scopePage, "Select one:", 1);
        deepMakefileRadioButton = createRadioButton(group1, "Deep (recommended)", "Process all source files from this subdirectory tree");
        recursiveMakefileRadioButton = createRadioButton(group1, "Recursive", "Process source files in this directory only, and invoke \"make\" in all subdirectories; Makefiles must exist");
        localMakefileRadioButton = createRadioButton(group1, "Local", "Process source files in this directory only; ignore subdirectories");
        createLabel(group1, "Makefiles will ignore directories marked as \"Excluded\"");
        createLabel(scopePage, "Additionally, invoke \"make\" in the following directories:");
        subdirsDirsList = new EditableList(scopePage, SWT.NONE);
        subdirsDirsList.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        // "Target" page
        targetPage.setLayout(new GridLayout(1,false));
        Group group = createGroup(targetPage, "Target type:", 1);
        targetExecutableRadioButton = createRadioButton(group, "Executable", null);
        targetSharedLibRadioButton = createRadioButton(group, "Shared library (.dll or .so)", null);
        targetStaticLibRadioButton = createRadioButton(group, "Static library (.lib or .a)", null);
        targetCompileOnlyRadioButton = createRadioButton(group, "Compile only", null);
        Group targetNameGroup = createGroup(targetPage, "Target name:", 2);
        defaultTargetName = createRadioButton(targetNameGroup, "Default", "Default target name will be derived from the directory name");
        defaultTargetName.setLayoutData(new GridData());
        ((GridData)defaultTargetName.getLayoutData()).horizontalSpan = 2;
        specifyTargetNameRadioButton = createRadioButton(targetNameGroup, "Specify name or relative path: ", null);
        targetNameText = new Text(targetNameGroup, SWT.BORDER);
        targetNameText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        Group outGroup = createGroup(targetPage, "Output:", 2);
        String tooltip = "Project relative; when empty, build artifacts \nare created in the source directory";
        Label outputDirLabel = createLabel(outGroup, "Output directory:");
        outputDirLabel.setToolTipText(tooltip);
        outputDirText = new Text(outGroup, SWT.BORDER);
        outputDirText.setToolTipText(tooltip);
        outputDirText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        
        // "Include" page
        includePage.setLayout(new GridLayout(1,false));
        autoIncludePathCheckbox = createCheckbox(includePage, "Automatic include path, inferred from #include lines", null); //FIXME really? for deep, this should also enable that all source folders are include dirs as well. Or specify a different flag for that?
        autoIncludePathCheckbox.setToolTipText(StringUtils.breakLines("Automatically add directories where #included files are located. Only workspace locations (open projects marked as \"referenced project\") are considered.", 60));
        createLabel(includePage, "NOTE: Additional include directories can be specified in the C/C++ General -> Paths and symbols page.");

//        createLabel(includePage, "Additional include directories:");
//        includeDirsList = new EditableList(includePage, SWT.NONE);
//        includeDirsList.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
//        includeDirsList.setAddDialogTitle("Add Include Directory");
//        includeDirsList.setAddDialogMessage("Enter include directory:");

        // "Compile" page
        compilePage.setLayout(new GridLayout(1,false));
        Group srcGroup = createGroup(compilePage, "Sources", 2);
        createLabel(srcGroup, "C++ file extension:");
        ccextCombo = new Combo(srcGroup, SWT.BORDER | SWT.READ_ONLY);  //XXX new
        ccextCombo.add("autodetect");
        ccextCombo.add(".cc");
        ccextCombo.add(".cpp");
        
        Group dllGroup = createGroup(compilePage, "Windows DLLs", 2);
        compileForDllCheckbox = createCheckbox(dllGroup, "Compile object files for use in DLLs", "Defines WIN32_DLL as preprocessor symbol"); //XXX new
        compileForDllCheckbox.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false, 2, 1));
        Label exportDefLabel = createLabel(dllGroup, "DLL export symbol for msg files:");
        exportDefText = new Text(dllGroup, SWT.BORDER); //XXX needs to be validated! no spaces etc
        exportDefText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        String exportDefTooltip = 
            "Name of the macro (#define) which expands to __dllexport/__dllimport \n" +
            "when WIN32_DLL is defined. The message compiler needs to know it \n" +
            "in order to be able to add it to generated classes.";
        exportDefLabel.setToolTipText(exportDefTooltip);
        exportDefText.setToolTipText(exportDefTooltip);

        createLabel(compilePage, "NOTE: Additional preprocessor symbols can be specified in the C/C++ General -> Paths and symbols page.");

//        createLabel(compilePage, "Preprocessor symbols to define:");
//        definesList = new EditableList(compilePage, SWT.NONE); 
//        definesList.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
//        definesList.setAddDialogTitle("Add Preprocessor Symbol");
//        definesList.setAddDialogMessage("Enter symbol in NAME or NAME=value format:");
//        definesList.setInputValidator(new IInputValidator() {
//            public String isValid(String newText) {
//                String name = newText.contains("=") ? StringUtils.substringBefore(newText, "=") : newText;
//                String value = newText.contains("=") ? StringUtils.substringAfter(newText, "=") : null;
//                if (name.equals(""))
//                    return "Symbol name is empty";
//                if (!name.matches("(?i)[A-Z_][A-Z0-9_]*"))
//                    return "Invalid symbol name \""+ name +"\"";
//                if (value != null && (value.contains(" ") || value.contains("\t")))
//                    return "Value should not contain whitespace";
//                return null;
//            }
//        });

        // "Link" page
        linkPage.setLayout(new GridLayout(1,false));

        Group envGroup = createGroup(linkPage, "User interface:", 2);
        createLabel(envGroup, "User interface libraries to link with:");
        userInterfaceCombo = new Combo(envGroup, SWT.BORDER | SWT.READ_ONLY);  //XXX new
        userInterfaceCombo.add("All");
        userInterfaceCombo.add("Tkenv");
        userInterfaceCombo.add("Cmdenv");

        Group linkGroup = createGroup(linkPage, "Link additionally with:", 1);
//        //FIXME are these combo boxes needed? do they correspond to any makemake settings?
//        Button cb1 = createCheckbox(linkGroup, "All object files in this project", null); //XXX radiobutton?
//        Button cb2 = createCheckbox(linkGroup, "All object files in this project, except in folders with custom Makefiles", null);
//        Button cb3 = createCheckbox(linkGroup, "All objects from referenced projects", null); //XXX or static/dynamic libs?
        createLabel(linkGroup, "Libraries to link with (-l):");
        libsList = new EditableList(linkGroup, SWT.NONE);
        libsList.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        createLabel(linkGroup, "NOTE: Library paths can be specified in the C/C++ General -> Paths and symbols page.");
        createLabel(linkGroup, "Extra object files and libs to link with (wildcards allowed):");
        linkObjectsList = new EditableList(linkGroup, SWT.NONE);
        linkObjectsList.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        // "Custom" page
        customPage.setLayout(new GridLayout(1,false));
        createLabel(customPage, "Code fragment to be inserted into the Makefile (Makefrag):");
        makefragText = new Text(customPage, SWT.MULTI | SWT.BORDER);
        makefragText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        createLabel(customPage, "Other fragment files to include:");
        makefragsList = new EditableList(customPage, SWT.NONE);
        makefragsList.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        // "Preview" page
        //TODO readonly text for vanilla options and translated options
        
        Dialog.applyDialogFont(composite);

        return composite;
    }

    protected Label createLabel(Composite composite, String text) {
        Label label = new Label(composite, SWT.NONE);
        label.setText(text);
        return label;
    }

    protected Group createGroup(Composite composite, String text, int numColumns) {
        Group group = new Group(composite, SWT.NONE);
        group.setText(text);
        group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        group.setLayout(new GridLayout(numColumns,false));
        return group;
    }

    protected Button createCheckbox(Composite parent, String text, String tooltip) {
        Button button = new Button(parent, SWT.CHECK);
        button.setText(text);
        if (tooltip != null)
            button.setToolTipText(tooltip);
        return button;
    }

    protected Button createRadioButton(Composite parent, String text, String tooltip) {
        Button button = new Button(parent, SWT.RADIO);
        button.setText(text);
        if (tooltip != null)
            button.setToolTipText(tooltip);
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

//  public void setEnabled(boolean enabled) {
//  for (TabItem item : tabfolder.getItems())
//  setEnabledRecursive(item.getControl(), enabled);
//  }

//  private static void setEnabledRecursive(Control control, boolean enabled) {
//  control.setEnabled(enabled);
//  if (control instanceof Composite)
//  for (Control child : ((Composite)control).getChildren())
//  setEnabledRecursive(child, enabled);
//  }

    public void populate(MakemakeOptions data) {
        // "Scope" page
        deepMakefileRadioButton.setSelection(data.isDeep);
        recursiveMakefileRadioButton.setSelection(data.recursive);
        localMakefileRadioButton.setSelection(!data.isDeep && !data.recursive);
        subdirsDirsList.setItemsAsList(data.subdirs);

        // "Target" page
        switch (data.type) {
            case EXE: targetExecutableRadioButton.setSelection(true); break;
            case NOLINK: targetCompileOnlyRadioButton.setSelection(true); break;
            case SO: targetSharedLibRadioButton.setSelection(true); break;
            //TODO case STATICLIB: targetStaticLibRadioButton.setSelection(true); break;
        }
        if (StringUtils.isEmpty(data.target))
            defaultTargetName.setSelection(true); 
        else {
            specifyTargetNameRadioButton.setSelection(true);
            targetNameText.setText(data.target);
        }
        outputDirText.setText(StringUtils.nullToEmpty(data.outRoot));

        // "Include" page
        autoIncludePathCheckbox.setSelection(false); //TODO with deep: all source dir is implicitly an include dir as well

        // "Compile" page
        if (data.ccext == null)
            ccextCombo.setText("autodetect"); //XXX duplicate string literal
        else
            ccextCombo.setText("." + data.ccext);
        compileForDllCheckbox.setSelection(data.compileForDll);
        exportDefText.setText(StringUtils.nullToEmpty(data.exportDefOpt));

        // "Link" page
        userInterfaceCombo.setText(data.userInterface);
        libsList.setItemsAsList(data.libs);
        linkObjectsList.setItemsAsList(data.extraArgs);
        
        // "Custom" page
        makefragsList.setItemsAsList(data.fragmentFiles);
        
        // to the "Link" page:
        data.linkWithObjects = false; //TODO explain: "link with object files in directories given as extra include dirs" -- probably not needed... 
        data.tstamp = true; //TODO
        data.mode = null; //TODO
        makefragText.setText("xxx..."); //TODO
    }
    
    /**
     * Returns the current settings
     */
    public MakemakeOptions getResult() {
        MakemakeOptions result = new MakemakeOptions();

        // "Scope" page
        result.isDeep = deepMakefileRadioButton.getSelection();
        result.recursive = recursiveMakefileRadioButton.getSelection();
        result.subdirs.addAll(subdirsDirsList.getItemsAsList());

        // "Target" page
        if (targetExecutableRadioButton.getSelection())
            result.type = Type.EXE;
        else if (targetSharedLibRadioButton.getSelection())
            result.type = Type.SO; //FIXME rename to DYNAMICLIB
        else if (targetStaticLibRadioButton.getSelection())
            result.type = Type.SO; //FIXME introduce Type.STATICLIB !!
        else if (targetCompileOnlyRadioButton.getSelection())
            result.type = Type.NOLINK;
        result.target = defaultTargetName.getSelection() ? null : targetNameText.getText();
        result.outRoot = outputDirText.getText();

        // "Include" page
        autoIncludePathCheckbox.getSelection(); //TODO

        // "Compile" page
        String ccextText = ccextCombo.getText().trim().replace(".", "");
        result.ccext = (ccextText.equals("cc") || ccextText.equals("cpp")) ? ccextText : null;
        result.compileForDll = compileForDllCheckbox.getSelection();
        result.exportDefOpt = exportDefText.getText().trim();

        // "Link" page
        result.userInterface = userInterfaceCombo.getText().trim();
        result.libs.addAll(libsList.getItemsAsList());
        result.extraArgs.addAll(linkObjectsList.getItemsAsList());
        
        // "Custom" page
        result.fragmentFiles.addAll(makefragsList.getItemsAsList());

        //---
        result.linkWithObjects = false; //TODO
        result.tstamp = true; //TODO
        result.mode = ""; //TODO
	    makefragText.getText(); //TODO

        return result;
    }
}
