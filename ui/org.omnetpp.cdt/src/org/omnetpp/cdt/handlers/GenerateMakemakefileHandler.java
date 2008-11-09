package org.omnetpp.cdt.handlers;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.makefile.BuildSpecification;
import org.omnetpp.cdt.makefile.MakefileTools;
import org.omnetpp.common.util.StringUtils;

/**
 * Generate a "makemakefile" for the selected projects.
 * 
 * @author Andras
 */
//XXX half-cooked
public class GenerateMakemakefileHandler extends AbstractHandler {
    // boilerplate code for Makemakefile
    public static final String BOILERPLATE = 
        "#\n" + 
        "# Makefile to create all other makefiles for the project.\n" + 
        "# This same file is used on all platforms including Linux (gnu make) and Windows (nmake).\n" + 
        "#\n" + 
        "# GENERATED FILE -- DO NOT MODIFY.\n" + 
        "#\n" + 
        "# The vars ROOT, MAKEMAKE and EXT have to be specified externally, on the 'make' command line.\n" + 
        "#ROOT=/home/user/projects/foo\n" + 
        "#MAKEMAKE=opp_nmakemake\n" + 
        "#EXT=.vc\n" + 
        "\n" + 
        "# To build Windows DLLs, add this to OPTS below: -PINET_API\n" + 
        "OPTS=-f -b $(ROOT) -c $(ROOT)/inetconfig$(EXT)\n" + 
        "\n" + 
        "all:\n";

    private ISelection selection;

    public Object execute(ExecutionEvent event) throws ExecutionException {
        List<IProject> projects = getSelectedOpenProjects();

        for (IProject project : projects)
            try {
                generateMakeMakefile(project, null);
            }
            catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            catch (CoreException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        
        return null;
    }

    @SuppressWarnings("unchecked")
    protected List<IProject> getSelectedOpenProjects() {
        List<IProject> projects = new ArrayList<IProject>();
        if (selection instanceof IStructuredSelection) {
            for (Iterator it = ((IStructuredSelection) selection).iterator(); it.hasNext();) {
                Object element = it.next();
                IProject project = null;
                if (element instanceof IProject)
                    project = (IProject) element;
                else if (element instanceof IAdaptable)
                    project = (IProject) ((IAdaptable) element).getAdapter(IProject.class);
                if (project != null && project.isOpen())
                    projects.add(project);
            }
        }
        return projects;
    }

    //XXX currently unused; we may create some Action to invoke it
    public static void generateMakeMakefile(IProject project, IProgressMonitor monitor) throws IOException, CoreException {
        BuildSpecification buildSpec = BuildSpecification.readBuildSpecFile(project); //XXX possible IllegalArgumentException
        IContainer[] makemakeFolders = buildSpec.getMakemakeFolders().toArray(new IContainer[]{});
        Map<IContainer, String> targetNames = generateTargetNames(makemakeFolders);
        Map<IContainer, Set<IContainer>> folderDependencies = Activator.getDependencyCache().getFolderDependencies(project);
        String makeMakeFile = generateMakeMakefile(makemakeFolders, folderDependencies, targetNames);
        IFile file = project.getFile("Makemakefile");
        MakefileTools.ensureFileContent(file, makeMakeFile.getBytes(), monitor);
    }

    /**
     * Generate a script to re-generate makefiles from the command line.
     */
    protected static String generateMakeMakefile(IContainer[] containers, Map<IContainer, Set<IContainer>> deps,Map<IContainer, String> targetNames) {
        //FIXME this method totally out of date -- should use translateOptions()!!!
        // generate the makefile
        String result = BOILERPLATE;
        String allTargetNames = StringUtils.join(targetNames.values(), " ");
        result += StringUtils.indentLines(StringUtils.breakLines(allTargetNames, 90), "\t") + "\n\n";
        for (IContainer folder : containers) {
            List<String> includeOptions = new ArrayList<String>();
            if (deps.containsKey(folder))
                for (IContainer dep : deps.get(folder))
                    includeOptions.add("-I" + MakefileTools.makeRelativePath(dep.getFullPath(), folder.getFullPath()).toString());
            String folderPath = folder.getProjectRelativePath().toString();  //XXX refine: only relative if it fits best

            result += targetNames.get(folder) + ":\n";
            result += "\tcd " + folderPath + " && $(MAKEMAKE) $(OPTS) -n -r " + StringUtils.join(includeOptions, " ") + "\n";
        }
        return result;
    }

    public static Map<IContainer, String> generateTargetNames(IContainer[] containers) {
        // generate unique target name for each folder
        String reservedNames = "all clean makefiles dist install";
        for (IContainer folder : containers)
            reservedNames += " " + folder.getName(); 
        Map<IContainer,String> targetNames = new LinkedHashMap<IContainer, String>();
        for (IContainer folder : containers) {
            String targetName = folder.getName().toString().replaceAll("[^a-zA-Z0-9]+", "_") + "_dir";
            targetName = targetName.replaceFirst("_$", "");
            if (targetNames.values().contains(targetName)) {
                int k = 2;
                while (targetNames.values().contains(tweakName(targetName,k)) || reservedNames.contains(tweakName(targetName,k)))
                    k++;
                targetName = tweakName(targetName,k);
            }
            targetNames.put(folder, targetName);
        }
        return targetNames;
    }

    private static String tweakName(String name, int k) {
        Assert.isTrue(name.endsWith("_dir"));
        return name.replaceAll("_dir$", "_" + k + "_dir");
    }
}
