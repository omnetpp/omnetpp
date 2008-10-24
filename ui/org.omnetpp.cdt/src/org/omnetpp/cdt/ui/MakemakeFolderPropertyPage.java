package org.omnetpp.cdt.ui;

import java.io.IOException;
import java.util.List;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.cdt.core.settings.model.CSourceEntry;
import org.eclipse.cdt.core.settings.model.ICConfigurationDescription;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.cdt.core.settings.model.WriteAccessException;
import org.eclipse.cdt.ui.newui.CDTPropertyManager;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
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
import org.eclipse.swt.events.ControlAdapter;
import org.eclipse.swt.events.ControlEvent;
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
import org.eclipse.ui.dialogs.PropertyPage;
import org.eclipse.ui.internal.dialogs.PropertyDialog;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.CDTUtils;
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
//FIXME unify .oppbuildspec with .nedfolders, and include nedfolder-to-dll mapping too!!!
//FIXME per-configuration MakemakeOptions????
//TODO check consistency!!!
@SuppressWarnings("restriction")
public class MakemakeFolderPropertyPage extends PropertyPage {
    private static final String SELECTED = " <selected>";
    public static final String MAKEFRAG_FILENAME = "makefrag";
    public static final String MAKEFRAGVC_FILENAME = "makefrag.vc";

    // state
    protected BuildSpecification buildSpec;
    protected IContainer selectedFolder; // follows combo box

    // controls
    protected Combo sourceFolderCombo;
    protected Label errorMessageLabel;
    protected MakemakeOptionsPanel optionsPanel;
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
        final Group group = new Group(parent, SWT.NONE);
        group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));
        group.setLayout(new GridLayout(2,false));
        
        Label label0 = createLabel(group, "Source folder to configure:");
        label0.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false,2,1));
        createLabel(group, " ");
        sourceFolderCombo = new Combo(group, SWT.READ_ONLY | SWT.BORDER);
        sourceFolderCombo.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

        String text = "To add/remove source folders, ";
        if (getFolder() instanceof IProject)
            text += "go to the <A>Paths and symbols</A> page.";
        else
            text += "open Project Properties and go to the Paths and symbols page.";
        Link pathsAndSymbolsLink = createLink(group, text);
        pathsAndSymbolsLink.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false, 2, 1));
        
        errorMessageLabel = new Label(group, SWT.WRAP);
        errorMessageLabel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, false, 2, 1));
        errorMessageLabel.setForeground(ColorFactory.RED2);
        //errorMessageLabel.setBackground(errorMessageLabel.getDisplay().getSystemColor(SWT.COLOR_WIDGET_BACKGROUND));

        optionsPanel = new MakemakeOptionsPanel(parent, SWT.NONE); 
        optionsPanel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        sourceFolderCombo.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                comboSelectionChanged();
            } 
        });
        pathsAndSymbolsLink.addSelectionListener(new SelectionListener(){
            public void widgetSelected(SelectionEvent e) {
                gotoPathsAndSymbolsPage();
            }
            public void widgetDefaultSelected(SelectionEvent e) {
                gotoPathsAndSymbolsPage();
            }
        });

        optionsPanel.setOwnerPage(this);
        
        nonSourceFolderComposite = new Composite(parent, SWT.NONE);
        nonSourceFolderComposite.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        nonSourceFolderComposite.setLayout(new GridLayout(1,false));
        createLabel(nonSourceFolderComposite, "This is not a source folder. Makefiles can only be generated in source folders.");
        Button button = createButton(nonSourceFolderComposite, SWT.PUSH, "Mark as Source Folder", null);
        button.addSelectionListener(new SelectionListener() {
            public void widgetDefaultSelected(SelectionEvent e) {
                makeSourceFolder();
            }
            public void widgetSelected(SelectionEvent e) {
                makeSourceFolder();
            }
        });
        
        // make the error text label wrap properly; see https://bugs.eclipse.org/bugs/show_bug.cgi?id=9866
        group.addControlListener(new ControlAdapter(){
            public void controlResized(ControlEvent e){
                GridData data = (GridData)errorMessageLabel.getLayoutData();
                GridLayout layout = (GridLayout)group.getLayout();
                data.widthHint = group.getClientArea().width - 2*layout.marginWidth;
                group.layout(true);
            }
        });
        
        loadBuildSpecFile();

        updatePageState();

        return optionsPanel;  //XXX why?
    }

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
        boolean stillContainsOldSelection = ArrayUtils.indexOf(sourceFolderCombo.getItems(), oldComboSelection) != -1;
        if (StringUtils.isEmpty(oldComboSelection) || !stillContainsOldSelection) {
            sourceFolderCombo.select(thisFolderIndex==-1 ? 0 : thisFolderIndex);
        }
        else {
            // preserve old setting
            sourceFolderCombo.setText(oldComboSelection);
        }
    }

    protected void comboSelectionChanged() {
        IContainer newSelectedFolder = getComboSelection();
        Assert.isNotNull(newSelectedFolder);

        IProject project = getFolder().getProject();
        List<IContainer> sourceFolders = CDTUtils.getSourceFolders(project);
        boolean isSourceFolder = sourceFolders.contains(selectedFolder);

        if (!newSelectedFolder.equals(selectedFolder) || isOptionsPanelVisible() != isSourceFolder) {
            // save the folder we're switching away from, if: old folder was and still is a source folder, 
            // and panel contents are dirty. (i.e. if it meanwhile ceased to be a source folder, don't save)
            if (!newSelectedFolder.equals(selectedFolder) && isSourceFolder && isOptionsPanelVisible() && isDirty())
                if (MessageDialog.openQuestion(getShell(), "Save?", "Makemake options for folder \"" + selectedFolder.getFullPath() + "\" changed. Save changes?"))
                    performApply();

            // switch to the new folder
            selectedFolder = newSelectedFolder;
            isSourceFolder = sourceFolders.contains(selectedFolder);
            setOptionsPanelVisible(isSourceFolder);
            if (isOptionsPanelVisible()) {
                String makefragContents = readMakefrag(selectedFolder, MAKEFRAG_FILENAME);
                String makefragvcContents = readMakefrag(selectedFolder, MAKEFRAGVC_FILENAME);
                MakemakeOptions folderOptions = buildSpec.getMakemakeOptions(selectedFolder);
                Assert.isTrue(folderOptions!=null);
                optionsPanel.populate(selectedFolder, folderOptions, makefragContents, makefragvcContents, buildSpec.getMakeFolders());
            }

            String message = ProjectMakemakePropertyPage.getInformationalMessage(getFolder()); //XXX make common base class instead?
            errorMessageLabel.setText(message==null ? "" : message);
        }
    }

    protected IContainer getComboSelection() {
        // returns the IContainer selected in the combo
        String text = StringUtils.removeEnd(sourceFolderCombo.getText(), SELECTED);
        Path path = new Path(text);
        IWorkspaceRoot wsRoot = ResourcesPlugin.getWorkspace().getRoot();
        IContainer folder = path.segmentCount()>1 ? wsRoot.getFolder(path) : wsRoot.getProject(text);
        return folder;
    }

    protected void setOptionsPanelVisible(boolean visible) {
        optionsPanel.setVisible(visible);
        nonSourceFolderComposite.setVisible(!visible);
        ((GridData)optionsPanel.getLayoutData()).exclude = !visible;
        ((GridData)nonSourceFolderComposite.getLayoutData()).exclude = visible;
        optionsPanel.getParent().layout();
    }
    
    protected boolean isOptionsPanelVisible() {
        // note: control.isVisible() is not good, because it might return false 
        // even just after control.setVisible(true)!
        return ((GridData)optionsPanel.getLayoutData()).exclude == false;
    }

    protected boolean isDirty() {
        if (!optionsPanel.getResult().equals(buildSpec.getMakemakeOptions(selectedFolder)))
            return true;
        String makefragContents = readMakefrag(selectedFolder, MAKEFRAG_FILENAME);
        if (!optionsPanel.getMakefragContents().equals(StringUtils.nullToEmpty(makefragContents)))
            return true;
        String makefragvcContents = readMakefrag(selectedFolder, MAKEFRAGVC_FILENAME);
        if (!optionsPanel.getMakefragvcContents().equals(StringUtils.nullToEmpty(makefragvcContents)))
            return true;
        return false;
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
        if (isOptionsPanelVisible()) {
            buildSpec.setMakemakeOptions(selectedFolder, optionsPanel.getResult());
            saveBuildSpecFile();
            saveMakefrag(selectedFolder, MAKEFRAG_FILENAME, optionsPanel.getMakefragContents());
            saveMakefrag(selectedFolder, MAKEFRAGVC_FILENAME, optionsPanel.getMakefragvcContents());
        }
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

