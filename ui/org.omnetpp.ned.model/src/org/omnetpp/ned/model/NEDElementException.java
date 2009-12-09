/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model;


/**
 * Exception that may be thrown from INEDElement operations,
 * e.g. from trying to set an attribute to an invalid value.
 *
 *
 * @author rhornig
 */
public class NEDElementException extends RuntimeException {
	private static final long serialVersionUID = 1L; // to suppress warning
	private INEDElement node;

	public NEDElementException(INEDElement node, String message) {
		super(message);
		this.node = node;
	}

	INEDElement getNode() {
		return node;
	}
}
