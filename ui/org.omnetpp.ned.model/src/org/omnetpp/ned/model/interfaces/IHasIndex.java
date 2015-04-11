/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.interfaces;

/**
 * Objects that can have an optional index (ie. they can be vector or non vector)
 *
 * @author rhornig
 */
public interface IHasIndex extends IHasName {

    public String getVectorSize();

    public void setVectorSize(String indexstring);

    public String getNameWithIndex();

}
