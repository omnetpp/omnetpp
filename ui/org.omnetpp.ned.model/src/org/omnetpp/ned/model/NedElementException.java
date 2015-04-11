/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model;


/**
 * Exception that may be thrown from INedElement operations,
 * e.g. from trying to set an attribute to an invalid value.
 *
 *
 * @author rhornig
 */
public class NedElementException extends RuntimeException {
    private static final long serialVersionUID = 1L; // to suppress warning
    private INedElement node;

    public NedElementException(INedElement node, String message) {
        super(message);
        this.node = node;
    }

    INedElement getNode() {
        return node;
    }
}
