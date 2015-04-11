/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt;

import org.eclipse.cdt.managedbuilder.core.IManagedProject;
import org.eclipse.cdt.managedbuilder.macros.IBuildMacro;
import org.eclipse.cdt.managedbuilder.macros.IBuildMacroProvider;
import org.eclipse.cdt.managedbuilder.macros.IProjectBuildMacroSupplier;

/**
 * Macro provider, currently empty.
 *
 * @author Andras
 */
public class OmnetppProjectMacroSupplier implements IProjectBuildMacroSupplier {

    public IBuildMacro getMacro(String macroName, IManagedProject project, IBuildMacroProvider provider) {
        // note: be careful NOT to access the CDT project settings (IConfigurationDescription) here! see bug #299
        return null;
    }

    public IBuildMacro[] getMacros(IManagedProject project, IBuildMacroProvider provider) {
        // note: be careful NOT to access the CDT project settings (IConfigurationDescription) here! see bug #299
        return new IBuildMacro[0];
    }

}
