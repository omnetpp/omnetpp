/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

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
