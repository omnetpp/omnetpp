/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.model;

import java.util.Vector;

import org.omnetpp.ned.model.ex.PropertyElementEx;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;

/**
 * Value object, stores the result of a parameter resolution.
 */
public class SignalResolution {
	public String fullPath;
	public ISubmoduleOrConnection[] elementPath;
	public PropertyElementEx signalDeclaration;

	// during analysis of which section
	public String activeSection;

	// for convenience
	public SignalResolution(String fullPath, Vector<ISubmoduleOrConnection> elementPath, PropertyElementEx signalDeclaration, String activeSection) {
		this.fullPath = fullPath;
		this.elementPath = elementPath.toArray(new ISubmoduleOrConnection[0]);
		this.signalDeclaration = signalDeclaration;
		this.activeSection = activeSection;
	}
}
