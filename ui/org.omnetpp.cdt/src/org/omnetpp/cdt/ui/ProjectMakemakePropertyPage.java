package org.omnetpp.cdt.ui;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.cdt.core.model.CoreModel;
import org.eclipse.cdt.core.settings.model.ICConfigurationDescription;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.core.settings.model.ICSettingEntry;
import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.cdt.core.settings.model.util.CDataUtil;
import org.eclipse.cdt.managedbuilder.core.IBuilder;
import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.core.IManagedBuildInfo;
import org.eclipse.cdt.managedbuilder.core.IToolChain;
import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.preference.IPreferencePageContainer;
import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ControlAdapter;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Link;
import org.eclipse.ui.dialogs.PropertyPage;
import org.eclipse.ui.internal.dialogs.PropertyDialog;
import org.eclipse.ui.model.WorkbenchContentProvider;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.CDTUtils;
import org.omnetpp.cdt.makefile.BuildSpecification;
import org.omnetpp.cdt.makefile.MakefileTools;
import org.omnetpp.cdt.makefile.MakemakeOptions;
import org.omnetpp.cdt.makefile.MakemakeOptions.Type;
import org.omnetpp.common.color.ColorFactory;

/**
 * This property page is shown for an OMNeT++ CDT Project, and lets the user 
 * manage the C++ configuration.
 *
 * @author Andras
 */
//FIXME source folder changes immediatly get committed (no cancel!), but they aren't visible in cdt page. use CDTPropertyManager!
//FIXME improve tree labels (should be libname etc)
//FIXME warning if there's no makefile in a "custom makefile" folder
//XXX makemakeoptions: "copy to bin/ or lib/" option
//XXX support makemake -K option
//XXX per-makefile targets
//FIXME project template-kben a buildspecet atirni!!!
//XXX warn if there's no makefile generated at all
//XXX implement "mark as top"
//XXX multiselect?
//XXX hover!! "no makefile generated" etc!
//XXX print hint somewhere: project root should probably be excluded if there's a submakefile!
//XXX    "Compile" fulre: ha nem akarod hogy barmit forditson, excludold ki a makefile foldert!
//XXX makemake: add type=NONE ?
//XXX Options preview lapra kitenni helpet (melyik opcio mit csinal)
//XXX hintet kiirni: folderek kozotti sorrendet a makefrag-ban kellene megadni
//XXX hintet kiirni: source location tree (tele/ures folder) es makefile tree teljesen fuggetlen
//XXX recurse: valahova kiirni hogy tenylegesen hova fog bemenni (ne csak az options listbol legyen)
@SuppressWarnings("restriction")
public class ProjectMakemakePropertyPage extends PropertyPage {
    private static final String SOURCE_FOLDER_IMG = "icons/full/obj16/folder_srcfolder.gif";
    private static final String SOURCE_SUBFOLDER_IMG = "icons/full/obj16/folder_srcsubfolder.gif";
    private static final String NONSRC_FOLDER_IMG = "icons/full/obj16/folder_nonsrc.gif";
    
    protected static final String OVR_MAKEMAKE_IMG = "icons/full/obj16/ovr_makemake.png";
    protected static final String OVR_CUSTOMMAKE_IMG = "icons/full/obj16/ovr_custommake.png";
    
    // state
    protected BuildSpecification buildSpec;

    // controls
    protected Label errorMessageLabel;
    protected TreeViewer treeViewer;
    protected Button markAsToplevelButton;
    protected Button optionsButton;
    protected Button sourceLocationButton;
    protected Button excludeButton;
    protected Button includeButton;
    protected Button makemakeButton;
    protected Button customMakeButton;
    protected Button noMakeButton;

    /**
     * Constructor.
     */
    public ProjectMakemakePropertyPage() {
        super();
    }

    /**
     * @see PreferencePage#createContents(Composite)
     */
    protected Control createContents(Composite parent) {
        final Composite composite = new Composite(parent, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        
        composite.setLayout(new GridLayout(2,false));
        ((GridLayout)composite.getLayout()).marginWidth = 0;
        ((GridLayout)composite.getLayout()).marginHeight = 0;

        createLabel(composite, "Configure folders below. NOTE: changes will affect all configurations.", 2);
        String text = "Source Locations can also be managed on the <A>Paths and symbols</A> page.";
        Link pathsAndSymbolsLink = createLink(composite, text);
        pathsAndSymbolsLink.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, 2, 1));

        errorMessageLabel = new Label(composite, SWT.WRAP);
        errorMessageLabel.setForeground(ColorFactory.RED2);
        errorMessageLabel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, false, 2, 1));
        ((GridData)errorMessageLabel.getLayoutData()).widthHint = 300;

        treeViewer = new TreeViewer(composite, SWT.BORDER);
        treeViewer.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        ((GridData)treeViewer.getTree().getLayoutData()).widthHint = 300;

        Composite buttons = new Composite(composite, SWT.NONE);
        buttons.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, true));
        buttons.setLayout(new GridLayout(1,false));

        makemakeButton = createButton(buttons, SWT.PUSH, "Makemake", "Enable makefile generation in this folder");
        customMakeButton = createButton(buttons, SWT.PUSH, "Custom Make", "Folder contains custom makefile");
        noMakeButton = createButton(buttons, SWT.PUSH, "No Make", "No makefile in this folder");
        createLabel(buttons, "", 1);
        optionsButton = createButton(buttons, SWT.PUSH, "Options...", "Edit makefile generation options");
        createLabel(buttons, "", 1);
        markAsToplevelButton = createButton(buttons, SWT.PUSH, "Mark as toplevel", "Build command will invoke this makefile");
        createLabel(buttons, "", 1);
        sourceLocationButton = createButton(buttons, SWT.PUSH, "Source Location", "Mark folder as source location");
        excludeButton = createButton(buttons, SWT.PUSH, "Exclude", "Exclude from build");
        includeButton = createButton(buttons, SWT.PUSH, "Include", "Include into build");

        pathsAndSymbolsLink.addSelectionListener(new SelectionListener(){
            public void widgetSelected(SelectionEvent e) {
                gotoPathsAndSymbolsPage();
            }
            public void widgetDefaultSelected(SelectionEvent e) {
                gotoPathsAndSymbolsPage();
            }
        });

        makemakeButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                setFolderMakeType(getTreeSelection(), BuildSpecification.MAKEMAKE);
            }
        });
        customMakeButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                setFolderMakeType(getTreeSelection(), BuildSpecification.CUSTOM);
            }
        });
        noMakeButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                setFolderMakeType(getTreeSelection(), BuildSpecification.NONE);
            }
        });
        markAsToplevelButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                markAsToplevel(getTreeSelection());
            }
        });
        optionsButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                editFolderOptions(getTreeSelection());
            }
        });
        sourceLocationButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                markAsSourceLocation(getTreeSelection());
            }
        });
        excludeButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                excludeFolder(getTreeSelection());
            }
        });
        includeButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                includeFolder(getTreeSelection());
            }
        });

        treeViewer.setContentProvider(new WorkbenchContentProvider() {
            @Override
            public Object[] getChildren(Object element) {
                if (element instanceof IWorkspaceRoot)
                    return new Object[] {getProject()};
                List<Object> result = new ArrayList<Object>();
                for (Object object : super.getChildren(element))
                    if (object instanceof IContainer && MakefileTools.isGoodFolder((IContainer)object))
                        result.add(object);
                return result.toArray();
            }
        });
        
        treeViewer.setLabelProvider(new LabelProvider() {
            public Image getImage(Object element) {
                if (!(element instanceof IContainer))
                    return null;

                IContainer folder = (IContainer)element;
                ICProjectDescription projectDescription = CoreModel.getDefault().getProjectDescription(getProject());
                ICConfigurationDescription configuration = projectDescription.getActiveConfiguration();
                boolean isSrcFolder = CDTUtils.getSourceEntryFor(folder, configuration.getSourceEntries())!=null;
                boolean isExcluded = CDTUtils.isExcluded(folder, configuration.getSourceEntries());
                String imagePath = isSrcFolder ? SOURCE_FOLDER_IMG : !isExcluded ? SOURCE_SUBFOLDER_IMG : NONSRC_FOLDER_IMG;

                int makeType = buildSpec.getFolderMakeType(folder);
                String overlayImagePath = null; 
                switch (makeType) {
                    case BuildSpecification.MAKEMAKE: overlayImagePath = OVR_MAKEMAKE_IMG; break;
                    case BuildSpecification.CUSTOM: overlayImagePath = OVR_CUSTOMMAKE_IMG; break;
                    case BuildSpecification.NONE: overlayImagePath = null; break;
                }
                return Activator.getCachedDecoratedImage(imagePath, overlayImagePath, SWT.END, SWT.END);
            }

            public String getText(Object element) {
                if (element instanceof IContainer)
                    return getLabelFor((IContainer) element);
                return element.toString();
            }
        });

        treeViewer.getTree().addSelectionListener(new SelectionAdapter() {
            public void widgetDefaultSelected(SelectionEvent e) {
                editFolderOptions(getTreeSelection());
            }
        });
        
        // make the error text label wrap properly; see https://bugs.eclipse.org/bugs/show_bug.cgi?id=9866
        composite.addControlListener(new ControlAdapter(){
            public void controlResized(ControlEvent e){
                GridData data = (GridData)errorMessageLabel.getLayoutData();
                GridLayout layout = (GridLayout)composite.getLayout();
                data.widthHint = composite.getClientArea().width - 2*layout.marginWidth;
                composite.layout(true);
            }
        });
        
        loadBuildSpecFile();

        treeViewer.setInput(getProject().getParent());
        treeViewer.expandToLevel(2);
        
        treeViewer.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                updatePageState();
            }
        });
        
        updatePageState();
        return composite;
    }

    protected String getLabelFor(IContainer folder) {
        String result = folder.getName();

        if (buildSpec.getFolderMakeType(folder)==BuildSpecification.MAKEMAKE) {
            MakemakeOptions options = buildSpec.getMakemakeOptions(folder);
            
            String target = options.target == null ? folder.getProject().getName() : options.target;
            switch (options.type) {
                case NOLINK: target = null; break;
                case EXE: target += " (executable)"; break;
                case SHAREDLIB: target += " (dynamic lib)"; break;
                case STATICLIB: target += " (static lib)"; break;
            }

            ICProjectDescription projectDescription = CoreModel.getDefault().getProjectDescription(folder.getProject());
            ICConfigurationDescription configuration = projectDescription.getActiveConfiguration();
            boolean isExcluded = CDTUtils.isExcluded(folder, configuration.getSourceEntries());

            result += 
                ": makemake (" + 
                (isExcluded ? "no src" : options.isDeep ? "deep" : "shallow") + ", "+ 
                (options.metaRecurse ? "recurse" : "no-recurse") + ")" +
                (target==null ? "" : " --> " + target);
        }
        if (buildSpec.getFolderMakeType(folder)==BuildSpecification.CUSTOM) {
            result += ": custom makefile";
        }
        return result;
    }
    
    protected void setFolderMakeType(IContainer folder, int makeType) {
        buildSpec.setFolderMakeType(folder, makeType);
        if (makeType == BuildSpecification.MAKEMAKE) {
            ICProjectDescription projectDescription = CoreModel.getDefault().getProjectDescription(folder.getProject());
            ICConfigurationDescription configuration = projectDescription.getActiveConfiguration();
            boolean isExcluded = CDTUtils.isExcluded(folder, configuration.getSourceEntries());
            if (isExcluded)
                buildSpec.getMakemakeOptions(folder).type = Type.NOLINK;
        }
        updatePageState();
    }

    protected void markAsToplevel(IContainer folder) {
        // TODO write this folder into the "Build location" field (for all configurations)
    }

    protected void editFolderOptions(IContainer folder) {
        if (buildSpec.getFolderMakeType(folder) != BuildSpecification.MAKEMAKE)
            return;
        MakemakeOptionsDialog dialog = new MakemakeOptionsDialog(getShell(), folder, buildSpec.getMakemakeOptions(folder), buildSpec.getMakeFolders());
        if (dialog.open() == Dialog.OK) {
            buildSpec.setMakemakeOptions(folder, dialog.getResult());
            updatePageState();
        }
    }

    protected void markAsSourceLocation(IContainer folder) {
        try {
            IProject project = getProject();
            ICProjectDescription projectDescription = CoreModel.getDefault().getProjectDescription(project);
            for (ICConfigurationDescription configuration : projectDescription.getConfigurations()) {
                ICSourceEntry[] entries = configuration.getSourceEntries();
                ICSourceEntry entry = (ICSourceEntry)CDataUtil.createEntry(ICSettingEntry.SOURCE_PATH, folder.getProjectRelativePath().toString(), folder.getProjectRelativePath().toString(), new IPath[0], ICSettingEntry.VALUE_WORKSPACE_PATH);
                entries = (ICSourceEntry[]) ArrayUtils.add(entries, entry);
                configuration.setSourceEntries(entries);
            }
            CoreModel.getDefault().setProjectDescription(project, projectDescription);
        }
        catch (CoreException e) {
            errorDialog(e.getMessage(), e);
        }
        catch (Exception e) {
            errorDialog(e.getMessage(), e);
        }
        updatePageState();
    }

    //note: this method is unused -- the "Exclude" button seems to work just fine
    protected void removeSourceLocation(IContainer folder) {
        try {
            IProject project = getProject();
            ICProjectDescription projectDescription = CoreModel.getDefault().getProjectDescription(project);
            for (ICConfigurationDescription configuration : projectDescription.getConfigurations()) {
                ICSourceEntry[] entries = configuration.getSourceEntries();
                ICSourceEntry entry = CDTUtils.getSourceEntryFor(folder, entries);
                entries = (ICSourceEntry[]) ArrayUtils.removeElement(entries, entry); // works for null too
                configuration.setSourceEntries(entries);
            }
            CoreModel.getDefault().setProjectDescription(project, projectDescription);
        }
        catch (CoreException e) {
            errorDialog(e.getMessage(), e);
        }
        catch (Exception e) {
            errorDialog(e.getMessage(), e);
        }
        updatePageState();
    }

    protected void excludeFolder(IContainer folder) {
        setExcluded(folder, true);
    }

    protected void includeFolder(IContainer folder) {
        setExcluded(folder, false);
    }

    protected void setExcluded(IContainer folder, boolean exclude) {
        try {
            IProject project = getProject();
            ICProjectDescription projectDescription = CoreModel.getDefault().getProjectDescription(project);
            for (ICConfigurationDescription configuration : projectDescription.getConfigurations()) {
                ICSourceEntry[] newEntries = CDTUtils.setExcluded(folder, exclude, configuration.getSourceEntries());
                configuration.setSourceEntries(newEntries);
            }
            CoreModel.getDefault().setProjectDescription(project, projectDescription);
        }
        catch (CoreException e) {
            errorDialog(e.getMessage(), e);
        }
        catch (Exception e) {
            errorDialog(e.getMessage(), e);
        }
        updatePageState();
    }

    protected void gotoPathsAndSymbolsPage() {
        IPreferencePageContainer container = getContainer();
        if (container instanceof PropertyDialog)
            ((PropertyDialog)container).setCurrentPageId(MakemakeOptionsPanel.PROPERTYPAGE_PATH_AND_SYMBOLS);
    }

    protected Group createGroup(Composite composite, String text, int numColumns) {
        Group group = new Group(composite, SWT.NONE);
        group.setText(text);
        group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        group.setLayout(new GridLayout(numColumns,false));
        return group;
    }

    protected Label createLabel(Composite composite, String text, int hspan) {
        Label label = new Label(composite, SWT.NONE);
        label.setText(text);
        label.setLayoutData(new GridData());
        ((GridData)label.getLayoutData()).horizontalSpan = hspan;
        return label;
    }

    protected Button createButton(Composite parent, int style, String text, String tooltip) {
        Button button = new Button(parent, style);
        button.setText(text);
        if (tooltip != null)
            button.setToolTipText(tooltip);
        button.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        return button;
    }

    protected Link createLink(Composite composite, String text) {
        Link link = new Link(composite, SWT.NONE);
        link.setText(text);
        link.setLayoutData(new GridData());
        return link;
    }

    @Override
    public void setVisible(boolean visible) {
        super.setVisible(visible);
        if (visible == true)
            updatePageState();
    }

    protected void updatePageState() {
        // display warnings about CDT misconfiguration, etc 
        String message = getDiagnosticMessage(getProject());
        errorMessageLabel.setText(message==null ? "" : message);
        ((GridData)errorMessageLabel.getLayoutData()).exclude = (message==null); 
        errorMessageLabel.setVisible(message!=null);
        errorMessageLabel.getParent().layout(true);
        
        treeViewer.refresh();
        
        // if there's no tree selection, disable all buttons
        IContainer folder = getTreeSelection();
        if (folder == null) {
            makemakeButton.setEnabled(false);
            customMakeButton.setEnabled(false);
            noMakeButton.setEnabled(false);
            markAsToplevelButton.setEnabled(false);
            optionsButton.setEnabled(false);
            sourceLocationButton.setEnabled(false);
            excludeButton.setEnabled(false);
            includeButton.setEnabled(false);
            return;
        }
        
        // enable/disable buttons
        IProject project = getProject();
        int makeType = buildSpec.getFolderMakeType(folder);
        makemakeButton.setEnabled(makeType!=BuildSpecification.MAKEMAKE);
        customMakeButton.setEnabled(makeType!=BuildSpecification.CUSTOM);
        noMakeButton.setEnabled(makeType!=BuildSpecification.NONE);
        
        markAsToplevelButton.setEnabled(makeType!=BuildSpecification.NONE);
        optionsButton.setEnabled(makeType==BuildSpecification.MAKEMAKE);
        
        boolean isSourceLocation = CDTUtils.getSourceLocations(project).contains(folder);
        sourceLocationButton.setEnabled(!isSourceLocation);
        
        ICProjectDescription projectDescription = CoreModel.getDefault().getProjectDescription(project);
        ICConfigurationDescription configuration = projectDescription.getActiveConfiguration();
        boolean isExcluded = CDTUtils.isExcluded(folder, configuration.getSourceEntries());
        boolean isUnderSourceLocation = CDTUtils.getSourceEntryThatCovers(folder, configuration.getSourceEntries()) != null;
        
        excludeButton.setEnabled(!isExcluded);
        includeButton.setEnabled(isUnderSourceLocation && isExcluded);
    }

    protected IContainer getTreeSelection() {
        IContainer folder = (IContainer)((IStructuredSelection)treeViewer.getSelection()).getFirstElement();
        return folder;
    }

    protected boolean isDirty() {
        return false;
    }

    public static String getDiagnosticMessage(IContainer folder) {
        // Check CDT settings
        IManagedBuildInfo buildInfo = ManagedBuildManager.getBuildInfo(folder.getProject());
        if (buildInfo == null)
            return "Cannot access CDT build information for this project. Is this a C/C++ project?";
        IConfiguration configuration = buildInfo.getDefaultConfiguration();
        if (configuration == null)
            return "No active build configuration -- please create one.";
        boolean isOmnetppConfig = false;
        for (IConfiguration c = configuration; c != null; c = c.getParent())
            if (c.getId().startsWith("org.omnetpp.cdt.")) {
                isOmnetppConfig = true; break;}
        if (!isOmnetppConfig)
            return "The active build configuration \""+ configuration.getName() + "\" is " +
            		"not an OMNeT++ configuration. Please re-create the project it with the " +
            		"New OMNeT++ Project wizard, overwriting the existing project settings.";
        boolean isOmnetppToolchain = false;
        for (IToolChain tc = configuration.getToolChain(); tc != null; tc = tc.getSuperClass())
            if (tc.getId().startsWith("org.omnetpp.cdt.")) {
                isOmnetppToolchain = true; break;}
        if (!isOmnetppToolchain)
            return "The active toolchain \""+ configuration.getToolChain().getName() + "\" is " +
            		"not suitable for OMNeT++. Please re-create the project it with the " +
                    "New OMNeT++ Project wizard, overwriting the existing project settings.";

        if (configuration.isManagedBuildOn())
            return "CDT Managed Build is on! Please turn off CDT's Makefile generation on the C/C++ Build page.";
        IBuilder builder = configuration.getBuilder();
        if (builder == null)
            return "No CDT Project Builder. Activate one on the C/C++ Build / Tool Chain Editor page.";
        String builderId = builder.getId();
        if (builderId==null || !builderId.startsWith("org.omnetpp.cdt."))
            return "C/C++ Builder \"" + builder.getName()+ "\" set in the active build configuration " +
                   "is not suitable for OMNeT++. Please re-create the project with the " +
                   "New OMNeT++ Project wizard, overwriting the existing project settings.";
        
        //XXX "Out" dir should not overlap with source folders (check!!!)
        
        //XXX return some message to be displayed to the user, if:
        //  - CDT make folder is not the project root (or: if a makefile is unreachable)
        //  - makefile consistency error (i.e. a subdir doesn't contain a makefile)
        //  - something else is wrong?

        return null;
    }

    /**
     * The resource for which the Properties dialog was brought up.
     */
    protected IProject getProject() {
        return (IProject) getElement().getAdapter(IProject.class);
    }

    @Override
    public boolean performOk() {
        // note: performApply() delegates here too
        saveBuildSpecFile();
        return true;
    }

    protected void loadBuildSpecFile() {
        IProject project = getProject().getProject();
        try {
            buildSpec = BuildSpecification.readBuildSpecFile(project);
        } 
        catch (CoreException e) {
            errorDialog("Cannot read build specification, reverting page content to the default settings.", e);
        }

        if (buildSpec == null)
            buildSpec = BuildSpecification.createBlank(project);
    }

    protected void saveBuildSpecFile() {
        try {
            buildSpec.save();
        } 
        catch (CoreException e) {
            errorDialog("Cannot store build specification", e);
        }
    } 

    protected void errorDialog(String message, Throwable e) {
        Activator.logError(message, e);
        IStatus status = new Status(IMarker.SEVERITY_ERROR, Activator.PLUGIN_ID, e.getMessage(), e);
        ErrorDialog.openError(Display.getCurrent().getActiveShell(), "Error", message, status);
    }
}

