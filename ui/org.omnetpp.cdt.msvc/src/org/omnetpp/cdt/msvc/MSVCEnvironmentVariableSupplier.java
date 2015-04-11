/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.msvc;


import java.util.HashMap;
import java.util.Map;

import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.core.IManagedIsToolChainSupported;
import org.eclipse.cdt.managedbuilder.core.IToolChain;
import org.eclipse.cdt.managedbuilder.envvar.IBuildEnvironmentVariable;
import org.eclipse.cdt.managedbuilder.envvar.IConfigurationEnvironmentVariableSupplier;
import org.eclipse.cdt.managedbuilder.envvar.IEnvironmentVariableProvider;
import org.eclipse.core.runtime.Path;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.BuildEnvironmentVariable;
import org.omnetpp.cdt.msvc.ui.MSVCPreferencePage;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ide.OmnetppMainPlugin;
import org.osgi.framework.Version;

/**
 * Visual C environment detector for OMNeT++. Tries to detect Visual C and (optional) MS Windows SDK
 * installation. (Returns the latest version) Also checks whether the tool-chain is installed at all.
 * Looks for environment variables VCINSTALLDIR, VSINSTALLDIR, MSSdk. If you have several versions
 * of SDK / Visual C on your machine, and need a specific one (not the latest) define these variables
 * to point to the needed installation.
 *
 * Also adds omnetpp/bin and omnetpp/msys/bin to the PATH.
 *
 * @author DSchaefer
 * @author rhornig
 */
public class MSVCEnvironmentVariableSupplier implements IConfigurationEnvironmentVariableSupplier,
        IManagedIsToolChainSupported {

    public IBuildEnvironmentVariable getVariable(String variableName,
            IConfiguration configuration, IEnvironmentVariableProvider provider) {
        return createVars().get(variableName);
    }

    public IBuildEnvironmentVariable[] getVariables(
            IConfiguration configuration, IEnvironmentVariableProvider provider) {
        Map<String, IBuildEnvironmentVariable> vars = createVars();
        return vars.values().toArray(new IBuildEnvironmentVariable[vars.size()]);
    }

    /**
     * Returns the VS dir from the preferences; null if unset.
     */
    public static String getVSDir() {
        String vsDir = Activator.getDefault().getPreferenceStore().getString(MSVCPreferencePage.PREFKEY_VSDIR);
        return StringUtils.isEmpty(vsDir) ? null : vsDir;
    }

    /**
     * Returns the VC dir from the preferences; null if unset.
     */
    public static String getVCDir() {
        String vcDir = Activator.getDefault().getPreferenceStore().getString(MSVCPreferencePage.PREFKEY_VCDIR);
        return StringUtils.isEmpty(vcDir) ? null : vcDir;
    }

    /**
     * Returns the SDK dir from the preferences; null if unset.
     */
    public static String getSDKDir() {
        String sdkDir = Activator.getDefault().getPreferenceStore().getString(MSVCPreferencePage.PREFKEY_SDKDIR);
        return StringUtils.isEmpty(sdkDir) ? null : sdkDir;
    }

    /**
     * Returns true if the PATH environment variable should be appended to the build-time path.
     */
    public static boolean getAppendPath() {
        return Activator.getDefault().getPreferenceStore().getBoolean(MSVCPreferencePage.PREFKEY_APPENDPATH);
    }

    public boolean isSupported(IToolChain toolChain, Version version, String instance) {
        return getVCDir() != null;
    }

    private void addvar(Map<String, IBuildEnvironmentVariable> variables, IBuildEnvironmentVariable var) {
        variables.put(var.getName(), var);
    }

    private Map<String, IBuildEnvironmentVariable> createVars() {
        Map<String, IBuildEnvironmentVariable> vars = new HashMap<String, IBuildEnvironmentVariable>();

        // add OMNETPP_ROOT variable because it is needed by the nmake scripts
        addvar(vars, new BuildEnvironmentVariable("OMNETPP_ROOT", OmnetppMainPlugin.getOmnetppRootDir(), IBuildEnvironmentVariable.ENVVAR_REPLACE));

        String vcDir = getVCDir();
        if (vcDir == null)
            return vars;

        // The SDK Location
        String sdkDir = getSDKDir();

        // INCLUDE
        StringBuffer buff = new StringBuffer();

        buff.append(new Path(vcDir).append("INCLUDE").toOSString()+";");

        if (sdkDir != null)
            buff.append(new Path(sdkDir).append("Include").toOSString()+";");

        addvar(vars, new BuildEnvironmentVariable("INCLUDE", buff.toString(), IBuildEnvironmentVariable.ENVVAR_PREPEND));

        // LIB
        buff = new StringBuffer();
        buff.append(new Path(vcDir).append("LIB").toOSString()+";");
        if (sdkDir != null)
            buff.append(new Path(sdkDir).append("Lib").toOSString()+";");
        addvar(vars, new BuildEnvironmentVariable("LIB", buff.toString(), IBuildEnvironmentVariable.ENVVAR_PREPEND));

        // PATH
        buff = new StringBuffer();

        String msysDir = OmnetppMainPlugin.getMsysBinDir();
        if (!StringUtils.isEmpty(msysDir))
            buff.append(msysDir+";");

        String vsDir = getVSDir();
        if (vsDir != null)
            buff.append(new Path(vsDir).append("Common7\\IDE").toOSString()+";");

        buff.append(new Path(vcDir).append("Bin").toOSString()+";");

        if (sdkDir != null)
            buff.append(new Path(sdkDir).append("Bin").toOSString()+";");

        if (getAppendPath())
            buff.append(System.getenv("PATH")+";");
        addvar(vars, new BuildEnvironmentVariable("PATH", buff.toString(), IBuildEnvironmentVariable.ENVVAR_PREPEND));

        return vars;
    }

}
