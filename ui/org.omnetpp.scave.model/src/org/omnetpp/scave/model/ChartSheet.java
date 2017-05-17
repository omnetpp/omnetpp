/**
 */
package org.omnetpp.scave.model;

import org.eclipse.emf.common.util.EList;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Chart Sheet</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * </p>
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.ChartSheet#getCharts <em>Charts</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.ChartSheet#isTemporary <em>Temporary</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.ChartSheet#getProperties <em>Properties</em>}</li>
 * </ul>
 *
 * @see org.omnetpp.scave.model.ScaveModelPackage#getChartSheet()
 * @model
 * @generated
 */
public interface ChartSheet extends AnalysisItem {
    /**
     * Returns the value of the '<em><b>Charts</b></em>' reference list.
     * The list contents are of type {@link org.omnetpp.scave.model.Chart}.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Charts</em>' reference list isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Charts</em>' reference list.
     * @see org.omnetpp.scave.model.ScaveModelPackage#getChartSheet_Charts()
     * @model
     * @generated
     */
    EList<Chart> getCharts();

    /**
     * Returns the value of the '<em><b>Temporary</b></em>' attribute.
     * The default value is <code>"false"</code>.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Temporary</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Temporary</em>' attribute.
     * @see #setTemporary(boolean)
     * @see org.omnetpp.scave.model.ScaveModelPackage#getChartSheet_Temporary()
     * @model default="false" required="true"
     * @generated
     */
    boolean isTemporary();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.ChartSheet#isTemporary <em>Temporary</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Temporary</em>' attribute.
     * @see #isTemporary()
     * @generated
     */
    void setTemporary(boolean value);

    /**
     * Returns the value of the '<em><b>Properties</b></em>' containment reference list.
     * The list contents are of type {@link org.omnetpp.scave.model.Property}.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Properties</em>' containment reference list isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Properties</em>' containment reference list.
     * @see org.omnetpp.scave.model.ScaveModelPackage#getChartSheet_Properties()
     * @model containment="true"
     * @generated
     */
    EList<Property> getProperties();

} // ChartSheet
