package org.omnetpp.cdt;

import org.eclipse.cdt.internal.core.envvar.EnvironmentVariableManager;
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
    // BEWARE: https://bugs.eclipse.org/bugs/show_bug.cgi?id=213167
    // currently the environment variable cannot be pre/appended because of the above issue.
    // we have to get it by hand and append the rest automatically so we have to pass in the 
    // outer provider
    private static class OmnetppBinPathEnvironmentVariable implements IBuildEnvironmentVariable {
        private IEnvironmentVariableProvider provider;
        private IManagedProject project;

        public OmnetppBinPathEnvironmentVariable() {
        }
        
//        public OmnetppBinPathEnvironmentVariable(IManagedProject project,
//                IEnvironmentVariableProvider provider) {
//            this.project = project;
//            this.provider = provider;
//        }

        public String getName() {
            return "PATH";
        }
        
        public String getValue() {
            // FIXME in fact we have to look into the config files and get the bin directory from there
            // THIS is a workaround to get the original PATH value so we can append ourselves to it
            String omnetppBin = OmnetppMainPlugin.getDefault().getPreferenceStore().getString(OmnetppPreferencePage.OMNETPP_ROOT)+"\\bin";
//            IEnvironmentVariable oldVariable = provider.getVariable(getName(), ManagedBuildManager.getBuildInfo(project.getOwner()).getDefaultConfiguration(), true);
//            return omnetppBin + (oldVariable != null ? getDelimiter()+ oldVariable.getValue() : "");
            return omnetppBin;
        }
        
        public int getOperation() {
            return IBuildEnvironmentVariable.ENVVAR_PREPEND;
        }
        
        public String getDelimiter() {
            return EnvironmentVariableManager.getDefault().getDefaultDelimiter();
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
//        OmnetppBinPathEnvironmentVariable omnetppBinPathEnvironmentVariable 
//            = new OmnetppBinPathEnvironmentVariable(project, provider);
        return new IBuildEnvironmentVariable[] {omnetppBinPathEnvironmentVariable};
    }
    
}


    
    
