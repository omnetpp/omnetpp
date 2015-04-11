/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.model;

import java.util.Vector;

import org.omnetpp.ned.model.ex.PropertyElementEx;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;

/**
 * Value object, stores the result of a property resolution.
 */
public class PropertyResolution {
    public String fullPath;
    public ISubmoduleOrConnection[] elementPath;
    public PropertyElementEx propertyDeclaration;

    // during analysis of which section
    public String activeSection;

    // for convenience
    public PropertyResolution(String fullPath, Vector<ISubmoduleOrConnection> elementPath, PropertyElementEx signalDeclaration, String activeSection) {
        this.fullPath = fullPath;
        this.elementPath = elementPath.toArray(new ISubmoduleOrConnection[0]);
        this.propertyDeclaration = signalDeclaration;
        this.activeSection = activeSection;
    }
}
