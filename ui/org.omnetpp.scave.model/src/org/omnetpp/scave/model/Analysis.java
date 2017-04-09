/**
 */
package org.omnetpp.scave.model;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Analysis</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * </p>
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.Analysis#getInputs <em>Inputs</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.Analysis#getCharts <em>Charts</em>}</li>
 * </ul>
 *
 * @see org.omnetpp.scave.model.ScaveModelPackage#getAnalysis()
 * @model
 * @generated
 */
public interface Analysis extends EObject {
    /**
     * Returns the value of the '<em><b>Inputs</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Inputs</em>' containment reference isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Inputs</em>' containment reference.
     * @see #setInputs(Inputs)
     * @see org.omnetpp.scave.model.ScaveModelPackage#getAnalysis_Inputs()
     * @model containment="true"
     * @generated
     */
    Inputs getInputs();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.Analysis#getInputs <em>Inputs</em>}' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Inputs</em>' containment reference.
     * @see #getInputs()
     * @generated
     */
    void setInputs(Inputs value);

    /**
     * Returns the value of the '<em><b>Charts</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Charts</em>' containment reference isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Charts</em>' containment reference.
     * @see #setCharts(Charts)
     * @see org.omnetpp.scave.model.ScaveModelPackage#getAnalysis_Charts()
     * @model containment="true"
     * @generated
     */
    Charts getCharts();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.Analysis#getCharts <em>Charts</em>}' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Charts</em>' containment reference.
     * @see #getCharts()
     * @generated
     */
    void setCharts(Charts value);

} // Analysis
