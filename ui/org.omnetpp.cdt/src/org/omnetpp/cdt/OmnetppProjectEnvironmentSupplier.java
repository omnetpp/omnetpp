/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt;

import java.io.File;

import org.eclipse.cdt.managedbuilder.core.IManagedProject;
import org.eclipse.cdt.managedbuilder.envvar.IBuildEnvironmentVariable;
import org.eclipse.cdt.managedbuilder.envvar.IEnvironmentVariableProvider;
import org.eclipse.cdt.managedbuilder.envvar.IProjectEnvironmentVariableSupplier;
import org.omnetpp.common.OmnetppDirs;

/**
 * Adds environment variables specific to an omnetpp simulation project. Prepends the omnetpp_bin
 * directory to the path.
 */
public class OmnetppProjectEnvironmentSupplier implements IProjectEnvironmentVariableSupplier
{
    // BEWARE: https://bugs.eclipse.org/bugs/show_bug.cgi?id=213167
    // currently the environment variable cannot be pre/appended because of the above issue.
    // we have to get it by hand and append the rest automatically so we have to pass in the
    // outer provider
    // It seems that only the project properties dialog  has problems with it. If the focus is on the environment tab
    // and we open the properties dialog, the value is wrongly set. On the other hand if the focus is on an other
    // tab when we open it the path is correctly displayed

    // this path environment is used during build and debugging
    private static class OmnetppBinPathEnvironmentVariable implements IBuildEnvironmentVariable {

        public OmnetppBinPathEnvironmentVariable() {
        }

        public String getName() {
            return "PATH";
        }

        public String getValue() {
            String oppBinDir = OmnetppDirs.getOmnetppBinDir() + getDelimiter();

            boolean isClangC2 = OmnetppDirs.isOppsimClangC2LibraryPresent(true) || OmnetppDirs.isOppsimClangC2LibraryPresent(false);
            // add visualc or mingw specific binary directory depending whether we are using clang or mingw
            String toolsDir = (isClangC2 ? OmnetppDirs.getToolsVisualCBinDir() : OmnetppDirs.getToolsMingwBinDir());
            if (!toolsDir.isEmpty()) toolsDir += getDelimiter();

            String msysDir = OmnetppDirs.getToolsMsysBinDir();
            if (!msysDir.isEmpty()) msysDir += getDelimiter();

            return (toolsDir.isEmpty() && msysDir.isEmpty()) ? oppBinDir : oppBinDir + toolsDir + msysDir;
        }

        public int getOperation() {
            return IBuildEnvironmentVariable.ENVVAR_PREPEND;
        }

        public String getDelimiter() {
            return File.pathSeparator;
        }
    }

    OmnetppBinPathEnvironmentVariable omnetppBinPathEnvironmentVariable
        = new OmnetppBinPathEnvironmentVariable();

    public IBuildEnvironmentVariable getVariable(String variableName, IManagedProject project,
            IEnvironmentVariableProvider provider) {
        if (omnetppBinPathEnvironmentVariable.getName().equals(variableName)) {
            return omnetppBinPathEnvironmentVariable;
        }
        return null;
    }

    public IBuildEnvironmentVariable[] getVariables(IManagedProject project,
            IEnvironmentVariableProvider provider) {
        return new IBuildEnvironmentVariable[] {omnetppBinPathEnvironmentVariable};
    }
}
