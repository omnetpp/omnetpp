package org.omnetpp.academic;

import org.eclipse.cdt.managedbuilder.core.IManagedIsToolChainSupported;
import org.eclipse.cdt.managedbuilder.core.IToolChain;
import org.eclipse.core.runtime.PluginVersionIdentifier;

public class MsvcToolchainIsSupported implements IManagedIsToolChainSupported {

	public boolean isSupported(IToolChain toolChain, PluginVersionIdentifier version, String instance) {
		// we do not support MSVC in academic version
		return false;
	}

}
