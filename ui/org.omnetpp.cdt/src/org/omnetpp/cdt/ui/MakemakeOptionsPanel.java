package org.omnetpp.cdt.ui;

import java.util.Arrays;

import org.eclipse.cdt.utils.ui.controls.FileListControl;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
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
import org.omnetpp.cdt.makefile.MetaMakemake;
import org.omnetpp.cdt.makefile.MakemakeOptions.Type;
import org.omnetpp.common.util.StringUtils;


/**
 * UI for editing MakemakeOptions
 * 
 * @author Andras
 */
//XXX use [Advanced] button on Link page
//XXX use tabs for makefrag / makefrag.vc
//XXX if there's no buildspec, assume makefile generation in the project root folder (if no makefile exists already?) turn on "export", "autoincludes", "use exports" etc by default!
//XXX create "CDT Overview" page in the project properties dialog! should show if: excludes/include paths are inconsistent for different configurations;
//XXX "Out" dir should be marked as "output path" and as excluded in CDT !!!
//XXX "Out" dir should not overlap with source folders (check!!!)
//XXX verify that a .msg file alone can create folder dependency!
//XXX new View: cross-folder dependencies (use DOT to render the graph?)
//XXX totally eliminate possibility of in-directory build!
public class MakemakeOptionsPanel extends Composite {
    // constants for CDT's FileListControl which are private;
    // see https://bugs.eclipse.org/bugs/show_bug.cgi?id=213188
    protected static final int BROWSE_NONE = 0;
    protected static final int BROWSE_FILE = 1;
    protected static final int BROWSE_DIR = 2;

    private static final String CCEXT_AUTODETECT = "autodetect";

    // controls
    private TabFolder tabfolder;
    private Composite scopePage;
    private Composite targetPage;
    private Composite compilePage;
    private Composite linkPage;
    private Composite customPage;
    private Composite previewPage;


    // "Scope" page
    private Button deepMakefileRadioButton;
    private Button recursiveMakefileRadioButton;
    private Button localMakefileRadioButton;
    private FileListControl subdirsDirsList;

    // "Target" page
    private Button targetExecutableRadioButton;
    private Button targetSharedLibRadioButton;
    private Button targetStaticLibRadioButton;
    private Button targetCompileOnlyRadioButton;
    private Button defaultTargetNameRadionButton;
    private Button specifyTargetNameRadioButton;
    private Text targetNameText;
    private Button exportLibraryCheckbox;
    private Text outputDirText;

    // "Include" page
    private Button deepIncludesCheckbox;
    private Button autoIncludePathCheckbox;

    // "Compile" page
    private Combo ccextCombo;
    private Button compileForDllCheckbox;
    private Text dllExportMacroText;

    // "Link" page
    private Combo userInterfaceCombo;
    private Button useExportedLibs;
    private FileListControl libsList;
    private FileListControl linkObjectsList;

    // "Custom" page
    private Text makefragText;
    private FileListControl makefragsList;
    
    // "Preview" page
    private Text optionsText;
    private Text translatedOptionsText;
    
    // the folder whose properties we're editing; needed for Preview panel / translated options
    private IContainer folder;


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
        subdirsDirsList = new FileListControl(scopePage, "Sub-make directories", BROWSE_DIR);

        // "Target" page
        targetPage.setLayout(new GridLayout(1,false));
        Group group = createGroup(targetPage, "Target type:", 1);
        targetExecutableRadioButton = createRadioButton(group, "Executable", null);
        targetSharedLibRadioButton = createRadioButton(group, "Shared library (.dll or .so)", null);
        targetStaticLibRadioButton = createRadioButton(group, "Static library (.lib or .a)", null);
        exportLibraryCheckbox = createCheckbox(group, "Export this shared/static library for other projects", "Let dependent projects automatically use this library");
        ((GridData)exportLibraryCheckbox.getLayoutData()).horizontalIndent = 20;
        targetCompileOnlyRadioButton = createRadioButton(group, "Compile only", null);
        
        Group targetNameGroup = createGroup(targetPage, "Target name:", 2);
        defaultTargetNameRadionButton = createRadioButton(targetNameGroup, "Default", "Default target name will be derived from the directory name");
        defaultTargetNameRadionButton.setLayoutData(new GridData());
        ((GridData)defaultTargetNameRadionButton.getLayoutData()).horizontalSpan = 2;
        specifyTargetNameRadioButton = createRadioButton(targetNameGroup, "Specify name (without extension/lib prefix): ", null);
        targetNameText = new Text(targetNameGroup, SWT.BORDER);
        targetNameText.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        Group outGroup = createGroup(targetPage, "Output:", 2);
        String tooltip = "Specify project relative path. When empty, defaults to \"out\".";
        Label outputDirLabel = createLabel(outGroup, "Output directory:");
        outputDirLabel.setToolTipText(tooltip);
        outputDirText = new Text(outGroup, SWT.BORDER);
        outputDirText.setToolTipText(tooltip);
        outputDirText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        
        // "Compile" page
        compilePage.setLayout(new GridLayout(1,false));
        Group includeGroup = createGroup(compilePage, "Include", 1);
        deepIncludesCheckbox = createCheckbox(includeGroup, "Add all source folders under deep makefile to the include path", null); 
        autoIncludePathCheckbox = createCheckbox(includeGroup, "Automatically add other folders where #included files are located", "This project and its referenced projects are considered.");
        createLabel(includeGroup, "NOTE: Additional include directories can be specified in the C/C++ General -> Paths and symbols page.");

        Group srcGroup = createGroup(compilePage, "Sources", 2);
        createLabel(srcGroup, "C++ file extension:");
        ccextCombo = new Combo(srcGroup, SWT.BORDER | SWT.READ_ONLY);
        ccextCombo.add(CCEXT_AUTODETECT);
        ccextCombo.add(".cc");
        ccextCombo.add(".cpp");
        
        Group dllGroup = createGroup(compilePage, "Windows DLLs", 2);
        compileForDllCheckbox = createCheckbox(dllGroup, "Compile object files for use in DLLs", "Defines WIN32_DLL as preprocessor symbol"); //XXX new
        compileForDllCheckbox.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false, 2, 1));
        Label dllExportMacroLabel = createLabel(dllGroup, "DLL export symbol for msg files:");
        dllExportMacroText = new Text(dllGroup, SWT.BORDER);
        dllExportMacroText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        String dllExportMacroTooltip = 
            "Name of the macro (#define) which expands to __dllexport/__dllimport \n" +
            "when WIN32_DLL is defined. The message compiler needs to know it \n" +
            "in order to be able to add it to generated classes.";
        dllExportMacroLabel.setToolTipText(dllExportMacroTooltip);
        dllExportMacroText.setToolTipText(dllExportMacroTooltip);

        createLabel(compilePage, "NOTE: Additional preprocessor symbols can be specified in the C/C++ General -> Paths and symbols page.");

        // "Link" page
        linkPage.setLayout(new GridLayout(1,false));

        Group linkGroup = createGroup(linkPage, "Link", 2);
        useExportedLibs = createCheckbox(linkGroup, "Use libraries exported from referenced projects", null);
        ((GridData)useExportedLibs.getLayoutData()).horizontalSpan = 2;
        createLabel(linkGroup, "User interface libraries to link with:");
        userInterfaceCombo = new Combo(linkGroup, SWT.BORDER | SWT.READ_ONLY);
        for (String i : new String[] {"All", "Tkenv", "Cmdenv"}) // note: should be consistent with populate()!
            userInterfaceCombo.add(i);
        
//        Group linkGroup = createGroup(linkPage, "Link additionally with:", 1);
//        //FIXME are these combo boxes needed? do they correspond to any makemake settings?
//        Button cb1 = createCheckbox(linkGroup, "All object files in this project", null); //XXX radiobutton?
//        Button cb2 = createCheckbox(linkGroup, "All object files in this project, except in folders with custom Makefiles", null);
//        Button cb3 = createCheckbox(linkGroup, "All objects from referenced projects", null); //XXX or static/dynamic libs?
        libsList = new FileListControl(linkPage, "Additional libraries to link with: (-l option)", BROWSE_NONE);
        createLabel(linkPage, "NOTE: Library paths can be specified in the C/C++ General -> Paths and symbols page.");
        linkObjectsList = new FileListControl(linkPage, "Additional objects to link with: (folder-relative path; wildcards, macros allowed)", BROWSE_NONE);

        // "Custom" page
        customPage.setLayout(new GridLayout(1,false));
        createLabel(customPage, "Code fragment to be inserted into the Makefile (Makefrag):");
        makefragText = new Text(customPage, SWT.MULTI | SWT.BORDER);
        makefragText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        createLabel(customPage, "Other fragment files to include:");
        makefragsList = new FileListControl(customPage, "Make fragments", BROWSE_NONE);

        // "Preview" page
        previewPage.setLayout(new GridLayout(1,false));
        createLabel(previewPage, "Makemake options:");
        optionsText = new Text(previewPage, SWT.MULTI | SWT.BORDER | SWT.WRAP);
        optionsText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        createLabel(previewPage, "Makemake options modified with CDT settings:");
        translatedOptionsText = new Text(previewPage, SWT.MULTI | SWT.BORDER | SWT.READ_ONLY | SWT.WRAP);
        translatedOptionsText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        Dialog.applyDialogFont(composite);

        hookListeners();
        
        return composite;
    }

    protected Label createLabel(Composite composite, String text) {
        Label label = new Label(composite, SWT.NONE);
        label.setText(text);
        label.setLayoutData(new GridData());
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
        button.setLayoutData(new GridData());
        return button;
    }

    protected Button createRadioButton(Composite parent, String text, String tooltip) {
        Button button = new Button(parent, SWT.RADIO);
        button.setText(text);
        if (tooltip != null)
            button.setToolTipText(tooltip);
        button.setLayoutData(new GridData());
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

    protected void hookListeners() {
        // maintain consistency between dialog controls and the "Preview" page
        tabfolder.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                activeTabChanged();
            }
        });
        optionsText.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                optionsTextChanged();
            }
        });
        
        // validate dialog contents on changes
        SelectionListener sel = new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                updateDialogState();
            }
        };
        ModifyListener mod = new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                updateDialogState();
            }
        };

        deepMakefileRadioButton.addSelectionListener(sel);
        recursiveMakefileRadioButton.addSelectionListener(sel);
        localMakefileRadioButton.addSelectionListener(sel);
        subdirsDirsList.getListControl().addSelectionListener(sel);

        targetExecutableRadioButton.addSelectionListener(sel);
        targetSharedLibRadioButton.addSelectionListener(sel);
        targetStaticLibRadioButton.addSelectionListener(sel);
        targetCompileOnlyRadioButton.addSelectionListener(sel);

        defaultTargetNameRadionButton.addSelectionListener(sel);
        specifyTargetNameRadioButton.addSelectionListener(sel);
        targetNameText.addModifyListener(mod);
        exportLibraryCheckbox.addSelectionListener(sel);
        outputDirText.addModifyListener(mod);

        deepIncludesCheckbox.addSelectionListener(sel);
        autoIncludePathCheckbox.addSelectionListener(sel);

        ccextCombo.addSelectionListener(sel);
        compileForDllCheckbox.addSelectionListener(sel);
        dllExportMacroText.addModifyListener(mod);

        userInterfaceCombo.addSelectionListener(sel);
        useExportedLibs.addSelectionListener(sel);
        libsList.getListControl().addSelectionListener(sel);
        linkObjectsList.getListControl().addSelectionListener(sel);

        makefragText.addModifyListener(mod);
        makefragsList.getListControl().addSelectionListener(sel);  //XXX does not work for list control
    }

    protected void activeTabChanged() {
        if (tabfolder.getSelection().length>0 && tabfolder.getSelection()[0].getControl() == previewPage) {
            // switched to "Preview" page -- update its contents
            MakemakeOptions options = getResult();
            optionsText.setText(options.toString());
            try {
                translatedOptionsText.setText(MetaMakemake.translateOptions(folder, options, null).toString());
            } catch (CoreException e) {
                //XXX
            }
        }
    }

    protected void optionsTextChanged() {
        // re-parse options text modified by user
        MakemakeOptions updatedOptions = new MakemakeOptions(optionsText.getText()); //FIXME exception if invalid option is entered!
        populate(updatedOptions, makefragText.getText());
        try {
            //FIXME pass in folderDeps too! (ask from builder)
            translatedOptionsText.setText(MetaMakemake.translateOptions(folder, updatedOptions, null).toString());
        } catch (CoreException e) {
            //XXX
        }
    }

    protected void updateDialogState() {
        // enabled state
        Type type = getSelectedType();
        defaultTargetNameRadionButton.setEnabled(type!=Type.NOLINK);
        specifyTargetNameRadioButton.setEnabled(type!=Type.NOLINK);
        targetNameText.setEnabled(specifyTargetNameRadioButton.getSelection() && type!=Type.NOLINK);
        exportLibraryCheckbox.setEnabled(type==Type.STATICLIB || type==Type.SHAREDLIB);
        userInterfaceCombo.setEnabled(targetExecutableRadioButton.getSelection());
        useExportedLibs.setEnabled(type==Type.EXE || type==Type.SHAREDLIB);
        libsList.setEnabled(type!=Type.NOLINK);
        linkObjectsList.setEnabled(type!=Type.NOLINK);
        deepIncludesCheckbox.setEnabled(deepMakefileRadioButton.getSelection());
        
        // checkbox checked state
        if (type!=Type.STATICLIB && type!=Type.SHAREDLIB)
            exportLibraryCheckbox.setSelection(false);
        if (type!=Type.EXE && type!=Type.SHAREDLIB)
            useExportedLibs.setSelection(false);
        
        // validate text field contents
        setMessage(null);
        String dllExportMacro = dllExportMacroText.getText();
        if (!dllExportMacro.trim().matches("(?i)[A-Z_][A-Z0-9_]*"))
            setMessage("DLL export macro: contains illegal characters");
        //TODO others...
    }
    
    protected void setMessage(String string) {
        // TODO Auto-generated method stub
    }

    public void setFolder(IContainer folder) {
        this.folder = folder;
    }

    public void populate(MakemakeOptions data, String makefragContents) {
        // "Scope" page
        deepMakefileRadioButton.setSelection(data.isDeep);
        recursiveMakefileRadioButton.setSelection(data.isRecursive);
        localMakefileRadioButton.setSelection(!data.isDeep && !data.isRecursive);
        subdirsDirsList.setList(data.subdirs.toArray(new String[]{}));

        // "Target" page
        targetExecutableRadioButton.setSelection(data.type==Type.EXE);
        targetSharedLibRadioButton.setSelection(data.type==Type.SHAREDLIB);
        targetStaticLibRadioButton.setSelection(data.type==Type.STATICLIB);
        targetCompileOnlyRadioButton.setSelection(data.type==Type.NOLINK); 

        defaultTargetNameRadionButton.setSelection(StringUtils.isEmpty(data.target)); 
        specifyTargetNameRadioButton.setSelection(!StringUtils.isEmpty(data.target));
        targetNameText.setText(StringUtils.nullToEmpty(data.target));
        exportLibraryCheckbox.setSelection(data.metaExportLibrary);
        outputDirText.setText(StringUtils.nullToEmpty(data.outRoot));

        // "Include" page
        deepIncludesCheckbox.setSelection(!data.noDeepIncludes);
        autoIncludePathCheckbox.setSelection(data.metaAutoIncludePath);

        // "Compile" page
        if (data.ccext == null)
            ccextCombo.setText(CCEXT_AUTODETECT);
        else
            ccextCombo.setText("." + data.ccext);
        compileForDllCheckbox.setSelection(data.compileForDll);
        dllExportMacroText.setText(StringUtils.nullToEmpty(data.dllExportMacro));

        // "Link" page
        userInterfaceCombo.setText(StringUtils.capitalize(data.userInterface.toLowerCase()));
        useExportedLibs.setSelection(data.metaUseExportedLibs);
        libsList.setList(data.libs.toArray(new String[]{}));
        linkObjectsList.setList(data.extraArgs.toArray(new String[]{}));
        
        // "Custom" page
        makefragText.setText(StringUtils.nullToEmpty(makefragContents));
        makefragsList.setList(data.fragmentFiles.toArray(new String[]{}));
        
        // to the "Link" page:
        data.linkWithObjects = false; //TODO; also explain: "link with object files in directories given as extra include dirs" -- probably not needed... 
    }
    
    /**
     * Returns the current settings
     */
    public MakemakeOptions getResult() {
        MakemakeOptions result = new MakemakeOptions();

        // "Scope" page
        result.isDeep = deepMakefileRadioButton.getSelection();
        result.isRecursive = recursiveMakefileRadioButton.getSelection();
        result.subdirs.addAll(Arrays.asList(subdirsDirsList.getItems()));

        // "Target" page
        result.type = getSelectedType();
        result.target = defaultTargetNameRadionButton.getSelection() ? null : targetNameText.getText();
        result.metaExportLibrary = exportLibraryCheckbox.getSelection();
        result.outRoot = outputDirText.getText();

        // "Include" page
        result.noDeepIncludes = !deepIncludesCheckbox.getSelection();
        result.metaAutoIncludePath = autoIncludePathCheckbox.getSelection();

        // "Compile" page
        String ccextText = ccextCombo.getText().trim().replace(".", "");
        result.ccext = (ccextText.equals("cc") || ccextText.equals("cpp")) ? ccextText : null;
        result.compileForDll = compileForDllCheckbox.getSelection();
        result.dllExportMacro = dllExportMacroText.getText().trim();

        // "Link" page
        result.userInterface = userInterfaceCombo.getText().trim();
        result.metaUseExportedLibs = useExportedLibs.getSelection();
        result.libs.addAll(Arrays.asList(libsList.getItems()));
        result.extraArgs.addAll(Arrays.asList(linkObjectsList.getItems()));
        
        // "Custom" page
        result.fragmentFiles.addAll(Arrays.asList(makefragsList.getItems()));

        //---
        result.linkWithObjects = false; //XXX has wrong name!!

        return result;
    }

    protected Type getSelectedType() {
        if (targetExecutableRadioButton.getSelection())
            return Type.EXE;
        else if (targetSharedLibRadioButton.getSelection())
            return Type.SHAREDLIB;
        else if (targetStaticLibRadioButton.getSelection())
            return Type.STATICLIB;
        else if (targetCompileOnlyRadioButton.getSelection())
            return Type.NOLINK;
        else
            return Type.EXE; // cannot happen
    }
    
    public String getMakefragContents() {
        return makefragText.getText();
    }
}
