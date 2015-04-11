/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.interfaces;

public interface IMsgTypeResolver {
    public IMsgTypeElement lookupMsgType(String name);

    /**
     * INTERNAL Factory method, to be called from INedTypeElement constructors.
     */
    public IMsgTypeInfo createTypeInfoFor(IMsgTypeElement node);
}
