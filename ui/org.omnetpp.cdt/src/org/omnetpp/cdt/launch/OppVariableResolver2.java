/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.launch;

import java.io.File;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.variables.IDynamicVariable;
import org.eclipse.core.variables.IDynamicVariableResolver;
import org.omnetpp.cdt.Activator;
import org.omnetpp.common.OmnetppDirs;

/**
 * Resolves opp_additional_path
 *
 * @author andras
 */
public class OppVariableResolver2 implements IDynamicVariableResolver {
    public static final String OPP_ADDITIONAL_PATH = "opp_additional_path";

    public String resolveValue(IDynamicVariable variable, String argument) throws CoreException {
        String varName = variable.getName();

        if (!varName.equals(OPP_ADDITIONAL_PATH))
            abort("Variable ${"+ varName +"} unsupported by resolver class", null);
        if (argument != null)
            abort("${" + varName +"} requires no argument", null);

        boolean isMSABI = OmnetppDirs.isOppsimMSABILibraryPresent(true) || OmnetppDirs.isOppsimMSABILibraryPresent(false);
        // add visualc or mingw specific binary directory depending whether we are using MSABI or mingw libraries
        String p1 = (isMSABI ? OmnetppDirs.getToolsVisualCDepsBinDir() : OmnetppDirs.getToolsMingwDepsBinDir());
        if (!p1.isEmpty()) p1 += File.pathSeparator;
        String p2 = OmnetppDirs.getToolsMsysBinDir();
        if (!p2.isEmpty()) p2 += File.pathSeparator;
        String p3 = OmnetppDirs.getToolsMingwBinDir();
        if (!p3.isEmpty()) p3 += File.pathSeparator;

        return p1 + p2 + p3;
    }

    protected void abort(String message, Throwable exception) throws CoreException {
        throw new CoreException(new Status(IStatus.ERROR, Activator.PLUGIN_ID, 1, message, exception));
    }
}
