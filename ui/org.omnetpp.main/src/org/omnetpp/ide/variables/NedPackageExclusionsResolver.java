/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ide.variables;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.apache.commons.lang3.ArrayUtils;
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
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ide.OmnetppMainPlugin;

/**
 * Assembles the value for the NED package exclusions Eclipse variable. Argument
 * is the omnetpp.ini file, or any resource in its project.
 *
 * @author Andras
 */
public class NedPackageExclusionsResolver implements IDynamicVariableResolver {
    private static final String USAGE = "${" + IConstants.VAR_NED_PACKAGE_EXCLUSIONS + ":arg}";

    public String resolveValue(IDynamicVariable variable, String argument) throws CoreException {
        if (argument == null)
            abort(USAGE + " requires an argument", null);

        IResource resource = ResourcesPlugin.getWorkspace().getRoot().findMember(new Path(argument));
        if (resource == null || !resource.exists() || resource.getProject() == null)
            abort("argument to " + USAGE + " needs to be an existing file, folder, or project", null);
        IProject project = resource.getProject();

        try {
            List<String> result = new ArrayList<>();
            IProject[] projects = ArrayUtils.add(ProjectUtils.getAllReferencedOmnetppProjects(project), 0, project);
            for (IProject proj : projects)
                result.addAll(Arrays.asList(ProjectUtils.readNedFoldersFile(proj).getExcludedPackages()));
            return StringUtils.join(result, ";");
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
