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
 * A representation of the model object '<em><b>Processing Op</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.ProcessingOp#getOperation <em>Operation</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.ProcessingOp#getFilters <em>Filters</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.ProcessingOp#getParams <em>Params</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.ProcessingOp#getComputedFile <em>Computed File</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.scave.model.ScaveModelPackage#getProcessingOp()
 * @model abstract="true"
 * @generated
 */
public interface ProcessingOp extends DatasetItem {
	/**
	 * Returns the value of the '<em><b>Operation</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Operation</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Operation</em>' attribute.
	 * @see #setOperation(String)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getProcessingOp_Operation()
	 * @model
	 * @generated
	 */
	String getOperation();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.ProcessingOp#getOperation <em>Operation</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Operation</em>' attribute.
	 * @see #getOperation()
	 * @generated
	 */
	void setOperation(String value);

	/**
	 * Returns the value of the '<em><b>Filters</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.scave.model.SelectDeselectOp}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Filters</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Filters</em>' containment reference list.
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getProcessingOp_Filters()
	 * @model containment="true"
	 * @generated
	 */
	EList<SelectDeselectOp> getFilters();

	/**
	 * Returns the value of the '<em><b>Params</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.scave.model.Param}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Params</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Params</em>' containment reference list.
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getProcessingOp_Params()
	 * @model containment="true"
	 * @generated
	 */
	EList<Param> getParams();

	/**
	 * Returns the value of the '<em><b>Computed File</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Computed File</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Computed File</em>' attribute.
	 * @see #setComputedFile(String)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getProcessingOp_ComputedFile()
	 * @model transient="true"
	 * @generated
	 */
	String getComputedFile();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.ProcessingOp#getComputedFile <em>Computed File</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Computed File</em>' attribute.
	 * @see #getComputedFile()
	 * @generated
	 */
	void setComputedFile(String value);

} // ProcessingOp