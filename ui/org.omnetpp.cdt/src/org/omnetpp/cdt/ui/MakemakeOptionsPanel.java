package org.omnetpp.cdt.ui;

import java.util.Arrays;

import org.eclipse.cdt.utils.ui.controls.FileListControl;
import org.eclipse.cdt.utils.ui.controls.IFileListChangeListener;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.preference.IPreferencePageContainer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
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
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Link;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.dialogs.PropertyPage;
import org.eclipse.ui.internal.dialogs.PropertyDialog;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.makefile.MakemakeOptions;
import org.omnetpp.cdt.makefile.MetaMakemake;
import org.omnetpp.cdt.makefile.MakemakeOptions.Type;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.common.ui.ToggleLink;
import org.omnetpp.common.util.StringUtils;


/**
 * UI for editing MakemakeOptions.
 * 
 * @author Andras
 */
//XXX ha "Preview" lapon a user rossz opciot ir be, hibat jelezni!!!
//XXX kezzel beirt -D elveszik!!! ezek szinten: defaultMode, exceptSubdirs, includeDirs, libDirs, defines, makefileDefines
//XXX display which makefile is the primary makefile, and add "Make primary Makefile" button
public class MakemakeOptionsPanel extends Composite {
    // constants for CDT's FileListControl which are private;
    // see https://bugs.eclipse.org/bugs/show_bug.cgi?id=213188
    protected static final int BROWSE_NONE = 0;
    protected static final int BROWSE_FILE = 1;
    protected static final int BROWSE_DIR = 2;

    public static final String PROPERTYPAGE_PATH_AND_SYMBOLS = "org.eclipse.cdt.managedbuilder.ui.properties.Page_PathAndSymb";
    private static final String CCEXT_AUTODETECT = "autodetect";

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
    private HoverSupport hoverSupport;

    // "Scope" page
    private Button deepMakefileRadioButton;
    private Button recursiveMakefileRadioButton;
    private Button localMakefileRadioButton;
    private ToggleLink scopePageToggle;
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
    private Text dllSymbolText;

    // "Link" page
    private Button useExportedLibsCheckbox;
    private Button linkAllObjectsCheckbox;
    private Combo userInterfaceCombo;
    private ToggleLink linkPageToggle;
    private FileListControl libsList;
    private FileListControl linkObjectsList;

    // "Custom" page
    private Text makefragText;
    private Text makefragvcText;
    private ToggleLink customPageToggle; 
    private FileListControl makefragsList;

    // "Preview" page
    private Text optionsText;
    private Text translatedOptionsText;

    // auxiliary variables
    private boolean beingUpdated = false;
    private int jobSerial = 0;


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

        scopePage = createTabPage(tabfolder, "Scope");
        targetPage = createTabPage(tabfolder, "Target");
        compilePage = createTabPage(tabfolder, "Compile");
        linkPage = createTabPage(tabfolder, "Link");
        customPage = createTabPage(tabfolder, "Custom");
        previewPage = createTabPage(tabfolder, "Preview");
        tabfolder.setSelection(0);

        hoverSupport = new HoverSupport();
        hoverSupport.setHoverSizeConstaints(500, 400); // DLL help text is long & wide

        // "Scope" page
        scopePage.setLayout(new GridLayout(1,false));
        Group group1 = createGroup(scopePage, "Select one:", 1);
        deepMakefileRadioButton = createRadioButton(group1, "Deep (recommended)", "Process all source files from this subdirectory tree");
        recursiveMakefileRadioButton = createRadioButton(group1, "Recursive", "Process source files in this directory only, and invoke \"make\" in all subdirectories; Makefiles must exist");
        localMakefileRadioButton = createRadioButton(group1, "Local", "Process source files in this directory only; ignore subdirectories");
        createLabel(group1, "Makefiles will ignore directories marked as \"Excluded\"");
        Label subdirsLabel = createLabel(scopePage, "Additionally, invoke \"make\" in the following directories:");
        subdirsDirsList = new FileListControl(scopePage, "Sub-make directories (relative path)", BROWSE_DIR);
        scopePageToggle = createToggleLink(scopePage, new Control[] {subdirsLabel, subdirsDirsList.getListControl().getParent()});
        
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
        compileForDllCheckbox = createCheckbox(dllGroup, "Compile object files for use in DLLs", "Defines the FOO_EXPORT macro, where FOO is the DLL export/import symbol");
        compileForDllCheckbox.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false, 2, 1));
        dllSymbolText = createLabelAndText(dllGroup, "DLL export/import symbol (e.g. FOO):", "Base name for the FOO_API, FOO_EXPORT and FOO_IMPORT macros"); 
        final Link dllHelpLink = createLink(dllGroup, "Hover or <A>click here</A> for more info on creating DLLs.");
        Link pathsPageLink2 = createLink(compilePage, "NOTE: Additional preprocessor symbols can be specified in the <A>Paths and symbols</A> page.");

        // "Link" page
        linkPage.setLayout(new GridLayout(1,false));

        Group linkGroup = createGroup(linkPage, "Link:", 2);
        linkAllObjectsCheckbox = createCheckbox(linkGroup, "Link with all object files in this project", null);
        ((GridData)linkAllObjectsCheckbox.getLayoutData()).horizontalSpan = 2;
        useExportedLibsCheckbox = createCheckbox(linkGroup, "Link with libraries exported from referenced projects", null);
        ((GridData)useExportedLibsCheckbox.getLayoutData()).horizontalSpan = 2;
        createLabel(linkGroup, "User interface libraries to link with:");
        userInterfaceCombo = new Combo(linkGroup, SWT.BORDER | SWT.READ_ONLY);
        for (String i : new String[] {"All", "Tkenv", "Cmdenv"}) // note: should be consistent with populate()!
            userInterfaceCombo.add(i);
        libsList = new FileListControl(linkPage, "Additional libraries to link with: (-l option)", BROWSE_NONE);
        Link pathsPageLink3 = createLink(linkPage, "NOTE: Library paths can be specified in the <A>Paths and symbols</A> page.");
        linkObjectsList = new FileListControl(linkPage, "Additional objects to link with: (folder-relative path; wildcards, macros allowed)", BROWSE_NONE);
        linkPageToggle = createToggleLink(linkPage, new Control[] {libsList.getListControl().getParent(), pathsPageLink3, linkObjectsList.getListControl().getParent()});
        
        // "Custom" page
        customPage.setLayout(new GridLayout(1,false));
        CTabFolder makefragTabFolder = new CTabFolder(customPage, SWT.TOP | SWT.BORDER);
        makefragTabFolder.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        ((GridData)makefragTabFolder.getLayoutData()).heightHint = 100;

        Composite makefragComposite = createCTabPage(makefragTabFolder, "Makefrag");
        makefragComposite.setLayout(new GridLayout(1, false));
        createLabel(makefragComposite, "Code fragment to be inserted into Makefile:");
        makefragText = new Text(makefragComposite, SWT.MULTI | SWT.BORDER | SWT.H_SCROLL | SWT.V_SCROLL);
        makefragText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        Composite makefragvcComposite = createCTabPage(makefragTabFolder, "Makefrag.vc");
        makefragvcComposite.setLayout(new GridLayout(1, false));
        createLabel(makefragvcComposite, "Code fragment to be inserted into Makefile.vc:");
        makefragvcText = new Text(makefragvcComposite, SWT.MULTI | SWT.BORDER | SWT.H_SCROLL | SWT.V_SCROLL);
        makefragvcText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        Label makefragsLabel = createLabel(customPage, "Other fragment files to include:");
        makefragsList = new FileListControl(customPage, "Makefile fragments", BROWSE_NONE);
        customPageToggle = createToggleLink(customPage, new Control[] {makefragsLabel, makefragsList.getListControl().getParent()});
        makefragTabFolder.setSelection(0);

        // "Preview" page
        previewPage.setLayout(new GridLayout(1,false));
        createLabel(previewPage, "Makemake options:");
        optionsText = new Text(previewPage, SWT.MULTI | SWT.BORDER | SWT.WRAP | SWT.V_SCROLL);
        optionsText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        createLabel(previewPage, "Makemake options modified with CDT settings:");
        translatedOptionsText = new Text(previewPage, SWT.MULTI | SWT.BORDER | SWT.READ_ONLY | SWT.WRAP | SWT.V_SCROLL);
        translatedOptionsText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        translatedOptionsText.setBackground(translatedOptionsText.getDisplay().getSystemColor(SWT.COLOR_WIDGET_BACKGROUND));

        Dialog.applyDialogFont(composite);

        hoverSupport.adapt(dllHelpLink, new IHoverTextProvider() {
            public String getHoverTextFor(Control control, int x, int y, SizeConstraint outSizeConstraint) {
                return HoverSupport.addHTMLStyleSheet(getHelpTextForBuildingDLLs());
            }
        });
        dllHelpLink.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                hoverSupport.makeHoverSticky(dllHelpLink);
            }
        });

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

        hookChangeListeners();

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

    protected Composite createTabPage(TabFolder tabfolder, String text) {
        TabItem item = new TabItem(tabfolder, SWT.NONE);
        item.setText(text);
        Composite composite = new Composite(tabfolder, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        item.setControl(composite);
        return composite;
    }

    protected Composite createCTabPage(CTabFolder tabfolder, String text) {
        CTabItem item = new CTabItem(tabfolder, SWT.NONE);
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

    protected void hookChangeListeners() {
        // maintain consistency between dialog controls and the "Preview" page
        tabfolder.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                activeTabChanged();
            }
        });
        optionsText.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                updateDialogState();
            }
        });

        // validate dialog contents on changes
        SelectionListener selectionChangeListener = new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                updateDialogState();
            }};
        ModifyListener modifyListener = new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                updateDialogState();
            }};
        IFileListChangeListener fileListChangeListener = new IFileListChangeListener() {
            public void fileListChanged(FileListControl fileList, String[] oldValue, String[] newValue) {
                updateDialogState();
            }};

        deepMakefileRadioButton.addSelectionListener(selectionChangeListener);
        recursiveMakefileRadioButton.addSelectionListener(selectionChangeListener);
        localMakefileRadioButton.addSelectionListener(selectionChangeListener);
        subdirsDirsList.addChangeListener(fileListChangeListener);

        targetExecutableRadioButton.addSelectionListener(selectionChangeListener);
        targetSharedLibRadioButton.addSelectionListener(selectionChangeListener);
        targetStaticLibRadioButton.addSelectionListener(selectionChangeListener);
        targetCompileOnlyRadioButton.addSelectionListener(selectionChangeListener);

        defaultTargetNameRadionButton.addSelectionListener(selectionChangeListener);
        specifyTargetNameRadioButton.addSelectionListener(selectionChangeListener);
        targetNameText.addModifyListener(modifyListener);
        exportLibraryCheckbox.addSelectionListener(selectionChangeListener);
        outputDirText.addModifyListener(modifyListener);

        deepIncludesCheckbox.addSelectionListener(selectionChangeListener);
        autoIncludePathCheckbox.addSelectionListener(selectionChangeListener);

        ccextCombo.addSelectionListener(selectionChangeListener);
        compileForDllCheckbox.addSelectionListener(selectionChangeListener);
        dllSymbolText.addModifyListener(modifyListener);

        userInterfaceCombo.addSelectionListener(selectionChangeListener);
        useExportedLibsCheckbox.addSelectionListener(selectionChangeListener);
        linkAllObjectsCheckbox.addSelectionListener(selectionChangeListener);
        libsList.addChangeListener(fileListChangeListener);
        linkObjectsList.addChangeListener(fileListChangeListener);

        makefragText.addModifyListener(modifyListener);
        makefragvcText.addModifyListener(modifyListener);
        makefragsList.addChangeListener(fileListChangeListener);
    }

    protected void activeTabChanged() {
        if (isPreviewPageSelected()) {
            // switched to "Preview" page -- update its contents
            MakemakeOptions options = getResult();
            optionsText.setText(options.toString());
            refreshTranslatedOptions(options);
        }
    }

    protected boolean isPreviewPageSelected() {
        return tabfolder.getSelection().length>0 && tabfolder.getSelection()[0].getControl() == previewPage;
    }

    public void populate(IContainer folder, MakemakeOptions options, String makefragContents, String makefragvcContents) {
        this.folder = folder;

        makefragText.setText(StringUtils.nullToEmpty(makefragContents));
        makefragvcText.setText(StringUtils.nullToEmpty(makefragvcContents));
        if (isPreviewPageSelected())
            optionsText.setText(options.toString());
        else
            populateControls(options);

        updateDialogState();
    }

    protected void populateControls(MakemakeOptions options) {
        // "Scope" page
        deepMakefileRadioButton.setSelection(options.isDeep);
        recursiveMakefileRadioButton.setSelection(options.isRecursive);
        localMakefileRadioButton.setSelection(!options.isDeep && !options.isRecursive);
        subdirsDirsList.setList(options.subdirs.toArray(new String[]{}));

        // "Target" page
        targetExecutableRadioButton.setSelection(options.type==Type.EXE);
        targetSharedLibRadioButton.setSelection(options.type==Type.SHAREDLIB);
        targetStaticLibRadioButton.setSelection(options.type==Type.STATICLIB);
        targetCompileOnlyRadioButton.setSelection(options.type==Type.NOLINK); 

        defaultTargetNameRadionButton.setSelection(StringUtils.isEmpty(options.target)); 
        specifyTargetNameRadioButton.setSelection(!StringUtils.isEmpty(options.target));
        targetNameText.setText(StringUtils.nullToEmpty(options.target));
        exportLibraryCheckbox.setSelection(options.metaExportLibrary);
        outputDirText.setText(StringUtils.nullToEmpty(options.outRoot));

        // "Include" page
        deepIncludesCheckbox.setSelection(!options.noDeepIncludes);
        autoIncludePathCheckbox.setSelection(options.metaAutoIncludePath);

        // "Compile" page
        if (options.ccext == null)
            ccextCombo.setText(CCEXT_AUTODETECT);
        else
            ccextCombo.setText("." + options.ccext);
        compileForDllCheckbox.setSelection(options.compileForDll);
        dllSymbolText.setText(StringUtils.nullToEmpty(options.dllSymbol));

        // "Link" page
        userInterfaceCombo.setText(StringUtils.capitalize(options.userInterface.toLowerCase()));
        useExportedLibsCheckbox.setSelection(options.metaUseExportedLibs);
        linkAllObjectsCheckbox.setSelection(options.metaLinkWithAllObjectsInProject);
        libsList.setList(options.libs.toArray(new String[]{}));
        linkObjectsList.setList(options.extraArgs.toArray(new String[]{}));

        // "Custom" page
        // Note: makefrag texts need to be set differently
        makefragsList.setList(options.fragmentFiles.toArray(new String[]{}));

        // open ToggleLinks if controls are not empty
        if (subdirsDirsList.getListControl().getItemCount() != 0)
            scopePageToggle.setExpanded(true);
        if (libsList.getListControl().getItemCount() != 0 || linkObjectsList.getListControl().getItemCount() != 0)
            linkPageToggle.setExpanded(true);
        if (makefragsList.getListControl().getItemCount() != 0)
            customPageToggle.setExpanded(true);
    }

    protected void updateDialogState() {
        if (beingUpdated)
            return;  // prevent recursive invocations via listeners
        try {
            beingUpdated = true;

            if (isPreviewPageSelected()) {
                // re-parse options text modified by user
                MakemakeOptions updatedOptions = new MakemakeOptions(optionsText.getText());
                //XXX check makemakeOptions.getParseErrors() !!!
                populateControls(updatedOptions);
                refreshTranslatedOptions(updatedOptions);
            }
            else {
                // update enabled states
                Type type = getSelectedType();
                defaultTargetNameRadionButton.setEnabled(type!=Type.NOLINK);
                specifyTargetNameRadioButton.setEnabled(type!=Type.NOLINK);
                targetNameText.setEnabled(specifyTargetNameRadioButton.getSelection() && type!=Type.NOLINK);
                exportLibraryCheckbox.setEnabled(type==Type.STATICLIB || type==Type.SHAREDLIB);
                userInterfaceCombo.setEnabled(targetExecutableRadioButton.getSelection());
                useExportedLibsCheckbox.setEnabled(type==Type.EXE || type==Type.SHAREDLIB);
                linkAllObjectsCheckbox.setEnabled(type==Type.EXE || type==Type.SHAREDLIB);
                libsList.setEnabled(type!=Type.NOLINK);
                linkObjectsList.setEnabled(type!=Type.NOLINK);
                deepIncludesCheckbox.setEnabled(deepMakefileRadioButton.getSelection());

                // update checkbox checked states
                if (type!=Type.STATICLIB && type!=Type.SHAREDLIB)
                    exportLibraryCheckbox.setSelection(false);
                if (type!=Type.EXE && type!=Type.SHAREDLIB) {
                    useExportedLibsCheckbox.setSelection(false);
                    linkAllObjectsCheckbox.setSelection(false);
                }

                // validate text field contents
                setErrorMessage(null);
                if (!targetNameText.getText().trim().matches("(?i)|([A-Z_][A-Z0-9_]*)"))
                    setErrorMessage("Target name contains illegal characters");
                if (outputDirText.getText().matches(".*[:/\\\\].*"))
                    setErrorMessage("Output folder: cannot contain /, \\ or :.");
                if (!dllSymbolText.getText().trim().matches("(?i)|([A-Z_][A-Z0-9_]*)"))
                    setErrorMessage("DLL export macro: contains illegal characters");
            }
        } finally {
            beingUpdated = false;
        }
    }

    protected void refreshTranslatedOptions(final MakemakeOptions updatedOptions) {
        // All the following is basically just for running the following code asynchronously:
        //   translatedOptionsText.setText(MetaMakemake.translateOptions(folder, updatedOptions).toString());

        if (translatedOptionsText.getText().equals(""))
            translatedOptionsText.setText("Processing...");

        // start background job with a new job serial number. Serial number is needed
        // so that we only put the result of the latest job into the dialog.
        final int thisJobSerial = ++jobSerial;
        Job job = new Job("Scanning source files...") {
            @Override
            protected IStatus run(IProgressMonitor monitor) {
                try {
                    // calculate
                    final String translatedOptions = MetaMakemake.translateOptions(folder, updatedOptions).toString();

                    // display result if it's still relevant 
                    if (jobSerial == thisJobSerial) {
                        Display.getDefault().asyncExec(new Runnable() {
                            public void run() {
                                if (jobSerial == thisJobSerial && !translatedOptionsText.isDisposed()) {
                                    translatedOptionsText.setText(translatedOptions);
                                }
                            }});
                    }
                }
                catch (CoreException e) {
                    return new Status(IStatus.ERROR, Activator.PLUGIN_ID, 0, "An error occurred during processing makemake options.", e);
                }
                return Status.OK_STATUS;
            }
        };
        job.setPriority(Job.INTERACTIVE); // high priority
        job.schedule();
    }

    @SuppressWarnings("restriction")
    protected void gotoPathsAndSymbolsPage() {
        IPreferencePageContainer container = ownerPage.getContainer();
        if (container instanceof PropertyDialog)
            ((PropertyDialog)container).setCurrentPageId(PROPERTYPAGE_PATH_AND_SYMBOLS);
    }

    protected void setErrorMessage(String text) {
        ownerPage.setErrorMessage(text);
        ownerPage.setValid(text == null);
    }

    public void setOwner(PropertyPage page) {
        this.ownerPage = page;
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
        result.dllSymbol = dllSymbolText.getText().trim();

        // "Link" page
        result.userInterface = userInterfaceCombo.getText().trim();
        result.metaUseExportedLibs = useExportedLibsCheckbox.getSelection();
        result.metaLinkWithAllObjectsInProject = linkAllObjectsCheckbox.getSelection();
        result.libs.addAll(Arrays.asList(libsList.getItems()));
        result.extraArgs.addAll(Arrays.asList(linkObjectsList.getItems()));

        // "Custom" page
        result.fragmentFiles.addAll(Arrays.asList(makefragsList.getItems()));

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

    public String getMakefragvcContents() {
        return makefragvcText.getText();
    }

    protected String getHelpTextForBuildingDLLs() {
        return 
        "Unlike Linux shared libraries which can be built from any C/C++ code,\n" + 
        "Windows has special rules for code that goes into DLLs.\n" + 
        "When a DLL is built, all symbols (functions, global variables, etc.)\n" + 
        "that you want to expose as C/C++ API to users of your DLL need to be\n" + 
        "marked with the <tt>__declspec(dllexport)</tt> qualifier.\n" + 
        "Likewise, when you refer a symbol (function, variable, etc) that\n" + 
        "comes from a DLL, the C/C++ declaration of that symbol needs to be\n" + 
        "annotated with <tt>__declspec(dllimport)</tt>.\n" + 
        "\n" + 
        "<p>\n" + 
        "In OMNeT++, we introduce a convention to automate the process\n" + 
        "as far as possible, using macros. To build a DLL, you need to pick\n" + 
        "a short name for it, say <tt>FOO</tt>, and add the following code\n" + 
        "to a header file (say <tt>foodefs.h</tt>):\n" + 
        "\n" + 
        "<pre>\n" + 
        "#include &lt;omnetpp.h&gt;\n" + 
        "\n" + 
        "#if defined(FOO_EXPORT)\n" + 
        "#  define FOO_API OPP_DLLEXPORT\n" + 
        "#elif defined(FOO_IMPORT)\n" + 
        "#  define FOO_API OPP_DLLIMPORT\n" + 
        "#else\n" + 
        "#  define FOO_API\n" + 
        "#endif\n" + 
        "</pre>\n" + 
        "\n" + 
        "Then you need to include <tt>foodefs.h</tt> into all your header files, and\n" + 
        "annotate public symbols in them with <tt>FOO_API</tt>. When building the\n" + 
        "DLL, OMNeT++-generated makefiles will ensure that <tt>FOO_EXPORT</tt> is\n" + 
        "defined, and so <tt>FOO_API</tt> becomes <tt>__declspec(dllexport)</tt>.\n" + 
        "Likewise, when you use the DLL from external code, the makefile will define\n" + 
        "<tt>FOO_IMPORT</tt>, causing <tt>FOO_API</tt> to become\n" + 
        "<tt>__declspec(dllimport)</tt>. In all other cases, for example when compiling on\n" + 
        "Linux, <tt>FOO_API</tt> will be empty.\n" + 
        "\n" + 
        "<p>\n" + 
        "Here\'s how to annotate classes, functions and global variables:\n" + 
        "\n" + 
        "<pre>\n" + 
        "class FOO_API SomeClass {\n" + 
        "  ...\n" + 
        "};\n" + 
        "\n" + 
        "int FOO_API someFunction(double a, int b);\n" + 
        "\n" + 
        "extern int FOO_API globalVariable; //note: global variables are discouraged\n" + 
        "</pre>\n" +
        "If you have 3rd-party DLLs which use a different convention to handle " +
        "dllexport/dllimport, you need to manually specify the corresponding " +
        "macros on the \"Paths and symbols\" page of the project properties dialog."; 
    }
}
