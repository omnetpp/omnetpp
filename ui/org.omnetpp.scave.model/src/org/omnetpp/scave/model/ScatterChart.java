/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.scave.model;


/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Scatter Chart</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.ScatterChart#getModuleName <em>Module Name</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.ScatterChart#getDataName <em>Data Name</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.scave.model.ScaveModelPackage#getScatterChart()
 * @model
 * @generated
 */
public interface ScatterChart extends Chart {
	/**
	 * Returns the value of the '<em><b>Module Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Module Name</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Module Name</em>' attribute.
	 * @see #setModuleName(String)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getScatterChart_ModuleName()
	 * @model
	 * @generated
	 */
	String getModuleName();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.ScatterChart#getModuleName <em>Module Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Module Name</em>' attribute.
	 * @see #getModuleName()
	 * @generated
	 */
	void setModuleName(String value);

	/**
	 * Returns the value of the '<em><b>Data Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Data Name</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Data Name</em>' attribute.
	 * @see #setDataName(String)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getScatterChart_DataName()
	 * @model
	 * @generated
	 */
	String getDataName();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.ScatterChart#getDataName <em>Data Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Data Name</em>' attribute.
	 * @see #getDataName()
	 * @generated
	 */
	void setDataName(String value);

} // ScatterChart