package org.omnetpp.ide.variables;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.variables.IDynamicVariable;
import org.eclipse.core.variables.IDynamicVariableResolver;
import org.omnetpp.ide.OmnetppMainPlugin;

/**
 * resolves OMNETPP_XXX variables (root,bin,include,lib)
 *
 */
public class OmnetppDirResolver implements IDynamicVariableResolver {
    public static final String OMNETPP_ROOT = "OMNETPP_ROOT";
    public static final String OMNETPP_BIN_DIR = "OMNETPP_BIN_DIR";
    public static final String OMNETPP_INCL_DIR = "OMNETPP_INCL_DIR";
    public static final String OMNETPP_LIB_DIR = "OMNETPP_LIB_DIR";
    
	public String resolveValue(IDynamicVariable variable, String argument)
			throws CoreException {
		if (OMNETPP_ROOT.equalsIgnoreCase(variable.getName())) {
			return OmnetppMainPlugin.getOmnetppRootDir();
		}
		if (OMNETPP_BIN_DIR.equalsIgnoreCase(variable.getName())) {
			return OmnetppMainPlugin.getOmnetppBinDir();
		}
		if (OMNETPP_INCL_DIR.equalsIgnoreCase(variable.getName())) {
			return OmnetppMainPlugin.getOmnetppInclDir();
		}
		if (OMNETPP_LIB_DIR.equalsIgnoreCase(variable.getName())) {
			return OmnetppMainPlugin.getOmnetppLibDir();
		}
		
		return null;
	}

}
