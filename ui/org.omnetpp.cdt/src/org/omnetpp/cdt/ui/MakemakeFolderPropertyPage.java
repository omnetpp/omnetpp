package org.omnetpp.cdt.ui;

import java.io.IOException;
import java.util.List;

import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.core.IManagedBuildInfo;
import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.dialogs.PropertyPage;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.CDTUtils;
import org.omnetpp.cdt.makefile.BuildSpecUtils;
import org.omnetpp.cdt.makefile.BuildSpecification;
import org.omnetpp.cdt.makefile.MakemakeOptions;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;

/**
 * This property page is shown for folders in an OMNeT++ CDT Project, and lets the user 
 * edit the contents of the ".oppbuildspec" file.
 *
 * @author Andras
 */
//XXX add [Make source folder] [Remove source folder] buttons next to the combo box...
//XXX add link which goes to the "paths & symbols" page... (if it has the "source path" tab!!!)
//XXX before switching away from a folder, ask whether to save changes!
//XXX --deep should be the default!
public class MakemakeFolderPropertyPage extends PropertyPage {
    public static final String MAKEFRAG_FILENAME = "makefrag";
    public static final String MAKEFRAGVC_FILENAME = "makefrag.vc";

    // state
    protected BuildSpecification buildSpec;
    protected IContainer selectedSourceFolder; // follows combo box; null if selection is not a source folder

    // controls
    protected Combo sourceFolderCombo;
    protected Text errorMessageText;
    protected MakemakeOptionsPanel contents;

    /**
     * Constructor.
     */
    public MakemakeFolderPropertyPage() {
        super();
    }

    /**
     * @see PreferencePage#createContents(Composite)
     */
    protected Control createContents(Composite parent) {
        Group group = new Group(parent, SWT.NONE);
        group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        group.setLayout(new GridLayout(2,false));
        createLabel(group, "Source folder:");
        sourceFolderCombo = new Combo(group, SWT.READ_ONLY | SWT.BORDER);
        sourceFolderCombo.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        errorMessageText = new Text(group, SWT.MULTI);
        errorMessageText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        ((GridData)errorMessageText.getLayoutData()).horizontalSpan = 2;
        errorMessageText.setEditable(false);
        errorMessageText.setForeground(ColorFactory.RED2);
        errorMessageText.setBackground(errorMessageText.getDisplay().getSystemColor(SWT.COLOR_WIDGET_BACKGROUND));

        contents = new MakemakeOptionsPanel(parent, SWT.NONE); 
        contents.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        sourceFolderCombo.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                comboSelectionChanged();
            } 
        });

        contents.setOwner(this);
        
        loadBuildSpecFile();

        updatePageState();

        return contents;
    }

    protected Label createLabel(Composite composite, String text) {
        Label label = new Label(composite, SWT.NONE);
        label.setText(text);
        label.setLayoutData(new GridData());
        return label;
    }

    @Override
    public void setVisible(boolean visible) {
        super.setVisible(visible);
        if (visible == true)
            updatePageState();
    }

    protected void updatePageState() {
        // fill/refresh combo contents
        String oldComboSelection = sourceFolderCombo.getText();
        List<IContainer> sourceFolders = CDTUtils.getSourceFolders(getResource().getProject());
        int thisFolderIndex = sourceFolders.indexOf(getResource());

        sourceFolderCombo.removeAll();
        if (thisFolderIndex == -1)
            sourceFolderCombo.add(getResource().getFullPath().toString() + " <selected>");
        for (int i = 0; i < sourceFolders.size(); i++)
            if (i != thisFolderIndex)
                sourceFolderCombo.add(sourceFolders.get(i).getFullPath().toString());
            else
                sourceFolderCombo.add(sourceFolders.get(i).getFullPath().toString() + " <selected>");
        if (!StringUtils.isEmpty(oldComboSelection))
            sourceFolderCombo.setText(oldComboSelection);  // preserve old
        else
            sourceFolderCombo.select(thisFolderIndex==-1 ? 0 : thisFolderIndex);

        // update rest of the page
        comboSelectionChanged();
    }

    protected void comboSelectionChanged() {
        String text = StringUtils.removeEnd(sourceFolderCombo.getText(), " <selected>");
        
        // map combo selection to an IContainer
        Path path = new Path(text);
        IWorkspaceRoot wsRoot = ResourcesPlugin.getWorkspace().getRoot();
        IContainer folder = path.segmentCount()>1 ? wsRoot.getFolder(path) : wsRoot.getProject(text);

        // show/hide makemake options panel
        IProject project = getResource().getProject();
        List<IContainer> sourceFolders = CDTUtils.getSourceFolders(project);
        selectedSourceFolder = sourceFolders.contains(folder) ? folder : null;
        contents.setVisible(selectedSourceFolder != null);

        // configure options panel
        if (selectedSourceFolder != null) {
            String makefragContents = readMakefrag(selectedSourceFolder, MAKEFRAG_FILENAME);
            String makefragvcContents = readMakefrag(selectedSourceFolder, MAKEFRAGVC_FILENAME);
            MakemakeOptions folderOptions = buildSpec.getMakemakeOptions(selectedSourceFolder);
            if (folderOptions == null)
                folderOptions = new MakemakeOptions(); //XXX set defaults!
            contents.populate(selectedSourceFolder, folderOptions, makefragContents, makefragvcContents);
        }
        else {
            //XXX display some message
        }
        
        //setErrorMessage(getInformationalMessage());
        String message = getInformationalMessage();
        if (message == null)
            errorMessageText.setText("");
        else
            errorMessageText.setText(message);
    }


    protected String getInformationalMessage() {
        // Check CDT settings
        IManagedBuildInfo buildInfo = ManagedBuildManager.getBuildInfo(getResource().getProject());
        if (buildInfo == null)
            return "Cannot access CDT build information for this project. Is this a CDT project?";
        IConfiguration configuration = buildInfo.getDefaultConfiguration();
        if (configuration == null)
            return "No active CDT configuration -- please create one.";
        if (configuration.isManagedBuildOn())
            return "CDT Managed Build is on! Please turn off CDT's Makefile generation on the \"C/C++ Build\" page.";
        
//FIXME revise/remove or something...        
//        ICSourceEntry[] sourceEntries = configuration.getSourceEntries();
//        if (CDataUtil.isExcluded(getResource().getProjectRelativePath(), sourceEntries)) 
//            return "This folder is excluded from build (or not marked as source folder).";  //XXX clear & disable checkbox too?
//        //FIXME does not work: macro resolver returns "" so buildLocation becomes empty path. why???
//        //IPath buildLocation = configuration.getBuilder().getBuildLocation();
//        //IPath projectLocation = getResource().getProject().getLocation();
//        //if (!projectLocation.isPrefixOf(buildLocation))
//        //    return "Build directory is outside the project! Please adjust it on the \"C/C++ Build\" page.";
//
//        // check build command; check builder type
//
//        IContainer ancestorMakemakeFolder = ancestorMakemakeFolder();
//        if (ancestorMakemakeFolder != null) {
//            MakemakeOptions ancestorMakemakeOptions = buildSpec.getMakemakeOptions(ancestorMakemakeFolder);
//            if (ancestorMakemakeOptions.isDeep /*FIXME and this folder is not excluded manually from it*/)
//                return "This folder is already covered by Makefile in: " + ancestorMakemakeFolder.getFullPath(); //XXX clean and disable checkbox too?
//        }

        //XXX return some message to be displayed to the user, if:
        //  - CDT make folder is not the project root (or: if a makefile is unreachable)
        //  - makefile consistency error (i.e. a subdir doesn't contain a makefile)
        //  - something else is wrong?
        return null;
    }

    //XXX needed??
    protected IContainer ancestorMakemakeFolder() {
        IContainer folder = getResource().getParent();
        while (!(folder instanceof IWorkspaceRoot) && buildSpec.getMakemakeOptions(folder)==null)
            folder = folder.getParent();
        return buildSpec.getMakemakeOptions(folder)==null ? null : folder;
    }

    /**
     * The resource whose properties we are editing.
     */
    protected IContainer getResource() {
        IContainer container = (IContainer) getElement().getAdapter(IContainer.class);
        return container;
    }

    @Override
    public boolean performOk() {
        // note: performApply() delegates here too
        if (selectedSourceFolder != null) {
            buildSpec.setMakemakeOptions(selectedSourceFolder, contents.getResult());
            saveBuildSpecFile();
            saveMakefrag(selectedSourceFolder, MAKEFRAG_FILENAME, contents.getMakefragContents());
            saveMakefrag(selectedSourceFolder, MAKEFRAGVC_FILENAME, contents.getMakefragvcContents());
        }
        return true;
    }

    protected void loadBuildSpecFile() {
        try {
            IProject project = getResource().getProject();
            buildSpec = BuildSpecUtils.readBuildSpecFile(project);
            if (buildSpec == null)
                buildSpec = new BuildSpecification();
        } 
        catch (IOException e) {
            errorDialog("Cannot read build specification: ", e);
        } catch (CoreException e) {
            errorDialog("Cannot read build specification: ", e);
        }

    }

    protected void saveBuildSpecFile() {
        try {
            //XXX remove obsolete entries from buildSpec? (i.e. those folders that are no longer source folder)
            IProject project = getResource().getProject();
            BuildSpecUtils.saveBuildSpecFile(project, buildSpec);
        } 
        catch (CoreException e) {
            errorDialog("Cannot store build specification: ", e);
        }
    } 

    protected String readMakefrag(IContainer sourceFolder, String makefragFilename) {
        IFile makefragFile = sourceFolder.getFile(new Path(makefragFilename));
        if (makefragFile.exists()) {
            try {
                return FileUtils.readTextFile(makefragFile.getContents());
            }
            catch (IOException e1) {
                errorDialog("Cannot read "+makefragFile.toString(), e1);
            }
            catch (CoreException e1) {
                errorDialog("Cannot read "+makefragFile.toString(), e1);
            }
        }
        return null;
    }

    protected void saveMakefrag(IContainer sourceFolder, String makefragFilename, String makefragContents) {
        String currentContents = readMakefrag(sourceFolder, makefragFilename);
        if (StringUtils.isBlank(makefragContents))
            makefragContents = null;
        if (!StringUtils.equals(currentContents, makefragContents)) {
            IFile makefragFile = sourceFolder.getFile(new Path(makefragFilename));
            try {
                if (makefragContents == null)
                    makefragFile.delete(true, null);
                else
                    FileUtils.writeTextFile(makefragFile.getLocation().toFile(), makefragContents);
                makefragFile.refreshLocal(IResource.DEPTH_ZERO, null);
            }
            catch (IOException e1) {
                errorDialog("Cannot write "+makefragFile.toString(), e1);
            }
            catch (CoreException e1) {
                errorDialog("Cannot write "+makefragFile.toString(), e1);
            }
        }
    }

    protected void errorDialog(String message, Throwable e) {
        Activator.logError(message, e);
        IStatus status = new Status(IMarker.SEVERITY_ERROR, Activator.PLUGIN_ID, e.getMessage(), e);
        ErrorDialog.openError(Display.getCurrent().getActiveShell(), "Error", message, status);
    }
}

