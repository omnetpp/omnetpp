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
	 * Returns the value of the '<em><b>Chart Sheets</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.scave.model.ChartSheet}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Chart Sheets</em>' containment reference isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Chart Sheets</em>' containment reference list.
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getChartSheets_ChartSheets()
	 * @model type="org.omnetpp.scave.model.ChartSheet" containment="true"
	 * @generated
	 */
	EList<ChartSheet> getChartSheets();

} // ChartSheets