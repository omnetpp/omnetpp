/**
 */
package org.omnetpp.scave.model;

import org.eclipse.emf.common.util.EList;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Chart</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * </p>
 * <ul>
 *   <li>{@link org.omnetpp.scave.model.Chart#getScript <em>Script</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.Chart#getProperties <em>Properties</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.Chart#isTemporary <em>Temporary</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.Chart#getForm <em>Form</em>}</li>
 *   <li>{@link org.omnetpp.scave.model.Chart#getTemplateID <em>Template ID</em>}</li>
 * </ul>
 *
 * @see org.omnetpp.scave.model.ScaveModelPackage#getChart()
 * @model abstract="true"
 * @generated
 */
public interface Chart extends AnalysisItem {
    /**
     * Returns the value of the '<em><b>Script</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Script</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Script</em>' attribute.
     * @see #setScript(String)
     * @see org.omnetpp.scave.model.ScaveModelPackage#getChart_Script()
     * @model
     * @generated
     */
    String getScript();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.Chart#getScript <em>Script</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Script</em>' attribute.
     * @see #getScript()
     * @generated
     */
    void setScript(String value);

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
     * @see org.omnetpp.scave.model.ScaveModelPackage#getChart_Properties()
     * @model containment="true"
     * @generated
     */
    EList<Property> getProperties();

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
     * @see org.omnetpp.scave.model.ScaveModelPackage#getChart_Temporary()
     * @model default="false" required="true"
     * @generated
     */
    boolean isTemporary();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.Chart#isTemporary <em>Temporary</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Temporary</em>' attribute.
     * @see #isTemporary()
     * @generated
     */
    void setTemporary(boolean value);

    /**
     * Returns the value of the '<em><b>Form</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Form</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Form</em>' attribute.
     * @see #setForm(String)
     * @see org.omnetpp.scave.model.ScaveModelPackage#getChart_Form()
     * @model
     * @generated
     */
    String getForm();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.Chart#getForm <em>Form</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Form</em>' attribute.
     * @see #getForm()
     * @generated
     */
    void setForm(String value);

    /**
     * Returns the value of the '<em><b>Template ID</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Template ID</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Template ID</em>' attribute.
     * @see #setTemplateID(String)
     * @see org.omnetpp.scave.model.ScaveModelPackage#getChart_TemplateID()
     * @model
     * @generated
     */
    String getTemplateID();

    /**
     * Sets the value of the '{@link org.omnetpp.scave.model.Chart#getTemplateID <em>Template ID</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Template ID</em>' attribute.
     * @see #getTemplateID()
     * @generated
     */
    void setTemplateID(String value);

} // Chart
