package org.omnetpp.cdt.launch;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.variables.IDynamicVariable;
import org.eclipse.core.variables.IDynamicVariableResolver;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.MingwEnvironmentVariableSupplier_COPY;
import org.omnetpp.common.util.StringUtils;

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

	    IPath msysBinDir = MingwEnvironmentVariableSupplier_COPY.getMsysBinDir();
        IPath mingwBinDir = MingwEnvironmentVariableSupplier_COPY.getBinDir();
        
        String pathSep = System.getProperty("path.separator");
        
        String result = "";
        if (msysBinDir != null)
            result += msysBinDir.toOSString();
        if (mingwBinDir != null)
            result += (StringUtils.isEmpty(result)? "" : pathSep) + mingwBinDir.toOSString();
		return result;
	}
	
    protected void abort(String message, Throwable exception) throws CoreException {
        throw new CoreException(new Status(IStatus.ERROR, Activator.PLUGIN_ID, 1, message, exception));
    }
}
