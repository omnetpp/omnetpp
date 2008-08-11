package org.omnetpp.cdt;

import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.envvar.IBuildEnvironmentVariable;
import org.eclipse.cdt.managedbuilder.envvar.IEnvironmentVariableProvider;
import org.eclipse.core.runtime.Platform;


/**
 * We use the same toolchain for gcc on all platforms, so we need a combined
 * environment variable supplier that works on all platforms. As of CDT 5.0,
 * CDT contains a MingwEnvironmentVariableSupplier but no supplier for other platforms
 * (except Cygwin which we don't support). So here we invoke the MinGW one if platform 
 * is win32, and do nothing in other cases.
 * 
 * @author Andras
 */
public class GCCEnvironmentVariableSupplier extends MingwEnvironmentVariableSupplier_COPY {

    @Override
    public IBuildEnvironmentVariable getVariable(String variableName, IConfiguration configuration, IEnvironmentVariableProvider provider) {
        if (Platform.getOS().equals(Platform.OS_WIN32))
            return super.getVariable(variableName, configuration, provider);
        else
            return null;
    }

    @Override
    public IBuildEnvironmentVariable[] getVariables(IConfiguration configuration, IEnvironmentVariableProvider provider) {
        if (Platform.getOS().equals(Platform.OS_WIN32))
            return super.getVariables(configuration, provider);
        else
            return new IBuildEnvironmentVariable[0];
    }

}
