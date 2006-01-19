/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned.model.emf;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Simple Module</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.ned.model.emf.SimpleModule#getParams <em>Params</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.SimpleModule#getGates <em>Gates</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.SimpleModule#getDisplayString <em>Display String</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.SimpleModule#getBannerComment <em>Banner Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.SimpleModule#getName <em>Name</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.SimpleModule#getRightComment <em>Right Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.SimpleModule#getTrailingComment <em>Trailing Comment</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.ned.model.emf.NedPackage#getSimpleModule()
 * @model extendedMetaData="name='simple-module' kind='elementOnly'"
 * @generated
 */
public interface SimpleModule extends EObject {
    /**
     * Returns the value of the '<em><b>Params</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Params</em>' containment reference isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Params</em>' containment reference.
     * @see #setParams(Params)
     * @see org.omnetpp.ned.model.emf.NedPackage#getSimpleModule_Params()
     * @model containment="true" resolveProxies="false"
     *        extendedMetaData="kind='element' name='params' namespace='##targetNamespace'"
     * @generated
     */
    Params getParams();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.SimpleModule#getParams <em>Params</em>}' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Params</em>' containment reference.
     * @see #getParams()
     * @generated
     */
    void setParams(Params value);

    /**
     * Returns the value of the '<em><b>Gates</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Gates</em>' containment reference isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Gates</em>' containment reference.
     * @see #setGates(Gates)
     * @see org.omnetpp.ned.model.emf.NedPackage#getSimpleModule_Gates()
     * @model containment="true" resolveProxies="false"
     *        extendedMetaData="kind='element' name='gates' namespace='##targetNamespace'"
     * @generated
     */
    Gates getGates();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.SimpleModule#getGates <em>Gates</em>}' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Gates</em>' containment reference.
     * @see #getGates()
     * @generated
     */
    void setGates(Gates value);

    /**
     * Returns the value of the '<em><b>Display String</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Display String</em>' containment reference isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Display String</em>' containment reference.
     * @see #setDisplayString(DisplayString)
     * @see org.omnetpp.ned.model.emf.NedPackage#getSimpleModule_DisplayString()
     * @model containment="true" resolveProxies="false"
     *        extendedMetaData="kind='element' name='display-string' namespace='##targetNamespace'"
     * @generated
     */
    DisplayString getDisplayString();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.SimpleModule#getDisplayString <em>Display String</em>}' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Display String</em>' containment reference.
     * @see #getDisplayString()
     * @generated
     */
    void setDisplayString(DisplayString value);

    /**
     * Returns the value of the '<em><b>Banner Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Banner Comment</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Banner Comment</em>' attribute.
     * @see #setBannerComment(String)
     * @see org.omnetpp.ned.model.emf.NedPackage#getSimpleModule_BannerComment()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='banner-comment'"
     * @generated
     */
    String getBannerComment();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.SimpleModule#getBannerComment <em>Banner Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Banner Comment</em>' attribute.
     * @see #getBannerComment()
     * @generated
     */
    void setBannerComment(String value);

    /**
     * Returns the value of the '<em><b>Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Name</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Name</em>' attribute.
     * @see #setName(String)
     * @see org.omnetpp.ned.model.emf.NedPackage#getSimpleModule_Name()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN" required="true"
     *        extendedMetaData="kind='attribute' name='name'"
     * @generated
     */
    String getName();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.SimpleModule#getName <em>Name</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Name</em>' attribute.
     * @see #getName()
     * @generated
     */
    void setName(String value);

    /**
     * Returns the value of the '<em><b>Right Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Right Comment</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Right Comment</em>' attribute.
     * @see #setRightComment(String)
     * @see org.omnetpp.ned.model.emf.NedPackage#getSimpleModule_RightComment()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='right-comment'"
     * @generated
     */
    String getRightComment();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.SimpleModule#getRightComment <em>Right Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Right Comment</em>' attribute.
     * @see #getRightComment()
     * @generated
     */
    void setRightComment(String value);

    /**
     * Returns the value of the '<em><b>Trailing Comment</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Trailing Comment</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Trailing Comment</em>' attribute.
     * @see #setTrailingComment(String)
     * @see org.omnetpp.ned.model.emf.NedPackage#getSimpleModule_TrailingComment()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='trailing-comment'"
     * @generated
     */
    String getTrailingComment();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.SimpleModule#getTrailingComment <em>Trailing Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Trailing Comment</em>' attribute.
     * @see #getTrailingComment()
     * @generated
     */
    void setTrailingComment(String value);

} // SimpleModule
