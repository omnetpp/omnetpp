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
 * A representation of the model object '<em><b>Chart Sheets</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.ChartSheets#getChartSheets <em>Chart Sheets</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.scave.model.ScaveModelPackage#getChartSheets()
 * @model
 * @generated
 */
public interface ChartSheets extends EObject {
	/**
	 * Returns the value of the '<em><b>Chart Sheets</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Chart Sheets</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Chart Sheets</em>' containment reference.
	 * @see #setChartSheets(ChartSheet)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getChartSheets_ChartSheets()
	 * @model containment="true"
	 * @generated
	 */
	ChartSheet getChartSheets();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.ChartSheets#getChartSheets <em>Chart Sheets</em>}' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Chart Sheets</em>' containment reference.
	 * @see #getChartSheets()
	 * @generated
	 */
	void setChartSheets(ChartSheet value);

} // ChartSheets