package org.omnetpp.cdt.wizard;

import java.net.URL;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import org.eclipse.cdt.core.model.CoreModel;
import org.eclipse.cdt.core.settings.model.CSourceEntry;
import org.eclipse.cdt.core.settings.model.ICConfigurationDescription;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.cdt.core.settings.model.WriteAccessException;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Plugin;
import org.eclipse.jface.wizard.IWizard;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.build.BuildSpecification;
import org.omnetpp.cdt.build.MakemakeOptions;
import org.omnetpp.common.project.NedSourceFoldersConfiguration;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.wizard.CreationContext;
import org.omnetpp.common.wizard.FileBasedContentTemplate;
import org.omnetpp.common.wizard.XSWTDataBinding;
import org.osgi.framework.Bundle;

/**
 * Content template specialized for OMNeT++ projects
 *
 * @author Andras
 */
public class FileBasedProjectTemplate extends FileBasedContentTemplate {
    public static final String PROP_ADDPROJECTREFERENCE = "addProjectReference"; // boolean: if true, make project as dependent on this one
    public static final String PROP_SOURCEFOLDERS = "sourceFolders"; // source folders to be created and configured
    public static final String PROP_NEDSOURCEFOLDERS = "nedSourceFolders"; // NED source folders to be created and configured
    public static final String PROP_MAKEMAKEOPTIONS = "makemakeOptions"; // makemake options, as "folder1:options1,folder2:options2,..."
    public static final String PROP_REQUIRESCPLUSPLUS = "requiresCPlusPlus"; // affects template filtering in the wizard
    public static final String PROP_WITHCPLUSPLUSSUPPORT = "withCplusplusSupport"; // affects template filtering in the wizard

    public FileBasedProjectTemplate(IFolder folder) throws CoreException {
        super(folder);
    }

    public FileBasedProjectTemplate(Plugin plugin, String templateFolder) throws CoreException {
        super(plugin, templateFolder);
    }

    public FileBasedProjectTemplate(URL templateUrl, Bundle bundleOfTemplate) throws CoreException {
        super(templateUrl, bundleOfTemplate);
    }

    public FileBasedProjectTemplate(URL templateUrl) throws CoreException {
        super(templateUrl);
    }

    @Override
    protected CreationContext createContext(IContainer folder, IWizard wizard, String wizardType) {
        CreationContext context = super.createContext(folder, wizard, wizardType);

        // default values of recognized options
        context.setVariableIfMissing(PROP_ADDPROJECTREFERENCE, "true");
        context.setVariableIfMissing(PROP_SOURCEFOLDERS, "");
        context.setVariableIfMissing(PROP_NEDSOURCEFOLDERS, "");
        context.setVariableIfMissing(PROP_MAKEMAKEOPTIONS, "");

        return context;
    }

    public void performFinish(CreationContext context) throws CoreException {
        super.performFinish(context);

        Assert.isTrue(context.getFolder() instanceof IProject);
        if (getTemplateFolder()!=null && XSWTDataBinding.toBoolean(context.getVariables().get(PROP_ADDPROJECTREFERENCE)))
            ProjectUtils.addReferencedProject((IProject)context.getFolder(), getTemplateFolder().getProject(), context.getProgressMonitor());

        String[] nedSrcFolders = XSWTDataBinding.toStringArray(context.getVariables().get(PROP_NEDSOURCEFOLDERS), " *, *");
        if (nedSrcFolders.length > 0)
            createAndSetNedSourceFolders(nedSrcFolders, context);

        boolean supportCPlusPlus = XSWTDataBinding.toBoolean(context.getVariables().get(PROP_WITHCPLUSPLUSSUPPORT));
        if (supportCPlusPlus) {
            String[] srcFolders = XSWTDataBinding.toStringArray(context.getVariables().get(PROP_SOURCEFOLDERS), " *, *");
            if (srcFolders.length > 0)
                createAndSetSourceFolders(srcFolders, context);

            Map<String,String> makemakeOptions = XSWTDataBinding.toStringMap(context.getVariables().get(PROP_MAKEMAKEOPTIONS));
            if (!makemakeOptions.isEmpty())
                createBuildSpec(makemakeOptions, context);
        }
    }

    /**
     * Creates a default build spec (project root being makemake folder)
     */
    protected void createDefaultBuildSpec(CreationContext context) throws CoreException {
        Assert.isTrue(context.getFolder() instanceof IProject);
        BuildSpecification.createInitial((IProject)context.getFolder()).save();
    }

    /**
     * Creates the given folders, and a folder. If the parent folder(s) do not exist, they are created.
     */
    public void createAndSetNedSourceFolders(String[] projectRelativePaths, CreationContext context) throws CoreException {
        Assert.isTrue(context.getFolder() instanceof IProject);
        for (String path : projectRelativePaths)
            if (!path.equals("."))
                createFolder(path, context);
        IContainer[] folders = toUniqueFolders(context.getFolder(), projectRelativePaths);
        NedSourceFoldersConfiguration config = new NedSourceFoldersConfiguration(folders, new String[0]);
        ProjectUtils.saveNedFoldersFile((IProject)context.getFolder(), config);
    }

    /**
     * Sets C++ source folders to the given list.
     */
    public void createAndSetSourceFolders(String[] projectRelativePaths, CreationContext context) throws CoreException {
        Assert.isTrue(context.getFolder() instanceof IProject);
        for (String path : projectRelativePaths)
            if (!path.equals("."))
                createFolder(path, context);
        IContainer[] folders = toUniqueFolders(context.getFolder(), projectRelativePaths);
        setSourceLocations(folders, context);
    }

    protected static IContainer[] toUniqueFolders(IContainer folder, String[] folderRelativePaths) {
        Set<IContainer> folders = new HashSet<IContainer>();
        for (String path : folderRelativePaths)
            folders.add(getFolder(folder, path));
        return folders.toArray(new IContainer[]{});
    }

    /**
     * Sets the project's source locations list to the given list of folders, in all configurations.
     * (Previous source entries get overwritten.)
     */
    public void setSourceLocations(IContainer[] folders, CreationContext context) throws CoreException {
        if (System.getProperty("org.omnetpp.test.unit.running") != null)
            return; // in the test case we don't create a full CDT project, so the code below would throw NPE

        Assert.isTrue(context.getFolder() instanceof IProject);
        ICProjectDescription projectDescription = CoreModel.getDefault().getProjectDescription((IProject)context.getFolder(), true);
        int n = folders.length;
        for (ICConfigurationDescription configuration : projectDescription.getConfigurations()) {
            ICSourceEntry[] entries = new CSourceEntry[n];
            for (int i=0; i<n; i++) {
                Assert.isTrue(folders[i].getProject().equals(context.getFolder()));
                entries[i] = new CSourceEntry(folders[i].getProjectRelativePath(), new IPath[0], 0);
            }
            try {
                configuration.setSourceEntries(entries);
            }
            catch (WriteAccessException e) {
                Activator.logError(e); // should not happen, as we called getProjectDescription() with write=true
            }
        }
        CoreModel.getDefault().setProjectDescription((IProject)context.getFolder(), projectDescription);
    }

    /**
     * Sets the makemake options on the given project. Array must contain folderPath1, options1,
     * folderPath2, options2, etc. If an options string is null or "CUSTOM", the make type
     * for the corresponding folder will be set to CUSTOM, otherwise to MAKEMAKE.
     */
    public void createBuildSpec(String[] pathsAndMakemakeOptions, CreationContext context) throws CoreException {
        Assert.isTrue(pathsAndMakemakeOptions.length%2 == 0);
        Map<String, String> tmp = new HashMap<String, String>();
        for (int i=0; i<pathsAndMakemakeOptions.length; i+=2)
            tmp.put(pathsAndMakemakeOptions[i], pathsAndMakemakeOptions[i+1]);
        createBuildSpec(tmp, context);
    }

    /**
     * Sets the makemake options on the given project. Array must contain folderPaths
     * as keys, and options as values. If an options string is null or "CUSTOM",
     * the make type for the corresponding folder will be set to CUSTOM, otherwise to MAKEMAKE.
     */
    public void createBuildSpec(Map<String,String> pathsAndMakemakeOptions, CreationContext context) throws CoreException {
        Assert.isTrue(context.getFolder() instanceof IProject);
        IProject project = (IProject)context.getFolder();
        BuildSpecification buildSpec = BuildSpecification.createBlank(project);
        for (String folderPath: pathsAndMakemakeOptions.keySet()) {
            String args = pathsAndMakemakeOptions.get(folderPath);
            IContainer folder = getFolder(project, folderPath);
            if (args==null || args.equals("CUSTOM"))
                buildSpec.setFolderMakeType(folder, BuildSpecification.CUSTOM);
            else {
                buildSpec.setFolderMakeType(folder, BuildSpecification.MAKEMAKE);
                MakemakeOptions options = MakemakeOptions.parse(args);
                buildSpec.setMakemakeOptions(folder, options);
            }
        }
        buildSpec.save();
    }

    /**
     * An IContainer.getFolder(IPath) that understands "." (or "") as path as well.
     */
    protected static IContainer getFolder(IContainer folder, String path) {
        return (path.equals("") || path.equals(".")) ? folder : folder.getFolder(new Path(path));
    }
}
