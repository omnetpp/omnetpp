/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.scave.model;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Inputs</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.Inputs#getInputs <em>Inputs</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.scave.model.ScaveModelPackage#getInputs()
 * @model
 * @generated
 */
public interface Inputs extends EObject {
	/**
	 * Returns the value of the '<em><b>Inputs</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.scave.model.InputFile}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Inputs</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Inputs</em>' containment reference list.
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getInputs_Inputs()
	 * @model type="org.omnetpp.scave.model.InputFile" containment="true"
	 * @generated
	 */
	EList getInputs();

} // Inputs