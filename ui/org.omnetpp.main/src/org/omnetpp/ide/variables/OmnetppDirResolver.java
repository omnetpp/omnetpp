/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ide.variables;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.variables.IDynamicVariable;
import org.eclipse.core.variables.IDynamicVariableResolver;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.OmnetppDirs;

/**
 * Resolves OMNeT++-related Eclipse variables (root, bin, include, lib)
 */
public class OmnetppDirResolver implements IDynamicVariableResolver {
    public String resolveValue(IDynamicVariable variable, String argument) throws CoreException {
        switch (variable.getName().toLowerCase()) {
        case IConstants.VAR_OMNETPP_ROOT: return OmnetppDirs.getOmnetppRootDir();
        case IConstants.VAR_OMNETPP_BIN_DIR: return OmnetppDirs.getOmnetppBinDir();
        case IConstants.VAR_OMNETPP_INCL_DIR: return OmnetppDirs.getOmnetppInclDir();
        case IConstants.VAR_OMNETPP_LIB_DIR: return OmnetppDirs.getOmnetppLibDir();
        default: return null;
        }
    }

}
