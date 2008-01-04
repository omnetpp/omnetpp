package org.omnetpp.cdt.ui;

import java.io.IOException;
import java.util.List;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.cdt.core.settings.model.CSourceEntry;
import org.eclipse.cdt.core.settings.model.ICConfigurationDescription;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.cdt.core.settings.model.WriteAccessException;
import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.core.IManagedBuildInfo;
import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.cdt.ui.newui.CDTPropertyManager;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.preference.IPreferencePageContainer;
import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.swt.SWT;
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
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.dialogs.PropertyPage;
import org.eclipse.ui.internal.dialogs.PropertyDialog;
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
public class MakemakeFolderPropertyPage extends PropertyPage {
    private static final String SELECTED = " <selected>";
    public static final String MAKEFRAG_FILENAME = "makefrag";
    public static final String MAKEFRAGVC_FILENAME = "makefrag.vc";

    // state
    protected BuildSpecification buildSpec;
    protected IContainer selectedSourceFolder; // follows combo box; null if selection is not a source folder

    // controls
    protected Combo sourceFolderCombo;
    protected Text errorMessageText;
    protected MakemakeOptionsPanel contents;
    protected Composite nonSourceFolderComposite;

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
        
        nonSourceFolderComposite = new Composite(parent, SWT.NONE);
        nonSourceFolderComposite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        nonSourceFolderComposite.setLayout(new GridLayout(1,false));
        Link link = createLink(nonSourceFolderComposite, "This is not a source folder. Source folders can be specified in the <A>Paths and symbols</A> page.");
        createLabel(nonSourceFolderComposite, "To generate a Makefile, you need to make this folder a source folder.");
        Button button = createButton(nonSourceFolderComposite, SWT.PUSH, "Make source folder", null);
        button.addSelectionListener(new SelectionListener() {
            public void widgetDefaultSelected(SelectionEvent e) {
                makeSourceFolder();
            }
            public void widgetSelected(SelectionEvent e) {
                makeSourceFolder();
            }
        });
        link.addSelectionListener(new SelectionListener(){
            public void widgetSelected(SelectionEvent e) {
                gotoPathsAndSymbolsPage();
            }
            public void widgetDefaultSelected(SelectionEvent e) {
                gotoPathsAndSymbolsPage();
            }
        });
        
        
        loadBuildSpecFile();

        updatePageState();

        return contents;
    }

    @SuppressWarnings("restriction")
    protected void gotoPathsAndSymbolsPage() {
        IPreferencePageContainer container = getContainer();
        if (container instanceof PropertyDialog)
            ((PropertyDialog)container).setCurrentPageId(MakemakeOptionsPanel.PROPERTYPAGE_PATH_AND_SYMBOLS);
    }

    protected void makeSourceFolder() {
        IContainer folder = getComboSelection();
        if (MessageDialog.openQuestion(getShell(), "Make Source Folder", 
                "Do you want to add \"" + folder.getFullPath() + "\" to the list of source folders? " +
                "It will be added to all configurations. You can check the result or revert " +
                "this operation on the \"Paths and symbols\" page of the project properties dialog."
                )) {

            IProject project = getFolder().getProject();
            ICProjectDescription projectDescription = CDTPropertyManager.getProjectDescription(this, project);
            for (ICConfigurationDescription configuration : projectDescription.getConfigurations()) {
                ICSourceEntry[] entries = configuration.getSourceEntries();
                ICSourceEntry entry = new CSourceEntry(folder.getProjectRelativePath(), new IPath[0], 0);
                entries = (ICSourceEntry[]) ArrayUtils.add(entries, entry);
                try {
                    configuration.setSourceEntries(entries);
                }
                catch (WriteAccessException e) {
                    Activator.logError(e);
                }
                catch (CoreException e) {
                    Activator.logError(e);
                }
            }
            CDTPropertyManager.performOk(this);
            updatePageState();
        }
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

    protected void updatePageState() {
        refreshComboContents();

        // update rest of the page
        comboSelectionChanged();
    }

    private void refreshComboContents() {
        String oldComboSelection = sourceFolderCombo.getText();
        IContainer folder = getFolder();
        List<IContainer> sourceFolders = CDTUtils.getSourceFolders(folder.getProject());
        int thisFolderIndex = sourceFolders.indexOf(folder);

        // fill combo
        sourceFolderCombo.removeAll();
        if (thisFolderIndex == -1)
            sourceFolderCombo.add(folder.getFullPath().toString() + SELECTED);
        for (int i = 0; i < sourceFolders.size(); i++)
            sourceFolderCombo.add(sourceFolders.get(i).getFullPath() + (i==thisFolderIndex ? SELECTED : ""));
        
        // select one entry 
        if (StringUtils.isEmpty(oldComboSelection)) {
            // initialize combo and variable
            selectedSourceFolder = sourceFolders.contains(folder) ? folder : null;
            sourceFolderCombo.select(thisFolderIndex==-1 ? 0 : thisFolderIndex);
        }
        else {
            // preserve old setting
            sourceFolderCombo.setText(oldComboSelection);  
        }
    }

    protected void comboSelectionChanged() {
        IContainer folder = getComboSelection();

        IProject project = getFolder().getProject();
        List<IContainer> sourceFolders = CDTUtils.getSourceFolders(project);
        IContainer newSelectedSourceFolder = sourceFolders.contains(folder) ? folder : null;

        if (newSelectedSourceFolder != selectedSourceFolder) {
            // save the folder we're switching away from
            if (selectedSourceFolder != null && isDirty())
                if (MessageDialog.openQuestion(getShell(), "Save?", "Makemake options for folder \"" + selectedSourceFolder.getFullPath() + "\" changed. Save changes?"))
                    performApply();

            // switch to the new folder
            selectedSourceFolder = newSelectedSourceFolder;

            // configure options panel
            if (selectedSourceFolder != null) {
                String makefragContents = readMakefrag(selectedSourceFolder, MAKEFRAG_FILENAME);
                String makefragvcContents = readMakefrag(selectedSourceFolder, MAKEFRAGVC_FILENAME);
                MakemakeOptions folderOptions = buildSpec.getMakemakeOptions(selectedSourceFolder);
                if (folderOptions == null)
                    folderOptions = MakemakeOptions.createInitial();
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
        
        // this needs to be done unconditionally, so that it runs when page is first displayed
        setControlVisible(contents, selectedSourceFolder != null);
        setControlVisible(nonSourceFolderComposite, selectedSourceFolder == null);
    }

    protected IContainer getComboSelection() {
        // returns the IContainer selected in the combo
        String text = StringUtils.removeEnd(sourceFolderCombo.getText(), SELECTED);
        Path path = new Path(text);
        IWorkspaceRoot wsRoot = ResourcesPlugin.getWorkspace().getRoot();
        IContainer folder = path.segmentCount()>1 ? wsRoot.getFolder(path) : wsRoot.getProject(text);
        return folder;
    }

    protected void setControlVisible(Control control, boolean visible) {
        control.setVisible(visible);
        ((GridData)control.getLayoutData()).exclude = !visible;
        control.getParent().layout();
    }

    protected boolean isDirty() {
        if (!contents.getResult().equals(buildSpec.getMakemakeOptions(selectedSourceFolder)))
            return true;
        String makefragContents = readMakefrag(selectedSourceFolder, MAKEFRAG_FILENAME);
        if (!contents.getMakefragContents().equals(StringUtils.nullToEmpty(makefragContents)))
            return true;
        String makefragvcContents = readMakefrag(selectedSourceFolder, MAKEFRAGVC_FILENAME);
        if (!contents.getMakefragvcContents().equals(StringUtils.nullToEmpty(makefragvcContents)))
            return true;
        return false;
    }


    protected String getInformationalMessage() {
        // Check CDT settings
        IManagedBuildInfo buildInfo = ManagedBuildManager.getBuildInfo(getFolder().getProject());
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
        IContainer folder = getFolder().getParent();
        while (!(folder instanceof IWorkspaceRoot) && buildSpec.getMakemakeOptions(folder)==null)
            folder = folder.getParent();
        return buildSpec.getMakemakeOptions(folder)==null ? null : folder;
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
            IProject project = getFolder().getProject();
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
            IProject project = getFolder().getProject();
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

