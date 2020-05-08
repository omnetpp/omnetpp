/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model;

public class ModelChangeEvent {
    private ModelObject subject;

    public ModelChangeEvent(ModelObject subject) {
        this.subject = subject;
    }

    public ModelObject getSubject() {
        return subject;
    }
}
