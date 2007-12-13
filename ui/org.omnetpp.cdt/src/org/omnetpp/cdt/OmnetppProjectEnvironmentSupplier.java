package org.omnetpp.cdt;

import org.eclipse.cdt.managedbuilder.core.IManagedProject;
import org.eclipse.cdt.managedbuilder.envvar.IBuildEnvironmentVariable;
import org.eclipse.cdt.managedbuilder.envvar.IEnvironmentVariableProvider;
import org.eclipse.cdt.managedbuilder.envvar.IProjectEnvironmentVariableSupplier;

import org.omnetpp.ide.OmnetppMainPlugin;
import org.omnetpp.ide.preferences.OmnetppPreferencePage;

/**
 * Adds environment variables specific to an omnetpp simulation project. Prepends the omnetpp_bin 
 * directory to the path. 
 */
public class OmnetppProjectEnvironmentSupplier implements IProjectEnvironmentVariableSupplier 
{
    private static class OmnetppBinPathEnvironmentVariable implements IBuildEnvironmentVariable {
        public OmnetppBinPathEnvironmentVariable() {
        }
        
        public String getName() {
            return "PATH";
        }
        
        public String getValue() {
            // FIXME in fact we have to look into the config files and get the bin directory from there
            return OmnetppMainPlugin.getDefault().getPreferenceStore().getString(OmnetppPreferencePage.OMNETPP_ROOT)+"\\bin";
            // return "C:\\Work\\Projects\\omnest\\omnetpp4\\omnetpp\\bin";
        }
        
        public int getOperation() {
            return IBuildEnvironmentVariable.ENVVAR_PREPEND;
        }
        
        public String getDelimiter() {
            return ";";
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


    
    
