/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.interfaces;


public interface IMsgTypeElement extends ITypeElement {

    /**
     * Returns the typeinfo belonging to this MSG type. This can be trusted
     * to be NOT null for all MSG types, including duplicate and invalid ones.
     *
     * Only null for detached trees that haven't been seen at all by MsgResources
     * (i.e. not part of any MSG file).
     */
    public IMsgTypeInfo getMsgTypeInfo();
}
