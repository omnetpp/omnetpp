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
 * A representation of the model object '<em><b>Bar Chart</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.BarChart#getGroupBy <em>Group By</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.scave.model.ScaveModelPackage#getBarChart()
 * @model
 * @generated
 */
public interface BarChart extends Chart {
	/**
	 * Returns the value of the '<em><b>Group By</b></em>' attribute list.
	 * The list contents are of type {@link java.lang.String}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Group By</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Group By</em>' attribute list.
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getBarChart_GroupBy()
	 * @model type="java.lang.String" ordered="false"
	 * @generated
	 */
	EList getGroupBy();

} // BarChart