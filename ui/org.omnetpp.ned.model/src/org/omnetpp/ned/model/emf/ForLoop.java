/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package org.omnetpp.ned.model.emf;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>For Loop</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>
 * The following features are supported:
 * <ul>
 *   <li>{@link org.omnetpp.ned.model.emf.ForLoop#getForLoop <em>For Loop</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.ForLoop#getConnection <em>Connection</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.ForLoop#getBannerComment <em>Banner Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.ForLoop#getFromValue <em>From Value</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.ForLoop#getParamName <em>Param Name</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.ForLoop#getRightComment <em>Right Comment</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.ForLoop#getToValue <em>To Value</em>}</li>
 *   <li>{@link org.omnetpp.ned.model.emf.ForLoop#getTrailingComment <em>Trailing Comment</em>}</li>
 * </ul>
 * </p>
 *
 * @see org.omnetpp.ned.model.emf.NedPackage#getForLoop()
 * @model extendedMetaData="name='for-loop' kind='elementOnly'"
 * @generated
 */
public interface ForLoop extends EObject {
    /**
     * Returns the value of the '<em><b>For Loop</b></em>' containment reference.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>For Loop</em>' containment reference isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>For Loop</em>' containment reference.
     * @see #setForLoop(ForLoop)
     * @see org.omnetpp.ned.model.emf.NedPackage#getForLoop_ForLoop()
     * @model containment="true" resolveProxies="false"
     *        extendedMetaData="kind='element' name='for-loop' namespace='##targetNamespace'"
     * @generated
     */
    ForLoop getForLoop();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.ForLoop#getForLoop <em>For Loop</em>}' containment reference.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>For Loop</em>' containment reference.
     * @see #getForLoop()
     * @generated
     */
    void setForLoop(ForLoop value);

    /**
     * Returns the value of the '<em><b>Connection</b></em>' containment reference list.
     * The list contents are of type {@link org.omnetpp.ned.model.emf.Connection}.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Connection</em>' containment reference list isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Connection</em>' containment reference list.
     * @see org.omnetpp.ned.model.emf.NedPackage#getForLoop_Connection()
     * @model type="org.omnetpp.ned.model.emf.Connection" containment="true" resolveProxies="false"
     *        extendedMetaData="kind='element' name='connection' namespace='##targetNamespace'"
     * @generated
     */
    EList getConnection();

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
     * @see org.omnetpp.ned.model.emf.NedPackage#getForLoop_BannerComment()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='banner-comment'"
     * @generated
     */
    String getBannerComment();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.ForLoop#getBannerComment <em>Banner Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Banner Comment</em>' attribute.
     * @see #getBannerComment()
     * @generated
     */
    void setBannerComment(String value);

    /**
     * Returns the value of the '<em><b>From Value</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>From Value</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>From Value</em>' attribute.
     * @see #setFromValue(String)
     * @see org.omnetpp.ned.model.emf.NedPackage#getForLoop_FromValue()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
     *        extendedMetaData="kind='attribute' name='from-value'"
     * @generated
     */
    String getFromValue();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.ForLoop#getFromValue <em>From Value</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>From Value</em>' attribute.
     * @see #getFromValue()
     * @generated
     */
    void setFromValue(String value);

    /**
     * Returns the value of the '<em><b>Param Name</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>Param Name</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>Param Name</em>' attribute.
     * @see #setParamName(String)
     * @see org.omnetpp.ned.model.emf.NedPackage#getForLoop_ParamName()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NMTOKEN" required="true"
     *        extendedMetaData="kind='attribute' name='param-name'"
     * @generated
     */
    String getParamName();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.ForLoop#getParamName <em>Param Name</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Param Name</em>' attribute.
     * @see #getParamName()
     * @generated
     */
    void setParamName(String value);

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
     * @see org.omnetpp.ned.model.emf.NedPackage#getForLoop_RightComment()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='right-comment'"
     * @generated
     */
    String getRightComment();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.ForLoop#getRightComment <em>Right Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Right Comment</em>' attribute.
     * @see #getRightComment()
     * @generated
     */
    void setRightComment(String value);

    /**
     * Returns the value of the '<em><b>To Value</b></em>' attribute.
     * <!-- begin-user-doc -->
     * <p>
     * If the meaning of the '<em>To Value</em>' attribute isn't clear,
     * there really should be more of a description here...
     * </p>
     * <!-- end-user-doc -->
     * @return the value of the '<em>To Value</em>' attribute.
     * @see #setToValue(String)
     * @see org.omnetpp.ned.model.emf.NedPackage#getForLoop_ToValue()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
     *        extendedMetaData="kind='attribute' name='to-value'"
     * @generated
     */
    String getToValue();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.ForLoop#getToValue <em>To Value</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>To Value</em>' attribute.
     * @see #getToValue()
     * @generated
     */
    void setToValue(String value);

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
     * @see org.omnetpp.ned.model.emf.NedPackage#getForLoop_TrailingComment()
     * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
     *        extendedMetaData="kind='attribute' name='trailing-comment'"
     * @generated
     */
    String getTrailingComment();

    /**
     * Sets the value of the '{@link org.omnetpp.ned.model.emf.ForLoop#getTrailingComment <em>Trailing Comment</em>}' attribute.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     * @param value the new value of the '<em>Trailing Comment</em>' attribute.
     * @see #getTrailingComment()
     * @generated
     */
    void setTrailingComment(String value);

} // ForLoop
