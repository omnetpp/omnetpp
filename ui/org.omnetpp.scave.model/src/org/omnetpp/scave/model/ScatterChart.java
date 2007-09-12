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
 * A representation of the model object '<em><b>Scatter Chart</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.ScatterChart#getXDataPattern <em>XData Pattern</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.ScatterChart#getIsoDataPattern <em>Iso Data Pattern</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.ScatterChart#isAverageReplications <em>Average Replications</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.scave.model.ScaveModelPackage#getScatterChart()
 * @model
 * @generated
 */
public interface ScatterChart extends Chart {
	/**
	 * Returns the value of the '<em><b>XData Pattern</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>XData Pattern</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>XData Pattern</em>' attribute.
	 * @see #setXDataPattern(String)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getScatterChart_XDataPattern()
	 * @model
	 * @generated
	 */
	String getXDataPattern();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.ScatterChart#getXDataPattern <em>XData Pattern</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>XData Pattern</em>' attribute.
	 * @see #getXDataPattern()
	 * @generated
	 */
	void setXDataPattern(String value);

	/**
	 * Returns the value of the '<em><b>Iso Data Pattern</b></em>' attribute list.
	 * The list contents are of type {@link java.lang.String}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Iso Data Pattern</em>' attribute list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Iso Data Pattern</em>' attribute list.
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getScatterChart_IsoDataPattern()
	 * @model
	 * @generated
	 */
	EList<String> getIsoDataPattern();

	/**
	 * Returns the value of the '<em><b>Average Replications</b></em>' attribute.
	 * The default value is <code>"true"</code>.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Average Replications</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Average Replications</em>' attribute.
	 * @see #setAverageReplications(boolean)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getScatterChart_AverageReplications()
	 * @model default="true"
	 * @generated
	 */
	boolean isAverageReplications();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.ScatterChart#isAverageReplications <em>Average Replications</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>Average Replications</em>' attribute.
	 * @see #isAverageReplications()
	 * @generated
	 */
	void setAverageReplications(boolean value);

} // ScatterChart