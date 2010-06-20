package org.omnetpp.scave.autoimport;

import java.io.File;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IProjectDescription;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.SubProgressMonitor;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.dialogs.ProgressMonitorDialog;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.wizard.WizardDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IStartup;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.part.ISetSelectionTarget;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.wizard.ScaveModelWizard;
import org.omnetpp.scave.wizard.ScaveWizardUtil;


/**
 * Offers the user to auto-import as project the directory that contains the 
 * files listed on the command-line.
 * 
 * @author Andras
 */
public class ScaveStartup implements IStartup {
    public static final int ARGS_NONE = 0;
    public static final int ARGS_ANFFILES = 1;
    public static final int ARGS_RESULTFILES = 2;
    public static final int ARGS_ANFFILE_AND_RESULTSFILES = 3; // 1stf arg is ANF, followed by result files
    
    private int invocationType;  // one of the ARGS_ constants above
    private String[] cleansedArgs;
    
    /**
     * Entry point. Invoked by the platform.
     */
    public void earlyStartup() {
        Display.getDefault().asyncExec(new Runnable() {
            public void run() {
                processCommandLine();
            }
        });
    }

    /**
     * The main method, responsible for the command-line processing of the Scave application.
     */
    private void processCommandLine() {
        // fill in invocationType and cleansedArgs
        analyzeCommandLine();  
        
        // import or open projects to make files accessible
        if (!ensureFilesAreAvailableInWorkspace())
            return;
        
        // open or create ANF files, etc.
        switch (invocationType) {
            case ARGS_NONE: break;
            case ARGS_ANFFILES: openAnfFiles(cleansedArgs); break;
            case ARGS_RESULTFILES: createAnfFileForResults(cleansedArgs); break;
            case ARGS_ANFFILE_AND_RESULTSFILES: createAnfFileForResults(cleansedArgs[0], (String[]) ArrayUtils.remove(cleansedArgs, 0)); break;
        }
    }

    /**
     * Fills in invocationType and cleansedArgs.
     */
    private void analyzeCommandLine() {
        String args[] = Platform.getCommandLineArgs();

        // synopsis: 
        // - no args
        // - existing-anffiles
        // - existing-resultfiles-and-dirs...
        // - nonexistent-anf-file resultfiles-and-dirs

        // remove args added by Eclipse when launched from the IDE during development
        if (args.length >= 2 && args[0].equals("-product"))
            args = (String[]) ArrayUtils.subarray(args, 2, args.length);

        // we only deal with directories, and ANF, VEC and SCA files
        List<String> bogusArgs = new ArrayList<String>();
        for (String arg : args)
            if (!new File(arg).isDirectory() && !arg.endsWith(".anf") && !arg.endsWith(".sca") && !arg.endsWith(".vec"))
                bogusArgs.add(arg);
        if (!bogusArgs.isEmpty()) {
            MessageDialog.openWarning(getParentShell(), "Warning", "Unrecognized file type, ignoring:\n  "+ StringUtils.join(bogusArgs, "\n  ") + "\nOnly vec, sca and anf files, and existing directories are accepted.");
            for (String arg : bogusArgs)
                args = (String[]) ArrayUtils.removeElement(args, arg);
        }

        // analyze what we have
        if (args.length == 0) {
            // no args -- no action
            invocationType = ARGS_NONE;
        }
        else if (args[0].endsWith(".anf")) {
            // either several existing ANF files, or a maybe-nonexistent and file and several result files and dirs
            boolean hasResultFilesOrDirs = false;
            for (String arg : args) 
                if (new File(arg).isDirectory() || arg.endsWith(".vec") || arg.endsWith(".sca"))
                    hasResultFilesOrDirs = true;
            if (hasResultFilesOrDirs) {
                // must not contain any further ANF files
                List<String> extraAnfFiles = new ArrayList<String>();
                for (int i=1; i<args.length; i++)
                    if (args[i].endsWith(".anf"))
                        extraAnfFiles.add(args[i]);
                if (!extraAnfFiles.isEmpty()) {
                    MessageDialog.openWarning(getParentShell(), "Warning", "Adding result files to first 'anf' file " + args[0] + ", ignoring further 'anf' file(s):\n  "+ StringUtils.join(extraAnfFiles, "\n  "));
                    for (String anfFile : extraAnfFiles)
                        args = (String[]) ArrayUtils.removeElement(args, anfFile);
                }
                
                // note: do NOT check that result file exist, because they may contain wildcards, future files etc!
                
                invocationType = ARGS_ANFFILE_AND_RESULTSFILES;
            }
            else {
                // command line only contains one or more ANF files, to be opened; they must all exist
                List<String> nonexistentFileArgs = new ArrayList<String>();
                for (int i=0; i<args.length; i++)
                    if (!new File(args[i]).isFile())
                        nonexistentFileArgs.add(args[i]);
                if (!nonexistentFileArgs.isEmpty()) {
                    MessageDialog.openWarning(getParentShell(), "Warning", "File(s) do not exist, ignoring:\n  "+ StringUtils.join(nonexistentFileArgs, "\n  "));
                    for (String arg : nonexistentFileArgs)
                        args = (String[]) ArrayUtils.removeElement(args, arg);
                }
                invocationType = args.length==0 ? ARGS_NONE : ARGS_ANFFILES;
            }
        }
        else {
            // result files and dirs expected -- they will be opened in a new ANF file
            // note: do NOT check that result file exist, because they may contain wildcards, future files etc!
            invocationType = ARGS_RESULTFILES;
        }
        
        cleansedArgs = args;
    }

    /**
     * Create, import or create projects so that the files given as command line args
     * become accessible in the workspace.
     * 
     * @return true on (probable) success, false on failure or user cancellation.
     */
    private boolean ensureFilesAreAvailableInWorkspace() {
        // get files specified to Eclipse on the command-line
        String[] args = cleansedArgs;

        // check if all files are available via the workspace
        if (args.length > 0 && availableInOpenProjects(args))
            return true;
        
        // find common ancestor of all files
        File workingDir = new File((String) System.getProperties().get("user.dir"));
        File commonDir = args.length == 0 ? workingDir : determineCommonDir(args);
        if (commonDir == null) {
            MessageDialog.openError(getParentShell(), "Error", "Error: Cannot create a project that covers all input files. Please create project(s) manually, or move/soft link files and start this program again.");
            return false;
        }
        
        // maybe a closed project contains it, offer opening the project
        IProject project = findOpenOrClosedProjectContaining(commonDir); // actually there may be more than one such project (projects may overlap), but oh well...
        if (project != null) {
            if (project.isOpen())
                return true;
            if (MessageDialog.openQuestion(getParentShell(), "Open Project?", "Open existing project '" + project.getName() + "' that covers location " + commonDir.toString() + "?")) {
                openProject(project);
                return true;
            }
            return false;
        }
        
        // bring up "Create Project" dialog to choose between project locations below (stored in IProjectDescription objects) 
        IProjectDescription[] descriptions = collectPotentialProjectDescriptions(commonDir);
        if (descriptions.length == 0) {
            if (args.length == 0)
                return true; // no args, don't worry about project creation
            File workspaceLocation = ResourcesPlugin.getWorkspace().getRoot().getLocation().toFile();
            if (isPrefixOf(commonDir, workspaceLocation))
                MessageDialog.openError(getParentShell(), "Error", "Error: Cannot create a project to cover directory " + commonDir + ", because it contains the workspace location " + workspaceLocation + ". Project directories must not contain the workspace where administrative information is stored about them.\n\nPlease choose File->Switch Workspace from the menu, and select another workspace location.");
            else
                MessageDialog.openError(getParentShell(), "Error", "Error: Cannot create a project to cover directory " + commonDir + ". Try changing the workspace location."); // we really don't know what's wrong, but suggest something anyway...
            return false;
        }

        // now really bring up the dialog
        String message = args.length == 0 ? 
                "Do you want to create a project to make files in the current directory accessible?" : 
                    "Create a project to make your files accessible in the workspace.";
        CreateProjectDialog dialog = new CreateProjectDialog(getParentShell(), message, descriptions);
        if (dialog.open() == Dialog.OK) {
            IProjectDescription description = dialog.getResult();
            createOrImportProject(description);
            return true;
        }
        return false;
    }

    private boolean availableInOpenProjects(String[] args) {
        IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
        for (String arg : args) {
            // note: findFilesForLocationURI() also finds closed projects, so we need to check if any are open
            IFile[] candidates = root.findFilesForLocationURI(new File(arg).toURI());
            boolean foundOpenProject = false;
            for (IFile candidate : candidates)
                if (candidate.getProject().isOpen())
                    foundOpenProject = true;
            if (!foundOpenProject)
                return false;
        }
        return true;
    }

    /**
     * Determine a common filesystem directory that contains all given files/dirs.
     * Args are filesystem paths (absolute or working directory relative).
     */
    private File determineCommonDir(String[] args) {
        Assert.isTrue(args.length > 0);

        File[] files = new File[args.length];
        for (int i=0; i<args.length; i++)
            files[i] = new File(args[i]).getAbsoluteFile();

        // find common prefix
        File prefix = files[0];
        if (!prefix.isDirectory())
            prefix = prefix.getParentFile();
        while (prefix != null) {
            // prefix of all files?
            boolean isPrefixOfAll = true;
            for (File file : files)
                if (!isPrefixOf(prefix, file))
                    isPrefixOfAll = false;
            if (isPrefixOfAll)
                return prefix;
            prefix = prefix.getParentFile();
        }

        return null;  // no common prefix (i.e. files are on different drives)
    }

    /**
     * Find first project that covers the given filesystem path. 
     */
    private IProject findOpenOrClosedProjectContaining(File dir) {
        // note: this implementation ignores linked dirs inside the projects
        IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
        for (IProject project : root.getProjects()) {
            IPath location = project.getLocation();
            if (location != null && isPrefixOf(location.toFile(), dir))
                return project;
        }
        return null;
    }
    
    private boolean isPrefixOf(File prefix, File file) {
        while (file != null) {
            if (file.equals(prefix))
                return true;
            file = file.getParentFile();
        }
        return false;
    }

    /**
     * Collect a list of places where we'll offer to create a project; this is the
     * "common" directory of files, and all ancestor directories.
     *  
     * Invalid locations (such as the dirs that contain the whole workspace)
     * are filtered out. 
     * 
     * The result may be zero length, e.g. when dir is the workspace dir or its
     * ancestor.
     * 
     * We return the result as a list of project descriptions, for convenience.
     * (Essentially we use them as (location, projectName) pairs.)
     */
    private IProjectDescription[] collectPotentialProjectDescriptions(File dir) {
        List<IProjectDescription> result = new ArrayList<IProjectDescription>();
        IWorkspace workspace = ResourcesPlugin.getWorkspace();
        IProjectDescription firstExistingDescription = null;
        while (dir != null) {
            IStatus status = workspace.validateProjectLocation(null, new Path(dir.getAbsolutePath()));
            if (status.getSeverity() != IStatus.ERROR) {
                IProjectDescription description = null; 
                try {
                    File projectFile = new File(dir.getPath() + File.separator + IProjectDescription.DESCRIPTION_FILE_NAME);
                    description = workspace.loadProjectDescription(new Path(projectFile.getPath()));
                    if (firstExistingDescription == null)
                        firstExistingDescription = description;
                }
                catch (CoreException e1) { }
                if (description == null)
                    description = workspace.newProjectDescription(suggestProjectNameFor(dir.getAbsolutePath()));
                description.setLocation(new Path(dir.getPath()));
                result.add(description);
            }
            dir = dir.getParentFile();
        }
        return result.toArray(new IProjectDescription[]{});
    }

    /**
     * Suggest a name for a project; should be valid and different from existing project names.
     */
    private String suggestProjectNameFor(String location) {
        String projectName = new Path(location).lastSegment();
        if (projectName == null)
            projectName = "unnamed";

        // if already exists, tweak the name a little
        IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
        if (root.getProject(projectName).exists()) {
            int k = 2;
            while (root.getProject(projectName + "-" + k).exists())
                k++;
            projectName += "-" + k;
        }
        return projectName;
    }

    /**
     * Create or import a project. Actually, if the project description was obtained 
     * by reading it from disk then it's "import", and if it was created from scratch
     * then it is "create".
     * 
     * Project will also be opened.
     * 
     * This a synchronous operation, i.e. we'll return only when the whole thing is done.
     */
    @SuppressWarnings("deprecation")
    private void createOrImportProject(final IProjectDescription description) {
        boolean exists = new File(description.getLocation().append(IProjectDescription.DESCRIPTION_FILE_NAME).toOSString()).isFile();
        final String taskLabel = exists ?
                "Importing project" : "Creating project";
        try {
            IRunnableWithProgress op = new IRunnableWithProgress() {
                public void run(IProgressMonitor monitor) throws InvocationTargetException, InterruptedException {
                    try {
                        monitor.beginTask(taskLabel, 100);
                        IWorkspace workspace = ResourcesPlugin.getWorkspace();
                        IProject project = workspace.getRoot().getProject(description.getName());
                        project.create(description, new SubProgressMonitor(monitor, 50));
                        project.open(IResource.NONE, new SubProgressMonitor(monitor, 50));
                    }
                    catch (Exception e) {
                        throw new InvocationTargetException(e);
                    }
                }
            };
            new ProgressMonitorDialog(getParentShell()).run(true, true, op);
        } 
        catch (InterruptedException e) {
            // cancelled
        }
        catch (InvocationTargetException e) {
            showErrorDialog("Error creating project " + description.getName(), e.getCause());
        } 
    }

    /** 
     * Open project in a synchronous operation, i.e. we'll return only when the whole thing is done.
     */
    private void openProject(final IProject project) {
        try {
            IRunnableWithProgress op = new IRunnableWithProgress() {
                public void run(IProgressMonitor monitor) throws InvocationTargetException, InterruptedException {
                    try {
                        monitor.beginTask("Opening project", 100);
                        project.open(IResource.NONE, new SubProgressMonitor(monitor, 100));
                    }
                    catch (Exception e) {
                        throw new InvocationTargetException(e);
                    }
                }
            };
            new ProgressMonitorDialog(getParentShell()).run(true, true, op);
        } 
        catch (InterruptedException e) {
            // cancelled
        }
        catch (InvocationTargetException e) {
            showErrorDialog("Error opening project " + project.getName(), e);
        } 
    }

    /**
     * Interprets all arguments as ANF files (should have .anf extension), and opens editors for them.
     * All args are filesystem paths (absolute or working directory relative).
     */
    private void openAnfFiles(String[] args) {
        for (String arg : args) {
            IFile file = findFileArgInWorkspace(arg);
            if (file == null) {
                MessageDialog.openWarning(getParentShell(), "Warning", "Cannot open '"  + arg + "': There is no open project that contains it.");
            }
            else {
                try {
                    IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
                    if (page != null)
                        IDE.openEditor(page, file, true);
                }
                catch (org.eclipse.ui.PartInitException e) {
                    showErrorDialog("Cannot open '" + arg + "'", e);
                }
            }
        }
    }

    /**
     * Invokes the New ANF File wizard. The ANF file will have the given args 
     * (sca/vec files and directories) as inputs. Directories will be added
     * as "DIR/*.vec" and "DIR/*.sca".
     * 
     * All args are filesystem paths (absolute or working directory relative).
     */
    private void createAnfFileForResults(String[] resultFileAndDirArgs) {
        // collect the strings that will be written into the "Inputs" section of the ANF file
        List<String> inputFiles = produceAnfFileInputs(resultFileAndDirArgs, null);

        String arg0 = resultFileAndDirArgs.length==0 ? "." : resultFileAndDirArgs[0];
        IResource placeHintForAnfFile = new File(arg0).isDirectory() ? findContainerInWorkspace(arg0) : findFileArgInWorkspace(arg0);
        
        ScaveModelWizard wizard = new ScaveModelWizard();
        wizard.init(PlatformUI.getWorkbench(), new StructuredSelection(placeHintForAnfFile));
        wizard.setInitialInputFiles(inputFiles.toArray(new String[]{}));
        WizardDialog dialog = new WizardDialog(getParentShell(), wizard);
        dialog.open();
    }

    /**
     * Creates and opens an ANF file, with the given name (and location), and with the given
     * result files as inputs. Directories will be added as "DIR/*.vec" and "DIR/*.sca".
     * 
     * All args are filesystem paths (absolute or working directory relative).
     */
    @SuppressWarnings("deprecation")
    private void createAnfFileForResults(String anfFileArg, String[] resultFileAndDirArgs) {
        IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
        IFile anfFile = root.getFileForLocation(new Path(new File(anfFileArg).getAbsolutePath()));
        if (anfFile == null) {
            MessageDialog.openError(getParentShell(), "Warning", "Cannot create '"  + anfFileArg + "': Location does not map to any open project in the workspace.");
            return;
        }
        if (anfFile.exists()) {
            if (!MessageDialog.openConfirm(getParentShell(), "Overwrite?", "The analysis file '" + anfFile.getFullPath() + "' already exists, overwrite?"))
                return; // cancelled
        }

        // collect the strings that will be written into the "Inputs" section of the ANF file
        List<String> inputFiles = produceAnfFileInputs(resultFileAndDirArgs, null);
        
        // save ANF file
        try {
            ScaveWizardUtil.createAnfFile(anfFile, inputFiles.toArray(new String[]{}));
        }
        catch (Exception e) {
            showErrorDialog("Cannot create '" + anfFile.getFullPath() + "'", e);
            return;
        } 
        
        // select in Project Explorer or Navigator
        IStructuredSelection selection = new StructuredSelection(anfFile);
        selectAndRevealInView(IPageLayout.ID_PROJECT_EXPLORER, selection);
        selectAndRevealInView(IPageLayout.ID_RES_NAV, selection);
        
        // open an editor on the new file.
        try {
            IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
            IWorkbenchPage page = workbenchWindow.getActivePage();
            IDE.openEditor(page, anfFile);
        }
        catch (PartInitException e) {
            showErrorDialog("Cannot open editor for '" + anfFile.getFullPath() + "'", e);
        }
    }

    /**
     * Inputs are filesystem paths (absolute or working directory relative); the method
     * converts them to workspace full paths, for putting them into an ANF files' "inputs" part.
     * 
     * If the ANF file is given, files under the ANF file's directory will be returned as 
     * relative to that directory.
     */
    private List<String> produceAnfFileInputs(String[] resultFileAndDirArgs, IFile optAnfFileToBeRelativeTo) {
        // produce a list of strings to be used as "inputs" in the ANF file.
        // Relative paths policy: files that are in the same folder or below the ANF file will be 
        // stored with relative path, others are stored with absolute workspace path

        boolean makeRelativePaths = optAnfFileToBeRelativeTo!=null;
        IPath anfFolderFullPath = optAnfFileToBeRelativeTo==null ? null : optAnfFileToBeRelativeTo.getParent().getFullPath();
        
        List<String> inputFiles = new ArrayList<String>();
        List<String> bogusArgs = new ArrayList<String>();

        for (String arg : resultFileAndDirArgs) {
            if (new File(arg).isDirectory()) {
                // add "*.sca" and "*.vec" in that directory
                IContainer folder = findContainerInWorkspace(arg);
                if (folder == null) 
                    bogusArgs.add(arg);
                else {
                    IPath path = makeRelativePaths && anfFolderFullPath.isPrefixOf(folder.getFullPath()) ?
                            folder.getFullPath().removeFirstSegments(anfFolderFullPath.segmentCount()) :
                                folder.getFullPath();
                    inputFiles.add(path.append("*.vec").toString());
                    inputFiles.add(path.append("*.sca").toString());
                }
            }
            else {
                IFile file = findFileArgInWorkspace(arg);
                if (file == null)
                    bogusArgs.add(arg);
                else {
                    IPath path = makeRelativePaths && anfFolderFullPath.isPrefixOf(file.getFullPath()) ?
                            file.getFullPath().removeFirstSegments(anfFolderFullPath.segmentCount()) :
                                file.getFullPath();
                    inputFiles.add(path.toString());
                }
            }
        }
        if (!bogusArgs.isEmpty())
            MessageDialog.openWarning(getParentShell(), "Warning", "The following files are not accessible in the workspace and will be ignored (there is no open project that contains them):\n  "+ StringUtils.join(bogusArgs, "\n  "));

        //TODO warn about items that don't match any file?
        
        return inputFiles;
    }
    
    private IFile findFileArgInWorkspace(String arg) {
        IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
        IFile[] candidates = root.findFilesForLocationURI(new File(arg).toURI());
        for (IFile candidate : candidates)
            if (candidate.getProject().isAccessible())
                return candidate;
        return null;
    }

    private IContainer findContainerInWorkspace(String arg) {
        IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
        IContainer[] candidates = root.findContainersForLocationURI(new File(arg).toURI());
        for (IContainer candidate : candidates)
            if (candidate.getProject().isAccessible())
                return candidate;
        return null;
    }

    /**
     * Select and reveal the given selection in the given view, if the view exists (is open).
     * This method does not create, bring forward or focus the given view.
     * The view must implement ISetSelectionTarget.
     */
    private void selectAndRevealInView(String viewId, IStructuredSelection selection) {
        IViewPart view = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage().findView(viewId);
        if (view != null)
            ((ISetSelectionTarget)view).selectReveal(selection);
    }
    
    private void showErrorDialog(String message, Throwable e) {
        if (e != null && !StringUtils.isEmpty(e.getMessage()))
            message += ": " + e.getMessage();
        MessageDialog.openError(getParentShell(), "Error", message);
    }

    private Shell getParentShell() {
        return Display.getCurrent().getActiveShell(); // or: PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell(), but workbench window may be null
    }
}
