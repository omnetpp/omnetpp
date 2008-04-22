/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.scave.model;


/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Line Chart</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.LineChart#getLineNameFormat <em>Line Name Format</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.scave.model.ScaveModelPackage#getLineChart()
 * @model
 * @generated
 */
public interface LineChart extends Chart {

	/**
	 * Returns the value of the '<em><b>Line Name Format</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Line Name Format</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Line Name Format</em>' attribute.
	 * @see #setLineNameFormat(String)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getLineChart_LineNameFormat()
	 * @model
	 * @generated
	 */
	String getLineNameFormat();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.LineChart#getLineNameFormat <em>Line Name Format</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Line Name Format</em>' attribute.
	 * @see #getLineNameFormat()
	 * @generated
	 */
	void setLineNameFormat(String value);
} // LineChart