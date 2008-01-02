package org.omnetpp.cdt.ui;

import java.io.IOException;

import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.cdt.core.settings.model.util.CDataUtil;
import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.core.IManagedBuildInfo;
import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
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
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.dialogs.PropertyPage;
import org.omnetpp.cdt.Activator;
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
public class MakemakeFolderPropertyPage extends PropertyPage {
    public static final String MAKEFRAG_FILENAME = "makefrag";
    public static final String MAKEFRAGVC_FILENAME = "makefrag.vc";

    // state
    protected BuildSpecification buildSpec;

    // controls
    protected Button enableMakefileCheckbox;
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
        group.setLayout(new GridLayout(1,false));
        enableMakefileCheckbox = createCheckbox(group, "Generate OMNeT++/OMNEST Makefile automatically");
        enableMakefileCheckbox.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        //FIXME TODO add label: "CDT Makefile generation must be turned off at the <A>C/C++ Build</A> page"

        errorMessageText = new Text(group, SWT.MULTI);
        errorMessageText.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        errorMessageText.setEditable(false);
        errorMessageText.setForeground(ColorFactory.RED2);
        errorMessageText.setBackground(errorMessageText.getDisplay().getSystemColor(SWT.COLOR_WIDGET_BACKGROUND));

        contents = new MakemakeOptionsPanel(parent, SWT.NONE); 
        contents.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        enableMakefileCheckbox.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                boolean enabled = enableMakefileCheckbox.getSelection();
                contents.setVisible(enabled);
            } 
        });

        contents.setOwner(this);
        
        loadBuildSpecFile();
        String makefragContents = readMakefrag(MAKEFRAG_FILENAME);
        String makefragvcContents = readMakefrag(MAKEFRAGVC_FILENAME);
        MakemakeOptions folderOptions = buildSpec.getMakemakeOptions(getResource());
        enableMakefileCheckbox.setSelection(folderOptions != null);
        contents.populate(folderOptions != null ? folderOptions : new MakemakeOptions(), makefragContents, makefragvcContents);

        updatePageState();

        return contents;
    }

    @Override
    public void setVisible(boolean visible) {
        super.setVisible(visible);
        updatePageState();
    }

    protected void updatePageState() {
        contents.setVisible(enableMakefileCheckbox.getSelection());
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
        ICSourceEntry[] sourceEntries = configuration.getSourceEntries();
        if (CDataUtil.isExcluded(getResource().getProjectRelativePath(), sourceEntries)) 
            return "This folder is excluded from build (or not marked as source folder).";  //XXX clear & disable checkbox too?
        //FIXME does not work: macro resolver returns "" so buildLocation becomes empty path. why???
        //IPath buildLocation = configuration.getBuilder().getBuildLocation();
        //IPath projectLocation = getResource().getProject().getLocation();
        //if (!projectLocation.isPrefixOf(buildLocation))
        //    return "Build directory is outside the project! Please adjust it on the \"C/C++ Build\" page.";

        // check build command; check builder type

        IContainer ancestorMakemakeFolder = ancestorMakemakeFolder();
        if (ancestorMakemakeFolder != null) {
            MakemakeOptions ancestorMakemakeOptions = buildSpec.getMakemakeOptions(ancestorMakemakeFolder);
            if (ancestorMakemakeOptions.isDeep /*FIXME and this folder is not excluded manually from it*/)
                return "This folder is already covered by Makefile in: " + ancestorMakemakeFolder.getFullPath(); //XXX clean and disable checkbox too?
        }

        //XXX return some message to be displayed to the user, if:
        //  - CDT make folder is not the project root (or: if a makefile is unreachable)
        //  - makefile consistency error (i.e. a subdir doesn't contain a makefile)
        //  - something else is wrong?
        return null;
    }

    protected IContainer ancestorMakemakeFolder() {
        IContainer folder = getResource().getParent();
        while (!(folder instanceof IWorkspaceRoot) && buildSpec.getMakemakeOptions(folder)==null)
            folder = folder.getParent();
        return buildSpec.getMakemakeOptions(folder)==null ? null : folder;
    }

    protected Button createCheckbox(Composite parent, String text) {
        Button button = new Button(parent, SWT.CHECK);
        button.setText(text);
        return button;
    }

    /**
     * The resource whose properties we are editing.
     */
    protected IContainer getResource() {
        IContainer container = (IContainer) getElement().getAdapter(IContainer.class);
        return container;
    }

    public boolean performOk() {
        if (enableMakefileCheckbox.getSelection() == true)
            buildSpec.setMakemakeOptions(getResource(), contents.getResult());
        else
            buildSpec.setMakemakeOptions(getResource(), null);
        saveBuildSpecFile();
        saveMakefrag(MAKEFRAG_FILENAME, contents.getMakefragContents());
        saveMakefrag(MAKEFRAGVC_FILENAME, contents.getMakefragvcContents());
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
            IProject project = getResource().getProject();
            BuildSpecUtils.saveBuildSpecFile(project, buildSpec);
        } 
        catch (CoreException e) {
            errorDialog("Cannot store build specification: ", e);
        }
    } 

    protected String readMakefrag(String makefragFilename) {
        IFile makefragFile = getResource().getFile(new Path(makefragFilename));
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

    protected void saveMakefrag(String makefragFilename, String makefragContents) {
        String currentContents = readMakefrag(makefragFilename);
        if (StringUtils.isBlank(makefragContents))
            makefragContents = null;
        if (!StringUtils.equals(currentContents, makefragContents)) {
            IFile makefragFile = getResource().getFile(new Path(makefragFilename));
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

