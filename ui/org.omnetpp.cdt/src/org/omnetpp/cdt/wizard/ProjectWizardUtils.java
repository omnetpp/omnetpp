/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.wizard;

import java.util.HashMap;
import java.util.Map;

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
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.makefile.BuildSpecification;
import org.omnetpp.cdt.makefile.MakemakeOptions;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.wizard.CreationContext;

public class ProjectWizardUtils {
//TODO: context.getVariables().put("IDEUtils", BeansWrapper.getDefaultInstance().getStaticModels().get(IDEUtils.class.getName()));

    /**
     * Creates a folder. If the parent folder(s) do not exist, they are created.
     */
    public static void createFolder(String projectRelativePath, CreationContext context) throws CoreException {
        IFolder folder = context.getFolder().getFolder(new Path(projectRelativePath));
        if (!folder.getParent().exists())
            createFolder(folder.getParent().getProjectRelativePath().toString(), context);
        if (!folder.exists())
            folder.create(true, true, context.getProgressMonitor());
    }
    
    /**
     * Creates the given folders, and a folder. If the parent folder(s) do not exist, they are created.
     */
    public static void createAndSetNedSourceFolders(String[] projectRelativePaths, CreationContext context) throws CoreException {
        for (String path : projectRelativePaths)
            createFolder(path, context);
        IContainer[] folders = new IContainer[projectRelativePaths.length];
        for (int i=0; i<projectRelativePaths.length; i++)
            folders[i] = context.getFolder().getFolder(new Path(projectRelativePaths[i]));
        ProjectUtils.saveNedFoldersFile((IProject)context.getFolder(), folders);
    }

    /**
     * Sets C++ source folders to the given list.
     */
    public static void createAndSetSourceFolders(String[] projectRelativePaths, CreationContext context) throws CoreException {
        for (String path : projectRelativePaths)
            createFolder(path, context);
        IContainer[] folders = new IContainer[projectRelativePaths.length];
        for (int i=0; i<projectRelativePaths.length; i++)
            folders[i] = context.getFolder().getFolder(new Path(projectRelativePaths[i]));
        setSourceLocations(folders, context);
    }
    
    /**
     * Sets the project's source locations list to the given list of folders, in all configurations. 
     * (Previous source entries get overwritten.)  
     */
    public static void setSourceLocations(IContainer[] folders, CreationContext context) throws CoreException {
        if (System.getProperty("org.omnetpp.test.unit.running") != null)
            return; // in the test case we don't create a full CDT project, so the code below would throw NPE

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
     * Sets the makemake options on the given project. Array must contain folderPaths 
     * as keys, and options as values.
     */
    public static void createBuildSpec(String[] pathsAndMakemakeOptions, CreationContext context) throws CoreException {
    	Assert.isTrue(pathsAndMakemakeOptions.length%2 == 0);
    	Map<String, String> tmp = new HashMap<String, String>();
    	for (int i=0; i<pathsAndMakemakeOptions.length; i+=2)
    		tmp.put(pathsAndMakemakeOptions[i], pathsAndMakemakeOptions[i+1]);
    	createBuildSpec(tmp, context);    
    }
    
    /**
     * Sets the makemake options on the given project. Array must contain folderPath1, options1, 
     * folderPath2, options2, etc.
     */
    public static void createBuildSpec(Map<String,String> pathsAndMakemakeOptions, CreationContext context) throws CoreException {
        IProject project = (IProject)context.getFolder();
        BuildSpecification buildSpec = BuildSpecification.createBlank(project);
        for (String folderPath: pathsAndMakemakeOptions.keySet()) {
            String args = pathsAndMakemakeOptions.get(folderPath);
            IContainer folder = folderPath.equals(".") ? project : project.getFolder(new Path(folderPath));
            buildSpec.setFolderMakeType(folder, BuildSpecification.MAKEMAKE);
            MakemakeOptions options = new MakemakeOptions(args);
            buildSpec.setMakemakeOptions(folder, options);
        }
        buildSpec.save();
    }

}
