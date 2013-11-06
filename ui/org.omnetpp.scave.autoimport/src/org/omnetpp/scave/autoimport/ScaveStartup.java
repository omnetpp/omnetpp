package org.omnetpp.scave.autoimport;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.wizard.WizardDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IStartup;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.part.ISetSelectionTarget;
import org.omnetpp.common.util.CommandlineUtils;
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
    public static final int ARGS_HELP = 1;
    public static final int ARGS_ANFFILES = 2;
    public static final int ARGS_RESULTFILES = 3;
    public static final int ARGS_ANFFILE_AND_RESULTSFILES = 4; // 1stf arg is ANF, followed by result files

    private int invocationType;  // one of the ARGS_ constants above
    private String[] cleansedArgs;

    public static final String SYNOPSIS =
        "scave -h or --help\n" +
        "scave ANFFILE...\n" +
        "scave [RESULTFILE | DIR]...\n" +
        "scave NEW_ANFFILE [RESULTFILE | DIR]...\n";

    public static final String DESCRIPTION =  // note: only per-paragraph newlines!
        "Opens the given ANFFILE(s) in editors, or create and open NEW_ANFFILE " +
        "with the given RESULTFILE(s) and DIR(s) as input. If only RESULTFILE(s) " +
        "and DIR(s) are specified, the program will prompt for the name of the " +
        "new ANF file to be created." +
        "\n" +
        "The -h option displays the synopsis. The --help option displays the " +
        "synopsis and the full description (this message)." +
        "\n" +
        "ANFFILE should be an existing analysis file with the .anf extension. " +
        "The .anf file name extension is mandatory and case sensitive. ANF files " +
        "are created with this program, and describe the \"recipe\" for the result " +
        "analysis (list of input files, processing steps to take, charts to " +
        "create.)" +
        "\n" +
        "NEW_ANFFILE should be the name of an ANF file to be created. If NEW_ANFFILE " +
        "exists, the user will be prompted for confirmation to overwrite the file. " +
        "\n" +
        "RESULTFILE should be an OMNeT++ output vector file (.vec extension) or " +
        "an output scalar file (.sca extension). The file name extensions are " +
        "mandatory and case sensitive. Wildcards are also accepted (e.g. " +
        "Foo-*.vec or results/*.sca); note that wildcard characters need to be " +
        "protected against shell expansion (e.g. 'Foo-*.vec' or Foo-\\*.vec). " +
        "The file name(s) or pattern(s) will be stored in the ANF file as inputs. " +
        "The pattern(s) will expanded and the result file(s) loaded dynamically " +
        "for the result analysis, so RESULTFILEs that do not name currently " +
        "existing files will also be accepted." +
        "\n" +
        "DIR should be the the name of an existing directory. It will be added " +
        "to the ANF file as DIR/*.vec and DIR/*.sca, that is, it will expand to " +
        "all result files in that directory.\n";

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

        if (invocationType == ARGS_HELP) {
            displayHelp();
            return;
        }

        // import or open projects to make files accessible
        if (!CommandlineUtils.ensureFilesAreAvailableInWorkspace(cleansedArgs))
            return;

        // open or create ANF files, etc.
        switch (invocationType) {
            case ARGS_NONE: break;
            case ARGS_HELP: break; // already handled
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

        // recognize -h and --help
        if (args.length > 0 && (args[0].equals("-h") || args[0].equals("--help"))) {
            invocationType = ARGS_HELP;
            cleansedArgs = args;
            return;
        }

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

    private void displayHelp() {
        String[] args = cleansedArgs;
        if (args[0].equals("-h")) {
            // synopsis
            String synopsys =
                "Usage:\n" + StringUtils.indentLines(SYNOPSIS, "   ") + "\n" +
                "Type scave --help for more information.";
            MessageDialog.openInformation(getParentShell(), "Usage", synopsys);
        }
        else {
            // full help
            String synopsis = "Usage:\n" + StringUtils.indentLines(SYNOPSIS, "   ");
            final String description = "Description:\n\n" + DESCRIPTION.replace("\n", "\n\n");
            MessageDialog dialog = new MessageDialog(getParentShell(), "Usage", null, synopsis, MessageDialog.INFORMATION, new String[] { IDialogConstants.OK_LABEL }, 0) {
                @Override
                protected Control createCustomArea(Composite parent) {
                    Text text = new Text(parent, SWT.READ_ONLY | SWT.MULTI | SWT.V_SCROLL | SWT.WRAP);
                    text.setText(description);
                    GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
                    gridData.widthHint = this.getMinimumMessageWidth();
                    text.setLayoutData(gridData);
                    return text;
                }
                @Override
                protected int getShellStyle() {
                    return super.getShellStyle() | SWT.RESIZE;
                }
                @Override
                protected boolean customShouldTakeFocus() {
                    return false;
                }
            };
            dialog.open();
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

    private static void showErrorDialog(String message, Throwable e) {
        if (e != null && !StringUtils.isEmpty(e.getMessage()))
            message += ": " + e.getMessage();
        MessageDialog.openError(getParentShell(), "Error", message);
    }

    private static Shell getParentShell() {
        return Display.getCurrent().getActiveShell(); // or: PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell(), but workbench window may be null
    }
}
