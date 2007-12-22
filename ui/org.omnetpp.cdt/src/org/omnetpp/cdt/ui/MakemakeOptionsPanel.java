package org.omnetpp.cdt.ui;

import java.util.Arrays;

import org.eclipse.cdt.utils.ui.controls.FileListControl;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.preference.IPreferencePageContainer;
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
import org.eclipse.swt.widgets.Link;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.dialogs.PropertyPage;
import org.eclipse.ui.internal.dialogs.PropertyDialog;
import org.omnetpp.cdt.makefile.MakemakeOptions;
import org.omnetpp.cdt.makefile.MetaMakemake;
import org.omnetpp.cdt.makefile.MakemakeOptions.Type;
import org.omnetpp.common.ui.ToggleLink;
import org.omnetpp.common.util.StringUtils;


/**
 * UI for editing MakemakeOptions
 * 
 * @author Andras
 */
//XXX introduce "buildingDllMacro" option into MakemakeOptions
//XXX use tabs for makefrag / makefrag.vc
//XXX if there's no buildspec, assume makefile generation in the project root folder (if no makefile exists already?) turn on "export", "autoincludes", "use exports" etc by default!
//XXX create "CDT Overview" page in the project properties dialog! should show if: excludes/include paths are inconsistent for different configurations;
//XXX "Out" dir should be marked as "output path" and as excluded in CDT !!!
//XXX "Out" dir should not overlap with source folders (check!!!)
//XXX verify that a .msg file alone can create folder dependency!
//XXX create new View: cross-folder dependencies (use DOT to render the graph?)
//XXX totally eliminate possibility of in-directory build!
public class MakemakeOptionsPanel extends Composite {
    // constants for CDT's FileListControl which are private;
    // see https://bugs.eclipse.org/bugs/show_bug.cgi?id=213188
    protected static final int BROWSE_NONE = 0;
    protected static final int BROWSE_FILE = 1;
    protected static final int BROWSE_DIR = 2;

    private static final String CCEXT_AUTODETECT = "autodetect";
    private static final String PROPERTYPAGE_PATH_AND_SYMBOLS = "org.eclipse.cdt.managedbuilder.ui.properties.Page_PathAndSymb";

    // the folder whose properties we're editing; needed for Preview panel / translated options
    private IContainer folder;
    private PropertyPage ownerPage;
    
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
    private Text buildingDllMacroText;

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
        Label subdirsLabel = createLabel(scopePage, "Additionally, invoke \"make\" in the following directories:");
        subdirsDirsList = new FileListControl(scopePage, "Sub-make directories", BROWSE_DIR);
        createToggleLink(scopePage, new Control[] {subdirsLabel, subdirsDirsList.getListControl().getParent()});

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
        outputDirText = createLabelAndText(outGroup, "Output directory:", "Specify project relative path. When empty, defaults to \"out\".");
        
        // "Compile" page
        compilePage.setLayout(new GridLayout(1,false));
        Group includeGroup = createGroup(compilePage, "Include Path", 1);
        deepIncludesCheckbox = createCheckbox(includeGroup, "Add all source folders under this deep makefile", null); 
        autoIncludePathCheckbox = createCheckbox(includeGroup, "Automatically add other folders where included files are located", "This project and its referenced projects are considered.");
        Link pathsPageLink1 = createLink(includeGroup, "NOTE: Additional include directories can be specified in the <A>Paths and symbols</A> page.");

        Group srcGroup = createGroup(compilePage, "Sources", 2);
        createLabel(srcGroup, "C++ file extension:");
        ccextCombo = new Combo(srcGroup, SWT.BORDER | SWT.READ_ONLY);
        ccextCombo.add(CCEXT_AUTODETECT);
        ccextCombo.add(".cc");
        ccextCombo.add(".cpp");
        
        Group dllGroup = createGroup(compilePage, "Windows DLLs", 2);
        compileForDllCheckbox = createCheckbox(dllGroup, "Compile object files for use in DLLs", "Defines the WIN32_DLL preprocessor symbol");
        compileForDllCheckbox.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false, 2, 1));
        dllExportMacroText = createLabelAndText(dllGroup, "DLL export/import symbol (e.g. FOO_API):", 
                "Name of the macro (#define) which expands to \n" +
                "__dllexport/__dllimport when WIN32_DLL is defined.\n" +
                "It will be used to annotate C++ classes generated\n" +
                "from msg files.");
        buildingDllMacroText = createLabelAndText(dllGroup, "Symbol to define when building this DLL (e.g. BUILDING_FOO):", 
                "The following code needs to be present in one of your\n" +
                "header files, and be included in all sources (replace FOO\n" +
                "with a name of your choice):\n" +
                "#ifdef BUILDING_FOO\n" +
                "#  define FOO_API  OPP_DLLEXPORT\n" +
                "#else\n" +
                "#  define FOO_API  OPP_DLLIMPORT\n" +
                "#endif");
        Link pathsPageLink2 = createLink(compilePage, "NOTE: Additional preprocessor symbols can be specified in the <A>Paths and symbols</A> page.");

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
//        Button cb2 = createCheckbox(linkGroup, "All objects from referenced projects", null); //XXX or static/dynamic libs?
        libsList = new FileListControl(linkPage, "Additional libraries to link with: (-l option)", BROWSE_NONE);
        Link pathsPageLink3 = createLink(linkPage, "NOTE: Library paths can be specified in the <A>Paths and symbols</A> page.");
        linkObjectsList = new FileListControl(linkPage, "Additional objects to link with: (folder-relative path; wildcards, macros allowed)", BROWSE_NONE);
        createToggleLink(linkPage, new Control[] {libsList.getListControl().getParent(), pathsPageLink3, linkObjectsList.getListControl().getParent()});
        
        // "Custom" page
        customPage.setLayout(new GridLayout(1,false));
        createLabel(customPage, "Code fragment to be inserted into the Makefile (Makefrag):");
        makefragText = new Text(customPage, SWT.MULTI | SWT.BORDER);
        makefragText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        Label makefragsLabel = createLabel(customPage, "Other fragment files to include:");
        makefragsList = new FileListControl(customPage, "Make fragments", BROWSE_NONE);
        createToggleLink(customPage, new Control[] {makefragsLabel, makefragsList.getListControl().getParent()});

        // "Preview" page
        previewPage.setLayout(new GridLayout(1,false));
        createLabel(previewPage, "Makemake options:");
        optionsText = new Text(previewPage, SWT.MULTI | SWT.BORDER | SWT.WRAP);
        optionsText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        createLabel(previewPage, "Makemake options modified with CDT settings:");
        translatedOptionsText = new Text(previewPage, SWT.MULTI | SWT.BORDER | SWT.READ_ONLY | SWT.WRAP);
        translatedOptionsText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        translatedOptionsText.setBackground(translatedOptionsText.getDisplay().getSystemColor(SWT.COLOR_WIDGET_BACKGROUND));

        Dialog.applyDialogFont(composite);

        hookListeners();
        
        SelectionListener gotoListener = new SelectionListener(){
            public void widgetSelected(SelectionEvent e) {
                gotoPathsAndSymbolsPage();
            }
            public void widgetDefaultSelected(SelectionEvent e) {
                gotoPathsAndSymbolsPage();
            }
        };
        pathsPageLink1.addSelectionListener(gotoListener);
        pathsPageLink2.addSelectionListener(gotoListener);
        pathsPageLink3.addSelectionListener(gotoListener);
        
        return composite;
    }

    protected Label createLabel(Composite composite, String text) {
        Label label = new Label(composite, SWT.NONE);
        label.setText(text);
        label.setLayoutData(new GridData());
        return label;
    }

    protected Link createLink(Composite composite, String text) {
        Link link = new Link(composite, SWT.NONE);
        link.setText(text);
        link.setLayoutData(new GridData());
        return link;
    }

    protected Group createGroup(Composite composite, String text, int numColumns) {
        Group group = new Group(composite, SWT.NONE);
        group.setText(text);
        group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        group.setLayout(new GridLayout(numColumns,false));
        return group;
    }

    protected Button createButton(Composite parent, String text, String tooltip) {
        return createButton(parent, SWT.NONE, text, tooltip);
    }

    protected Button createCheckbox(Composite parent, String text, String tooltip) {
        return createButton(parent, SWT.CHECK, text, tooltip);
    }

    protected Button createRadioButton(Composite parent, String text, String tooltip) {
        return createButton(parent, SWT.RADIO, text, tooltip);
    }

    private Button createButton(Composite parent, int style, String text, String tooltip) {
        Button button = new Button(parent, style);
        button.setText(text);
        if (tooltip != null)
            button.setToolTipText(tooltip);
        button.setLayoutData(new GridData());
        return button;
    }

    protected Text createLabelAndText(Composite parent, String labelText, String tooltip) {
        Label label = createLabel(parent, labelText);
        Text text = new Text(parent, SWT.BORDER);
        text.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        if (tooltip != null) {
            label.setToolTipText(tooltip);
            text.setToolTipText(tooltip);
        }
        return text;
    }
    
    protected Composite createTabPage(String text) {
        TabItem item = new TabItem(tabfolder, SWT.NONE);
        item.setText(text);
        Composite composite = new Composite(tabfolder, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        item.setControl(composite);
        return composite;
    }

    protected ToggleLink createToggleLink(Composite parent, Control[] controls) {
        ToggleLink toggleLink = new ToggleLink(parent, SWT.NONE);
        toggleLink.setControls(controls);
        return toggleLink;
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
        buildingDllMacroText.addModifyListener(mod);

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
        setErrorMessage(null);
        if (outputDirText.getText().matches(".*[:/\\\\].*"))
            setErrorMessage("Output folder: cannot contain /, \\ or :.");
        if (!dllExportMacroText.getText().trim().matches("(?i)|([A-Z_][A-Z0-9_]*)"))
            setErrorMessage("DLL export macro: contains illegal characters");
        if (!buildingDllMacroText.getText().trim().matches("(?i)|([A-Z_][A-Z0-9_]*)"))
            setErrorMessage("\"Building DLL\" macro: contains illegal characters");
        //TODO others...
    }

    @SuppressWarnings("restriction")
    protected void gotoPathsAndSymbolsPage() {
        IPreferencePageContainer container = ownerPage.getContainer();
        if (container instanceof PropertyDialog)
            ((PropertyDialog)container).setCurrentPageId(PROPERTYPAGE_PATH_AND_SYMBOLS);
    }

    protected void setErrorMessage(String text) {
        ownerPage.setErrorMessage(text);
    }

    public void setOwner(PropertyPage page) {
        this.ownerPage = page;
        this.folder = (IContainer) page.getElement();  // must be a folder!
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
        buildingDllMacroText.setText(StringUtils.nullToEmpty(data.buildingDllMacro));

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
        result.buildingDllMacro = buildingDllMacroText.getText().trim();

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
