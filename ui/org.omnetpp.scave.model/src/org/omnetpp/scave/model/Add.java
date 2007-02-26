/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.scave.model;

import org.eclipse.emf.common.util.EList;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Add</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.Add#getExcepts <em>Excepts</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.scave.model.ScaveModelPackage#getAdd()
 * @model
 * @generated
 */
public interface Add extends AddDiscardOp {
	/**
	 * Returns the value of the '<em><b>Excepts</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.scave.model.Except}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Excepts</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Excepts</em>' containment reference list.
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getAdd_Excepts()
	 * @model type="org.omnetpp.scave.model.Except" containment="true"
	 * @generated
	 */
	EList getExcepts();

} // Add