package org.omnetpp.cdt;

import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.envvar.IBuildEnvironmentVariable;
import org.eclipse.cdt.managedbuilder.envvar.IEnvironmentVariableProvider;
import org.eclipse.core.runtime.Platform;


/**
 * Based on MingwEnvironmentVariableSupplier from CDT 5.0.
 * Note: we cannot extend CDT's class because it is not exported from the plugin.
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
