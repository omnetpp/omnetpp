package org.omnetpp.cdt.launch;

import org.eclipse.cdt.launch.ui.CDebuggerTab;

/**
 * A Debugger tab with default constructor. Allows to be contributed via the extension registry.
 *
 */
public class OmnetppCDebuggerTab extends CDebuggerTab {

	public OmnetppCDebuggerTab() {
		super(false);
	}
}
