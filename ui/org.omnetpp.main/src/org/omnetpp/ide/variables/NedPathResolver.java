/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ide.variables;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.variables.IDynamicVariable;
import org.eclipse.core.variables.IDynamicVariableResolver;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.ide.OmnetppMainPlugin;

/**
 * Assembles the value for the NED path Eclipse environment variable. Argument
 * is the omnetpp.ini file, or any resource in its project.
 *
 * @author Andras
 */
public class NedPathResolver implements IDynamicVariableResolver {
    private static final String USAGE = "${" + IConstants.VAR_NED_PATH + ":arg}";

    public String resolveValue(IDynamicVariable variable, String argument) throws CoreException {
        if (argument == null)
            abort(USAGE + " requires an argument", null);

        boolean wantLocation = variable.getName().endsWith("_loc");

        IResource resource = ResourcesPlugin.getWorkspace().getRoot().findMember(new Path(argument));
        if (resource == null || !resource.exists() || resource.getProject() == null)
            abort("argument to " + USAGE + " needs to be an existing file, folder, or project", null);
        IProject project = resource.getProject();

        try {
            String result = "";
            String pathSep = System.getProperty("path.separator");
            // read the actual projects nedfolders file
            for (IContainer folder : ProjectUtils.readNedFoldersFile(project).getSourceFolders())
                result += pathSep + (wantLocation ? folder.getLocation().toOSString() : folder.getFullPath().toString());

            // do the same for the referenced projects
            for (IProject p : ProjectUtils.getAllReferencedOmnetppProjects(project))
                for(IContainer folder : ProjectUtils.readNedFoldersFile(p).getSourceFolders())
                    result += pathSep + (wantLocation ? folder.getLocation().toOSString() : folder.getFullPath().toString());
            return result;
        }
        catch (Exception e) {
            OmnetppMainPlugin.logError(e);
            return "";
        }
    }

    protected void abort(String message, Throwable exception) throws CoreException {
        throw new CoreException(new Status(IStatus.ERROR, OmnetppMainPlugin.PLUGIN_ID, 1, message, exception));
    }
}
