package org.omnetpp.cdt.ui;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.core.IManagedBuildInfo;
import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.preference.IPreferencePageContainer;
import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
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
    private enum FolderType {
        MAKEMAKE_SOURCE_FOLDER, 
        CUSTOMMAKE_SOURCE_FOLDER,
        SOURCE_SUBFOLDER,
        CUSTOMMAKE_NONSRC_FOLDER,
        MAKEMAKE_NONSRC_FOLDER,
        EXCLUDED_FOLDER,
        NONSRC_FOLDER,
    }
    
    private static final Image MAKEMAKE_SOURCE_FOLDER_IMG = Activator.getCachedImage("icons/full/obj16/folder_makemake.gif");
    private static final Image CUSTOMMAKE_SOURCE_FOLDER_IMG = Activator.getCachedImage("icons/full/obj16/folder_custommake.gif");
    private static final Image SOURCE_SUBFOLDER_IMG = Activator.getCachedImage("icons/full/obj16/folder_srcsubfolder.gif");
    private static final Image CUSTOMMAKE_NONSRC_FOLDER_IMG = Activator.getCachedImage("icons/full/obj16/folder_nonsrc_custommake.gif");
    private static final Image MAKEMAKE_NONSRC_FOLDER_IMG = Activator.getCachedImage("icons/full/obj16/folder_nonsrc_makemake.gif");
    private static final Image EXCLUDED_FOLDER_IMG = Activator.getCachedImage("icons/full/obj16/folder_excluded.gif");
    private static final Image NONSRC_FOLDER_IMG = Activator.getCachedImage("icons/full/obj16/folder_nonsrc.gif");
    
    // state
    protected BuildSpecification buildSpec;

    // controls
    protected TreeViewer treeViewer;

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
        final Group group = new Group(parent, SWT.NONE);
        group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        group.setLayout(new GridLayout(1,false));

        createLabel(group, "Folders:");
        treeViewer = new TreeViewer(group, SWT.BORDER);
        treeViewer.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        treeViewer.setContentProvider(new WorkbenchContentProvider() {
            @Override
            public Object[] getChildren(Object element) {
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
                FolderType folderType = getFolderType((IContainer)element);
                switch (folderType) {
                    case MAKEMAKE_SOURCE_FOLDER: return MAKEMAKE_SOURCE_FOLDER_IMG; 
                    case CUSTOMMAKE_SOURCE_FOLDER: return CUSTOMMAKE_SOURCE_FOLDER_IMG;
                    case SOURCE_SUBFOLDER: return SOURCE_SUBFOLDER_IMG;
                    case CUSTOMMAKE_NONSRC_FOLDER: return CUSTOMMAKE_NONSRC_FOLDER_IMG;
                    case MAKEMAKE_NONSRC_FOLDER: return MAKEMAKE_NONSRC_FOLDER_IMG;
                    case EXCLUDED_FOLDER: return EXCLUDED_FOLDER_IMG;
                    case NONSRC_FOLDER: return NONSRC_FOLDER_IMG;
                    default: return null; // cannot happen
                }
            }

            public String getText(Object element) {
                if (element instanceof IContainer)
                    return getLabelFor((IContainer) element);
                return element.toString();
            }
        });
        
        loadBuildSpecFile();

        treeViewer.setInput(getFolder().getProject());
        updatePageState();
        return group;
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

    protected Label createLabel(Composite composite, String text) {
        Label label = new Label(composite, SWT.NONE);
        label.setText(text);
        label.setLayoutData(new GridData());
        return label;
    }

    protected Button createButton(Composite parent, int style, String text, String tooltip) {
        Button button = new Button(parent, style);
        button.setText(text);
        if (tooltip != null)
            button.setToolTipText(tooltip);
        button.setLayoutData(new GridData());
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

    public FolderType getFolderType(IContainer folder) {
        List<IContainer> sourceFolders = CDTUtils.getSourceFolders(getFolder().getProject());
       
        boolean isUnderSrcFolder = false;
        for (IContainer tmp = folder; !(tmp instanceof IProject); tmp = tmp.getParent())
            if (sourceFolders.contains(tmp))
                isUnderSrcFolder = true;

        //FIXME simplified....
        if (sourceFolders.contains(folder)) {
            if (ArrayUtils.contains(buildSpec.getMakemakeFolders(), folder))
                return FolderType.MAKEMAKE_SOURCE_FOLDER;
            else
                return FolderType.CUSTOMMAKE_SOURCE_FOLDER;
        }
        else if (ArrayUtils.contains(buildSpec.getMakemakeFolders(), folder))
            return FolderType.MAKEMAKE_NONSRC_FOLDER;
        else if (isUnderSrcFolder)
            return FolderType.SOURCE_SUBFOLDER;
        else
            return FolderType.NONSRC_FOLDER; // not exactly...
    }

    protected String getLabelFor(IContainer folder) {
        String result = folder.getName();
        
        if (ArrayUtils.contains(buildSpec.getMakemakeFolders(), folder)) {
            MakemakeOptions makemakeOptions = buildSpec.getMakemakeOptions(folder);
            result += " -- " + makemakeOptions.toString();
        }
        return result;
    }
    
    protected void updatePageState() {
        //TODO
    }

    protected boolean isDirty() {
        return false;
    }

    protected String getInformationalMessage() {
        // Check CDT settings
        IManagedBuildInfo buildInfo = ManagedBuildManager.getBuildInfo(getFolder().getProject());
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
    protected IContainer getFolder() {
        return (IContainer) getElement().getAdapter(IContainer.class);
    }

    @Override
    public boolean performOk() {
        // note: performApply() delegates here too
        //TODO
        saveBuildSpecFile();
        return true;
    }

    protected void loadBuildSpecFile() {
        IProject project = getFolder().getProject();
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

