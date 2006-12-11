package org.omnetpp.ned.model;

/**
 * Exception that may be thrown from NEDElement operations,
 * e.g. from trying to set an attribute to an invalid value.
 *
 */
public class NEDElementException extends RuntimeException {
	private static final long serialVersionUID = 1L; // to suppress warning
	private NEDElement node;

	public NEDElementException(NEDElement node, String message) {
		super(message);
		this.node = node;
	}
	
	NEDElement getNode() {
		return node;
	}
}
