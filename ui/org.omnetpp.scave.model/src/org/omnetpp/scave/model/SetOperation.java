/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.scave.model;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Set Operation</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.SetOperation#getSourceDataset <em>Source Dataset</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.SetOperation#getFilterPattern <em>Filter Pattern</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.SetOperation#getType <em>Type</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.SetOperation#getCachedIDs <em>Cached IDs</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.scave.model.ScaveModelPackage#getSetOperation()
 * @model abstract="true"
 * @generated
 */
public interface SetOperation extends EObject {
	/**
	 * Returns the value of the '<em><b>Source Dataset</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Source Dataset</em>' reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Source Dataset</em>' reference.
	 * @see #setSourceDataset(Dataset)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getSetOperation_SourceDataset()
	 * @model
	 * @generated
	 */
	Dataset getSourceDataset();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.SetOperation#getSourceDataset <em>Source Dataset</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Source Dataset</em>' reference.
	 * @see #getSourceDataset()
	 * @generated
	 */
	void setSourceDataset(Dataset value);

	/**
	 * Returns the value of the '<em><b>Filter Pattern</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Filter Pattern</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Filter Pattern</em>' attribute.
	 * @see #setFilterPattern(String)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getSetOperation_FilterPattern()
	 * @model
	 * @generated
	 */
	String getFilterPattern();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.SetOperation#getFilterPattern <em>Filter Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Filter Pattern</em>' attribute.
	 * @see #getFilterPattern()
	 * @generated
	 */
	void setFilterPattern(String value);

	/**
	 * Returns the value of the '<em><b>Type</b></em>' attribute.
	 * The literals are from the enumeration {@link org.omnetpp.scave.model.ResultType}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Type</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Type</em>' attribute.
	 * @see org.omnetpp.scave.model.ResultType
	 * @see #setType(ResultType)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getSetOperation_Type()
	 * @model
	 * @generated
	 */
	ResultType getType();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.SetOperation#getType <em>Type</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Type</em>' attribute.
	 * @see org.omnetpp.scave.model.ResultType
	 * @see #getType()
	 * @generated
	 */
	void setType(ResultType value);

	/**
	 * Returns the value of the '<em><b>Cached IDs</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Cached IDs</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Cached IDs</em>' attribute.
	 * @see #setCachedIDs(Object)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getSetOperation_CachedIDs()
	 * @model transient="true"
	 * @generated
	 */
	Object getCachedIDs();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.SetOperation#getCachedIDs <em>Cached IDs</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Cached IDs</em>' attribute.
	 * @see #getCachedIDs()
	 * @generated
	 */
	void setCachedIDs(Object value);

} // SetOperation