package org.omnetpp.ide.installer;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;
import java.util.zip.GZIPInputStream;

import org.apache.commons.compress.archivers.tar.TarArchiveEntry;
import org.apache.commons.compress.archivers.tar.TarArchiveInputStream;
import org.apache.commons.io.FileUtils;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.IncrementalProjectBuilder;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.OperationCanceledException;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.SubProgressMonitor;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.ui.IEditorDescriptor;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.navigator.resources.ProjectExplorer;
import org.eclipse.ui.part.FileEditorInput;
import org.omnetpp.cdt.build.ProjectFeaturesManager;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.ide.OmnetppMainPlugin;

/**
 * Base class for project specific installer classes with a couple of helpful
 * methods.
 *
 * @author levy
 */
public abstract class AbstractProjectInstaller {
    protected ProjectDescription projectDescription;
    protected ProjectInstallationOptions projectInstallationOptions;

    public AbstractProjectInstaller(ProjectDescription projectDescription, ProjectInstallationOptions projectInstallationOptions) {
        this.projectDescription = projectDescription;
        this.projectInstallationOptions = projectInstallationOptions;
    }

    public abstract void run(IProgressMonitor progressMonitor) throws CoreException;

    protected File downloadProjectDistribution(IProgressMonitor progressMonitor, String projectDistribution) {
        try {
            progressMonitor.subTask("Downloading " + projectDistribution);
            URL projectDistributionURL = new URL(projectDistribution);
            File projectDistributionFile = File.createTempFile(projectDescription.getName(), ".tgz");
            copyURLToFile(progressMonitor, projectDistributionURL, projectDistributionFile);
            return projectDistributionFile;
        }
        catch (OperationCanceledException e) {
            throw e;
        }
        catch (Exception e) {
            throw new RuntimeException("Cannot download archive from " + projectDistribution, e);
        }
    }

    protected File extractProjectDistribution(IProgressMonitor progressMonitor, File distributionFile) {
        try {
            // unzip the tgz archive into a tar file
            progressMonitor.subTask("Extracting archive");
            String projectDistributionPath = distributionFile.getPath();
            File tarFile = new File(projectDistributionPath.replace(".tgz", ".tar"));
            GZIPInputStream gzipInputStream = new GZIPInputStream(new FileInputStream(distributionFile));
            FileOutputStream tarOutputStream = new FileOutputStream(tarFile);
            byte[] buffer = new byte[1024];
            int length;
            while ((length = gzipInputStream.read(buffer)) > 0) {
                tarOutputStream.write(buffer, 0, length);
                if (progressMonitor.isCanceled())
                    throw new OperationCanceledException();
            }
            gzipInputStream.close();
            tarOutputStream.close();

            // count files in tar file for better progress reporting
            TarArchiveInputStream countTarArchiveInputStream = new TarArchiveInputStream(new FileInputStream(tarFile));
            int count = 0;
            while (countTarArchiveInputStream.getNextTarEntry() != null) {
                count++;
                if (progressMonitor.isCanceled())
                    throw new OperationCanceledException();
            }
            countTarArchiveInputStream.close();

            // extract files
            SubProgressMonitor subProgressMonitor = new SubProgressMonitor(progressMonitor, 1);
            subProgressMonitor.beginTask("Extracting file", count);
            TarArchiveInputStream tarArchiveInputStream = new TarArchiveInputStream(new FileInputStream(tarFile));
            TarArchiveEntry tarArchiveEntry = null;
            String outputDirectory;
            if (projectInstallationOptions.getUseDefaultLocation())
                outputDirectory = ResourcesPlugin.getWorkspace().getRoot().getLocation().toString() + "/" + projectInstallationOptions.getName();
            else
                outputDirectory = projectInstallationOptions.getLocation();
            while ((tarArchiveEntry = tarArchiveInputStream.getNextTarEntry()) != null) {
                String tarArchiveEntryName = tarArchiveEntry.getName();
                File tarArchiveEntryFile =  new File(outputDirectory + "/" + tarArchiveEntryName.substring(tarArchiveEntryName.indexOf('/')));
                if (!tarArchiveEntryFile.getParentFile().exists())
                    tarArchiveEntryFile.getParentFile().mkdirs();
                if (tarArchiveEntry.isDirectory())
                    tarArchiveEntryFile.mkdirs();
                else {
                    if (tarArchiveEntryFile.exists()) {
                        tarArchiveInputStream.close();
                        throw new RuntimeException("Cannot extract archive, " + tarArchiveEntryFile.getName() + " already exists");
                    }
                    byte[] content = new byte[(int)tarArchiveEntry.getSize()];
                    tarArchiveInputStream.read(content, 0, content.length);
                    File outputFile = new File(tarArchiveEntryFile.getPath());
                    FileUtils.writeByteArrayToFile(outputFile, content);
                    boolean isExecutable = (tarArchiveEntry.getMode() & 1) != 0;
                    outputFile.setExecutable(isExecutable);
                }
                if (progressMonitor.isCanceled())
                    throw new OperationCanceledException();
                subProgressMonitor.worked(1);
            }
            tarArchiveInputStream.close();
            tarFile.delete();
            subProgressMonitor.done();
            return new File(outputDirectory);
        }
        catch (OperationCanceledException e) {
            throw e;
        }
        catch (Exception e) {
            throw new RuntimeException("Cannot extract archive " + distributionFile, e);
        }
        finally {
            distributionFile.delete();
        }
    }

    protected IProject importProjectIntoWorkspace(IProgressMonitor progressMonitor, File projectDirectory) throws CoreException {
        try {
            progressMonitor.subTask("Importing project");
            IWorkspace workspace = ResourcesPlugin.getWorkspace();
            Path projectFileLocation = new Path(projectDirectory.getPath() + "/.project");
            IProjectDescription projectDescription = workspace.loadProjectDescription(projectFileLocation);
            IProject project = workspace.getRoot().getProject(projectInstallationOptions.getName());
            SubProgressMonitor subProgressMonitor = new SubProgressMonitor(progressMonitor, 1);
            project.create(projectDescription, subProgressMonitor);

            return project;
        }
        catch (OperationCanceledException e) {
            throw e;
        }
        catch (CoreException e) {
            throw e;
        }
        catch (Exception e) {
            throw new RuntimeException("Cannot import project into workspace from " + projectDirectory, e);
        }
    }

    protected void openProject(IProgressMonitor progressMonitor, IProject project) throws CoreException {
        try {
            progressMonitor.subTask("Opening project");
            SubProgressMonitor subProgressMonitor = new SubProgressMonitor(progressMonitor, 1);
            project.open(subProgressMonitor);
            // KLUDGE: this is a workaround for the fact that the project becomes immediately open after the call.
            // KLUDGE: building the project right after the call might fail due to incomplete open
            // KLUDGE: so we must wait (at most 10s) until the project becomes really open (checking isOpen isn't enough)
            int i = 100;
            while (i-- > 0 && (!project.isOpen() || project.getModificationStamp() == IResource.NULL_STAMP))
                Thread.sleep(100);
        }
        catch (OperationCanceledException e) {
            throw e;
        }
        catch (CoreException e) {
            throw e;
        }
        catch (Exception e) {
            throw new RuntimeException("Cannot open project " + projectDescription.getName(), e);
        }
    }

    protected void initializeProjectFeaturesState(IProject project) throws CoreException {
        ProjectFeaturesManager features = new ProjectFeaturesManager(project);
        if (features.loadFeaturesFile()) {
            Debug.println("Project Features: activating default feature selection for new project " + project.getName());
            features.initializeProjectState();
        }
    }

    protected void buildProject(IProgressMonitor progressMonitor, IProject project) throws CoreException {
        try {
            progressMonitor.subTask("Building project");
            SubProgressMonitor subProgressMonitor = new SubProgressMonitor(progressMonitor, 1);
            project.build(IncrementalProjectBuilder.FULL_BUILD, subProgressMonitor);
        }
        catch (OperationCanceledException e) {
            throw e;
        }
        catch (CoreException e) {
            throw e;
        }
        catch (Exception e) {
            throw new RuntimeException("Cannot build project " + projectDescription.getName(), e);
        }
    }

    protected void expandProject(final IProject project) {
        DisplayUtils.runNowOrAsyncInUIThread(new Runnable() {
            @Override
            public void run() {
                try {
                    ProjectExplorer projectExplorer = (ProjectExplorer)PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage().findView(ProjectExplorer.VIEW_ID);
                    // KLUDGE: we have to reveal one real file or folder first and then select the project
                    IResource[] members = project.members();
                    for (int i = 0; i < members.length; i++) {
                        if (!members[i].getName().startsWith(".") && !members[i].isVirtual() && !members[i].isHidden() && !members[i].isPhantom() && !members[i].isDerived()) {
                            projectExplorer.selectReveal(new StructuredSelection(members[i]));
                            break;
                        }
                    }
                    projectExplorer.selectReveal(new StructuredSelection(project));
                }
                catch (Exception e) {
                    OmnetppMainPlugin.logError("Cannot expand " + projectDescription.getName() + " project", e);
                }
            }
        });
    }

    protected void openView(final String viewID) {
        DisplayUtils.runNowOrAsyncInUIThread(new Runnable() {
            @Override
            public void run() {
                try {
                    IWorkbench workbench = PlatformUI.getWorkbench();
                    IWorkbenchWindow activeWorkbenchWindow = workbench.getActiveWorkbenchWindow();
                    if (activeWorkbenchWindow == null)
                        activeWorkbenchWindow = workbench.getWorkbenchWindows()[0];
                    activeWorkbenchWindow.getActivePage().showView(viewID);
                }
                catch (Exception e) {
                    MessageDialog.openError(null, "Error", "Cannot open view " + viewID);
                    OmnetppMainPlugin.logError("Cannot open view " + viewID, e);
                }
            }
        });
    }

    protected void openEditor(final IFile file) {
        DisplayUtils.runNowOrAsyncInUIThread(new Runnable() {
            @Override
            public void run() {
                try {
                    IWorkbench workbench = PlatformUI.getWorkbench();
                    IWorkbenchWindow activeWorkbenchWindow = workbench.getActiveWorkbenchWindow();
                    if (activeWorkbenchWindow == null)
                        activeWorkbenchWindow = workbench.getWorkbenchWindows()[0];
                    IWorkbenchPage workbenchPage = activeWorkbenchWindow.getActivePage();
                    IEditorDescriptor editorDescriptor = workbench.getEditorRegistry().getDefaultEditor(file.getName());
                    workbenchPage.openEditor(new FileEditorInput(file), editorDescriptor.getId());
                }
                catch (Exception e) {
                    MessageDialog.openError(null, "Error", "Cannot open editor for file " + file);
                    OmnetppMainPlugin.logError("Cannot open editor for file " + file, e);
                }
            }
        });
    }

    protected void copyURLToFile(IProgressMonitor progressMonitor, URL source, File destination) throws IOException {
        InputStream input = null;
        FileOutputStream output = null;
        try {
            URLConnection sourceConnection = source.openConnection();
            int size = sourceConnection.getContentLength();
            SubProgressMonitor subProgressMonitor = new SubProgressMonitor(progressMonitor, 1);
            subProgressMonitor.beginTask("Downloading file", size);
            input = sourceConnection.getInputStream();
            output = new FileOutputStream(destination);
            byte[] buffer = new byte[1024 * 4];
            int n = 0;
            while (-1 != (n = input.read(buffer))) {
                output.write(buffer, 0, n);
                subProgressMonitor.worked(n);
                if (progressMonitor.isCanceled())
                    throw new OperationCanceledException();
            }
            subProgressMonitor.done();
        }
        finally {
            if (input != null)
                input.close();
            if (output != null)
                output.close();
        }
    }
}