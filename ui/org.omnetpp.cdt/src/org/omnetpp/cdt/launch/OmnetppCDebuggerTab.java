/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.launch;

import org.eclipse.cdt.dsf.gdb.internal.ui.launching.LocalApplicationCDebuggerTab;


/**
 * A Debugger tab with default constructor. Left here only that we detect if it is removed.
 * (because it is an internal class)
 *
 */
public class OmnetppCDebuggerTab extends LocalApplicationCDebuggerTab {

    public OmnetppCDebuggerTab() {
        super();
    }
}
