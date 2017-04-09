/**
 */
package org.omnetpp.scave.model;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Charts</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * </p>
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.Charts#getCharts <em>Charts</em>}</li>
 * </ul>
 *
 * @see org.omnetpp.scave.model.ScaveModelPackage#getCharts()
 * @model
 * @generated
 */
public interface Charts extends EObject {
    /**
     * Returns the value of the '<em><b>Charts</b></em>' containment reference list.
     * The list contents are of type {@link org.omnetpp.scave.model.Chart}.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Charts</em>' containment reference list isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Charts</em>' containment reference list.
     * @see org.omnetpp.scave.model.ScaveModelPackage#getCharts_Charts()
     * @model containment="true"
     * @generated
     */
    EList<Chart> getCharts();

} // Charts
