/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.engineext;

import java.util.EventListener;

import org.omnetpp.scave.engine.ResultFileManager;

public interface IResultFilesChangeListener extends EventListener {
	public void resultFileManagerChanged(ResultFileManager manager);
}
