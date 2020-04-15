package org.omnetpp.ide.installer;

import java.io.File;
import java.net.URL;

import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.OmnetppDirs;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.ide.OmnetppMainPlugin;


/**
 *
 * @author levy, andras
 */
public class FirstStepsDialog extends TitleAreaDialog {
    private Button installINETButton;
    private Button importSamplesButton;
    private boolean isInstallINETRequested;
    private boolean isImportSamplesRequested;

    public FirstStepsDialog(Shell shell) {
        super(shell);
        setShellStyle(getShellStyle() | SWT.RESIZE);
    }

    public boolean isInstallINETRequested() {
        return isInstallINETRequested;
    }

    public boolean isImportSamplesRequested() {
        return isImportSamplesRequested;
    }

    @Override
    protected void configureShell(Shell shell) {
        super.configureShell(shell);
        shell.setText("First Steps");
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        setTitle("Empty workspace");
        setMessage("Your workspace is empty. Would you like to install or import projects?");
        setHelpAvailable(false);

        Composite composite = (Composite)super.createDialogArea(parent);
        Group group = new Group(composite, SWT.NONE);
        GridLayout gridLayout = new GridLayout();
        gridLayout.marginWidth = gridLayout.marginHeight = 20;
        gridLayout.verticalSpacing = gridLayout.horizontalSpacing = 10;
        group.setLayout(gridLayout);
        group.setLayoutData(new GridData(GridData.FILL_BOTH));

        installINETButton = createCheckbox(group, "Install INET Framework", true);
        createWrappingLabel(group,
                "The INET Framework is the primary model library for the simulation of communication networks. " +
                "It contains models for several wired and wireless networking protocols, Internet protocols and " +
                "technologies, support for wireless ad-hoc mobile networks, and much more. " +
                "This option will download the latest matching INET release from http://inet.omnetpp.org, " +
                "and install it into your workspace. Select it if you want to simulate communication networks.",
                true);
        importSamplesButton = createCheckbox(group, "Import OMNeT++ programming examples", true);
        createWrappingLabel(group,
                "Import the examples provided with OMNeT++ into the workspace. " +
                "The examples demonstrate how to use various features of the simulation framework via " +
                "queueing, resource allocation, and simplified communication network models. " +
                "It also contains a step-by-step tutorial called TicToc. Select this item if you " +
                "are new to OMNeT++ and want to familiarize yourself with it.",
                true);

        //TODO community page; Help|Install models; open tictoc tutorial; open documentation; etc

        return composite;
    }

    protected Label createLabel(Composite parent, String text) {
        Label label = new Label(parent, SWT.NONE);
        GridData gridData = new GridData(SWT.FILL, SWT.CENTER, false, false);
        label.setLayoutData(gridData);
        label.setText(text);
        return label;
    }

    protected Button createCheckbox(Composite parent, String label, boolean checked) {
        Button b = new Button(parent, SWT.CHECK);
        b.setText(label);
        b.setSelection(checked);
        b.setFont(JFaceResources.getBannerFont());
        return b;
    }

    protected Label createWrappingLabel(Composite parent, String text, boolean indented) {
        Label label = new Label(parent, SWT.WRAP);
        GridData gridData = new GridData(SWT.FILL, SWT.CENTER, true, false);
        gridData.widthHint = 800; // note: this affects requested height
        if (indented)
            gridData.horizontalIndent = 20;
        label.setLayoutData(gridData);
        label.setText(text);
        return label;
    }

    @Override
    protected void okPressed() {
        if (importSamplesButton.getSelection())
            importSampleProjects(false);
        if (installINETButton.getSelection())
            installINET();
        super.okPressed();
    }

    protected void importSampleProjects(final boolean open) {
        isImportSamplesRequested = importSamplesButton.getSelection();
        WorkspaceJob job = new WorkspaceJob("Importing sample projects") {
            @Override
            public IStatus runInWorkspace(IProgressMonitor monitor) throws CoreException {
                ProjectUtils.importAllProjectsFromDirectory(new File(OmnetppDirs.getOmnetppSamplesDir()), open, monitor);
                return Status.OK_STATUS;
            }
        };
        job.setRule(ResourcesPlugin.getWorkspace().getRoot());
        job.setPriority(Job.LONG);
        job.schedule();
    }

    protected void installINET() {
        try {
            isInstallINETRequested = installINETButton.getSelection();
            String omnetppVersion = "omnetpp-" + OmnetppDirs.getMajorVersion() + "." + OmnetppDirs.getMinorVersion();
            URL projectDescriptionURL = new URL(InstallSimulationModelsDialog.DESCRIPTORS_URL + "/" + omnetppVersion + "/inet.xml");
            InstallProjectJob installProjectJob = new InstallProjectJob(projectDescriptionURL, new ProjectInstallationOptions("inet4"));
            installProjectJob.setUser(true);
            installProjectJob.schedule();
        }
        catch (Exception e) {
            OmnetppMainPlugin.logError("Error installing INET", e);
            MessageDialog.openError(null, "Error", "Error installing INET Framework!");
        }
    }

    protected void openCommunityCatalog() {
        Display.getCurrent().asyncExec(new Runnable() {
            @Override
            public void run() {
                try {
                    IWorkbench workbench = PlatformUI.getWorkbench();
                    workbench.getBrowserSupport().createBrowser("open-community-catalog").openURL(new URL("http://omnetpp.org/download/models-and-tools"));
                    //XXX workbench.getBrowserSupport().getExternalBrowser().openURL(new URL(event.location));
                }
                catch (Exception e) {
                    throw new RuntimeException(e);
                }
            }
        });
    }
}
