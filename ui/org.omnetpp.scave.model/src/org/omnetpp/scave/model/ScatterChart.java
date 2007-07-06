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
 *   <li>{@link org.omnetpp.scave.model.ScatterChart#getXDataModule <em>XData Module</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.ScatterChart#getXDataName <em>XData Name</em>}</li>
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
	 * Returns the value of the '<em><b>XData Module</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>XData Module</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>XData Module</em>' attribute.
	 * @see #setXDataModule(String)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getScatterChart_XDataModule()
	 * @model
	 * @generated
	 */
	String getXDataModule();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.ScatterChart#getXDataModule <em>XData Module</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>XData Module</em>' attribute.
	 * @see #getXDataModule()
	 * @generated
	 */
	void setXDataModule(String value);

	/**
	 * Returns the value of the '<em><b>XData Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>XData Name</em>' attribute isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>XData Name</em>' attribute.
	 * @see #setXDataName(String)
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getScatterChart_XDataName()
	 * @model
	 * @generated
	 */
	String getXDataName();

	/**
	 * Sets the value of the '{@link org.omnetpp.scave.model.ScatterChart#getXDataName <em>XData Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @param value the new value of the '<em>XData Name</em>' attribute.
	 * @see #getXDataName()
	 * @generated
	 */
	void setXDataName(String value);

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