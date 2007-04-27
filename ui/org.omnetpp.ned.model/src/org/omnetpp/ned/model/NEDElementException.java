package org.omnetpp.ned.model;


/**
 * Exception that may be thrown from INEDElement operations,
 * e.g. from trying to set an attribute to an invalid value.
 *
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
