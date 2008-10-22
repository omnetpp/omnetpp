package org.omnetpp.cdt.ui;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.cdt.core.model.CoreModel;
import org.eclipse.cdt.core.settings.model.CSourceEntry;
import org.eclipse.cdt.core.settings.model.ICConfigurationDescription;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.cdt.core.settings.model.util.CDataUtil;
import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.core.IManagedBuildInfo;
import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.runtime.Assert;
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

/**
 * This property page is shown for an OMNeT++ CDT Project, and lets the user 
 * manage the C++ configuration.
 *
 * @author Andras
 */
@SuppressWarnings("restriction")
public class ProjectMakemakePropertyPage extends PropertyPage {
    private static final String MAKEMAKE_SOURCE_FOLDER_IMG = "icons/full/obj16/folder_makemake.gif";
    private static final String CUSTOMMAKE_SOURCE_FOLDER_IMG = "icons/full/obj16/folder_custommake.gif";
    private static final String SOURCE_SUBFOLDER_IMG = "icons/full/obj16/folder_srcsubfolder.gif";
    private static final String CUSTOMMAKE_NONSRC_FOLDER_IMG = "icons/full/obj16/folder_nonsrc_custommake.gif";
    private static final String MAKEMAKE_NONSRC_FOLDER_IMG = "icons/full/obj16/folder_nonsrc_makemake.gif";
    private static final String EXCLUDED_FOLDER_IMG = "icons/full/obj16/folder_excluded.gif";
    private static final String NONSRC_FOLDER_IMG = "icons/full/obj16/folder_nonsrc.gif";
    
    // state
    protected BuildSpecification buildSpec;

    // controls
    protected TreeViewer treeViewer;
    private Button markAsToplevelButton;
    private Button optionsButton;
    private Button markAsSourceFolderButton;
    private Button removeSourceFolderButton;
    private Button excludeButton;
    private Button includeButton;
    private Button makemakeButton;
    private Button customMakeButton;
    private Button noMakeButton;

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
        Composite composite = new Composite(parent, SWT.NONE);
        composite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        
        composite.setLayout(new GridLayout(2,false));
        ((GridLayout)composite.getLayout()).marginWidth = 0;
        ((GridLayout)composite.getLayout()).marginHeight = 0;

        createLabel(composite, "Folders:", 2);
        treeViewer = new TreeViewer(composite, SWT.BORDER);
        treeViewer.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        ((GridData)treeViewer.getTree().getLayoutData()).widthHint = 300;

        Composite buttons = new Composite(composite, SWT.NONE);
        buttons.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, true));
        buttons.setLayout(new GridLayout(1,false));

        makemakeButton = createButton(buttons, SWT.PUSH, "Makemake", null);
        customMakeButton = createButton(buttons, SWT.PUSH, "Custom Make", null);
        noMakeButton = createButton(buttons, SWT.PUSH, "No Make", null);
        createLabel(buttons, "", 1);
        optionsButton = createButton(buttons, SWT.PUSH, "Options...", null);
        createLabel(buttons, "", 1);
        markAsToplevelButton = createButton(buttons, SWT.PUSH, "Mark as toplevel", null);
        createLabel(buttons, "", 1);
        markAsSourceFolderButton = createButton(buttons, SWT.PUSH, "Mark as Source Folder", null);
        removeSourceFolderButton = createButton(buttons, SWT.PUSH, "Remove Source Folder", null);
        excludeButton = createButton(buttons, SWT.PUSH, "Exclude", null);
        includeButton = createButton(buttons, SWT.PUSH, "Include", null);

        String text = "Source folder settings can also be managed on the Source Location tab of the <A>Paths and symbols</A> page.";
        Link pathsAndSymbolsLink = createLink(composite, text);
        pathsAndSymbolsLink.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, 2, 1));

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
        markAsSourceFolderButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                markAsSourceFolder(getTreeSelection());
            }
        });
        removeSourceFolderButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                removeSourceFolder(getTreeSelection());
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
                int makeType = buildSpec.getFolderMakeType(folder);

                ICProjectDescription projectDescription = CoreModel.getDefault().getProjectDescription(getProject());
                ICConfigurationDescription configuration = projectDescription.getActiveConfiguration();
                boolean isExcluded = CDataUtil.isExcluded(folder.getFullPath(), configuration.getSourceEntries());

                String iconPath = null;
                if (isExcluded) {
                    switch (makeType) {
                        case BuildSpecification.MAKEMAKE: iconPath = MAKEMAKE_NONSRC_FOLDER_IMG; break;
                        case BuildSpecification.CUSTOM: iconPath = CUSTOMMAKE_NONSRC_FOLDER_IMG; break;
                        case BuildSpecification.NONE: iconPath = NONSRC_FOLDER_IMG; break;
                    }
                }
                else {
                    switch (makeType) {
                        case BuildSpecification.MAKEMAKE: iconPath = MAKEMAKE_SOURCE_FOLDER_IMG; break;
                        case BuildSpecification.CUSTOM: iconPath = CUSTOMMAKE_SOURCE_FOLDER_IMG; break;
                        case BuildSpecification.NONE: iconPath = SOURCE_SUBFOLDER_IMG; break;
                    }
                }
                return Activator.getCachedImage(iconPath);
            }

            public String getText(Object element) {
                if (element instanceof IContainer)
                    return getLabelFor((IContainer) element);
                return element.toString();
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

    
    protected void setFolderMakeType(IContainer folder, int makeType) {
        buildSpec.setFolderMakeType(folder, makeType);
        updatePageState();
    }

    protected void markAsToplevel(IContainer folder) {
        // TODO write this folder into the "Build location" field (for all configurations)
    }

    protected void editFolderOptions(IContainer folder) {
        Assert.isTrue(buildSpec.getFolderMakeType(folder) == BuildSpecification.MAKEMAKE);
        MakemakeOptionsDialog dialog = new MakemakeOptionsDialog(getShell(), folder, buildSpec.getMakemakeOptions(folder));
        if (dialog.open() == Dialog.OK) {
            buildSpec.setMakemakeOptions(folder, dialog.getResult());
            updatePageState();
        }
    }

    protected void markAsSourceFolder(IContainer folder) {
        try {
            IProject project = getProject();
            ICProjectDescription projectDescription = CoreModel.getDefault().getProjectDescription(project);
            for (ICConfigurationDescription configuration : projectDescription.getConfigurations()) {
                ICSourceEntry[] entries = configuration.getSourceEntries();
                ICSourceEntry entry = new CSourceEntry(folder.getProjectRelativePath(), new IPath[0], 0);
                entries = (ICSourceEntry[]) ArrayUtils.add(entries, entry);
                configuration.setSourceEntries(entries);
            }
            CoreModel.getDefault().setProjectDescription(project, projectDescription);
        }
        catch (CoreException e) {
            //TODO errordialog
            Activator.logError(e);
        }
        catch (Exception e) {
            Activator.logError(e);
        }
        updatePageState();
    }

    protected void removeSourceFolder(IContainer folder) {
        try {
            IProject project = getProject();
            ICProjectDescription projectDescription = CoreModel.getDefault().getProjectDescription(project);
            for (ICConfigurationDescription configuration : projectDescription.getConfigurations()) {
                ICSourceEntry[] entries = configuration.getSourceEntries();
                for (int i=0; i<entries.length; i++) 
                    if (entries[i].getFullPath().equals(folder.getFullPath()))
                        entries = (ICSourceEntry[]) ArrayUtils.remove(entries, i--);
                configuration.setSourceEntries(entries);
            }
            CoreModel.getDefault().setProjectDescription(project, projectDescription);
        }
        catch (CoreException e) {
            //TODO errordialog
            Activator.logError(e);
        }
        catch (Exception e) {
            Activator.logError(e);
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
                ICSourceEntry[] newEntries = CDataUtil.setExcluded(folder.getFullPath(), true, exclude, configuration.getSourceEntries());
                configuration.setSourceEntries(newEntries);
            }
            CoreModel.getDefault().setProjectDescription(project, projectDescription); //FIXME use CDT property page manager stuff, like on the other makemake options panel
        }
        catch (CoreException e) {
            //TODO errordialog
            Activator.logError(e);
        }
        catch (Exception e) {
            Activator.logError(e);
        }
        updatePageState();
    }

//XXX maybe useful fragments
//    List<IContainer> sourceFolders = CDTUtils.getSourceFolders(getProject().getProject());
//    boolean isUnderSrcFolder = false;
//    for (IContainer tmp = folder; !(tmp instanceof IProject); tmp = tmp.getParent())
//        if (sourceFolders.contains(tmp))
//            isUnderSrcFolder = true;

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

    protected String getLabelFor(IContainer folder) {
        String result = folder.getName();

        if (buildSpec.getFolderMakeType(folder)==BuildSpecification.MAKEMAKE) {
            MakemakeOptions makemakeOptions = buildSpec.getMakemakeOptions(folder);
            result += ": makemake " + makemakeOptions.toString(); //FIXME show target name instead ("-> libINET.so") or something
        }
        if (buildSpec.getFolderMakeType(folder)==BuildSpecification.CUSTOM) {
            result += ": custom makefile";
        }
        return result;
    }
    
    protected void updatePageState() {
        treeViewer.refresh();
        
        IContainer folder = getTreeSelection();
        if (folder == null) {
            makemakeButton.setEnabled(false);
            customMakeButton.setEnabled(false);
            noMakeButton.setEnabled(false);
            markAsToplevelButton.setEnabled(false);
            optionsButton.setEnabled(false);
            markAsSourceFolderButton.setEnabled(false);
            removeSourceFolderButton.setEnabled(false);
            excludeButton.setEnabled(false);
            includeButton.setEnabled(false);
            return;
        }
        
        IProject project = getProject();

        int makeType = buildSpec.getFolderMakeType(folder);
        makemakeButton.setEnabled(makeType!=BuildSpecification.MAKEMAKE);
        customMakeButton.setEnabled(makeType!=BuildSpecification.CUSTOM);
        noMakeButton.setEnabled(makeType!=BuildSpecification.NONE);
        
        markAsToplevelButton.setEnabled(makeType!=BuildSpecification.NONE);
        optionsButton.setEnabled(makeType==BuildSpecification.MAKEMAKE);
        
        boolean isSourceFolder = CDTUtils.getSourceFolders(project).contains(folder);
        markAsSourceFolderButton.setEnabled(!isSourceFolder);
        removeSourceFolderButton.setEnabled(isSourceFolder);
        
        ICProjectDescription projectDescription = CoreModel.getDefault().getProjectDescription(project);
        ICConfigurationDescription configuration = projectDescription.getActiveConfiguration();
        boolean isExcluded = CDataUtil.isExcluded(folder.getFullPath(), configuration.getSourceEntries());
        
        excludeButton.setEnabled(!isExcluded);  //FIXME not really...
        includeButton.setEnabled(isExcluded); //FIXME not really: only enable if exactly matches an exclusion pattern!
    }

    protected IContainer getTreeSelection() {
        IContainer folder = (IContainer)((IStructuredSelection)treeViewer.getSelection()).getFirstElement();
        return folder;
    }

    protected boolean isDirty() {
        return false;
    }

    protected String getInformationalMessage() {
        // Check CDT settings
        IManagedBuildInfo buildInfo = ManagedBuildManager.getBuildInfo(getProject().getProject());
        if (buildInfo == null)
            return "Cannot access CDT build information for this project. Is this a C/C++ project?";
        IConfiguration configuration = buildInfo.getDefaultConfiguration();
        if (configuration == null)
            return "No active build configuration -- please create one.";
        if (configuration.isManagedBuildOn())
            return "CDT Managed Build is on! Please turn off CDT's Makefile generation on the C/C++ Build page.";
        String builderId = configuration.getBuilder()!=null ? configuration.getBuilder().getId() : null; 
        if (builderId==null)
            return "Cannot determine CDT builder Id. Is the project properly configured for C/C++?";
        if (!builderId.startsWith("org.omnetpp.cdt."))
            return "C/C++ configuration of this project is not suitable for OMNeT++. To fix that, " +
            		"delete the project from the workspace (keeping the files), and re-create it " +
            		"with the New OMNeT++ Project wizard, overwriting existing project settings.";
        
        //XXX "Out" dir should not overlap with source folders (check!!!)
        
        //XXX return some message to be displayed to the user, if:
        //  - CDT make folder is not the project root (or: if a makefile is unreachable)
        //  - makefile consistency error (i.e. a subdir doesn't contain a makefile)
        //  - something else is wrong?
        
        //FIXME revise/remove or something...        
        // IContainer ancestorMakemakeFolder = ancestorMakemakeFolder();
        // if (ancestorMakemakeFolder != null) {
        //     MakemakeOptions ancestorMakemakeOptions = buildSpec.getMakemakeOptions(ancestorMakemakeFolder);
        //     if (ancestorMakemakeOptions.isDeep /*FIXME and this folder is not excluded manually from it*/)
        //     return "This folder is already covered by Makefile in: " + ancestorMakemakeFolder.getFullPath(); //XXX clean and disable checkbox too?
        // }

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
        //TODO
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

