package org.omnetpp.ide.installer;

import java.io.File;
import java.io.InputStream;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SegmentEvent;
import org.eclipse.swt.events.SegmentListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.DirectoryDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Link;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.OmnetppDirs;
import org.omnetpp.ide.OmnetppMainPlugin;

public class InstallSimulationModelsDialog extends TitleAreaDialog {
    public static final String DESCRIPTORS_URL = "http://omnetpp.org/download/contrib/descriptors";
    public static final String COMMUNITY_MODEL_CATALOG_URL = "http://www.omnetpp.org/models/catalog";

    protected ArrayList<ProjectDescription> projectDescriptions;
    protected ArrayList<URL> projectDescriptionURLs;
    protected Table projectsTable;
    protected Label longDescription;
    protected Text projectName;
    protected Button useDefaultLocation;
    protected Text location;
    protected Button browseLocation;

    protected InstallSimulationModelsDialog(Shell shell) {
        super(shell);
        setShellStyle(getShellStyle() | SWT.RESIZE);
    }

    protected void downloadProjectDescriptions(URL url) {
        try {
            projectDescriptions = new ArrayList<ProjectDescription>();
            projectDescriptionURLs = new ArrayList<URL>();
            InputStream inputStream = url.openConnection().getInputStream();
            String descriptors = FileUtils.readTextFile(inputStream, "utf-8");
            String[] descriptorFileNames = StringUtils.strip(descriptors).split("\n");
            for (String descriptorFileName : descriptorFileNames) {
                String descriptorStrippedFileName = StringUtils.strip(descriptorFileName);
                if (!descriptorStrippedFileName.isEmpty()) {
                    String projectDescriptionPath = url.getPath().replaceFirst("/[^/]*$", "/") + descriptorStrippedFileName;
                    URL projectDescriptionURL = new URL(url.getProtocol(), url.getHost(), url.getPort(), projectDescriptionPath);
                    File projectDescriptionFile = downloadProjectDescription(projectDescriptionURL);
                    ProjectDescription projectDescription = parseProjectDescription(projectDescriptionFile);
                    projectDescriptions.add(projectDescription);
                    projectDescriptionURLs.add(projectDescriptionURL);
                }
            }
        }
        catch (Exception e) {
            // TODO: error handling
            throw new RuntimeException(e);
        }
    }

    protected File downloadProjectDescription(URL projectDescriptionURL) {
        try {
            File projectDescriptionFile = File.createTempFile("projectDescription", ".xml");
            org.apache.commons.io.FileUtils.copyURLToFile(projectDescriptionURL, projectDescriptionFile);
            return projectDescriptionFile;
        }
        catch (Exception e) {
            throw new RuntimeException("Cannot download project description from " + projectDescriptionURL, e);
        }
    }

    protected ProjectDescription parseProjectDescription(File descriptionFile) {
        try {
            DocumentBuilderFactory documentBuilderFactory = DocumentBuilderFactory.newInstance();
            DocumentBuilder documentBuilder = documentBuilderFactory.newDocumentBuilder();
            return new ProjectDescription(documentBuilder.parse(descriptionFile));
        }
        catch (Exception e) {
            throw new RuntimeException("Cannot parse project description from " + descriptionFile.getAbsolutePath(), e);
        }
    }

    @Override
    protected void configureShell(Shell shell) {
        super.configureShell(shell);
        Point size = new Point(1000, 700);
        Display display = shell.getDisplay();
        Rectangle screen = display.getMonitors()[0].getBounds();
        shell.setBounds((screen.width - size.x)/2, (screen.height-size.y)/2, size.x, size.y);
        shell.setText("Install Simulation Models");
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        setHelpAvailable(false);
        setTitle("Install Simulation Models");
        setMessage("Please select a model and edit the options below");
        Composite container = (Composite)super.createDialogArea(parent);
        container.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        GridLayout gridLayout = new GridLayout(2, false);
        gridLayout.marginWidth = gridLayout.marginHeight = 10;
        container.setLayout(gridLayout);
        Group group = new Group(container, SWT.NONE);
        gridLayout = new GridLayout(2, false);
        gridLayout.marginWidth = gridLayout.marginHeight = 10;
        group.setLayout(gridLayout);
        group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
        group.setText("Model");
        projectsTable = new Table(group, SWT.BORDER);
        projectsTable.setHeaderVisible(true);
        projectsTable.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
        Link link = new Link(group, SWT.WRAP);
        link.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false, 2, 1));
        link.setText("Other simulation models are available for download in the community <a>catalog</a>.");
        link.addSelectionListener(new SelectionListener() {
            public void widgetDefaultSelected(SelectionEvent e) {
                openCommunityCatalog();
            }

            public void widgetSelected(SelectionEvent e) {
                openCommunityCatalog();
            }});
        group = new Group(container, SWT.NONE);
        group.setText("Description");
        gridLayout = new GridLayout();
        gridLayout.marginWidth = gridLayout.marginHeight = 10;
        group.setLayout(gridLayout);
        group.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
        longDescription = new Label(group, SWT.WRAP | SWT.V_SCROLL);
        longDescription.setLayoutData(new GridData(GridData.FILL_BOTH));
        group = new Group(container, SWT.NONE);
        group.setText("Options");
        gridLayout = new GridLayout(3, false);
        gridLayout.marginWidth = gridLayout.marginHeight = 10;
        group.setLayout(gridLayout);
        group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false, 2, 1));
        Label label = new Label(group, SWT.NONE);
        label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 1, 1));
        label.setText("Project name: ");
        projectName = new Text(group, SWT.BORDER);
        // TODO: revive this when importing a project with a different name than the one in the .project file becomes possible
        projectName.setEnabled(false);
        projectName.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
        useDefaultLocation = new Button(group, SWT.CHECK);
        useDefaultLocation.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 3, 1));
        useDefaultLocation.setText("Use default location");
        useDefaultLocation.setSelection(true);
        label = new Label(group, SWT.NONE);
        label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 1, 1));
        label.setText("Location: ");
        location = new Text(group, SWT.BORDER);
        location.setEnabled(false);
        location.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 1, 1));
        browseLocation = new Button(group, SWT.NONE);
        browseLocation.setText("Browse...");
        browseLocation.setEnabled(false);
        browseLocation.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 1, 1));
        browseLocation.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                DirectoryDialog dialog = new DirectoryDialog(getShell(), SWT.NONE);
                dialog.setText("Select location");
                dialog.setFilterPath(location.getText());
                String file = dialog.open();
                if (file != null)
                    location.setText(file);
            }
        });
        projectsTable.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent event) {
                ProjectDescription projectDescription = (ProjectDescription)event.item.getData();
                longDescription.setText(projectDescription.getLongDescription());
                // TODO: revive this when importing a project with a different name than the one in the .project file becomes possible
                // projectName.setText(projectDescription.getName() + "-" + projectDescription.getVersion());
                projectName.setText(projectDescription.getName());
                updateDefaultLocation();
                getButton(IDialogConstants.OK_ID).setEnabled(true);
            }
        });
        projectName.addSegmentListener(new SegmentListener() {
            @Override
            public void getSegments(SegmentEvent event) {
                if (useDefaultLocation.getSelection())
                    updateDefaultLocation();
            }
        });
        useDefaultLocation.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                location.setEnabled(!useDefaultLocation.getSelection());
                browseLocation.setEnabled(!useDefaultLocation.getSelection());
                if (useDefaultLocation.getSelection())
                    updateDefaultLocation();
            }
        });
        scheduleDownloadProjectDescriptions();
        return container;
    }

    protected void scheduleDownloadProjectDescriptions() {
        try {
            String omnetppVersion = "omnetpp-" + OmnetppDirs.getMajorVersion() + "." + OmnetppDirs.getMinorVersion();
            final URL descriptorsURL = new URL(DESCRIPTORS_URL + "/" + omnetppVersion + "/descriptors.txt");
            Job job = new Job("Download project descriptors") {
                @Override
                protected IStatus run(IProgressMonitor monitor) {
                    try {
                        downloadProjectDescriptions(descriptorsURL);
                        DisplayUtils.runNowOrAsyncInUIThread(new Runnable() {
                            @Override
                            public void run() {
                                fillProjects();
                            }
                        });
                    }
                    catch (Exception e) {
                        CommonPlugin.logError("An error occurred while downloading the list of models available for installation from " + descriptorsURL, e);
                        setErrorMessage("An error occurred while downloading the list of models available for installation");
                    }
                    return Status.OK_STATUS;
                }
            };
            job.setSystem(true);
            job.schedule();
        }
        catch (MalformedURLException e) {
            throw new RuntimeException(e);
        }
    }

    protected void updateDefaultLocation() {
        location.setText(ResourcesPlugin.getWorkspace().getRoot().getLocation().toOSString() + "/" + projectName.getText());
    }

    protected void fillProjects() {
        TableColumn tableColumn = new TableColumn(projectsTable, SWT.NONE);
        tableColumn.setWidth(200);
        tableColumn.setText("Name");
        tableColumn = new TableColumn(projectsTable, SWT.NONE);
        tableColumn.setWidth(100);
        tableColumn.setText("Version");
        tableColumn = new TableColumn(projectsTable, SWT.NONE);
        tableColumn.setWidth(600);
        tableColumn.setText("Description");
        for (ProjectDescription projectDescription : projectDescriptions) {
            TableItem tableItem = new TableItem(projectsTable, SWT.NONE);
            tableItem.setData(projectDescription);
            tableItem.setText(0, projectDescription.getTitle());
            tableItem.setText(1, projectDescription.getVersion());
            tableItem.setText(2, projectDescription.getShortDescription());
        }
    }

    @Override
    protected void createButtonsForButtonBar(Composite parent) {
        createButton(parent, IDialogConstants.OK_ID, "Install Project", true);
        createButton(parent, IDialogConstants.CANCEL_ID, IDialogConstants.CANCEL_LABEL, false);
        getButton(IDialogConstants.OK_ID).setEnabled(projectsTable.getItemCount() > 0);
    }

    @Override
    protected void okPressed() {
        int index = projectsTable.getSelectionIndex();
        if (index != -1)
            installProject(projectDescriptionURLs.get(index), projectDescriptions.get(index));
        super.okPressed();
    }

    protected void installProject(URL projectDescriptionURL, ProjectDescription projectDescription) {
        try {
            ProjectInstallationOptions projectInstallationOptions = new ProjectInstallationOptions(projectName.getText(), useDefaultLocation.getSelection(), location.getText());
            InstallProjectJob installProjectJob = new InstallProjectJob(projectDescriptionURL, projectInstallationOptions);
            installProjectJob.setUser(true);
            installProjectJob.schedule();
        }
        catch (Exception e) {
            OmnetppMainPlugin.logError("Error installing " + projectName.getText(), e);
            MessageDialog.openError(null, "Error", "Error installing " + projectDescription.getTitle() + "!");
        }
    }

    protected void openCommunityCatalog() {
        Display.getCurrent().asyncExec(new Runnable() {
            @Override
            public void run() {
                try {
                    IWorkbench workbench = PlatformUI.getWorkbench();
                    workbench.getBrowserSupport().getExternalBrowser().openURL(new URL(COMMUNITY_MODEL_CATALOG_URL));
                }
                catch (Exception e) {
                    throw new RuntimeException(e);
                }
            }
        });
    }
}
