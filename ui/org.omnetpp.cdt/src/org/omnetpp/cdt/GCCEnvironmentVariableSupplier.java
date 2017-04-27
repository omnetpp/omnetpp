/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt;

import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.envvar.IBuildEnvironmentVariable;
import org.eclipse.cdt.managedbuilder.envvar.IConfigurationEnvironmentVariableSupplier;
import org.eclipse.cdt.managedbuilder.envvar.IEnvironmentVariableProvider;
import org.eclipse.core.runtime.Platform;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.OmnetppDirs;

/**
 * We use the same toolchain for gcc on all platforms, so we need a combined
 * environment variable supplier that works on all platforms.
 *
 * Note: we use omnetpp/msys/bin and omnetpp/mingw/bin instead of the registry reading code
 * in CDT's MingwEnvironmentVariableSupplier.
 *
 * @author Andras
 */
public class GCCEnvironmentVariableSupplier implements IConfigurationEnvironmentVariableSupplier {
    public IBuildEnvironmentVariable getPathVariable() {
        StringBuilder buff = new StringBuilder();

        if (Platform.getOS().equals(Platform.OS_WIN32)) {
            // add the visualc/bin dir also on OMNEST if we are using clang/c2 compiler
            if (OmnetppDirs.isOppsimClangC2LibraryPresent(true) || OmnetppDirs.isOppsimClangC2LibraryPresent(false)) {
                String visualcBinDir = OmnetppDirs.getToolsVisualCBinDir();
                if (!StringUtils.isEmpty(visualcBinDir))
                    buff.append(visualcBinDir).append(';');
            }

            // if we are using MINGW compiler (unix style libs)
            if (OmnetppDirs.isOppsimUnixStyleLibraryPresent(true) || OmnetppDirs.isOppsimUnixStyleLibraryPresent(false)) {
                String mingwBinDir = OmnetppDirs.getToolsMingwBinDir();
                if (!StringUtils.isEmpty(mingwBinDir))
                    buff.append(mingwBinDir).append(';');
            }

            String msysBinDir = OmnetppDirs.getToolsMsysBinDir();
            if (!StringUtils.isEmpty(msysBinDir))
                buff.append(msysBinDir).append(';');
        }

        return new BuildEnvironmentVariable("PATH", buff.toString(), IBuildEnvironmentVariable.ENVVAR_APPEND);
    }

    public IBuildEnvironmentVariable getVariable(String variableName, IConfiguration configuration, IEnvironmentVariableProvider provider) {
        return variableName.equals("PATH") ? getPathVariable() : null;
    }

    public IBuildEnvironmentVariable[] getVariables(IConfiguration configuration, IEnvironmentVariableProvider provider) {
        return new IBuildEnvironmentVariable[] { getPathVariable() };
    }
}
