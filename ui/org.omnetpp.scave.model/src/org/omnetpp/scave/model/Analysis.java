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
 * A representation of the model object '<em><b>Analysis</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.Analysis#getDatasets <em>Datasets</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.Analysis#getChartPages <em>Chart Pages</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.scave.model.ScaveModelPackage#getAnalysis()
 * @model
 * @generated
 */
public interface Analysis extends EObject {
	/**
	 * Returns the value of the '<em><b>Datasets</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.scave.model.Dataset}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Datasets</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Datasets</em>' containment reference list.
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getAnalysis_Datasets()
	 * @model type="org.omnetpp.scave.model.Dataset" containment="true"
	 * @generated
	 */
	EList getDatasets();

	/**
	 * Returns the value of the '<em><b>Chart Pages</b></em>' containment reference list.
	 * The list contents are of type {@link org.omnetpp.scave.model.ChartSheet}.
	 * <!-- begin-user-doc -->
	 * <p>
	 * If the meaning of the '<em>Chart Pages</em>' containment reference list isn't clear,
	 * there really should be more of a description here...
	 * </p>
	 * <!-- end-user-doc -->
	 * @return the value of the '<em>Chart Pages</em>' containment reference list.
	 * @see org.omnetpp.scave.model.ScaveModelPackage#getAnalysis_ChartPages()
	 * @model type="org.omnetpp.scave.model.ChartSheet" containment="true"
	 * @generated
	 */
	EList getChartPages();

} // Analysis