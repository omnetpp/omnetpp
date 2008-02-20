package org.omnetpp.cdt;

import org.eclipse.cdt.internal.core.envvar.EnvironmentVariableManager;
import org.eclipse.cdt.managedbuilder.core.IManagedProject;
import org.eclipse.cdt.managedbuilder.envvar.IBuildEnvironmentVariable;
import org.eclipse.cdt.managedbuilder.envvar.IEnvironmentVariableProvider;
import org.eclipse.cdt.managedbuilder.envvar.IProjectEnvironmentVariableSupplier;
import org.eclipse.core.runtime.Path;
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
    // It seems that only the project properties dialog  hasprobles with it. If the focs is on the environment tab
    // and we open the properties dialog, the value is wrongly set. On the other hand if the focus is on an other
    // tab when we open it the path is correctly displayed
    private static class OmnetppBinPathEnvironmentVariable implements IBuildEnvironmentVariable {

        public OmnetppBinPathEnvironmentVariable() {
        }
        
        public String getName() {
            return "PATH";
        }
        
        public String getValue() {
            // FIXME in fact we have to look into the config files and get the bin directory from there
            String omnetppBin = new Path(OmnetppMainPlugin.getDefault().getPreferenceStore().getString(OmnetppPreferencePage.OMNETPP_ROOT)).append("bin").toOSString();
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
        return new IBuildEnvironmentVariable[] {omnetppBinPathEnvironmentVariable};
    }
    
}


    
    
